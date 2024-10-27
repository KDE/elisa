/*
   SPDX-FileCopyrightText: 2021 Han Young <hanyoung@protonmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */
#include "lyricsmodel.h"
#include <algorithm>
#include <unordered_map>
#include <KLocalizedString>
class LyricsModel::LyricsModelPrivate
{
public:
    bool parse(const QString &lyric);
    int highlightedIndex{-1};
    bool isLRC {false};

    std::vector<std::pair<QString, qint64>> lyrics;

private:
    qint64 parseOneTimeStamp(QString::const_iterator &begin, QString::const_iterator end);
    QString parseOneLine(QString::const_iterator &begin, QString::const_iterator end);
    QString parseTags(QString::const_iterator &begin, QString::const_iterator end);

    qint64 offset = 0;
};
/*###########parseOneTimeStamp###########
 * Function to parse timestamp of one LRC line
 * if successful, return timestamp in milliseconds
 * otherwise return -1
 * */
qint64 LyricsModel::LyricsModelPrivate::parseOneTimeStamp(
    QString::const_iterator &begin,
    QString::const_iterator end)
{
    /* Example of LRC format and corresponding states
     *
     * States:
     *
     *  [00:01.02]bla bla
     * ^^^ ^^ ^^ ^^
     * ||| || || ||
     * ||| || || |End
     * ||| || || RightBracket
     * ||| || |Hundredths
     * ||| || Period
     * ||| |Seconds
     * ||| Colon
     * ||Minutes
     * |LeftBracket
     * Start
     * */
    enum States {Start, LeftBracket, Minutes, Colon, Seconds, Period, Hundredths, RightBracket, End};
    auto states {States::Start};
    auto minute {0}, second {0}, hundred {0};

    while (begin != end) {
        switch (begin->toLatin1()) {
        case '.':
            if (states == Seconds)
                states = Period;
            break;
        case '[':
            if (states == Start)
                states = LeftBracket;
            break;
        case ']':
            begin++;
            if (states == Hundredths) {
                return minute * 60 * 1000 + second * 1000 +
                    hundred * 10; // we return milliseconds
            }
            else {
                return -1;
            }
        case ':':
            if (states == Minutes)
                states = Colon;
            break;
        default:
            if (begin->isDigit()) {
                switch (states) {
                    case LeftBracket:
                        states = Minutes;
                        [[fallthrough]];
                    case Minutes:
                        minute *= 10;
                        minute += begin->digitValue();
                        break;
                    case Colon:
                        states = Seconds;
                        [[fallthrough]];
                    case Seconds:
                        second *= 10;
                        second += begin->digitValue();
                        break;
                    case Period:
                        states = Hundredths;
                        [[fallthrough]];
                    case Hundredths:
                        // we only parse to hundredth second
                        if (hundred >= 100) {
                            break;
                        }
                        hundred *= 10;
                        hundred += begin->digitValue();
                        break;
                    default:
                        // lyric format is corrupt
                        break;
                }
            } else {
                begin++;
                return -1;
            }
            break;
        }
        begin++;
    }

    // end of lyric and no correct value found
    return -1;
}

QString
LyricsModel::LyricsModelPrivate::parseOneLine(QString::const_iterator &begin,
                                              QString::const_iterator end)
{
    auto size{0};
    auto it = begin;
    while (begin != end) {
        if (begin->toLatin1() != '[') {
            size++;
        } else
            break;
        begin++;
    }
    if (size) {
        return QString(--it, size); // FIXME: really weird workaround for QChar,
                                    // otherwise first char is lost
    } else
        return {};
}

/*
 * [length:04:07.46]
 * [re:www.megalobiz.com/lrc/maker]
 * [ve:v1.2.3]
 */
QString LyricsModel::LyricsModelPrivate::parseTags(QString::const_iterator &begin, QString::const_iterator end)
{
    static auto skipTillChar = [](QString::const_iterator begin, QString::const_iterator end, char endChar) {
        while (begin != end && begin->toLatin1() != endChar) {
            begin++;
        }
        return begin;
    };
    static std::unordered_map<QString, QString> map = {
        {QStringLiteral("ar"), i18nc("@label musical artist", "Artist")},
        {QStringLiteral("al"), i18nc("@label musical album", "Album")},
        {QStringLiteral("ti"), i18nc("@label song title", "Title")},
        {QStringLiteral("au"), i18nc("@label", "Creator")},
        {QStringLiteral("length"), i18nc("@label song length", "Length")},
        {QStringLiteral("by"), i18nc("@label as in 'Created by: Joe'", "Created by")},
        {QStringLiteral("re"), i18nc("@label as in 'a person who edits'", "Editor")},
        {QStringLiteral("ve"), i18nc("@label", "Version")}};
    QString tags;

    while (begin != end) {
        // skip till tags
        begin = skipTillChar(begin, end, '[');
        if (begin != end) {
            begin++;
        }
        else {
            break;
        }

        auto tagIdEnd = skipTillChar(begin, end, ':');
        auto tagId = QString(begin, std::distance(begin, tagIdEnd));
        if (tagIdEnd != end &&
            (map.count(tagId) || tagId == QStringLiteral("offset"))) {
          tagIdEnd++;

          auto tagContentEnd = skipTillChar(tagIdEnd, end, ']');
          bool ok = true;
          if (map.count(tagId)) {
            tags += i18nc(
                "@label this is a key => value map", "%1: %2\n", map[tagId],
                QString(tagIdEnd, std::distance(tagIdEnd, tagContentEnd)));
          } else {
            // offset tag
            offset = QString(tagIdEnd, std::distance(tagIdEnd, tagContentEnd))
                         .toLongLong(&ok);
          }

          if (ok) {
            begin = tagContentEnd;
          } else {
            // Invalid offset tag, we step back one to compensate the '[' we
            // step over
            begin--;
            break;
          }
        } else {
          // No tag, we step back one to compensate the '[' we step over
          begin--;
          break;
        }
    }
    return tags;
}

bool LyricsModel::LyricsModelPrivate::parse(const QString &lyric)
{
    lyrics.clear();
    offset = 0;

    if (lyric.isEmpty())
        return false;

    QString::const_iterator begin = lyric.begin(), end = lyric.end();
    auto tag = parseTags(begin, end);
    std::vector<qint64> timeStamps;

    while (begin != lyric.end()) {
        auto timeStamp = parseOneTimeStamp(begin, end);
        while (timeStamp >= 0) {
          // one line can have multiple timestamps
          // [00:12.00][00:15.30]Some more lyrics ...
          timeStamps.push_back(timeStamp);
          timeStamp = parseOneTimeStamp(begin, end);
        }
        auto string = parseOneLine(begin, end);
        if (!string.isEmpty() && !timeStamps.empty()) {
            for (auto time : timeStamps) {
                lyrics.push_back({string, time});
            }
        }
        timeStamps.clear();
    }

    std::sort(lyrics.begin(),
              lyrics.end(),
              [](const std::pair<QString, qint64> &lhs,
                 const std::pair<QString, qint64> &rhs) {
                  return lhs.second < rhs.second;
              });
    if (offset) {
      std::transform(lyrics.begin(), lyrics.end(),
                     lyrics.begin(),
                     [this](std::pair<QString, qint64> &element) {
                       element.second = std::max(element.second - offset, 0ll);
                       return element;
                     });
    }
    // insert tags to first lyric front
    if (!lyrics.empty() && !tag.isEmpty()) {
        lyrics.insert(lyrics.begin(), {tag, 0});
    }
    return !lyrics.empty();
}

LyricsModel::LyricsModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(std::make_unique<LyricsModelPrivate>())
{
}

LyricsModel::~LyricsModel() = default;

int LyricsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return d->lyrics.size();
}

QVariant LyricsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= (int)d->lyrics.size())
        return {};

    switch (role) {
    case LyricsRole::Lyric:
        return d->lyrics.at(index.row()).first;
    case LyricsRole::TimeStamp:
        return d->lyrics.at(index.row()).second;
    }

    return QVariant();
}

void LyricsModel::setLyric(const QString &lyric)
{
    bool isLRC = true;

    beginResetModel();
    auto ret = d->parse(lyric);

    // has non-LRC formatted lyric
    if (!ret && !lyric.isEmpty()) {
        d->lyrics = {{lyric, 0ll}};
        d->highlightedIndex = -1;
        isLRC = false;
    }
    endResetModel();

    Q_EMIT highlightedIndexChanged();
    Q_EMIT lyricChanged();
    if (isLRC != d->isLRC) {
        d->isLRC = isLRC;
        Q_EMIT isLRCChanged();
    }
}

void LyricsModel::setPosition(qint64 position)
{
    if (!isLRC()) {
        return;
    }

    // do binary search
    auto result =
        std::lower_bound(d->lyrics.begin(),
                         d->lyrics.end(),
                         position,
                         [](const std::pair<QString, qint64> &lhs, qint64 value) {
                             return lhs.second < value;
                         });

    int highlightedIndex = -1;

    if (result != d->lyrics.end() && result->second == position) {
        highlightedIndex = std::distance(d->lyrics.begin(), result);
    } else if (result != d->lyrics.begin()) {
        highlightedIndex = std::distance(d->lyrics.begin(), --result);
    }

    if (highlightedIndex != d->highlightedIndex) {
        d->highlightedIndex = highlightedIndex;
        Q_EMIT highlightedIndexChanged();
    }
}

int LyricsModel::highlightedIndex() const
{
    return d->highlightedIndex;
}

bool LyricsModel::isLRC() const
{
    return d->isLRC;
}

QHash<int, QByteArray> LyricsModel::roleNames() const
{
    return {{LyricsRole::Lyric, QByteArrayLiteral("lyric")}, {LyricsRole::TimeStamp, QByteArrayLiteral("timestamp")}};
}

#include "moc_lyricsmodel.cpp"

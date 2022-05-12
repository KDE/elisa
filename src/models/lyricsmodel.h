/*
   SPDX-FileCopyrightText: 2021 Han Young <hanyoung@protonmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */
#ifndef LYRICSMODEL_H
#define LYRICSMODEL_H
#include "elisaLib_export.h"
#include <QAbstractListModel>
#include <memory>
#include <vector>
class ELISALIB_EXPORT LyricsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int highlightedIndex
               READ highlightedIndex
               NOTIFY highlightedIndexChanged)
    Q_PROPERTY(bool isLRC READ isLRC NOTIFY isLRCChanged)
public:
    enum LyricsRole {Lyric = Qt::UserRole, TimeStamp};
    LyricsModel(QObject *parent = nullptr);
    ~LyricsModel() override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    int highlightedIndex() const;
    bool isLRC() const;

public Q_SLOTS:
    Q_INVOKABLE void setLyric(const QString &lyric);
    Q_INVOKABLE void setPosition(qint64 position);

Q_SIGNALS:
    void lyricChanged();
    void highlightedIndexChanged();
    void positionChanged();
    void isLRCChanged();
private:
    class LyricsModelPrivate;
    std::unique_ptr<LyricsModelPrivate> d;
};

#endif // LYRICSMODEL_H

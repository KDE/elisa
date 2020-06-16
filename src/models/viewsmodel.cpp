/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "viewsmodel.h"

#include "viewslistdata.h"

#include <KI18n/KLocalizedString>

class ViewsModelPrivate
{

public:

    ViewsListData *mViewsData = nullptr;

    ElisaUtils::PlayListEntryType mEmbeddedCategory = ElisaUtils::Unknown;

    QMap<ElisaUtils::PlayListEntryType, QUrl> mDefaultIcons = {{ElisaUtils::Album, QUrl{QStringLiteral("image://icon/view-media-album-cover")}},
                                                               {ElisaUtils::Artist, QUrl{QStringLiteral("image://icon/view-media-artist")}},
                                                               {ElisaUtils::Genre, QUrl{QStringLiteral("image://icon/view-media-genre")}},};
};

ViewsModel::ViewsModel(QObject *parent)
    : QAbstractListModel(parent), d(std::make_unique<ViewsModelPrivate>())
{
}

ViewsModel::~ViewsModel() = default;

QHash<int, QByteArray> ViewsModel::roleNames() const
{
    auto result = QAbstractListModel::roleNames();

    result[ImageNameRole] = "image";
    result[UseColorOverlayRole] = "useColorOverlay";
    result[DatabaseIdRole] = "databaseId";
    result[UseSecondTitleRole] = "useSecondTitle";
    result[SecondTitleRole] = "secondTitle";
    result[EntryCategoryRole] = "entryCategory";

    return result;
}

Qt::ItemFlags ViewsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

int ViewsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    if (!d->mViewsData) {
        return 0;
    }

    return d->mViewsData->count();
}

QVariant ViewsModel::data(const QModelIndex &index, int role) const
{
    auto result = QVariant{};

    if (!d->mViewsData) {
        return result;
    }

    if (!index.isValid()) {
        return result;
    }

    switch(role)
    {
    case Qt::DisplayRole:
        result = d->mViewsData->title(index.row());
        break;
    case ColumnRoles::ImageNameRole:
        result = d->mViewsData->iconUrl(index.row());
        break;
    case ColumnRoles::UseColorOverlayRole:
        result = d->mViewsData->iconUseColorOverlay(index.row());
        break;
    case ColumnRoles::DatabaseIdRole:
        result = d->mViewsData->databaseId(index.row());
        break;
    case ColumnRoles::SecondTitleRole:
        result = d->mViewsData->secondTitle(index.row());
        break;
    case ColumnRoles::UseSecondTitleRole:
        result = d->mViewsData->useSecondTitle(index.row());
        break;
    case ColumnRoles::EntryCategoryRole:
        if (d->mViewsData->defaultEntry(index.row())) {
            result = QStringLiteral("default");
        } else {
            switch (d->mViewsData->embeddedCategory())
            {
            case ElisaUtils::Album:
                result = i18nc("Title of the view of all albums", "Albums");
                break;
            case ElisaUtils::Genre:
                result = i18nc("Title of the view of all genres", "Genres");
                break;
            case ElisaUtils::Artist:
                result = i18nc("Title of the view of all artists", "Artists");
                break;
            case ElisaUtils::Radio:
            case ElisaUtils::Track:
            case ElisaUtils::Unknown:
            case ElisaUtils::Composer:
            case ElisaUtils::FileName:
            case ElisaUtils::Lyricist:
            case ElisaUtils::Container:
                break;
            }
        }

        break;
    }

    return result;
}

QModelIndex ViewsModel::index(int row, int column, const QModelIndex &parent) const
{
    auto result = QModelIndex();

    if (column != 0) {
        return result;
    }

    if (parent.isValid()) {
        return result;
    }

    result = createIndex(row, column);

    return result;
}

QModelIndex ViewsModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    auto result = QModelIndex();

    return result;
}

ViewsListData *ViewsModel::viewsData() const
{
    return d->mViewsData;
}

void ViewsModel::setViewsData(ViewsListData *viewsData)
{
    if (d->mViewsData == viewsData) {
        return;
    }

    if (viewsData) {
        beginResetModel();
    }

    d->mViewsData = viewsData;
    Q_EMIT viewsDataChanged();

    if (d->mViewsData) {
        connect(d->mViewsData, &ViewsListData::dataAboutToBeAdded,
                this, &ViewsModel::dataAboutToBeAdded);
        connect(d->mViewsData, &ViewsListData::dataAdded,
                this, &ViewsModel::dataAdded);
        connect(d->mViewsData, &ViewsListData::dataAboutToBeRemoved,
                this, &ViewsModel::dataAboutToBeRemoved);
        connect(d->mViewsData, &ViewsListData::dataRemoved,
                this, &ViewsModel::dataRemoved);
        connect(d->mViewsData, &ViewsListData::dataModified,
                this, &ViewsModel::dataModified);
        connect(d->mViewsData, &ViewsListData::dataAboutToBeReset,
                this, &ViewsModel::dataAboutToBeReset);
        connect(d->mViewsData, &ViewsListData::dataReset,
                this, &ViewsModel::dataReset);

        endResetModel();
    }
}

void ViewsModel::dataAboutToBeAdded(int startIndex, int lastIndex)
{
    beginInsertRows({}, startIndex, lastIndex);
}

void ViewsModel::dataAdded()
{
    endInsertRows();
}

void ViewsModel::dataAboutToBeRemoved(int startIndex, int lastIndex)
{
    beginRemoveRows({}, startIndex, lastIndex);
}

void ViewsModel::dataRemoved()
{
    endRemoveRows();
}

void ViewsModel::dataModified(int currentIndex)
{
    Q_EMIT dataChanged(index(currentIndex, 0), index(currentIndex, 0));
}

void ViewsModel::dataAboutToBeReset()
{
    beginResetModel();
}

void ViewsModel::dataReset()
{
    endResetModel();
}


#include "moc_viewsmodel.cpp"

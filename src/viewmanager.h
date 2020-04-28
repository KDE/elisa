/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include "elisaLib_export.h"

#include "elisautils.h"
#include "datatypes.h"

#include <QObject>
#include <QUrl>
#include <Qt>

#include <memory>

class ViewManagerPrivate;
class ViewParameters;
class ViewsListData;
class ViewConfigurationData;

class ELISALIB_EXPORT ViewManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int viewIndex
               READ viewIndex
               NOTIFY viewIndexChanged)

    Q_PROPERTY(ViewsListData* viewsData READ viewsData WRITE setViewsData NOTIFY viewsDataChanged)

public:

    enum AlbumCardinality {
        SingleAlbum = true,
        MultipleAlbum = false,
    };

    Q_ENUM(AlbumCardinality)

    enum AlbumViewStyle {
        NoDiscHeaders,
        DiscHeaders,
    };

    Q_ENUM(AlbumViewStyle)

    enum RadioSpecificStyle {
        IsTrack,
        IsRadio,
    };

    Q_ENUM(RadioSpecificStyle)

    enum DelegateUseSecondaryText {
        DelegateWithSecondaryText = true,
        DelegateWithoutSecondaryText = false,
    };

    Q_ENUM(DelegateUseSecondaryText)

    enum ViewCanBeRated {
        ViewShowRating = true,
        ViewHideRating = false,
    };

    Q_ENUM(ViewCanBeRated)

    enum ViewPresentationType {
        ContextView,
        GridView,
        ListView,
        UnknownViewPresentation,
    };

    Q_ENUM(ViewPresentationType)

    enum ModelType {
        GenericDataModel,
        FileBrowserModel,
        UnknownModelType,
    };

    Q_ENUM(ModelType)

    enum IsTreeModelType {
        IsTreeModel = true,
        IsFlatModel = false,
    };

    Q_ENUM(IsTreeModelType)

    explicit ViewManager(QObject *parent = nullptr);

    ~ViewManager() override;

    int viewIndex() const;

    ViewsListData* viewsData() const;

Q_SIGNALS:

    void openGridView(ViewConfigurationData *configurationData);

    void openListView(ViewConfigurationData *configurationData);

    void switchFilesBrowserView(int expectedDepth, const QString &mainTitle, const QUrl &imageUrl);

    void switchContextView(int expectedDepth, const QString &mainTitle, const QUrl &imageUrl);

    void popOneView();

    void viewIndexChanged();

    void viewsDataChanged();

public Q_SLOTS:

    void openView(int viewIndex);

    void openChildView(const DataTypes::MusicDataType &fullData);

    void openAlbumView(const QString &title, const QString &artist, qulonglong databaseId, const QUrl &albumCoverUrl);

    void openArtistView(const QString &artist);

    void viewIsLoaded();

    void goBack();

    void setViewsData(ViewsListData* viewsData);

private:

    void openViewFromData(const ViewParameters &viewParamaters);

private:

    std::unique_ptr<ViewManagerPrivate> d;

};

#endif // VIEWMANAGER_H

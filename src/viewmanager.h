/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include "elisaLib_export.h"

#include "config-upnp-qt.h"

#include "elisautils.h"
#include "datatypes.h"

#include <QObject>
#include <QQmlEngine>
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

    QML_ELEMENT

    Q_PROPERTY(int viewIndex
               READ viewIndex
               NOTIFY viewIndexChanged)

    Q_PROPERTY(int initialIndex
               READ initialIndex
               WRITE setInitialIndex
               NOTIFY initialIndexChanged)

    Q_PROPERTY(QString initialFilesViewPath
               READ initialFilesViewPath
               WRITE setInitialFilesViewPath
               NOTIFY initialFilesViewPathChanged)

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
        TrackView,
        UnknownViewPresentation,
    };

    Q_ENUM(ViewPresentationType)

    enum ModelType {
        GenericDataModel,
        FileBrowserModel,
        UnknownModelType,
    };

    Q_ENUM(ModelType)

    explicit ViewManager(QObject *parent = nullptr);

    ~ViewManager() override;

    [[nodiscard]] int viewIndex() const;

    [[nodiscard]] int initialIndex() const;

    [[nodiscard]] QString initialFilesViewPath() const;

    [[nodiscard]] ViewsListData* viewsData() const;

Q_SIGNALS:

    void openGridView(ViewConfigurationData *configurationData);

    void openTrackView(ViewConfigurationData *configurationData);

    void switchContextView(int expectedDepth, const QString &mainTitle, const QUrl &imageUrl);

    void popOneView();

    void viewIndexChanged();

    void initialIndexChanged();

    void viewsDataChanged();

    void initialFilesViewPathChanged();

public Q_SLOTS:

    void openView(int viewIndex);

    void openChildView(const DataTypes::MusicDataType &fullData);

    void openAlbumView(const QString &title, const QString &artist, qulonglong databaseId, const QUrl &albumCoverUrl);

    void openArtistView(const QString &artist);

    void openNowPlaying();

    void goBack();

    void setViewsData(ViewsListData* viewsData);

    void sortOrderChanged(Qt::SortOrder sortOrder);

    void sortRoleChanged(int sortRole);

    void setInitialIndex(int newIndex);

    void setInitialFilesViewPath(const QString &initialPath);

private Q_SLOTS:

    void openInitialView();

private:

    void openViewFromData(const ViewParameters &viewParamaters);

    void applyFilter(ViewParameters &nextViewParameters,
                     QString title, const ViewParameters &lastView) const;

    [[nodiscard]] QString buildViewId() const;

    QStringList::iterator findViewPreference(QStringList &list, const QString &viewId) const;

    template<typename T>
    T computeViewPreference(const T initialValue, QStringList &preferences) const;

    template<typename T>
    void updateViewPreference(const T newValue, QStringList &preferences) const;

    bool viewHasDefaultSortRole(const ElisaUtils::FilterType filterType) const;

    [[nodiscard]] Qt::SortOrder computePreferredSortOrder(Qt::SortOrder initialValue, ElisaUtils::FilterType filterType) const;

    [[nodiscard]] int computePreferredSortRole(int initialValue, ElisaUtils::FilterType filterType) const;

private:

    std::unique_ptr<ViewManagerPrivate> d;

    bool isSameView(const ViewParameters &currentView, const ViewParameters &otherView) const;
};

#endif // VIEWMANAGER_H

/*
   SPDX-FileCopyrightText: 2018 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef SIMPLEVIEWMANAGER_H
#define SIMPLEVIEWMANAGER_H

#include "elisaLib_export.h"

#include "config-upnp-qt.h"

#include "elisautils.h"
#include "datatypes.h"

#include <QObject>
#include <QUrl>
#include <Qt>

#include <memory>

class ViewParameters;
class ViewsListData;
class ViewConfigurationData;
class SimpleViewManagerPrivate;

class ELISALIB_EXPORT SimpleViewManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(ViewsListData* viewsData READ viewsData WRITE setViewsData NOTIFY viewsDataChanged)

public:

    explicit SimpleViewManager(QObject *parent = nullptr);

    ~SimpleViewManager() override;

    [[nodiscard]] ViewsListData* viewsData() const;

Q_SIGNALS:

    void openGridView(ViewConfigurationData *configurationData);

    void openListView(ViewConfigurationData *configurationData);

    void switchContextView(int expectedDepth, const QString &mainTitle, const QUrl &imageUrl);

    void viewsDataChanged();

public Q_SLOTS:

    void openView(ElisaUtils::PlayListEntryType viewType);

    void openChildView(const DataTypes::MusicDataType &fullData);

    void viewIsLoaded();

    void setViewsData(ViewsListData* viewsData);

private Q_SLOTS:

private:

    void openViewFromData(const ViewParameters &viewParamaters);

    void applyFilter(ViewParameters &nextViewParameters,
                     QString title, const ViewParameters &lastView) const;

    [[nodiscard]] QString buildViewId(const ViewParameters &oneView) const;

    [[nodiscard]] QStringList::iterator findViewPreference(QStringList &list,
                                                           const QString &viewId) const;

    [[nodiscard]] Qt::SortOrder computePreferredSortOrder(const ViewParameters &oneView,
                                                          Qt::SortOrder initialValue) const;

    [[nodiscard]] int computePreferredSortRole(const ViewParameters &oneView,
                                               int initialValue) const;

private:

    std::unique_ptr<SimpleViewManagerPrivate> d;

};

#endif // SIMPLEVIEWMANAGER_H

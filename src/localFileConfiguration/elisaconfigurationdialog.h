/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#if !defined ELISACONFIGURATIONDIALOG_H_
#define ELISACONFIGURATIONDIALOG_H_

#include "elisaLib_export.h"

#include <QStringList>
#include <QFileSystemWatcher>

class ELISALIB_EXPORT ElisaConfigurationDialog : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QStringList rootPath
               READ rootPath
               WRITE setRootPath
               NOTIFY rootPathChanged)

    Q_PROPERTY(bool forceUsageOfFastFileSearch
               READ forceUsageOfFastFileSearch
               WRITE setForceUsageOfFastFileSearch
               NOTIFY forceUsageOfFastFileSearchChanged)

    Q_PROPERTY(bool showProgressInTaskBar
               READ showProgressInTaskBar
               WRITE setShowProgressInTaskBar
               NOTIFY showProgressInTaskBarChanged)

    Q_PROPERTY(bool isDirty
               READ isDirty
               NOTIFY isDirtyChanged)

public:

    explicit ElisaConfigurationDialog(QObject *parent = nullptr);

    ~ElisaConfigurationDialog() override;

    QStringList rootPath() const;

    bool isDirty() const
    {
        return mIsDirty;
    }

    bool showProgressInTaskBar() const
    {
        return mShowProgressInTaskBar;
    }

    bool forceUsageOfFastFileSearch() const
    {
        return mForceUsageOfFastFileSearch;
    }

Q_SIGNALS:

    void rootPathChanged(const QStringList &rootPath);

    void isDirtyChanged();

    void showProgressInTaskBarChanged();

    void forceUsageOfFastFileSearchChanged();

public Q_SLOTS:

    void setRootPath(const QStringList &rootPath);

    void save();

    void setShowProgressInTaskBar(bool showProgressInTaskBar);

    void setForceUsageOfFastFileSearch(bool forceUsageOfFastFileSearch);

private Q_SLOTS:

    void configChanged();

private:

    void setDirty();

    QStringList mRootPath;

    QFileSystemWatcher mConfigFileWatcher;

    bool mIsDirty = false;

    bool mShowProgressInTaskBar = true;
    bool mForceUsageOfFastFileSearch;
};

#endif

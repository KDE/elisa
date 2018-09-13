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

#if !defined LOCALFILECONFIGURATION_H_
#define LOCALFILECONFIGURATION_H_

#include <KQuickAddons/ConfigModule>
#include <QStringList>
#include <QFileSystemWatcher>

class KCMElisaLocalFile : public KQuickAddons::ConfigModule
{

    Q_OBJECT

    Q_PROPERTY(QStringList rootPath
               READ rootPath
               WRITE setRootPath
               NOTIFY rootPathChanged)

public:

    explicit KCMElisaLocalFile(QObject *parent, const QVariantList &args);

    ~KCMElisaLocalFile() override;

    QStringList rootPath() const;

Q_SIGNALS:

    void rootPathChanged(QStringList rootPath);

public Q_SLOTS:

    void defaults() override final;

    void load() override final;

    void save() override final;

    void setRootPath(QStringList rootPath);

private Q_SLOTS:

    void configChanged();

private:

    QStringList mRootPath;

    QFileSystemWatcher mConfigFileWatcher;

};

#endif

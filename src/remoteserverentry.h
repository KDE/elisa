/*
 * Copyright 2015 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef REMOTESERVERENTRY_H
#define REMOTESERVERENTRY_H

#include <QtCore/QObject>
#include <QtCore/QSharedPointer>

class UpnpControlContentDirectory;
class UpnpDeviceDescription;
class UpnpDeviceDescriptionParser;

class RemoteServerEntry : public QObject
{

    Q_OBJECT

    Q_PROPERTY(UpnpControlContentDirectory* contentDirectory
               READ contentDirectory
               WRITE setContentDirectory
               NOTIFY contentDirectoryChanged)

    Q_PROPERTY(bool isReady
               READ isReady
               WRITE setIsReady
               NOTIFY isReadyChanged)

public:

    explicit RemoteServerEntry(QSharedPointer<UpnpDeviceDescription> device,
                               QSharedPointer<UpnpDeviceDescriptionParser> deviceParser,
                               QObject *parent = 0);

    UpnpControlContentDirectory* contentDirectory() const;

    void setContentDirectory(UpnpControlContentDirectory *value);

    bool isReady() const;

    void setIsReady(bool value);

Q_SIGNALS:

    void contentDirectoryChanged();

    void isReadyChanged();

public Q_SLOTS:

private Q_SLOTS:

    void descriptionParsed(const QString &UDN);

private:

    bool mIsReady;

    QSharedPointer<UpnpDeviceDescription> mDevice;

    QSharedPointer<UpnpDeviceDescriptionParser> mDeviceParser;

};

#endif // REMOTESERVERENTRY_H

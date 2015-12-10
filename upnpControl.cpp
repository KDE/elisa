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

#include "config-upnp-qt.h"

#include "upnpssdpengine.h"
#include "upnpabstractservice.h"
#include "upnpcontrolabstractdevice.h"
#include "upnpcontrolabstractservice.h"
#include "upnpdevicemodel.h"
#include "upnpbasictypes.h"
#include "upnpcontrolswitchpower.h"
#include "upnpcontrolconnectionmanager.h"
#include "upnpcontrolmediaserver.h"
#include "upnpcontrolcontentdirectory.h"
#include "upnpcontentdirectorymodel.h"
#include "upnpdevicedescription.h"

#include "mediaplaylist.h"
#include "playlistcontroler.h"
#include "viewpagesmodel.h"
#include "remoteserverentry.h"

#if KF5Declarative_FOUND
#include <KDeclarative/KDeclarative>
#endif

#include <QtWidgets/QApplication>

#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlFileSelector>
#include <QtQml>

int __attribute__((visibility("default"))) main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<UpnpSsdpEngine>("org.mgallien.QmlExtension", 1, 0, "UpnpSsdpEngine");
    qmlRegisterType<UpnpAbstractDevice>("org.mgallien.QmlExtension", 1, 0, "UpnpAbstractDevice");
    qmlRegisterType<UpnpAbstractService>("org.mgallien.QmlExtension", 1, 0, "UpnpAbstractService");
    qmlRegisterType<UpnpControlAbstractDevice>("org.mgallien.QmlExtension", 1, 0, "UpnpControlAbstractDevice");
    qmlRegisterType<UpnpControlAbstractService>("org.mgallien.QmlExtension", 1, 0, "UpnpControlAbstractService");
    qmlRegisterType<UpnpDeviceModel>("org.mgallien.QmlExtension", 1, 0, "UpnpDeviceModel");
    qmlRegisterType<UpnpControlSwitchPower>("org.mgallien.QmlExtension", 1, 0, "UpnpControlSwitchPower");
    qmlRegisterType<UpnpControlConnectionManager>("org.mgallien.QmlExtension", 1, 0, "UpnpControlConnectionManager");
    qmlRegisterType<UpnpControlMediaServer>("org.mgallien.QmlExtension", 1, 0, "UpnpControlMediaServer");
    qmlRegisterType<UpnpControlContentDirectory>("org.mgallien.QmlExtension", 1, 0, "UpnpControlContentDirectory");
    qmlRegisterType<UpnpContentDirectoryModel>("org.mgallien.QmlExtension", 1, 0, "UpnpContentDirectoryModel");
    qmlRegisterType<UpnpDeviceDescription>("org.mgallien.QmlExtension", 1, 0, "UpnpDeviceDescription");

    qmlRegisterType<MediaPlayList>("org.mgallien.QmlExtension", 1, 0, "MediaPlayList");
    qmlRegisterType<PlayListControler>("org.mgallien.QmlExtension", 1, 0, "PlayListControler");
    qmlRegisterType<ViewPagesModel>("org.mgallien.QmlExtension", 1, 0, "ViewPagesModel");

    qRegisterMetaType<A_ARG_TYPE_InstanceID>();
    qRegisterMetaType<QPointer<UpnpAbstractDevice> >();
    qRegisterMetaType<UpnpControlConnectionManager*>();
    qRegisterMetaType<UpnpContentDirectoryModel*>();
    qRegisterMetaType<UpnpDeviceDescription*>();
    qRegisterMetaType<RemoteServerEntry*>();

    QQmlApplicationEngine engine;
    engine.addImportPath(QStringLiteral("qrc:/imports"));
    QQmlFileSelector selector(&engine);

#if KF5Declarative_FOUND
    KDeclarative::KDeclarative decl;
    decl.setDeclarativeEngine(&engine);
    decl.setupBindings();
#endif

    engine.load(QUrl(QStringLiteral("qrc:/MediaServer.qml")));

    app.exec();

    return 0;
}

/*
   SPDX-FileCopyrightText: 2024 (c) Jack Hill <jackhill3103@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef QMLFOREIGNTYPES_H
#define QMLFOREIGNTYPES_H

#include "config-upnp-qt.h"

#if UPNPQT_FOUND
#include "upnpdevicedescription.h"

#include "upnpssdpengine.h"
#include "upnpabstractservice.h"
#include "upnpcontrolabstractdevice.h"
#include "upnpcontrolabstractservice.h"
#include "upnpbasictypes.h"
#endif

#include <QAbstractProxyModel>
#include <QQmlEngine>

struct ForeignQAbstractItemModel
{
    Q_GADGET
    QML_FOREIGN(QAbstractItemModel)
    QML_NAMED_ELEMENT(AbstractItemModel)
    QML_UNCREATABLE("")
};

struct ForeignQAbstractProxyModel
{
    Q_GADGET
    QML_FOREIGN(QAbstractProxyModel)
    QML_NAMED_ELEMENT(AbstractProxyModel)
    QML_UNCREATABLE("")
};

#if UPNPQT_FOUND
struct ForeignUpnpSsdpEngine
{
    Q_GADGET
    QML_FOREIGN(UpnpSsdpEngine)
    QML_ELEMENT
};

struct ForeignUpnpAbstractDevice
{
    Q_GADGET
    QML_FOREIGN(UpnpAbstractDevice)
    QML_ELEMENT
};

struct ForeignUpnpAbstractService
{
    Q_GADGET
    QML_FOREIGN(UpnpAbstractService)
    QML_ELEMENT
};

struct ForeignUpnpControlAbstractDevice
{
    Q_GADGET
    QML_FOREIGN(UpnpControlAbstractDevice)
    QML_ELEMENT
};

struct ForeignUpnpControlAbstractService
{
    Q_GADGET
    QML_FOREIGN(UpnpControlAbstractService)
    QML_ELEMENT
};

struct ForeignUpnpDeviceDescription
{
    Q_GADGET
    QML_FOREIGN(UpnpDeviceDescription)
    QML_ELEMENT
};
#endif

#endif // QMLFOREIGNTYPES_H

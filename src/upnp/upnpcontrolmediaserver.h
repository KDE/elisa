/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef UPNPCONTROLMEDIASERVER_H
#define UPNPCONTROLMEDIASERVER_H

#include "upnpcontrolabstractdevice.h"

#include <memory>


class UpnpControlMediaServerPrivate;

class UpnpControlMediaServer : public UpnpControlAbstractDevice
{

    Q_OBJECT

    Q_PROPERTY(bool hasAVTransport
               READ hasAVTransport
               NOTIFY hasAVTransportChanged)

public:
    explicit UpnpControlMediaServer(QObject *parent = nullptr);

    ~UpnpControlMediaServer() override;

    [[nodiscard]] bool hasAVTransport() const;

Q_SIGNALS:

    void hasAVTransportChanged();

protected:

    //void parseDeviceDescription(QIODevice *deviceDescriptionContent, const QString &fallBackURLBase) override;

private:

    std::unique_ptr<UpnpControlMediaServerPrivate> d;

};

#endif // UPNPCONTROLMEDIASERVER_H

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

#include "upnpcontrolavtransport.h"

#include <QDebug>

UpnpControlAVTransport::UpnpControlAVTransport(QObject *parent) : UpnpControlAbstractService(parent)
{

}

UpnpControlAVTransport::~UpnpControlAVTransport()
{

}

void UpnpControlAVTransport::SetAVTransportURI(quint32 InstanceID, const AVTransportURI &CurrentURI, const AVTransportURIMetaData &CurrentURIMetaData)
{
    callAction(QStringLiteral("SetAVTransportURI"), {InstanceID, CurrentURI, CurrentURIMetaData});
}

void UpnpControlAVTransport::SetNextAVTransportURI(quint32 InstanceID, const AVTransportURI &NextURI, const AVTransportURIMetaData &NextURIMetaData)
{
    callAction(QStringLiteral("SetNextAVTransportURI"), {InstanceID, NextURI, NextURIMetaData});
}

void UpnpControlAVTransport::GetMediaInfo(quint32 InstanceID)
{
    callAction(QStringLiteral("GetMediaInfo"), {InstanceID});
}

void UpnpControlAVTransport::GetTransportInfo(quint32 InstanceID)
{
    callAction(QStringLiteral("GetTransportInfo"), {InstanceID});
}

void UpnpControlAVTransport::GetPositionInfo(quint32 InstanceID)
{
    callAction(QStringLiteral("GetPositionInfo"), {InstanceID});
}

void UpnpControlAVTransport::GetDeviceCapabilities(quint32 InstanceID)
{
    callAction(QStringLiteral("GetDeviceCapabilities"), {InstanceID});
}

void UpnpControlAVTransport::GetTransportSettings(quint32 InstanceID)
{
    callAction(QStringLiteral("GetTransportSettings"), {InstanceID});
}

void UpnpControlAVTransport::Stop(quint32 InstanceID)
{
    qDebug() << "UpnpControlAVTransport::Stop";

    callAction(QStringLiteral("Stop"), {InstanceID});
}

void UpnpControlAVTransport::Play(quint32 InstanceID, QString Speed)
{
    callAction(QStringLiteral("Play"), {InstanceID, Speed});
}

void UpnpControlAVTransport::Pause(quint32 InstanceID)
{
    callAction(QStringLiteral("Pause"), {InstanceID});
}

void UpnpControlAVTransport::Record(quint32 InstanceID)
{
    callAction(QStringLiteral("Record"), {InstanceID});
}

void UpnpControlAVTransport::Seek(quint32 InstanceID, const A_ARG_TYPE_SeekMode &Unit, const A_ARG_TYPE_SeekTarget &Target)
{
    callAction(QStringLiteral("Seek"), {InstanceID, Unit, Target});
}

void UpnpControlAVTransport::Next(quint32 InstanceID)
{
    callAction(QStringLiteral("Next"), {InstanceID});
}

void UpnpControlAVTransport::Previous(quint32 InstanceID)
{
    callAction(QStringLiteral("Previous"), {InstanceID});
}

void UpnpControlAVTransport::SetPlayMode(quint32 InstanceID, const QString &NewPlayMode)
{
    callAction(QStringLiteral("SetPlayMode"), {InstanceID, NewPlayMode});
}

void UpnpControlAVTransport::SetRecordQualityMode(quint32 InstanceID, const QString &NewRecordQualityMode)
{
    callAction(QStringLiteral("SetRecordQualityMode"), {InstanceID, NewRecordQualityMode});
}

void UpnpControlAVTransport::GetCurrentTransportActions(quint32 InstanceID)
{
    callAction(QStringLiteral("GetCurrentTransportActions"), {InstanceID});
}

#include "moc_upnpcontrolavtransport.cpp"

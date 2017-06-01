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

#ifndef UPNPCONTROLAVTRANSPORT_H
#define UPNPCONTROLAVTRANSPORT_H

#include "upnpQt_export.h"

#include "upnpcontrolabstractservice.h"
#include "upnpbasictypes.h"

class UpnpControlAVTransport : public UpnpControlAbstractService
{
    Q_OBJECT
public:
    explicit UpnpControlAVTransport(QObject *parent = 0);

    virtual ~UpnpControlAVTransport();

public Q_SLOTS:

    void SetAVTransportURI(quint32 InstanceID, const AVTransportURI &CurrentURI, const AVTransportURIMetaData &CurrentURIMetaData);

    void SetNextAVTransportURI(quint32 InstanceID, const AVTransportURI &NextURI, const AVTransportURIMetaData &NextURIMetaData);

    void GetMediaInfo(quint32 InstanceID);

    void GetTransportInfo(quint32 InstanceID);

    void GetPositionInfo(quint32 InstanceID);

    void GetDeviceCapabilities(quint32 InstanceID);

    void GetTransportSettings(quint32 InstanceID);

    void Stop(quint32 InstanceID);

    void Play(quint32 InstanceID, QString Speed);

    void Pause(quint32 InstanceID);

    void Record(quint32 InstanceID);

    void Seek(quint32 InstanceID, const A_ARG_TYPE_SeekMode &Unit, const A_ARG_TYPE_SeekTarget &Target);

    void Next(quint32 InstanceID);

    void Previous(quint32 InstanceID);

    void SetPlayMode(quint32 InstanceID, const QString &NewPlayMode);

    void SetRecordQualityMode(quint32 InstanceID, const QString &NewRecordQualityMode);

    void GetCurrentTransportActions(quint32 InstanceID);

Q_SIGNALS:

    void SetAVTransportURIFinished();

    void SetNextAVTransportURIFinished();

    void GetMediaInfoFinished();

    void GetTransportInfoFinished();

    void GetPositionInfoFinished();

    void GetDeviceCapabilitiesFinished();

    void GetTransportSettingsFinished();

    void StopFinished();

    void PlayFinished();

    void PauseFinished();

    void RecordFinished();

    void SeekFinished();

    void NextFinished();

    void PreviousFinished();

    void SetPlayModeFinished();

    void SetRecordQualityModeFinished();

    void GetCurrentTransportActionsFinished();

};

#endif // UPNPCONTROLAVTRANSPORT_H

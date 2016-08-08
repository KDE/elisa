/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "baloolistener.h"

#include "localbaloofilelisting.h"
#include "abstractalbummodel.h"

#include <QtCore/QThread>

class BalooListenerPrivate
{
public:

    QThread mBalooQueryThread;

    LocalBalooFileListing mFileListing;

    AbstractAlbumModel* mModel = nullptr;

};

BalooListener::BalooListener(QObject *parent) : QObject(parent), d(new BalooListenerPrivate)
{
    d->mBalooQueryThread.start();
    d->mFileListing.moveToThread(&d->mBalooQueryThread);
}

BalooListener::~BalooListener()
{
    delete d;
}

AbstractAlbumModel *BalooListener::model() const
{
    return d->mModel;
}

void BalooListener::setModel(AbstractAlbumModel *model)
{
    if (d->mModel == model) {
        return;
    }

    if (d->mModel) {
        disconnect(d->mModel);
    }

    d->mModel = model;

    if (d->mModel) {
        connect(this, &BalooListener::refreshContent, &d->mFileListing, &LocalBalooFileListing::refreshContent, Qt::QueuedConnection);
        connect(&d->mFileListing, &LocalBalooFileListing::tracksList, d->mModel, &AbstractAlbumModel::tracksList);

        QMetaObject::invokeMethod(&d->mFileListing, "init", Qt::QueuedConnection);

        Q_EMIT refreshContent();
    }

    emit modelChanged();
}

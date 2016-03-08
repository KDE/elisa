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

#include "localbaloofilelisting.h"

#include <Baloo/Query>
#include <Baloo/File>

#include <KFileMetaData/Properties>

#include <QtCore/QThread>
#include <QtCore/QDebug>

class LocalBalooFileListingPrivate
{
public:

    Baloo::Query mQuery;

};

LocalBalooFileListing::LocalBalooFileListing(QObject *parent) : QObject(parent), d(new LocalBalooFileListingPrivate)
{
    d->mQuery.addType(QStringLiteral("Audio"));
}

LocalBalooFileListing::~LocalBalooFileListing()
{
}

void LocalBalooFileListing::refreshContent()
{
    auto resultIterator = d->mQuery.exec();



    while(resultIterator.next()) {
        Baloo::File match(resultIterator.filePath());
        match.load();

        qDebug() << "LocalBalooFileListing::refreshContent";

        const auto &allProperties = match.properties();

        auto titleProperty = allProperties.find(KFileMetaData::Property::Title);
        auto durationProperty = allProperties.find(KFileMetaData::Property::Duration);
        auto authorProperty = allProperties.find(KFileMetaData::Property::Author);
        auto albumProperty = allProperties.find(KFileMetaData::Property::Album);

        if (titleProperty != allProperties.end()) {
            qDebug() << "title" << *titleProperty;
        }

        if (durationProperty != allProperties.end()) {
            qDebug() << "duration" << *durationProperty;
        }

        if (authorProperty != allProperties.end()) {
            qDebug() << "author" << *authorProperty;
        }

        if (albumProperty != allProperties.end()) {
            qDebug() << "album" << *albumProperty;
        }
    }
}


#include "moc_localbaloofilelisting.cpp"

/*
 * Copyright 2017 Alexander Stippich <a.stippich@gmx.net>
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef ALLARTISTSPROXYMODEL_H
#define ALLARTISTSPROXYMODEL_H

#include "elisaLib_export.h"

#include "abstractmediaproxymodel.h"
#include "elisautils.h"

class ELISALIB_EXPORT AllArtistsProxyModel : public AbstractMediaProxyModel
{
    Q_OBJECT

    Q_PROPERTY(QString genreFilterText
               READ genreFilterText
               WRITE setGenreFilterText
               NOTIFY genreFilterTextChanged)

public:

    explicit AllArtistsProxyModel(QObject *parent = nullptr);

    ~AllArtistsProxyModel() override;

    QString genreFilterText() const
    {
        return mGenreFilterText;
    }

Q_SIGNALS:

    void artistToEnqueue(QList<QString> artistNames,
                         ElisaUtils::PlayListEnqueueMode enqueueMode,
                         ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void genreFilterTextChanged(QString genreFilterText);

public Q_SLOTS:

    void enqueueToPlayList();

    void replaceAndPlayOfPlayList();

    void setGenreFilterText(const QString &filterText);

protected:

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:

    QString mGenreFilterText;

};


#endif // ALLARTISTSPROXYMODEL_H

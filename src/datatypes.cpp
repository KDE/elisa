/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#include "datatypes.h"


#include "moc_datatypes.cpp"
QDebug operator<<(QDebug debug, const DataTypes::MusicDataType &c)
{
    QDebugStateSaver saver(debug);
    if (c.isValid()) {
        debug.nospace() << "database id: " << c.databaseId().value() << "element type" << c.elementType().value();
    } else {
        debug.nospace() << "invalid";
    }
    return debug;
}

/*
   SPDX-FileCopyrightText: 2020 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef ELISAARGUMENTS_H
#define ELISAARGUMENTS_H

#include "datatypes.h"

#include <QObject>

class ElisaArguments : public QObject
{
    Q_OBJECT

    Q_PROPERTY(DataTypes::EntryDataList arguments READ arguments NOTIFY argumentsChanged)
public:

    ElisaArguments(QObject *parent = nullptr);

    [[nodiscard]] DataTypes::EntryDataList arguments() const
    {
        return mArguments;
    }

Q_SIGNALS:

    void argumentsChanged();

public Q_SLOTS:

    void setArguments(DataTypes::EntryDataList arguments)
    {
        if (mArguments == arguments) {
            return;
        }

        mArguments = std::move(arguments);
        Q_EMIT argumentsChanged();
    }
private:

    DataTypes::EntryDataList mArguments;
};



#endif // ELISAARGUMENTS_H

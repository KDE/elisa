/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef PROGRESSINDICATOR_H
#define PROGRESSINDICATOR_H

#include "elisaLib_export.h"

#include <QObject>

class ELISALIB_EXPORT ProgressIndicator : public QObject
{

    Q_OBJECT

    Q_PROPERTY(int position
               READ position
               WRITE setPosition
               NOTIFY positionChanged)

    Q_PROPERTY(QString progressDuration
               READ progressDuration
               NOTIFY progressDurationChanged)

public:

    explicit ProgressIndicator(QObject *parent = nullptr);

    ~ProgressIndicator() override;

    [[nodiscard]] int position() const;

    [[nodiscard]] QString progressDuration() const;

Q_SIGNALS:

    void positionChanged();

    void progressDurationChanged();

public Q_SLOTS:

    void setPosition(int position);

private:

    int mPosition = 0;

    QString mProgressDuration;

};

#endif // PROGRESSINDICATOR_H

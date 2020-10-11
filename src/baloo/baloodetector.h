/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef BALOODETECTOR_H
#define BALOODETECTOR_H

#include <QObject>
#include <QDBusServiceWatcher>

class BalooDetector : public QObject
{

    Q_OBJECT

    Q_PROPERTY(bool balooAvailability
               READ balooAvailability
               NOTIFY balooAvailabilityChanged)

public:

    explicit BalooDetector(QObject *parent = nullptr);

    [[nodiscard]] bool balooAvailability() const;

Q_SIGNALS:

    void balooAvailabilityChanged();

public Q_SLOTS:

    void checkBalooAvailability();

private Q_SLOTS:

    void serviceRegistered(const QString &service);

    void serviceUnregistered(const QString &service);

private:

    bool checkBalooConfiguration();

    bool checkBalooServiceIsRunning();

    void signalBalooAvailability(bool isNotAvailable);

    QDBusServiceWatcher mBalooWatcher;

    bool mBalooAvailabilityFirstChange = true;

    bool mBalooAvailability = false;

};

#endif // BALOODETECTOR_H

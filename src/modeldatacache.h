#ifndef MODELDATACACHE_H
#define MODELDATACACHE_H

#include "elisautils.h"

#include <QObject>

class ModelDataCache : public QObject
{

    Q_OBJECT

public:

    explicit ModelDataCache(QObject *parent = nullptr);

Q_SIGNALS:

public Q_SLOTS:

    void neededData(qulonglong databaseId) const;

};

#endif // MODELDATACACHE_H

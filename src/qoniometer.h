/*
 * Copyright 2017 Martin Sandsmark <martin.sandsmark@kde.org>
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

#ifndef QONIOMETER_H
#define QONIOMETER_H

#include <QQuickPaintedItem>
#include <QMultiMap>
#include <QImage>
#include "audiowrapper.h"

class QOniometer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(AudioWrapper *player WRITE setPlayer MEMBER m_wrapper NOTIFY playerChanged)

public:
    enum Effect {
        Dots,
        Lines,
        Splines,
        Colors,
        EffectCount
    };
    Q_ENUM(Effect)

    QOniometer(QQuickItem *parent = 0);
    virtual void paint(QPainter *painter) override;
    ~QOniometer();

    void setPlayer(AudioWrapper *player);

    Q_INVOKABLE void selectNextEffect();

Q_SIGNALS:
    void playerChanged();


private Q_SLOTS:
    void onAudioBufferAvailable(const QAudioBuffer &buffer);

protected:
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

private:
    void doDots(QPainter *painter);
    void doLines(QPainter *painter);
    void doColors(QPainter *painter);
    void doSplines(QPainter *painter);

    Effect m_currentEffect;
    AudioWrapper *m_wrapper;
    QAudioBuffer m_lastBuffer;
    int m_bufferPos;
    QVector<float> m_leftBuffer;
    QVector<float> m_rightBuffer;
    QImage m_backBuffer;
};

#endif // QONIOMETER_H

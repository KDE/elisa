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
//protected:
//    virtual void resizeEvent(QResizeEvent*);
//    virtual void keyPressEvent(QKeyEvent *);

private:
    void doDots(QPainter *painter);
    void doLines(QPainter *painter);
    void doColors(QPainter *painter);
    void doSplines(QPainter *painter);

//    PulseAudioMonitor m_monitor;
    Effect m_currentEffect;
    int m_ghost;
    AudioWrapper *m_wrapper;
    QAudioBuffer m_lastBuffer;
    int m_bufferPos;
    QVector<float> m_leftBuffer;
    QVector<float> m_rightBuffer;
    QImage m_backBuffer;
};

#endif // QONIOMETER_H

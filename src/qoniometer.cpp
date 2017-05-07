#include "qoniometer.h"
#include <QDebug>
#include <qmath.h>
#include <QPainter>
#include <QElapsedTimer>
#include <QMouseEvent>
#include <QAudioProbe>
#include <QTimer>
#include <QFileInfo>
//#include <5.8.0/QtCore/private/qringbuffer_p.h>
#include "inlinehsv.h"

QOniometer::QOniometer(QQuickItem *parent)
    : QQuickPaintedItem(parent),
      m_currentEffect(Colors),
//      m_currentEffect(Dots),
      m_ghost(8),
      m_wrapper(nullptr),
      m_bufferPos(0)
{
//    setWindowFlags(Qt::Dialog);
//    resize(640, 480);
    QTimer *repaintTimer = new QTimer(this);
    repaintTimer->setInterval(50);
    //repaintTimer->setInterval(50);
    connect(repaintTimer, SIGNAL(timeout()), this, SLOT(update()));
    repaintTimer->start();

    setRenderTarget(QQuickPaintedItem::FramebufferObject);

//    m_monitor.start();
}

QOniometer::~QOniometer()
{
}

void QOniometer::setPlayer(AudioWrapper *player)
{
    if (m_wrapper) {
        disconnect(m_wrapper, 0, this, 0);
    }
    m_wrapper = player;
    if (m_wrapper) {
        connect(m_wrapper, &AudioWrapper::audioBufferAvailable, this, &QOniometer::onAudioBufferAvailable);
    }
}

void QOniometer::selectNextEffect()
{
    m_currentEffect = Effect((m_currentEffect + 1) % EffectCount);
}

void QOniometer::onAudioBufferAvailable(const QAudioBuffer &buffer)
{
    if (buffer.format().channelCount() != 2) {
        qDebug() << buffer.format();
        return;
    }

    const int numSamples = buffer.sampleCount() / 2;
    QVector<float> left(numSamples);
    QVector<float> right(numSamples);
    switch(buffer.format().sampleType()) {
    case QAudioFormat::Float: {
        for (int j=0, i=0; i<buffer.sampleCount() - 1; i+=2) {
            left[j] = buffer.data<float>()[i];
            right[j] = buffer.data<float>()[i+1];
            j++;
        }
        break;
    }
    case QAudioFormat::UnSignedInt: {
        if (buffer.format().sampleSize() == 16) {
            for (int j=0, i=0; i<buffer.sampleCount() - 1; i+=2) {
                left[j] = buffer.data<quint16>()[i] / 65536.f;
                right[j] = buffer.data<quint16>()[i+1] / 65536.f;
                j++;
            }
        } else if (buffer.format().sampleSize() == 8) {
            for (int j=0, i=0; i<buffer.sampleCount() - 1; i+=2) {
                left[j] = buffer.data<quint8>()[i] / 255.f;
                right[j] = buffer.data<quint8>()[i+1] / 255.f;
                j++;
            }
        }
        break;
    }
    case QAudioFormat::SignedInt: {
        if (buffer.format().sampleSize() == 16) {
            for (int j=0, i=0; i<buffer.sampleCount() - 1; i+=2) {
                left[j] = buffer.data<qint16>()[i] / 65536.f;
                right[j] = buffer.data<qint16>()[i+1] / 65536.f;
                j++;
            }
        } else if (buffer.format().sampleSize() == 8) {
            for (int j=0, i=0; i<buffer.sampleCount() - 1; i+=2) {
                left[j] = buffer.data<quint8>()[i] / 128.f;
                right[j] = buffer.data<quint8>()[i+1] / 128.f;
                j++;
            }
        }
        break;
    }
    default:
        qDebug() << buffer.format();
        return;
    }

    m_leftBuffer = left;
    m_rightBuffer = right;
}

void QOniometer::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    m_backBuffer = QImage(newGeometry.toAlignedRect().size(), QImage::Format_ARGB32_Premultiplied);
    m_backBuffer.fill(Qt::transparent);

    QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);
}

void QOniometer::paint(QPainter *painter)
{
    QImage oldBuffer = m_backBuffer;
    m_backBuffer.fill(Qt::transparent);

    QPainter bufferPainter(&m_backBuffer);
    bufferPainter.setOpacity(0.9);
    bufferPainter.drawImage(0, 0, oldBuffer);
    bufferPainter.setOpacity(1);

    switch(m_currentEffect) {
    case Dots:
        doDots(&bufferPainter);
        break;
    case Lines:
        doLines(&bufferPainter);
        break;
    case Splines:
        doSplines(&bufferPainter);
        break;
    case Colors:
        doColors(&bufferPainter);
        break;
    default:
        break;
    }
    bufferPainter.end();

    painter->drawImage(0, 0, m_backBuffer);
}

//void QOniometer::keyPressEvent(QKeyEvent *event)
//{
//    if (event->key() == Qt::Key_Escape) {
//        close();
//    } else if (event->key() == Qt::Key_Space) {
//        m_currentEffect = Effect((m_currentEffect + 1) % EffectCount);
//    } else if (event->key() == Qt::Key_Up) {
//        m_ghost = qMin(m_ghost + 10, 255);
//    } else if (event->key() == Qt::Key_Down) {
//        m_ghost = qMax(m_ghost - 10, 0);
//    }
//}

void QOniometer::doDots(QPainter *painter)
{
    QElapsedTimer timer;
    timer.start();

    //QImage newBuf(size(), QImage::Format_ARGB32_Premultiplied);
    //newBuf.fill(Qt::transparent);
    //QPainter painter(&newBuf);
    //painter.setCompositionMode(QPainter::CompositionMode_Source);
//    painter->fillRect(0, 0, width(), height(), QColor(0, 0, 0, m_ghost));
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(QColor(255, 255, 255 ), 10, Qt::SolidLine, Qt::RoundCap));

    //float lastX, lastY;
//    gmringbuf *ringBuffer = m_monitor.getBuf();
//    for (size_t i=1; i<gmrb_read_space(ringBuffer); i++) {
//        float left, right;
//        if (gmrb_read_one(ringBuffer, &left, &right)) {
//            break;
//        }
    const int centerX = width() / 2;
    const int centerY = height() / 2;
    const int scale = qMin(height(), width());
//    m_monitor.m_mutex.lock();
//    const int samplecount = sizeof(m_monitor.m_left)/sizeof(m_monitor.m_left[0]);
    const int samplecount = m_rightBuffer.size();
    for (int i=0; i<samplecount; i++) {
        const float left = m_leftBuffer[i];
        const float right = m_rightBuffer[i];


        const float ay = left - right;
        const float ax = left + right;
        const float x = centerX - ax * scale;
        const float y = centerY - ay * height() * 1.3;
        if (i > 1) {
//            painter.drawLine(lastX, lastY, x, y);
            painter->drawPoint(x, y);
        }
        //lastX = x;
        //lastY = y;
    }
//    m_monitor.m_mutex.unlock();
    //painter.end();
    //QPainter backPainter(this);
    //backPainter.fillRect(rect(), QColor(0, 0, 0, 128));
    //backPainter.drawImage(0, 0, newBuf);
//    m_monitor.releaseBuf();

}

void QOniometer::doLines(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(QColor(255, 255, 255 ), 5));
//    painter->fillRect(0, 0, width(), height(), QColor(0, 0, 0, m_ghost));

    float lastX, lastY;
    const int centerX = width() / 2;
    const int centerY = height() / 2;
    const int scale = qMin(height(), width());
//    m_monitor.m_mutex.lock();
//    const int samplecount = sizeof(m_monitor.m_left)/sizeof(m_monitor.m_left[0]);
    const int samplecount = m_rightBuffer.size();
    for (int i=0; i<samplecount; i++) {
//        const float left = m_monitor.m_left[i];
//        const float right = m_monitor.m_right[i];
        const float left = m_leftBuffer[i];
        const float right = m_rightBuffer[i];


        const float ay = left - right;
        const float ax = left + right;
        const float x = centerX - ax * scale;
        const float y = centerY - ay * scale;
        if (i > 1) {
            painter->setPen(QPen(QColor(255, 255, 255, 32), 10 - 10 * qMax(qAbs(ax), qAbs(ay))));
            painter->drawLine(lastX, lastY, x, y);
        }
        lastX = x;
        lastY = y;
    }
//    m_monitor.m_mutex.unlock();
}

void QOniometer::doColors(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(QColor(255, 255, 255), 5));
//    painter->fillRect(rect(), QColor(0, 0, 0, m_ghost));
//    painter->fillRect(0, 0, width(), height(), QColor(0, 0, 0, m_ghost));

    float lastX, lastY;
    const int centerX = width() / 2;
    const int centerY = height() / 2;
    const int scale = qMax(height(), width());
//    m_monitor.m_mutex.lock();
    QPen pen;
//    pen.setWidth(30);
//    pen.setColor(QColor(255, 255, 255, 255));
    pen.setCapStyle(Qt::RoundCap);
    InlineHSV hsv;
    hsv.setRGB(255, 255, 255);
//    const int sampleCount = sizeof(m_monitor.m_left)/sizeof(m_monitor.m_left[0]);
    const int sampleCount = m_rightBuffer.size();
    for (int i=0; i<sampleCount; i++) {
        const float left = m_leftBuffer[i];
        const float right = m_rightBuffer[i];

//        const float leftNext = m_monitor.m_left[i+1];
//        const float rightNext = m_monitor.m_right[i+1];

        const float ay = left - right;
        const float ax = left + right;
        const float x = centerX - qSqrt(qAbs(ax)) * scale * ax/qAbs(ax);
        const float y = centerY - qSqrt(qAbs(ay)) * scale * ay/qAbs(ay);

        //const float x = centerX - ax * scale;
        //const float y = centerY - ay * scale;

        //const float x = centerX - scale / ax;
        //const float y = centerY - scale / ay;
        //const float y = centerY - ay * height() * 2.;
        if (i > 1) {
            const float incre = i * 359 / sampleCount;
            const float speed = qBound(0.f, (qAbs(ay - lastY) + qAbs(ax - lastX)) + 0.75f, 1.f);
            const float speed2 = qBound(0.f, (qAbs(ay - lastY) + qAbs(ax - lastX)), 1.f);
            //const float tres = sin(ax) * cos(speed2);
            const float distance = 1.0f  - qBound(0.f, (qAbs(ay) + qAbs(ax)), 1.f);
            //const float distance = 1. - qBound(0., qSqrt((ax + 1.) * (ax + 1.) + (ay + 1.) * (ay + 1.)), 1.);
            hsv.convertHSV2RGB(incre, speed * 128.f + 128.f, qPow(distance, 1.5) * speed * 255.f);
            //hsv.convertHSV2RGB(incre, speed * 128.f + 128.f, speed * 192.f + 64.f);
//            hsv.convertHSV2RGB(incre, speed, speed2);
            pen.setColor(qRgb(hsv.red(), hsv.green(), hsv.blue()));
            //pen.setColor(qRgba(hsv.red(), hsv.green(), hsv.blue(), 255 - 255 * distance));
//            pen.setColor(QColor::fromHsv(incre, speed, speed2, 128));

//            pen.setColor(QColor::fromHsv(qBound(0., distance * 359.0, 359.), speed, speed2, 128));
//            pen.setColor(QColor::fromHsv(, 255, qBound(0.f, qAbs(y - lastY) + qAbs(x - lastX), 255.f)));
//            pen.setColor(QColor::fromHsv(qAbs(ay) * 255, qAbs(ax) * 255, 255.0 * qSqrt(ax * ax + ay*ay)));
//            pen.setWidth(qMax(50.0 * qSqrt(ax * ax + ay*ay), 1.));
            pen.setWidth(qBound(1.f, speed2 * scale / 10.f, 30.f));
//            pen.setWidth(qBound(1.f, qAbs(ax - lastX) * scale + qAbs(ay - lastY) * scale / 100.f, 30.f));
            painter->setPen(pen);
//            painter.drawPoint(x, y);
            painter->drawPoint(x, y);
//            painter.drawLine(lastX, lastY, x, y);
        }
        lastX = ax;
        lastY = ay;
    }
//    m_monitor.m_mutex.unlock();
}

void QOniometer::doSplines(QPainter *painter)
{
//    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(QColor(255, 255, 255, 255 ), 1));
//    painter->fillRect(rect(), QColor(0, 0, 0, m_ghost));
//    painter->fillRect(0, 0, width(), height(), QColor(0, 0, 0, m_ghost));

    const int centerX = width() / 2;
    const int centerY = height() / 2;
    const int scale = qMin(height(), width()) * 1.5;
//    m_monitor.m_mutex.lock();
    QPainterPath path;
    path.moveTo(centerX, centerY);
//    const int samplecount = sizeof(m_monitor.m_left)/sizeof(m_monitor.m_left[0]);
    const int sampleCount = m_rightBuffer.size();
    for (int i=0; i<sampleCount-1; i+=2) {
        const float left = m_leftBuffer[i];
        const float right = m_rightBuffer[i];
        const float leftNext = m_leftBuffer[i+1];
        const float rightNext = m_rightBuffer[i+1];


        const float ax = left + right;
        const float ay = left - right;
//        const float x = centerX - ax * width();
        const float x = centerX - ax * scale;
        const float y = centerY - ay * scale;
//        const float y = centerY - ay * height() * 1.1;

        const float axNext = leftNext + rightNext;
        const float ayNext = leftNext - rightNext;
        const float xNext = centerX - axNext * scale;
        const float yNext = centerY - ayNext * scale;

        path.quadTo(x, y, xNext, yNext);
    }
    painter->drawPath(path);
//    m_monitor.m_mutex.unlock();
}

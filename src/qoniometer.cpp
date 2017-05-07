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
      m_wrapper(nullptr),
      m_bufferPos(0)
{
    QTimer *repaintTimer = new QTimer(this);
    repaintTimer->setInterval(50);
    connect(repaintTimer, SIGNAL(timeout()), this, SLOT(update()));
    repaintTimer->start();

    setRenderTarget(QQuickPaintedItem::FramebufferObject);
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

void QOniometer::doDots(QPainter *painter)
{
    QElapsedTimer timer;
    timer.start();

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(QColor(255, 255, 255 ), 10, Qt::SolidLine, Qt::RoundCap));

    const int centerX = width() / 2;
    const int centerY = height() / 2;
    const int scale = qMin(height(), width()) * 1.5;
    const int samplecount = m_rightBuffer.size();
    for (int i=0; i<samplecount; i++) {
        const float left = m_leftBuffer[i];
        const float right = m_rightBuffer[i];


        const float ay = left - right;
        const float ax = left + right;
        const float x = centerX - ax * scale;
        const float y = centerY - ay * height() * 1.3;
        if (i > 1) {
            painter->drawPoint(x, y);
        }
    }
}

void QOniometer::doLines(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(QColor(255, 255, 255 ), 5));

    float lastX, lastY;
    const int centerX = width() / 2;
    const int centerY = height() / 2;
    const int scale = qMin(height(), width()) * 1.5;
    const int samplecount = m_rightBuffer.size();
    for (int i=0; i<samplecount; i++) {
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
}

void QOniometer::doColors(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(QColor(255, 255, 255), 5));

    float lastX, lastY;
    const int centerX = width() / 2;
    const int centerY = height() / 2;
    const int scale = qMax(height(), width()) / 1.5;
    QPen pen;
    pen.setCapStyle(Qt::RoundCap);
    InlineHSV hsv;
    hsv.setRGB(255, 255, 255);
    const int sampleCount = m_rightBuffer.size();
    for (int i=0; i<sampleCount; i++) {
        const float left = m_leftBuffer[i];
        const float right = m_rightBuffer[i];

        const float ay = left - right;
        const float ax = left + right;
        const float x = centerX - qSqrt(qAbs(ax)) * scale * ax/qAbs(ax);
        const float y = centerY - qSqrt(qAbs(ay)) * scale * ay/qAbs(ay);

        if (i > 1) {
            const float incre = i * 359 / sampleCount;
            const float speed = qBound(0.f, (qAbs(ay - lastY) + qAbs(ax - lastX)) + 0.75f, 1.f);
            const float speed2 = qBound(0.f, (qAbs(ay - lastY) + qAbs(ax - lastX)), 1.f);
            const float distance = 1.0f  - qBound(0.f, (qAbs(ay) + qAbs(ax)), 1.f);
            hsv.convertHSV2RGB(incre, speed * 128.f + 128.f, qPow(distance, 1.5) * speed * 255.f);
            pen.setColor(qRgb(hsv.red(), hsv.green(), hsv.blue()));

            pen.setWidth(qBound(1.f, speed2 * scale / 10.f, 30.f));
            painter->setPen(pen);
            painter->drawPoint(x, y);
        }
        lastX = ax;
        lastY = ay;
    }
}

void QOniometer::doSplines(QPainter *painter)
{
    painter->setPen(QPen(QColor(255, 255, 255, 255 ), 1));

    const int centerX = width() / 2;
    const int centerY = height() / 2;
    const int scale = qMin(height(), width()) * 1.5;
    QPainterPath path;
    path.moveTo(centerX, centerY);
    const int sampleCount = m_rightBuffer.size();
    for (int i=0; i<sampleCount-1; i+=2) {
        const float left = m_leftBuffer[i];
        const float right = m_rightBuffer[i];
        const float leftNext = m_leftBuffer[i+1];
        const float rightNext = m_rightBuffer[i+1];


        const float ax = left + right;
        const float ay = left - right;
        const float x = centerX - ax * scale;
        const float y = centerY - ay * scale;

        const float axNext = leftNext + rightNext;
        const float ayNext = leftNext - rightNext;
        const float xNext = centerX - axNext * scale;
        const float yNext = centerY - ayNext * scale;

        path.quadTo(x, y, xNext, yNext);
    }
    painter->drawPath(path);
}

#include "spinnercanvasitem.h"

#include <rhi/qrhi.h>

#include "canvas.h"
#include "vectorpathcanvasitem.h"

SpinnerCanvasItem::SpinnerCanvasItem() {
    m_timer.start();
}

SpinnerCanvasItem::~SpinnerCanvasItem()
{
    delete m_buffer;
}

void SpinnerCanvasItem::synchronize(QRhi *rhi, QRhiResourceUpdateBatch *updateBatch)
{
    if (m_buffer == nullptr) {
        m_buffer = rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::VertexBuffer, 4*sizeof(ColorVector2D));
        m_buffer->create();
    }

    QList<ColorVector2D> vertexData;
    for (int i = 0; i < 4; i++) {
        QVector2D vector(qCos(M_PI*i/2+m_timer.elapsed()/100.), qSin(M_PI*i/2+m_timer.elapsed()/100.));
        vertexData << ColorVector2D(vector*10, Qt::red);
    }
    updateBatch->updateDynamicBuffer(m_buffer, 0, vertexData.size() * sizeof(ColorVector2D), vertexData.data());
    setNeedsSync();
}

void SpinnerCanvasItem::render(QRhiCommandBuffer *cb)
{
    const QRhiCommandBuffer::VertexInput vbufBinding(m_buffer, 0);
    cb->setVertexInput(0, 1, &vbufBinding);

    cb->draw(4);
}

QRectF SpinnerCanvasItem::boundingRect()
{
    return QRectF(-10, -10, 20, 20);
}

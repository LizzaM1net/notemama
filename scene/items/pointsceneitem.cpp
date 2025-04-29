#include "pointsceneitem.h"


#include <rhi/qrhi.h>

#include "canvas.h"
#include "vectorpathsceneitem.h"



PointSceneItem::~PointSceneItem()
{
    delete m_buffer;
}

void PointSceneItem::synchronize(QRhi* rhi, QRhiResourceUpdateBatch* updateBatch)
{
    countpoint = 14;

    if (m_buffer == nullptr)
    {
        m_buffer = rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::VertexBuffer, countpoint*sizeof(ColorVector2D));
        m_buffer->create();
    }
    QList<ColorVector2D> vertexData;
    double radius = 0.1;
    m_radius = radius;
    for (int i = 0; i < countpoint; ++i)
    {
        int sign = (i%2*2-1);
        float x = radius*qCos(M_PI*sign*i/(countpoint-1)) + m_xy.x();
        float y = radius*qSin(M_PI*sign*i/(countpoint-1)) + m_xy.y();
        QVector2D vector{x, y};
        vertexData << ColorVector2D(vector, float(i)/countpoint, 1-float(i)/countpoint, 1, 0.8);
    }
    updateBatch->updateDynamicBuffer(m_buffer, 0, vertexData.size() * sizeof(ColorVector2D), vertexData.data());
}

void PointSceneItem::render(QRhiCommandBuffer* cb)
{
    const QRhiCommandBuffer::VertexInput vbufBinding(m_buffer, 0);
    cb->setVertexInput(0, 1, &vbufBinding);
    cb->draw(countpoint);
}

QRectF PointSceneItem::boundingRect()
{
    return QRectF(m_xy.x() - m_radius, m_xy.y() - m_radius, 2*m_radius, 2*m_radius);
}


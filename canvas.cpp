#include "canvas.h"

#include <QFile>
#include <rhi/qrhi.h>

#include <iostream>

CanvasRenderer::CanvasRenderer() {
}

static QShader getShader(const QString &name)
{
    QFile f(name);
    return f.open(QIODevice::ReadOnly) ? QShader::fromSerialized(f.readAll()) : QShader();
}

void CanvasRenderer::initialize(QRhiCommandBuffer *cb) {
    if (m_rhi != rhi()) {
        m_rhi = rhi();
        m_pipeline.reset();
    }

    if (!m_pipeline) {
        m_vbuf.reset(m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::VertexBuffer, m_vertexDataCapacity*4));
        m_vbuf->create();

        m_srb.reset(m_rhi->newShaderResourceBindings());
        m_srb->create();

        m_pipeline.reset(m_rhi->newGraphicsPipeline());
        m_pipeline->setShaderStages({
            { QRhiShaderStage::Vertex, getShader(QLatin1String(":/shaders/color.vert.qsb")) },
            { QRhiShaderStage::Fragment, getShader(QLatin1String(":/shaders/color.frag.qsb")) }
        });
        QRhiVertexInputLayout inputLayout;
        inputLayout.setBindings({
            { 5 * sizeof(float) }
        });
        inputLayout.setAttributes({
            { 0, 0, QRhiVertexInputAttribute::Float2, 0 },
            { 0, 1, QRhiVertexInputAttribute::Float3, 2 * sizeof(float) }
        });

        m_pipeline->setVertexInputLayout(inputLayout);
        m_pipeline->setShaderResourceBindings(m_srb.get());
        m_pipeline->setRenderPassDescriptor(renderTarget()->renderPassDescriptor());
        m_pipeline->setTopology(QRhiGraphicsPipeline::LineStrip);
        m_pipeline->create();

        QRhiResourceUpdateBatch *resourceUpdates = m_rhi->nextResourceUpdateBatch();
        resourceUpdates->updateDynamicBuffer(m_vbuf.get(), 0, 4*m_vertexData.size(), m_vertexData.data());
        cb->resourceUpdate(resourceUpdates);
    }
}

void CanvasRenderer::synchronize(QQuickRhiItem *item) {
    Canvas *canvas = static_cast<Canvas*>(item);

    m_vertexData.clear();

    if (canvas->m_lines.isEmpty())
        return;

    QList<QPointF> &line = canvas->m_lines.last();
    for (int j = 0; j < line.size(); j++) {
        m_vertexData << 2*line[j].x()/canvas->width()-1
                            << 1-2*line[j].y()/canvas->height()
                           << double(j)/line.size()
                          << 1-double(j)/line.size()
                         << 1;
    }

    if (m_vertexDataCapacity != m_vertexData.capacity()) {
        m_vertexDataCapacity = m_vertexData.capacity();
        m_vbuf->setSize(m_vertexDataCapacity*4);
        m_vbuf->create();
    }

    if (!m_updateBatch)
        m_updateBatch = m_rhi->nextResourceUpdateBatch();

    m_updateBatch->updateDynamicBuffer(m_vbuf.get(), 0, m_vertexDataCapacity*4, m_vertexData.data());
}

void CanvasRenderer::render(QRhiCommandBuffer *cb) {
    const QColor clearColor = QColor::fromRgbF(0.5f, 0.5f, 0.7f, 1.f);
    cb->beginPass(renderTarget(), clearColor, { 1.0f, 0 }, m_updateBatch);
    if (m_updateBatch)
        m_updateBatch = nullptr;

    cb->setGraphicsPipeline(m_pipeline.get());

    const QRhiCommandBuffer::VertexInput vbufBinding(m_vbuf.get(), 0);
    cb->setVertexInput(0, 1, &vbufBinding);

    cb->draw(m_vertexData.size()/5);

    cb->endPass();
}

Canvas::Canvas() {
    setAcceptedMouseButtons(Qt::AllButtons);
}

QQuickRhiItemRenderer *Canvas::createRenderer() {
    return new CanvasRenderer();
}

void Canvas::mousePressEvent(QMouseEvent *event) {
    m_pressed = true;
    m_lines.append(QList<QPointF>());
    m_lines.last().append(event->points().first().position());
    update();
}

void Canvas::mouseMoveEvent(QMouseEvent *event) {
    if (m_pressed) {
        m_lines.last().append(event->points().first().position());
        update();
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *event) {
    m_pressed = false;
}

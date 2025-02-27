#include "canvas.h"

#include <QFile>
#include <rhi/qrhi.h>

#include <iostream>

CanvasRenderer::CanvasRenderer() {
    // m_vertexDatasCapacity = m_vertexDatas.capacity();
    m_vertexDatas.append({
        0.0f,   0.5f,   1.0f, 0.0f, 0.0f,
        -0.5f,  -0.5f,   0.0f, 1.0f, 0.0f,
        0.5f,  -0.5f,   0.0f, 0.0f, 1.0f,
    });
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
        // m_vbufs.reset(m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::VertexBuffer, m_vertexDatasCapacity*4));
        // m_vbufs->create();

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
        // m_pipeline->setSampleCount(m_sampleCount);
        m_pipeline->setVertexInputLayout(inputLayout);
        m_pipeline->setShaderResourceBindings(m_srb.get());
        m_pipeline->setRenderPassDescriptor(renderTarget()->renderPassDescriptor());
        m_pipeline->setTopology(QRhiGraphicsPipeline::LineStrip);
        m_pipeline->create();
    }

    for (int i = 0; i < 1; i++) {
        if (m_vertexDatasCapacity.size() <= i) {
            m_vertexDatasCapacity.append(m_vertexDatas[i].capacity());
        }
        if (m_vbufs.size() <= i) {
            std::shared_ptr<QRhiBuffer> buf(m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::VertexBuffer, m_vertexDatasCapacity[i]*4));
            buf->create();
            m_vbufs.append(buf);
        }
        m_updateBatch = m_rhi->nextResourceUpdateBatch();
        // m_updateBatch->updateDynamicBuffer(m_vbufs[i].get(), 0, 4*m_vertexDatas[i].size(), m_vertexDatas[i].data());
        m_updateBatch->updateDynamicBuffer(m_vbufs[i].get(), 0, m_vertexDatasCapacity[i]*4, m_vertexDatas[i].data());
        // cb->resourceUpdate(m_updateBatch);
        // m_updateBatch = nullptr;
    }
}

void CanvasRenderer::synchronize(QQuickRhiItem *item) {
    Canvas *canvas = static_cast<Canvas*>(item);

    // m_vertexDatas.clear();

    if (canvas->m_lines.isEmpty())
        return;

    // for (int i = m_vertexDatasCapacity.size(); i < m_vertexDatas.size(); i++) {
    //     m_vertexDatasCapacity.append(m_vertexDatas[i].capacity());
    // }
    // for (int i = m_vbufs.size(); i < m_vertexDatas.size(); i++) {
    //     m_vbufs[i].reset(m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::VertexBuffer, m_vertexDatasCapacity[i]*4));
    // }

    for (int i = 0; i < canvas->m_lines.size(); i++) {
        // m_vertexDatas.append(QList<float>());
        // QList<QPointF> &line = canvas->m_lines[i];
        // for (int j = 0; j < line.size(); j++) {
        //     m_vertexDatas[i] << 2*line[j].x()/canvas->width()-1
        //                         << 1-2*line[j].y()/canvas->height()
        //                        << double(j)/line.size()
        //                       << 1-double(j)/line.size()
        //                      << 1;
        // }

        if (m_vertexDatasCapacity.size() <= i) {
            m_vertexDatasCapacity.append(m_vertexDatas[i].capacity());
        }
        if (m_vbufs.size() <= i) {
            std::shared_ptr<QRhiBuffer> buf(m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::VertexBuffer, m_vertexDatasCapacity[i]*4));
            buf->create();
            m_vbufs.append(buf);
        }
        if (m_vertexDatasCapacity[i] != m_vertexDatas[i].capacity()) {
            m_vertexDatasCapacity[i] = m_vertexDatas[i].capacity();
            // std::cout << "resize buffer" << m_vertexDataCapacity << std::endl;
            m_vbufs[i]->setSize(m_vertexDatasCapacity[i]*4);
            m_vbufs[i]->create();
        }
    }

    if (!m_updateBatch)
        m_updateBatch = m_rhi->nextResourceUpdateBatch();

    for (int i = 0; i < m_vbufs.size(); i++) {
        // m_updateBatch->updateDynamicBuffer(m_vbufs[i].get(), 0, 4*m_vertexDatas[i].size(), m_vertexDatas[i].data());
        m_updateBatch->updateDynamicBuffer(m_vbufs[i].get(), 0, m_vertexDatasCapacity[i]*4, m_vertexDatas[i].data());
    }
}

void CanvasRenderer::render(QRhiCommandBuffer *cb) {
    const QColor clearColor = QColor::fromRgbF(0.5f, 0.5f, 0.7f, 1.f);
    cb->beginPass(renderTarget(), clearColor, { 1.0f, 0 }, m_updateBatch);
    if (m_updateBatch)
        m_updateBatch = nullptr;

    cb->setGraphicsPipeline(m_pipeline.get());
    const QSize outputSize = renderTarget()->pixelSize();
    cb->setViewport(QRhiViewport(0, 0, outputSize.width(), outputSize.height()));
    // cb->setShaderResources();
    // for (int i = 0; i < m_vbufs.size(); i++) {
    //     const QRhiCommandBuffer::VertexInput vbufBinding(m_vbufs[i].get(), 0);
    //     std::cout << "draw" << i << " " << m_vertexDatas[i].size()/5 << std::endl;
    //     cb->setVertexInput(0, 1, &vbufBinding);
    //     cb->draw(m_vertexDatas[i].size()/5);
    // }
    std::cout << "draw" << m_vertexDatas[0].size()/5 << " " << m_vertexDatasCapacity[0] << std::endl;
    cb->draw(m_vertexDatas[0].size()/5);
    // cb->draw(3);

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

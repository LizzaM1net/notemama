#include "canvasrenderer.h"

#include <QFile>

#include "canvas.h"
#include "items/vectorpathcanvasitem.h"

CanvasRenderer::CanvasRenderer(Canvas *item)
    : QQuickRhiItemRenderer()
    , m_item(item) {}

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
        m_ubuf.reset(m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, 64));
        m_ubuf->create();

        m_srb.reset(m_rhi->newShaderResourceBindings());
        m_srb->setBindings({
            QRhiShaderResourceBinding::uniformBuffer(0, QRhiShaderResourceBinding::VertexStage, m_ubuf.get()),
        });
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

        m_pipeline->setSampleCount(4);
        m_pipeline->setVertexInputLayout(inputLayout);
        m_pipeline->setShaderResourceBindings(m_srb.get());
        m_pipeline->setRenderPassDescriptor(renderTarget()->renderPassDescriptor());
        // m_pipeline->setTopology(QRhiGraphicsPipeline::LineStrip);
        m_pipeline->setTopology(QRhiGraphicsPipeline::TriangleStrip);
        m_pipeline->setLineWidth(3.f);
        m_pipeline->create();
    }
}

void CanvasRenderer::synchronize(QQuickRhiItem *item) {
    Canvas *canvas = static_cast<Canvas*>(item);

    if (canvas->m_viewportChanged) {
        // Probably safe to use rhi from here because clipSpaceCorrMatrix just returns hardcoded matrix from graphical backend
        m_transformMatrix = m_rhi->clipSpaceCorrMatrix();
        m_transformMatrix.ortho(canvas->m_position.x(),
                                canvas->m_size.width()/canvas->m_scale+canvas->m_position.x(),
                                canvas->m_size.height()/canvas->m_scale+canvas->m_position.y(),
                                canvas->m_position.y(),
                                -1, 1);
        canvas->m_viewportChanged = false;

        if (!m_updateBatch)
            m_updateBatch = m_rhi->nextResourceUpdateBatch();
        m_updateBatch->updateDynamicBuffer(m_ubuf.get(), 0, 64, m_transformMatrix.constData());
    }

    if (!m_updateBatch)
        m_updateBatch = m_rhi->nextResourceUpdateBatch();

    for (int i = 0; i < m_vbufs.size(); i++) {
        canvas->m_items[i]->synchronize(m_vbufs[i].get(), m_updateBatch);
    }

    for (int i = m_vbufs.size(); i < canvas->m_items.size(); i++) {
        // Buffer will be later resized by renderNew
        QRhiBuffer *buf = m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::VertexBuffer, 0);
        m_vbufs << std::shared_ptr<QRhiBuffer>(buf);

        canvas->m_items[i]->synchronize(m_vbufs[i].get(), m_updateBatch);
    }
}

void CanvasRenderer::render(QRhiCommandBuffer *cb) {
    const QColor clearColor = QColor::fromRgbF(1.f, 1.f, 1.f, 1.f);
    cb->beginPass(renderTarget(), clearColor, { 1.0f, 0 }, m_updateBatch);
    if (m_updateBatch)
        m_updateBatch = nullptr;

    QMetaObject::invokeMethod(m_item, &Canvas::setLastCompletedTime, Qt::QueuedConnection, cb->lastCompletedGpuTime());

    cb->setGraphicsPipeline(m_pipeline.get());
    cb->setShaderResources();

    for (int i = 0; i < m_vbufs.size(); i++) {
        const QRhiCommandBuffer::VertexInput vbufBinding(m_vbufs[i].get(), 0);
        cb->setVertexInput(0, 1, &vbufBinding);

        // cb->draw(m_itemSizes[i]);
        cb->draw(m_vbufs[i]->size()/5/sizeof(float));
    }

    cb->endPass();
}

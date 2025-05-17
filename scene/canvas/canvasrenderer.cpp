#include "canvasrenderer.h"

#include <QFile>

#include "canvas.h"
#include "scene/scene.h"
#include "scene/items/sceneitem.h"

CanvasRenderer::CanvasRenderer(Canvas *item, Scene *scene)
    : QQuickRhiItemRenderer()
    , m_item(item)
    , m_scene(scene) {}

static QShader getShader(const QString &name) {
    QFile f(name);
    return f.open(QIODevice::ReadOnly) ? QShader::fromSerialized(f.readAll()) : QShader();
}

void CanvasRenderer::setupPipeline(QRhiGraphicsPipeline* pipeline,
                                   const QShader& vs,
                                   const QShader& fs,
                                   QRhiShaderResourceBindings* srb,
                                   const QRhiVertexInputLayout& layout)
{
    pipeline->setTopology(QRhiGraphicsPipeline::TriangleStrip);
    pipeline->setShaderStages({
        { QRhiShaderStage::Vertex,  vs},
        { QRhiShaderStage::Fragment, fs}
    });
    pipeline->setVertexInputLayout(layout);
    pipeline->setShaderResourceBindings(srb);
    pipeline->setRenderPassDescriptor(renderTarget()->renderPassDescriptor());
    pipeline->setSampleCount(4);
}

void CanvasRenderer::initialize(QRhiCommandBuffer *cb) {
    if (m_rhi != rhi()) {
        m_rhi = rhi();
        m_pipeline.reset();
        m_background.reset();
    }

    if (!m_background)
    {
        m_background = std::make_unique<Background>();

        QRhiVertexInputLayout bgLayout;
        bgLayout.setBindings({ { 2 * sizeof(float) } });
        bgLayout.setAttributes({ { 0, 0, QRhiVertexInputAttribute::Float2, 0 } });

        m_background->create(m_rhi);

        setupPipeline(m_background->m_backgroundpipline.get(),
                      getShader(QLatin1String(":/shaders/background.vert.qsb")),
                      getShader(QLatin1String(":/shaders/background.frag.qsb")),
                      m_background->m_backgroundsrb.get(),
                      bgLayout);

        m_background->m_backgroundpipline->create();
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
        QRhiVertexInputLayout inputLayout;
        inputLayout.setBindings({
            { 6 * sizeof(float) }
        });
        inputLayout.setAttributes({
            { 0, 0, QRhiVertexInputAttribute::Float2, 0 },
            { 0, 1, QRhiVertexInputAttribute::Float4, 2 * sizeof(float) }
        });

        setupPipeline(m_pipeline.get(),
                      getShader(QLatin1String(":/shaders/color.vert.qsb")),
                      getShader(QLatin1String(":/shaders/color.frag.qsb")),
                      m_srb.get(),
                      inputLayout);

        m_pipeline->setLineWidth(3.f);
        QRhiGraphicsPipeline::TargetBlend blend;
        blend.enable = true;
        m_pipeline->setTargetBlends({blend});
        m_pipeline->create();
    }
}

void CanvasRenderer::synchronize(QQuickRhiItem *item) {
    Canvas *canvas = static_cast<Canvas*>(item);

    if (canvas->m_viewportChanged) {
        // Probably safe to use rhi from here because clipSpaceCorrMatrix just returns hardcoded matrix from graphical backend
        m_transformMatrix = m_rhi->clipSpaceCorrMatrix();
        m_viewRect = QRectF(canvas->m_position.toPointF(), canvas->m_size/canvas->m_scale);
        m_transformMatrix.ortho(m_viewRect);
        canvas->m_viewportChanged = false;

        if (!m_updateBatch)
            m_updateBatch = m_rhi->nextResourceUpdateBatch();
        m_updateBatch->updateDynamicBuffer(m_ubuf.get(), 0, 64, m_transformMatrix.constData());
    }

    if (!m_updateBatch)
        m_updateBatch = m_rhi->nextResourceUpdateBatch();

    QMatrix4x4 mat = m_rhi->clipSpaceCorrMatrix();
    m_updateBatch->updateDynamicBuffer(m_background->m_backgroundubuf.get(),0, 64, mat.constData());

    for (int i = 0; i < m_scene->items.size(); i++) {
        QRectF itemRect = m_scene->items[i]->boundingRect();
        if ((itemRect.isEmpty() || m_viewRect.intersects(itemRect)) && m_scene->items[i]->trySync())
            m_scene->items[i]->synchronize(m_rhi, m_updateBatch);
    }
}

void CanvasRenderer::render(QRhiCommandBuffer *cb) {
    const QColor clearColor = Qt::transparent;


    float bgVertices[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
        1.0f,  1.0f,
        1.0f, -1.0f
    };

    m_tempVertexBuffer.reset(m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::VertexBuffer, sizeof(bgVertices)));
    m_tempVertexBuffer->create();
    m_updateBatch->updateDynamicBuffer(m_tempVertexBuffer.get(), 0, sizeof(bgVertices), bgVertices);

    cb->beginPass(renderTarget(), clearColor, { 1.0f, 0 }, m_updateBatch);

    QMetaObject::invokeMethod(m_item, &Canvas::setLastCompletedTime, Qt::QueuedConnection, cb->lastCompletedGpuTime());

    cb->setGraphicsPipeline(m_background->m_backgroundpipline.get());
    cb->setShaderResources(m_background->m_backgroundsrb.get());
    cb->setViewport({ 0, 0, float(renderTarget()->pixelSize().width()), float(renderTarget()->pixelSize().height()) });


    QRhiCommandBuffer::VertexInput vertexBindings[] = {
        { m_tempVertexBuffer.get(), 0 }
    };
    cb->setVertexInput(0, 1, vertexBindings);
    cb->draw(4);

    cb->setGraphicsPipeline(m_pipeline.get());
    cb->setShaderResources();

    for (int i = 0; i < m_scene->items.size(); i++) {
        QRectF itemRect = m_scene->items[i]->boundingRect();
        if (itemRect.isEmpty() || m_viewRect.intersects(itemRect))
            m_scene->items[i]->render(cb);
    }

    cb->endPass();
    m_updateBatch = nullptr;
}

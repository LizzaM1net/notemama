#ifndef CANVASRENDERER_H
#define CANVASRENDERER_H

#include <QQuickRhiItemRenderer>
#include <QSGRendererInterface>
#include <rhi/qrhi.h>

class Canvas;
class Scene;

class CanvasRenderer : public QQuickRhiItemRenderer {
public:
    CanvasRenderer(Canvas *item, Scene *scene);

protected:
    void initialize(QRhiCommandBuffer *cb);
    void synchronize(QQuickRhiItem *item);
    void render(QRhiCommandBuffer *cb);

private:
    Canvas *m_item = nullptr;
    Scene *m_scene = nullptr;

    QRhi *m_rhi = nullptr;

    QRhiResourceUpdateBatch *m_updateBatch = nullptr;

    std::unique_ptr<QRhiGraphicsPipeline> m_pipeline;
    std::unique_ptr<QRhiShaderResourceBindings> m_srb;
    QList<int> m_itemSizes;

    QRectF m_viewRect;
    QMatrix4x4 m_transformMatrix;
    std::unique_ptr<QRhiBuffer> m_ubuf;
};

#endif // CANVASRENDERER_H

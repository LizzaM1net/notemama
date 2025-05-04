#ifndef CANVASRENDERER_H
#define CANVASRENDERER_H

#include <QQuickRhiItemRenderer>
#include <QSGRendererInterface>
#include <rhi/qrhi.h>

#include "background/background.h"
class Canvas;
class Scene;
class Background;

class CanvasRenderer : public QQuickRhiItemRenderer {
public:
    CanvasRenderer(Canvas *item, Scene *scene);

protected:
    void initialize(QRhiCommandBuffer *cb);
    void synchronize(QQuickRhiItem *item);
    void render(QRhiCommandBuffer *cb);
    void setupPipeline(QRhiGraphicsPipeline* pipline, const QShader& vs, const QShader& fs, QRhiShaderResourceBindings* srb, const QRhiVertexInputLayout& layout);

private:
    Canvas *m_item = nullptr;
    Scene *m_scene = nullptr;

    QRhi *m_rhi = nullptr;

    QRhiResourceUpdateBatch *m_updateBatch = nullptr;


    std::unique_ptr<QRhiGraphicsPipeline> m_backgroundpipline; // bg
    std::unique_ptr<QRhiShaderResourceBindings> m_backgroundsrb; // bg
    std::unique_ptr<QRhiBuffer> m_bgubuf; // bg
    std::unique_ptr<QRhiBuffer> m_tempVertexBuffer; // bg

    std::unique_ptr<QRhiGraphicsPipeline> m_pipeline;
    std::unique_ptr<QRhiShaderResourceBindings> m_srb;
    QList<int> m_itemSizes;

    std::unique_ptr<Background> m_background; //bg

    QRectF m_viewRect;
    QMatrix4x4 m_transformMatrix;
    std::unique_ptr<QRhiBuffer> m_ubuf;
};

#endif // CANVASRENDERER_H

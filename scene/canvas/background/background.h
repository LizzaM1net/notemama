#ifndef BACKGROUND_H
#define BACKGROUND_H


#include <QQuickRhiItem>
#include <QSGRendererInterface>
#include <rhi/qrhi.h>
#include <memory>
#include "canvasrenderer.h"

class CanvasRenderer;


class Background{
protected:

    std::unique_ptr<QRhiGraphicsPipeline> m_backgroundpipline;
    std::unique_ptr<QRhiShaderResourceBindings> m_backgroundsrb;
    std::unique_ptr<QRhiBuffer> m_backgroundubuf;
    std::unique_ptr<QRhiBuffer> m_tempVertexBuffer;

    friend class CanvasRenderer;
public:

    Background();
    ~Background() { }
    bool create(QRhi* rhi);
    bool render_act(QRhi* rhi);
private:
};



#endif

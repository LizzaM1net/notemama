#include "scene/canvas/background/background.h"
#include <QFile>


Background::Background() {
}


bool Background::create(QRhi* rhi)
{
    try{
        if (!m_backgroundpipline && rhi)
        {
            m_backgroundubuf.reset(rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, 64));
            m_backgroundubuf->create();

            m_backgroundsrb.reset(rhi->newShaderResourceBindings());
            m_backgroundsrb->setBindings({
                QRhiShaderResourceBinding::uniformBuffer(0, QRhiShaderResourceBinding::VertexStage, m_backgroundubuf.get())
            });
            m_backgroundsrb->create();

            m_backgroundpipline.reset(rhi->newGraphicsPipeline());
        }
    } catch(...)
    {
        return false;
    }

    return true;
}

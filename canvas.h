#ifndef CANVAS_H
#define CANVAS_H

#include <QQuickRhiItem>

class QRhiBuffer;
class QRhiGraphicsPipeline;
class QRhiShaderResourceBindings;
class QRhiResourceUpdateBatch;

class CanvasRenderer : public QQuickRhiItemRenderer {
public:
    CanvasRenderer();

protected:
    void initialize(QRhiCommandBuffer *cb);
    void synchronize(QQuickRhiItem *item);
    void render(QRhiCommandBuffer *cb);

private:
    QRhi *m_rhi;

    QList<QList<float>> m_vertexDatas;
    QList<int> m_vertexDatasCapacity;

    QRhiResourceUpdateBatch *m_updateBatch = nullptr;

    std::unique_ptr<QRhiGraphicsPipeline> m_pipeline;
    std::unique_ptr<QRhiShaderResourceBindings> m_srb;
    QList<std::shared_ptr<QRhiBuffer>> m_vbufs;
};

class Canvas : public QQuickRhiItem {
    Q_OBJECT
    QML_ELEMENT

    friend class CanvasRenderer;

public:
    Canvas();

protected:
    QQuickRhiItemRenderer *createRenderer();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    QList<QList<QPointF>> m_lines;

    bool m_pressed = false;
};

#endif // CANVAS_H

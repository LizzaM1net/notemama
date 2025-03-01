#ifndef CANVAS_H
#define CANVAS_H

#include <QQuickRhiItem>

class QRhiBuffer;
class QRhiGraphicsPipeline;
class QRhiShaderResourceBindings;
class QRhiResourceUpdateBatch;

class Canvas;

struct LineUpdate {
    int index;
    int updateStart;
    int updateEnd;
};

class CanvasRenderer : public QQuickRhiItemRenderer {
public:
    CanvasRenderer(Canvas *item);

protected:
    void initialize(QRhiCommandBuffer *cb);
    void synchronize(QQuickRhiItem *item);
    void render(QRhiCommandBuffer *cb);

private:
    Canvas *m_item = nullptr;

    QRhi *m_rhi = nullptr;

    QList<QList<float>> m_vertexDatas;

    QRhiResourceUpdateBatch *m_updateBatch = nullptr;

    std::unique_ptr<QRhiGraphicsPipeline> m_pipeline;
    std::unique_ptr<QRhiShaderResourceBindings> m_srb;
    QList<std::shared_ptr<QRhiBuffer>> m_vbufs;
};

class Canvas : public QQuickRhiItem {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(double lastCompletedTime READ lastCompletedTime WRITE setLastCompletedTime NOTIFY lastCompletedTimeChanged FINAL)

    friend class CanvasRenderer;

public:
    Canvas();

    double lastCompletedTime() const;

signals:
    void lastCompletedTimeChanged();

private slots:
    void setLastCompletedTime(double newLastCompletedTime);

protected:
    QQuickRhiItemRenderer *createRenderer();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    QList<QList<QPointF>> m_lines;
    QList<LineUpdate> m_lineUpdates;

    bool m_pressed = false;

    double m_lastCompletedTime = 0;
};

#endif // CANVAS_H

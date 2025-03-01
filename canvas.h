#ifndef CANVAS_H
#define CANVAS_H

#include <QQuickRhiItem>
#include <QSGRendererInterface>
#include <rhi/qrhi.h>
#include <QQuickWindow>

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

    Q_PROPERTY(double lastCompletedTime READ lastCompletedTime NOTIFY lastCompletedTimeChanged FINAL)
    Q_PROPERTY(QString graphicsApi READ graphicsApi NOTIFY graphicsApiChanged FINAL)

    friend class CanvasRenderer;

public:
    Canvas();

    double lastCompletedTime() const;

    QString graphicsApi() const;

signals:
    void lastCompletedTimeChanged();

    void graphicsApiChanged();

private slots:
    void setLastCompletedTime(double newLastCompletedTime);

    void windowChanged(QQuickWindow *window);

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

    QSGRendererInterface::GraphicsApi m_graphicsApi = QSGRendererInterface::GraphicsApi::Null;
};

#endif // CANVAS_H

#ifndef CANVAS_H
#define CANVAS_H

#include <QQuickRhiItem>
#include <QSGRendererInterface>
#include <QQuickWindow>

class VectorPathCanvasItem;
class Tool;

class Canvas : public QQuickRhiItem {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(double lastCompletedTime READ lastCompletedTime NOTIFY lastCompletedTimeChanged FINAL)
    Q_PROPERTY(QString graphicsApi READ graphicsApi NOTIFY graphicsApiChanged FINAL)
    Q_PROPERTY(InputMode inputMode READ inputMode WRITE setInputMode NOTIFY inputModeChanged FINAL)

    Q_PROPERTY(QVector2D position READ position WRITE setPosition NOTIFY positionChanged FINAL)
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged FINAL)
    Q_PROPERTY(QVector2D transformOrigin READ transformOrigin WRITE setTransformOrigin NOTIFY transformOriginChanged FINAL)

    friend class CanvasRenderer;
    // This temporary
    friend class Tool;

public:
    enum InputMode {
        Raw,
        Lines,
    }; Q_ENUM(InputMode)

public:
    Canvas();
    ~Canvas();

    double lastCompletedTime() const;

    QString graphicsApi() const;

    InputMode inputMode() const;
    void setInputMode(InputMode newInputMode);

    QVector2D position() const;
    void setPosition(QVector2D position);
    Q_INVOKABLE void move(QVector2D delta);

    qreal scale() const;
    void setScale(qreal scale);

    QVector2D transformOrigin() const;
    void setTransformOrigin(QVector2D transformOrigin);

signals:
    void lastCompletedTimeChanged();
    void graphicsApiChanged();
    void inputModeChanged();
    void positionChanged();
    void scaleChanged();
    void transformOriginChanged();

private slots:
    void setLastCompletedTime(double newLastCompletedTime);

    void windowChanged(QQuickWindow *window);

protected:
    QQuickRhiItemRenderer *createRenderer() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

private:
    InputMode m_inputMode = Raw;
    QList<Tool*> m_tools;

    QList<VectorPathCanvasItem*> m_items;

    bool m_pressed = false;

    double m_lastCompletedTime = 0;

    QSGRendererInterface::GraphicsApi m_graphicsApi = QSGRendererInterface::GraphicsApi::Null;

    // Viewport data
    QVector2D m_position;
    QVector2D m_transformOrigin;
    qreal m_scale = 1;
    QSizeF m_size;
    bool m_viewportChanged = false;
};

#endif // CANVAS_H

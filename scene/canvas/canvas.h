#ifndef CANVAS_H
#define CANVAS_H

#include <QQuickRhiItem>
#include <QSGRendererInterface>
#include <QQuickWindow>

#include "scene/scene.h"

class SceneItem;
class Tool;

class Canvas : public QQuickRhiItem, public SceneObserver {
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(double lastCompletedTime READ lastCompletedTime NOTIFY lastCompletedTimeChanged FINAL)
    Q_PROPERTY(QString graphicsApi READ graphicsApi NOTIFY graphicsApiChanged FINAL)

    Q_PROPERTY(QVector2D position READ position WRITE setPosition NOTIFY positionChanged FINAL)
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged FINAL)
    Q_PROPERTY(QVector2D transformOrigin READ transformOrigin WRITE setTransformOrigin NOTIFY transformOriginChanged FINAL)

    friend class CanvasRenderer;

public:
    Canvas();
    ~Canvas();

    double lastCompletedTime() const;

    QString graphicsApi() const;

    QVector2D position() const;
    void setPosition(QVector2D position);
    Q_INVOKABLE void move(QVector2D delta);

    qreal scale() const;
    void setScale(qreal scale);

    QVector2D transformOrigin() const;
    void setTransformOrigin(QVector2D transformOrigin);

    Scene *currentScene();

signals:
    void lastCompletedTimeChanged();
    void graphicsApiChanged();
    void positionChanged();
    void scaleChanged();
    void transformOriginChanged();

private slots:
    void setLastCompletedTime(double newLastCompletedTime);

    void windowChanged(QQuickWindow *window);

protected:
    void itemChanged(SceneItem *item) override;

    QQuickRhiItemRenderer *createRenderer() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

private:
    QList<Tool*> m_tools;

    Scene *m_scene = nullptr;

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

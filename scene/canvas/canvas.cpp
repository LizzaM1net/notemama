#include "canvas.h"

#include "scene/scene.h"
#include "canvasrenderer.h"

#include "tools/pentool.h"
#include "tools/curvepentool.h"
#include "tools/fakedrawtool.h"

static Scene scene;
Canvas::Canvas()
    : QQuickRhiItem()
    , SceneObserver() {
    setAcceptedMouseButtons(Qt::AllButtons);

    m_scene = &scene;
    addObservedScene(m_scene);

    connect(this, &QQuickItem::windowChanged,
            this, &Canvas::windowChanged);

    m_tools << new PenTool(this);
    m_tools << new CurvePenTool(this);
    // m_tools << new FakeDrawTool(this, m_tools.last());
}

Canvas::~Canvas()
{
    // delete m_scene;
}

double Canvas::lastCompletedTime() const
{
    return m_lastCompletedTime;
}

QString Canvas::graphicsApi() const
{
    switch (m_graphicsApi) {
    case QSGRendererInterface::OpenGL:
        return "OpenGL";
        break;
    case QSGRendererInterface::Direct3D11:
        return "D3D11";
        break;
    case QSGRendererInterface::Direct3D12:
        return "D3D12";
        break;
    case QSGRendererInterface::Vulkan:
        return "Vulkan";
        break;
    case QSGRendererInterface::Metal:
        return "Metal";
        break;
    default:
        return "Unknown 3D API";
        break;
    }
}

QVector2D Canvas::position() const
{
    return m_position;
}

void Canvas::setPosition(QVector2D position)
{
    if (m_position == position)
        return;

    m_position = position;
    m_viewportChanged = true;

    emit positionChanged();
    update();
}

void Canvas::move(QVector2D delta)
{
    m_position -= delta/m_scale;
    m_viewportChanged = true;

    emit positionChanged();
    update();
}

qreal Canvas::scale() const
{
    return m_scale;
}

void Canvas::setScale(qreal scale)
{
    if (qFuzzyCompare(m_scale, scale))
        return;

    m_position += (scale-m_scale)/scale*m_transformOrigin/m_scale;
    m_scale = scale;
    m_viewportChanged = true;

    emit positionChanged();
    emit scaleChanged();
    update();
}

QVector2D Canvas::transformOrigin() const
{
    return m_transformOrigin;
}

void Canvas::setTransformOrigin(QVector2D transformOrigin)
{
    if (m_transformOrigin == transformOrigin)
        return;

    m_transformOrigin = transformOrigin;
    emit transformOriginChanged();
}

Scene *Canvas::currentScene() {
    return m_scene;
}

void Canvas::setLastCompletedTime(double newLastCompletedTime)
{
    if (qFuzzyCompare(m_lastCompletedTime, newLastCompletedTime))
        return;

    m_lastCompletedTime = newLastCompletedTime;
    emit lastCompletedTimeChanged();
}

void Canvas::windowChanged(QQuickWindow *window) {
    if (!window)
        return;

    QSGRendererInterface::GraphicsApi api = window->rendererInterface()->graphicsApi();
    if (api != m_graphicsApi) {
        m_graphicsApi = api;
        emit graphicsApiChanged();
    }
}

void Canvas::itemChanged(SceneItem *item) {
    update();
}

QQuickRhiItemRenderer *Canvas::createRenderer() {
    return new CanvasRenderer(this, m_scene);
}

void Canvas::mousePressEvent(QMouseEvent *event) {
    QVector2D point = QVector2D(event->points().first().position())/m_scale+m_position;
    m_tools.last()->mousePress(point);
}

void Canvas::mouseMoveEvent(QMouseEvent *event) {
    QVector2D point = QVector2D(event->points().first().position())/m_scale+m_position;
    m_tools.last()->mouseMove(point);
}

void Canvas::mouseReleaseEvent(QMouseEvent *event) {
    m_tools.last()->mouseRelease();
}

void Canvas::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) {
    m_size = newGeometry.size();
    m_viewportChanged = true;

    // It calls update
    QQuickRhiItem::geometryChange(newGeometry, oldGeometry);
}

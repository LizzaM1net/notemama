#include "canvas.h"

#include "canvasrenderer.h"
#include "tools/pentool.h"

#include "items/spinnercanvasitem.h"
#include "items/vectorpathcanvasitem.h"

Canvas::Canvas()
    : QQuickRhiItem() {
    setAcceptedMouseButtons(Qt::AllButtons);

    connect(this, &QQuickItem::windowChanged,
            this, &Canvas::windowChanged);

    addItem(new VectorPathCanvasItem(QVector2D{-10, 0}, {new VectorPath::LineSegment(QVector2D{10, 10}),
                                                         new VectorPath::LineSegment(QVector2D{10, -10}),
                                                         new VectorPath::LineSegment(QVector2D{-10, -10}),
                                                         new VectorPath::LineSegment(QVector2D{-10, 10})}));

    addItem(new VectorPathCanvasItem(QVector2D{10, 10+200}, {new VectorPath::QuadCurveSegment(QVector2D{0, -200}, QVector2D{200, -200}),
                                                             new VectorPath::LineSegment(QVector2D{100, 0}),
                                                             new VectorPath::LineSegment(QVector2D{20, 20}),
                                                             new VectorPath::LineSegment(QVector2D{-20, 20}),
                                                             new VectorPath::QuadCurveSegment(QVector2D{200, 0}, QVector2D{200, 200})}));

    addItem(new VectorPathCanvasItem(QVector2D{60, 60+200}, {new VectorPath::CubicCurveSegment(QVector2D{0, -110}, QVector2D{90, -200}, QVector2D{200, -200}),
                                                             new VectorPath::CubicCurveSegment(QVector2D{110, 0}, QVector2D{200, 90}, QVector2D{200, 200})}));

    addItem(new SpinnerCanvasItem());

    m_tools << new PenTool(this);
}

Canvas::~Canvas()
{
    qDeleteAll(m_items);
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

Canvas::InputMode Canvas::inputMode() const
{
    return m_inputMode;
}

void Canvas::setInputMode(InputMode newInputMode)
{
    if (m_inputMode == Lines) {
        m_pressed = false;
    }

    if (m_inputMode == newInputMode)
        return;
    m_inputMode = newInputMode;
    emit inputModeChanged();
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

void Canvas::addItem(CanvasItem *item)
{
    item->setCanvas(this);
    m_items << item;
}

void Canvas::setLastCompletedTime(double newLastCompletedTime)
{
    if (qFuzzyCompare(m_lastCompletedTime, newLastCompletedTime))
        return;

    m_lastCompletedTime = newLastCompletedTime;
    emit lastCompletedTimeChanged();
}

void Canvas::windowChanged(QQuickWindow *window)
{
    if (!window)
        return;

    QSGRendererInterface::GraphicsApi api = window->rendererInterface()->graphicsApi();
    if (api != m_graphicsApi) {
        m_graphicsApi = api;
        emit graphicsApiChanged();
    }
}

QQuickRhiItemRenderer *Canvas::createRenderer() {
    return new CanvasRenderer(this);
}

// static QVector2D lastPoint;
void Canvas::mousePressEvent(QMouseEvent *event) {
    QVector2D point = QVector2D(event->points().first().position())/m_scale+m_position;
    m_tools.first()->mousePress(point);
}

void Canvas::mouseMoveEvent(QMouseEvent *event) {
    QVector2D point = QVector2D(event->points().first().position())/m_scale+m_position;
    m_tools.first()->mouseMove(point);
}

void Canvas::mouseReleaseEvent(QMouseEvent *event) {
    m_tools.first()->mouseRelease();
    // if (m_inputMode == Raw) {
    //     m_pressed = false;
    // }
}

void Canvas::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) {
    m_size = newGeometry.size();
    m_viewportChanged = true;

    // It calls update
    QQuickRhiItem::geometryChange(newGeometry, oldGeometry);
}

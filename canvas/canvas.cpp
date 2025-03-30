#include "canvas.h"

#include <iostream>

#include "canvasrenderer.h"

void debugCurve(QList<QPointF> controlPoints) {
    QList<QPointF> tangentPoints = {-2*controlPoints[0]+2*controlPoints[1], -2*controlPoints[1]+2*controlPoints[2]};
    int points = 10;
    QList<QPointF> curve;
    for (int i = 0; i < points; i++) {
        float t = float(i)/(points-1.);
        QPointF point = controlPoints[0]*(1.-t)*(1.-t) + 2.*controlPoints[1]*t*(1.-t) + controlPoints[2]*t*t;
        QPointF tangent = tangentPoints[0]*(1.-t) + tangentPoints[1]*t;
        curve << point;
    }
    QList<float> fakeTs = {0};
    for (int i = 1; i < points; i++) {
        QPointF vector = curve[i-1] - curve[i];
        fakeTs << fakeTs.last() + qHypot(vector.x(), vector.y());
    }
    std::cout << "==============" << std::endl;
    for (int i = 0; i < points; i++) {
        fakeTs[i] /= fakeTs.last();
        std::cout << "real t:" << i/(points-1.) << "fake t:" << fakeTs[i] << std::endl;
    }
}

QList<QPointF> quadCurve(QList<QPointF> controlPoints) {
    // debugCurve(controlPoints);
    QList<QPointF> tangentPoints = {-2*controlPoints[0]+2*controlPoints[1], -2*controlPoints[1]+2*controlPoints[2]};
    QList<QPointF> curve;
    float width = 3;
    int points = 40;
    {
        float t = 0;
        QPointF point = controlPoints[0]*(1.-t)*(1.-t) + 2.*controlPoints[1]*t*(1.-t) + controlPoints[2]*t*t;
        QPointF tangent = tangentPoints[0]*(1.-t) + tangentPoints[1]*t;
        QPointF normal = QPointF(tangent.y(), -tangent.x());
        normal /= qHypot(normal.x(), normal.y());
        curve << point + -1 * normal * width;
    }
    for (int i = 0; i < points; i++) {
        float t = float(i)/(points-1.);
        QPointF point = controlPoints[0]*(1.-t)*(1.-t) + 2.*controlPoints[1]*t*(1.-t) + controlPoints[2]*t*t;
        QPointF tangent = tangentPoints[0]*(1.-t) + tangentPoints[1]*t;
        QPointF normal = QPointF(tangent.y(), -tangent.x());
        normal /= qHypot(normal.x(), normal.y());
        curve << point + (i%2==0 ? 1 : -1) * normal * width;
    }
    {
        float t = 1;
        QPointF point = controlPoints[0]*(1.-t)*(1.-t) + 2.*controlPoints[1]*t*(1.-t) + controlPoints[2]*t*t;
        QPointF tangent = tangentPoints[0]*(1.-t) + tangentPoints[1]*t;
        QPointF normal = QPointF(tangent.y(), -tangent.x());
        normal /= qHypot(normal.x(), normal.y());
        curve << point + 1 * normal * width;
    }
    return curve;
}

// QList<QPointF> cubicQuadFit(QList<QPointF>) {

// }

Canvas::Canvas()
    : QQuickRhiItem() {
    setAcceptedMouseButtons(Qt::AllButtons);

    connect(this, &QQuickItem::windowChanged,
            this, &Canvas::windowChanged);


    // m_lines << cubicCurve({{40-10+400, 40+10+400}, {40-10, 40+10+400}, {40-10, 40+10}})
    //         << cubicCurve({{40, 40}, {40+400, 40}, {40+400, 40+400}});
    m_lines << quadCurve({{40-10, 40-10+200}, {40-10, 40-10}, {40-10+200, 40-10}})
            << quadCurve({{40-10, 40+10+200}, {40-10, 40+10+400}, {40-10+200, 40+10+400}})
            << quadCurve({{40+10+200, 40-10}, {40+10+400, 40-10}, {40+10+400, 40-10+200}});
    // m_lines << cubicCurve({{40-10, 40-10+200}, {40-10, 40-10}, {40-10+200, 40-10}})
    //         << cubicCurve({{40-10, 40-10+400}, {40-10, 40-10}, {40-10+400, 40-10}});
}

QQuickRhiItemRenderer *Canvas::createRenderer() {
    return new CanvasRenderer(this);
}

void Canvas::mousePressEvent(QMouseEvent *event) {
    if (m_inputMode == Raw) {
        m_pressed = true;
        m_lines.append(QList<QPointF>());
        m_lines.last().append(event->points().first().position());
    } else if (m_inputMode == Lines) {
        if (!m_pressed) {
            m_pressed = true;
            m_lines.append(QList<QPointF>());
        }
        m_lines.last().append(event->points().first().position());
    } else {
        Q_UNREACHABLE();
    }
    update();
}

void Canvas::mouseMoveEvent(QMouseEvent *event) {
    if (m_inputMode == Raw) {
        if (m_pressed) {
            m_lines.last().append(event->points().first().position());
            update();
        }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *event) {
    if (m_inputMode == Raw) {
        m_pressed = false;
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

double Canvas::lastCompletedTime() const
{
    return m_lastCompletedTime;
}

void Canvas::setLastCompletedTime(double newLastCompletedTime)
{
    if (qFuzzyCompare(m_lastCompletedTime, newLastCompletedTime))
        return;

    m_lastCompletedTime = newLastCompletedTime;
    emit lastCompletedTimeChanged();
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

#include "canvas.h"

#include <QFile>
#include <QtMath>

#include <iostream>

#include "svgparser.h"

CanvasRenderer::CanvasRenderer(Canvas *item)
    : QQuickRhiItemRenderer()
    , m_item(item) {}

static QShader getShader(const QString &name)
{
    QFile f(name);
    return f.open(QIODevice::ReadOnly) ? QShader::fromSerialized(f.readAll()) : QShader();
}

void CanvasRenderer::initialize(QRhiCommandBuffer *cb) {
    if (m_rhi != rhi()) {
        m_rhi = rhi();
        m_pipeline.reset();
    }

    if (!m_pipeline) {
        m_srb.reset(m_rhi->newShaderResourceBindings());
        m_srb->create();

        m_pipeline.reset(m_rhi->newGraphicsPipeline());
        m_pipeline->setShaderStages({
            { QRhiShaderStage::Vertex, getShader(QLatin1String(":/shaders/color.vert.qsb")) },
            { QRhiShaderStage::Fragment, getShader(QLatin1String(":/shaders/color.frag.qsb")) }
        });
        QRhiVertexInputLayout inputLayout;
        inputLayout.setBindings({
            { 5 * sizeof(float) }
        });
        inputLayout.setAttributes({
            { 0, 0, QRhiVertexInputAttribute::Float2, 0 },
            { 0, 1, QRhiVertexInputAttribute::Float3, 2 * sizeof(float) }
        });

        m_pipeline->setSampleCount(4);
        m_pipeline->setVertexInputLayout(inputLayout);
        m_pipeline->setShaderResourceBindings(m_srb.get());
        m_pipeline->setRenderPassDescriptor(renderTarget()->renderPassDescriptor());
        m_pipeline->setTopology(QRhiGraphicsPipeline::LineStrip);
        // m_pipeline->setTopology(QRhiGraphicsPipeline::TriangleStrip);
        m_pipeline->setLineWidth(3.f);
        m_pipeline->create();
    }
}

void CanvasRenderer::synchronize(QQuickRhiItem *item) {
    Canvas *canvas = static_cast<Canvas*>(item);

    m_vertexDatas.clear();

    if (canvas->m_lines.isEmpty())
        return;

    for (int i = 0; i < canvas->m_lines.size(); i++) {
        m_vertexDatas.append(QList<float>());
        QList<QPointF> &line = canvas->m_lines[i];
        for (int j = 0; j < line.size(); j++) {
            m_vertexDatas[i] << 2*line[j].x()/canvas->width()-1
                         << 1-2*line[j].y()/canvas->height()
                         << (qSin(j)+1)/2
                         << (qCos(j)+1)/2
                         << 1;
        }
    }

    for (int i = 0; i < m_vbufs.size(); i++) {
        if (m_vbufs[i]->size() != m_vertexDatas[i].capacity()) {
            m_vbufs[i]->setSize(m_vertexDatas[i].capacity() * sizeof(float));
            m_vbufs[i]->create();
        }
    }
    for (int i = m_vbufs.size(); i < canvas->m_lines.size(); i++) {
        QRhiBuffer *buf = m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::VertexBuffer, m_vertexDatas[i].capacity() * sizeof(float));
        buf->create();
        m_vbufs << std::shared_ptr<QRhiBuffer>(buf);
    }

    if (!m_updateBatch)
        m_updateBatch = m_rhi->nextResourceUpdateBatch();

    for (int i = 0; i < m_vbufs.size(); i++) {
        m_updateBatch->updateDynamicBuffer(m_vbufs[i].get(), 0, m_vbufs[i]->size(), m_vertexDatas[i].data());
    }
}

void CanvasRenderer::render(QRhiCommandBuffer *cb) {
    const QColor clearColor = QColor::fromRgbF(1.f, 1.f, 1.f, 1.f);
    cb->beginPass(renderTarget(), clearColor, { 1.0f, 0 }, m_updateBatch);
    if (m_updateBatch)
        m_updateBatch = nullptr;

    QMetaObject::invokeMethod(m_item, &Canvas::setLastCompletedTime, Qt::QueuedConnection, cb->lastCompletedGpuTime());

    cb->setGraphicsPipeline(m_pipeline.get());

    for (int i = 0; i < m_vbufs.size(); i++) {
        const QRhiCommandBuffer::VertexInput vbufBinding(m_vbufs[i].get(), 0);
        cb->setVertexInput(0, 1, &vbufBinding);

        cb->draw(m_vertexDatas[i].size()/5);
    }

    cb->endPass();
}

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


    QList<QPointF> line;
    QPointF lastPos;
    SvgParser parser("/home/e.nuromskiy/Desktop/test1.svg");
    parser.gettokens();
    auto path = parser.paths.first();


    // int i = 0;
    for (auto command : path) {
        // if (i++ > 2) break;

        if (command.type == "m" && !line.isEmpty())
            break;

        if (command.points.isEmpty())
            break;

        for (auto point : command.points) {
            if (command.type.toLower() == "h") {
                if (command.type.isLower())
                    line << QPointF(lastPos.x()+point.x(), lastPos.y());
                else
                    line << QPointF(point.x(), lastPos.y());
            } else if (command.type.toLower() == "v") {
                if (command.type.isLower())
                    line << QPointF(lastPos.x(), lastPos.y()+point.y());
                else
                    line << QPointF(lastPos.x(), point.y());
            } else {
                if (command.type.isLower())
                    line << (lastPos+point);
                else
                    line << point;
            }
        }

        for (auto &point : line) {
            point.ry() -= 60;
            qDebug() << point;
        }

        // if (command.type.isLower())
        //     lastPos += command.points.last();
        // else
        //     lastPos = command.points.last();
        lastPos = line.last();

        // qDebug() << command.type << command.points;
    }
    m_lines << line;
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

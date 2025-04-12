#include "curvepentool.h"

#include <QDebug>

#include "canvas.h"
#include "items/vectorpathcanvasitem.h"

CurvePenTool::CurvePenTool(Canvas *canvas)
    : Tool(canvas) {}

QList<QVector2D> leastSquaresFitCurve(QList<QVector2D> points) {
    QList<float> Ts(points.size());
    Ts[0] = 0;
    for (int i = 1; i < points.size(); i++) {
        Ts[i] = Ts[i-1] + points[i-1].distanceToPoint(points[i]);
    }
    for (int i = 1; i < points.size(); i++) {
        Ts[i] /= Ts.last();
    }
    int last = points.size()-1;

    QMatrix4x4 tMatrix;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            tMatrix(i, j) = 0;

    for (const float &t : std::as_const(Ts)) {
        double B[4] = { qPow(1-t, 3), 3*qPow(1-t, 2)*t, 3*(1-t)*qPow(t, 2), qPow(t, 3) };
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                tMatrix(j, k) += B[j] * B[k];
    }

    bool wasInverted = false;
    QGenericMatrix<4, 4, float> tMatrixT = tMatrix.inverted(&wasInverted).toGenericMatrix<4, 4>();
    if (!wasInverted) {
        qDebug() << "failed to fit. is it even possible?";
        QList<QVector2D> curve;
        curve << points[0] << (points[last]-points[0])/3 << (points[0]-points[last])/3 << points[last];
        return curve;
    }

    QGenericMatrix<1, 4, float> xMatrix;
    QGenericMatrix<1, 4, float> yMatrix;
    for (int i = 0; i < 4; i++) {
        xMatrix(i, 0) = 0;
        yMatrix(i, 0) = 0;
    }

    for (int i = 0; i < points.size(); i++) {
        float t = Ts[i];
        double B[4] = { qPow(1-t, 3), 3*qPow(1-t, 2)*t, 3*(1-t)*qPow(t, 2), qPow(t, 3) };
        for (int j = 0; j < 4; j++) {
            xMatrix(j, 0) += B[j] * points[i].x();
            yMatrix(j, 0) += B[j] * points[i].y();
        }
    }

    QGenericMatrix<1, 4, float> xCM = tMatrixT*xMatrix;
    QGenericMatrix<1, 4, float> yCM = tMatrixT*yMatrix;

    QList<QVector2D> curve;
    curve << QVector2D(xCM(0, 0), yCM(0, 0)) << QVector2D(xCM(1, 0), yCM(1, 0))-QVector2D(xCM(0, 0), yCM(0, 0))
          << QVector2D(xCM(2, 0), yCM(2, 0))-QVector2D(xCM(3, 0), yCM(3, 0)) << QVector2D(xCM(3, 0), yCM(3, 0));
    return curve;
}

void CurvePenTool::mousePress(QVector2D position)
{
    m_points << position;
    m_pathItem = new VectorPathCanvasItem(position, {});
    m_canvas->addItem(m_pathItem);
}

void CurvePenTool::mouseMove(QVector2D position)
{
    m_points << position;
    QList<QVector2D> curve = leastSquaresFitCurve(m_points);

    if (!m_segment) {
        m_segment = new VectorPath::CubicCurveSegment({}, {}, {});
        m_pathItem->segments << m_segment;
    }
    m_pathItem->startPoint = curve[0];
    m_segment->relB = curve[1];
    m_segment->relC = curve[3]-curve[0]+curve[2];
    m_segment->relD = curve[3]-curve[0];
    m_pathItem->setNeedsSync();
}

void CurvePenTool::mouseRelease()
{
    m_points.clear();
    m_segment = nullptr;
    m_pathItem = nullptr;
}

#include "curvepentool.h"

#include <QDebug>

#include "canvas.h"
#include "scene/items/vectorpathsceneitem.h"

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

QList<QVector2D> leastSquaresFitCurveFixed(QList<QVector2D> points) {
    QList<float> Ts(points.size());
    Ts[0] = 0;
    for (int i = 1; i < points.size(); i++) {
        Ts[i] = Ts[i-1] + points[i-1].distanceToPoint(points[i]);
    }
    for (int i = 1; i < points.size(); i++) {
        Ts[i] /= Ts.last();
    }
    int last = points.size()-1;

    float A = 0, B = 0, C = 0, D = 0;
    for (const float &t : std::as_const(Ts)) {
        double Bs[4] = { qPow(1-t, 3), 3*qPow(1-t, 2)*t, 3*(1-t)*qPow(t, 2), qPow(t, 3) };
        A += Bs[1]*Bs[1];
        B += Bs[1]*Bs[2];
        C += Bs[2]*Bs[1];
        D += Bs[2]*Bs[2];
    }

    if (A*D == B*C) {
        qDebug() << "failed to fit. is it even possible?";
        QList<QVector2D> curve;
        curve << points[0] << (points[last]-points[0])/3 << (points[0]-points[last])/3 << points[last];
        return curve;
    }

    QGenericMatrix<2, 2, float> tMatrixT;
    tMatrixT(0, 0) = D/(A*D-B*C);
    tMatrixT(0, 1) = -B/(A*D-B*C);
    tMatrixT(1, 0) = -C/(A*D-B*C);
    tMatrixT(1, 1) = A/(A*D-B*C);

    QGenericMatrix<1, 2, float> xMatrix;
    QGenericMatrix<1, 2, float> yMatrix;
    for (int i = 0; i < 2; i++) {
        xMatrix(i, 0) = 0;
        yMatrix(i, 0) = 0;
    }

    for (int i = 0; i < points.size(); i++) {
        float t = Ts[i];
        double B[4] = { qPow(1-t, 3), 3*qPow(1-t, 2)*t, 3*(1-t)*qPow(t, 2), qPow(t, 3) };
        for (int j = 0; j < 2; j++) {
            xMatrix(j, 0) += B[j+1] * points[i].x() - B[0] * B[j+1] * points[0].x() - B[3] * B[j+1] * points[last].x();
            yMatrix(j, 0) += B[j+1] * points[i].y() - B[0] * B[j+1] * points[0].y() - B[3] * B[j+1] * points[last].y();
        }
    }

    QGenericMatrix<1, 2, float> xCM = tMatrixT*xMatrix;
    QGenericMatrix<1, 2, float> yCM = tMatrixT*yMatrix;

    QList<QVector2D> curve;
    curve << points[0] << QVector2D(xCM(0, 0), yCM(0, 0))-points[0]
          << QVector2D(xCM(1, 0), yCM(1, 0))-points[last] << points[last];
    return curve;
}

void CurvePenTool::mousePress(QVector2D position)
{
    m_points << position;
    m_pathItem = new VectorPathSceneItem(position, {});
    m_canvas->currentScene()->addItem(m_pathItem);
}

void CurvePenTool::mouseMove(QVector2D position)
{
    m_points << position;
    QList<QVector2D> curve = leastSquaresFitCurveFixed(m_points);

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

#include "curvepentool.h"

#include <QDebug>

#include "canvas.h"
#include "scene/items/vectorpathsceneitem.h"

struct QuadCurve {
    // coords are absolute
    QVector2D a, b, c;

    inline static QuadCurve fromLine(QVector2D a, QVector2D d) {
        return {a, (a+d)/2, d};
    }

    QVector2D operator[] (float t) {
        return qPow(1-t, 2)*a + 2*(1-t)*t*b + qPow(t, 2)*c;
    }
};

struct CubicCurve {
    // coords are absolute
    QVector2D a, b, c, d;

    // inline static CubicCurve fromAbsolute(QVector2D a, QVector2D absB, QVector2D absC, QVector2D d) {
    //     return {a, absB-a, absC-d, d};
    // }

    inline static CubicCurve fromLine(QVector2D a, QVector2D d) {
        return {a, (2*a+d)/3, (a+2*d)/3, d};
    }

    QVector2D operator[] (float t) {
        return qPow(1-t, 3)*a + 3*qPow(1-t, 2)*t*b + 3*(1-t)*qPow(t, 2)*c + qPow(t, 3)*d;
    }

    QuadCurve derivate() {
        return {-3*a+3*b, -3*b+3*c, -3*c+3*d};
    }
};

float angleBetweenVectors(QVector2D v1, QVector2D v2) {
    if (v1.isNull() || v2.isNull())
        return 0;

    return QVector2D::dotProduct(v1, v2)/v1.length()/v2.length();
}

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

CubicCurve leastSquaresFitCurveFixed(QList<QVector2D> points, QList<float> t) {
    int last = points.size()-1;

    float A = 0, B = 0, C = 0, D = 0;
    for (int i = 0; i < points.size(); i++) {
        double Bs[4] = { qPow(1-t[i], 3), 3*qPow(1-t[i], 2)*t[i], 3*(1-t[i])*qPow(t[i], 2), qPow(t[i], 3) };
        A += Bs[1]*Bs[1];
        B += Bs[1]*Bs[2];
        C += Bs[2]*Bs[1];
        D += Bs[2]*Bs[2];
    }

    if (A*D == B*C) {
        qDebug() << "failed to fit. is it even possible?" << points.size();
        return CubicCurve::fromLine(points[0], points[last]);
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
        double B[4] = { qPow(1-t[i], 3), 3*qPow(1-t[i], 2)*t[i], 3*(1-t[i])*qPow(t[i], 2), qPow(t[i], 3) };
        for (int j = 0; j < 2; j++) {
            xMatrix(j, 0) += B[j+1] * points[i].x() - B[0] * B[j+1] * points[0].x() - B[3] * B[j+1] * points[last].x();
            yMatrix(j, 0) += B[j+1] * points[i].y() - B[0] * B[j+1] * points[0].y() - B[3] * B[j+1] * points[last].y();
        }
    }

    QGenericMatrix<1, 2, float> xCM = tMatrixT*xMatrix;
    QGenericMatrix<1, 2, float> yCM = tMatrixT*yMatrix;

    return CubicCurve(points[0], QVector2D(xCM(0, 0), yCM(0, 0)),
                      QVector2D(xCM(1, 0), yCM(1, 0)), points[last]);
}

CubicCurve leastSquaresFitCurveFixedIgnoringLastFive(QList<QVector2D> points, QList<float> t) {
    int realLast = points.size()-1;
    int last = points.size() < 5 ? points.size()-1 : points.size() < 15 ? points.size()-1-(points.size()-5)/2 : points.size()-1-5;
    // qDebug() << realLast << last;

    float A = 0, B = 0, C = 0, D = 0;
    for (int i = 0; i <= last; i++) {
        double Bs[4] = { qPow(1-t[i], 3), 3*qPow(1-t[i], 2)*t[i], 3*(1-t[i])*qPow(t[i], 2), qPow(t[i], 3) };
        A += Bs[1]*Bs[1];
        B += Bs[1]*Bs[2];
        C += Bs[2]*Bs[1];
        D += Bs[2]*Bs[2];
    }

    if (A*D == B*C) {
        qDebug() << "failed to fit. is it even possible?";
        return CubicCurve::fromLine(points[0], points[last]);
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

    for (int i = 0; i <= last; i++) {
        double B[4] = { qPow(1-t[i], 3), 3*qPow(1-t[i], 2)*t[i], 3*(1-t[i])*qPow(t[i], 2), qPow(t[i], 3) };
        for (int j = 0; j < 2; j++) {
            xMatrix(j, 0) += B[j+1] * points[i].x() - B[0] * B[j+1] * points[0].x() - B[3] * B[j+1] * points[last].x();
            yMatrix(j, 0) += B[j+1] * points[i].y() - B[0] * B[j+1] * points[0].y() - B[3] * B[j+1] * points[last].y();
        }
    }

    QGenericMatrix<1, 2, float> xCM = tMatrixT*xMatrix;
    QGenericMatrix<1, 2, float> yCM = tMatrixT*yMatrix;

    return CubicCurve(points[0], QVector2D(xCM(0, 0), yCM(0, 0)),
                      QVector2D(xCM(1, 0), yCM(1, 0)), points[last]);
}

float computeMaxError(QList<QVector2D> points, QList<float> t, CubicCurve curve) {
    float maxError = 0;
    for (int i = 0; i < points.size(); i++) {
        QVector2D deltaP = curve[t[i]] - points[i];
        maxError = qMax(deltaP.length(), maxError);
    }
    return maxError;
}

std::pair<float, int> computeMaxAngleError(QList<QVector2D> points, QList<float> t, CubicCurve curve) {
    float maxError = 0;
    int maxI = 0;
    QuadCurve derivate = curve.derivate();
    for (int i = 0; i < points.size()-1; i++) {
        double angle = angleBetweenVectors(derivate[t[i]], points[i+1]-points[i]);
        if (1-qAbs(angle) > maxError) {
            maxI = i;
            // qDebug() << angleBetweenVectors(derivate[t[i]], points[i+1]-points[i]) << i;
            maxError = 1-qAbs(angle);
        }
    }
    return {maxError, maxI};
}

CurvePenTool::CurvePenTool(Canvas *canvas)
    : Tool(canvas) {}

void CurvePenTool::mousePress(QVector2D position)
{
    m_points << position;
    m_pathItem = new VectorPathSceneItem(position, {});
    m_canvas->currentScene()->addItem(m_pathItem);
}

void CurvePenTool::mouseMove(QVector2D position)
{
    m_points << position;

    if (m_points.size() < 4)
        return;

    QList<float> Ts(m_points.size());
    Ts[0] = 0;
    for (int i = 1; i < m_points.size(); i++) {
        Ts[i] = Ts[i-1] + m_points[i-1].distanceToPoint(m_points[i]);
    }
    for (int i = 1; i < m_points.size(); i++) {
        Ts[i] /= Ts.last();
    }

    CubicCurve curve = leastSquaresFitCurveFixed(m_points, Ts);
    // int aheadPoints = m_points.size() < 5 ? 0 : m_points.size() < 15 ? (m_points.size()-5)/2 : 5;
    // bool errorGrows = aheadPoints > 4;
    // for (int i = m_points.size()-aheadPoints; i < m_points.size() && errorGrows; i++)
    //     errorGrows = curve[Ts[i-1]].distanceToPoint(m_points[i-1]) < curve[Ts[i]].distanceToPoint(m_points[i]);
    // if (errorGrows && curve[Ts[m_points.size()-1]].distanceToPoint(m_points[m_points.size()-1]) > 15/m_canvas->scale()) {
    //     for (int i = 0; i < aheadPoints+2; i++)
    //         m_points[i] = m_points[m_points.size()-aheadPoints-2+i];
    //     // m_points[0] = m_points[m_points.size()-2];
    //     // m_points[1] = m_points[m_points.size()-1];
    //     m_points.resize(aheadPoints+2);
    //     m_segment = nullptr;
    //     return;
    // }
    double maxError = computeMaxError(m_points, Ts, curve);
    if (maxError > 10/m_canvas->scale()) {
        qDebug() << "break";
        m_points[0] = m_points[m_points.size()-2];
        m_points[1] = m_points[m_points.size()-1];
        m_points.resize(2);
        m_segment = nullptr;
        return;
    }
    // qDebug() << maxError;
    std::pair<double, int> maxAngleError = computeMaxAngleError(m_points, Ts, curve);
    if (maxAngleError.first > 0.75 && maxAngleError.second > 4 && m_segment) {
        qDebug() << "break at" << maxAngleError.second << "/" << m_points.size();
        QList firstPart = m_points.first(maxAngleError.second+1);
        CubicCurve curve = leastSquaresFitCurveFixed(firstPart, Ts);
        m_segment->b = curve.b;
        m_segment->c = curve.c;
        m_segment->d = curve.d;
        qDebug() << curve.a << curve.b << curve.c << curve.d;

        int newCurvePointsCount = m_points.size()-maxAngleError.second+1;
        for (int i = 0; i < newCurvePointsCount; i++)
            m_points[i] = m_points[m_points.size()-newCurvePointsCount+i];
        m_points.resize(newCurvePointsCount);
        m_segment = nullptr;
        return;
    }
    // if (m_points.size() >= 7) {
    //     QList firstPoints = m_points.first(m_points.size()-4+1);
    //     CubicCurve firstCurve = leastSquaresFitCurveFixed(firstPoints, Ts);
    //     QList lastTs = Ts.last(4);
    //     QList lastPoints = m_points.last(4);
    //     CubicCurve lastCurve = leastSquaresFitCurveFixed(lastPoints, lastTs);
    //     if (angleBetweenVectors(firstCurve.d-firstCurve.c, lastCurve.b-lastCurve.a) < 0.001) {
    //         qDebug() << (firstCurve.d-firstCurve.c).normalized() << (lastCurve.b-lastCurve.a).normalized() << angleBetweenVectors(firstCurve.d-firstCurve.c, lastCurve.b-lastCurve.a);
    //         m_segment->relB = firstCurve.b-firstCurve.a;
    //         m_segment->relC = firstCurve.c-firstCurve.a;
    //         m_segment->relD = firstCurve.d-firstCurve.a;

    //         int newCurvePointsCount = 4;
    //         for (int i = 0; i < newCurvePointsCount; i++)
    //             m_points[i] = m_points[m_points.size()-newCurvePointsCount+i];
    //         m_points.resize(newCurvePointsCount);
    //         m_segment = nullptr;
    //         return;
    //     }
    // }

    if (!m_segment) {
        m_segment = new VectorPath::CubicCurveSegment({}, {}, {});
        m_pathItem->segments << m_segment;
    }
    // m_pathItem->startPoint = curve[0];
    m_segment->b = curve.b;
    m_segment->c = curve.c;
    m_segment->d = curve.d;
    m_pathItem->setNeedsSync();
}

void CurvePenTool::mouseRelease()
{
    qDebug() << m_pathItem->segments.size();
    m_points.clear();
    m_segment = nullptr;
    m_pathItem = nullptr;
}

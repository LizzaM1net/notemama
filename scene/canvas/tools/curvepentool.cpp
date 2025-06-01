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

// hugs to https://iquilezles.org/articles/bezierbbox/
QRectF bboxBezier(CubicCurve curve)
{
    QVector2D mi(qMin(curve.a.x(), curve.d.x()), qMin(curve.a.y(), curve.d.y()));
    QVector2D ma(qMax(curve.a.x(), curve.d.x()), qMax(curve.a.y(), curve.d.y()));

    QVector2D c = -1.0*curve.a + 1.0*curve.b;
    QVector2D b =  1.0*curve.a - 2.0*curve.b + 1.0*curve.c;
    QVector2D a = -1.0*curve.a + 3.0*curve.b - 3.0*curve.c + 1.0*curve.d;

    QVector2D h = b*b - a*c;

    if (h.x() > 0.0) {
        h.setX(qSqrt(h.x()));
        float t = (-b.x() - h.x())/a.x();
        if (t > 0.0 && t < 1.0) {
            float s = 1.0-t;
            float q = curve[t].x();
            mi.setX(qMin(mi.x(),q));
            ma.setX(qMax(ma.x(),q));
        }
        t = (-b.x() + h.x())/a.x();
        if (t > 0.0 && t < 1.0) {
            float q = curve[t].x();
            mi.setX(qMin(mi.x(),q));
            ma.setX(qMax(ma.x(),q));
        }
    }

    if (h.y() > 0.0) {
        h.setY(qSqrt(h.y()));
        float t = (-b.y() - h.y())/a.y();
        if (t > 0.0 && t < 1.0) {
            float q = curve[t].y();
            mi.setY(qMin(mi.y(),q));
            ma.setY(qMax(ma.y(),q));
        }
        t = (-b.y() + h.y())/a.y();
        if (t > 0.0 && t < 1.0) {
            float q = curve[t].y();
            mi.setY(qMin(mi.y(),q));
            ma.setY(qMax(ma.y(),q));
        }
    }

    return QRectF(mi.toPointF(), ma.toPointF());
}

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

float importanceWeight(float t) {
    return 4 * t * (1 - t);
}

CubicCurve leastSquaresFitCurveFixed(QList<QVector2D> points, QList<float> t) {
    int last = points.size()-1;

    float A = 0, B = 0, C = 0, D = 0;
    for (int i = 0; i < points.size(); i++) {
        float w = importanceWeight(t[i]);
        double Bs[4] = { qPow(1-t[i], 3), 3*qPow(1-t[i], 2)*t[i], 3*(1-t[i])*qPow(t[i], 2), qPow(t[i], 3) };
        A += w*Bs[1]*Bs[1];
        B += w*Bs[1]*Bs[2];
        C += w*Bs[2]*Bs[1];
        D += w*Bs[2]*Bs[2];
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
        float w = importanceWeight(t[i]);
        double B[4] = { qPow(1-t[i], 3), 3*qPow(1-t[i], 2)*t[i], 3*(1-t[i])*qPow(t[i], 2), qPow(t[i], 3) };
        for (int j = 0; j < 2; j++) {
            xMatrix(j, 0) += w * (B[j+1] * points[i].x() - B[0] * B[j+1] * points[0].x() - B[3] * B[j+1] * points[last].x());
            yMatrix(j, 0) += w * (B[j+1] * points[i].y() - B[0] * B[j+1] * points[0].y() - B[3] * B[j+1] * points[last].y());
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
    if (m_points.last().distanceToPoint(position) < 0.5)
        return;

    m_points << position;

    if (m_points.size() < 4)
        return;

    QList<float> Ts(m_points.size());
    Ts[0] = 0;
    for (int i = 1; i < m_points.size(); i++) {
        Ts[i] = Ts[i-1] + qSqrt(m_points[i-1].distanceToPoint(m_points[i]));
    }
    for (int i = 1; i < m_points.size(); i++) {
        Ts[i] /= Ts.last();
    }

    CubicCurve curve = leastSquaresFitCurveFixed(m_points, Ts);

    double maxError = computeMaxError(m_points, Ts, curve);
    if (maxError > 10/m_canvas->scale()) {
        qDebug() << "break";
        m_pathItem->m_boundingRect = m_pathItem->m_boundingRect.united(bboxBezier(curve));
        m_points[0] = m_points[m_points.size()-2];
        m_points[1] = m_points[m_points.size()-1];
        m_points.resize(2);
        m_segment = nullptr;
        return;
    }

    std::pair<double, int> maxAngleError = computeMaxAngleError(m_points, Ts, curve);
    if (maxAngleError.first > 0.75 && maxAngleError.second > 4 && m_segment) {
        qDebug() << "break at" << maxAngleError.second << "/" << m_points.size();
        m_pathItem->m_boundingRect = m_pathItem->m_boundingRect.united(bboxBezier(curve));
        QList firstPart = m_points.first(maxAngleError.second+1);
        CubicCurve curve = leastSquaresFitCurveFixed(firstPart, Ts);
        m_segment->b = curve.b;
        m_segment->c = curve.c;
        m_segment->d = curve.d;
        // qDebug() << curve.a << curve.b << curve.c << curve.d;

        int newCurvePointsCount = m_points.size()-maxAngleError.second+1;
        for (int i = 0; i < newCurvePointsCount; i++)
            m_points[i] = m_points[m_points.size()-newCurvePointsCount+i];
        m_points.resize(newCurvePointsCount);
        m_segment = nullptr;
        return;
    }

    if (!m_segment) {
        m_segment = new VectorPath::CubicCurveSegment({}, {}, {});
        m_pathItem->segments << m_segment;
    }

    m_segment->b = curve.b;
    m_segment->c = curve.c;
    m_segment->d = curve.d;
    m_pathItem->setNeedsSync();
}

void CurvePenTool::mouseRelease()
{
    QVector2D startPoint = m_pathItem->startPoint;
    if (m_pathItem->segments.size() > 1)
        startPoint = m_pathItem->segments[m_pathItem->segments.size()-2]->lastPoint();
    CubicCurve curve(startPoint, m_segment->b, m_segment->c, m_segment->d);
    m_pathItem->m_boundingRect = m_pathItem->m_boundingRect.united(bboxBezier(curve));
    m_points.clear();
    m_segment = nullptr;
    m_pathItem = nullptr;
}

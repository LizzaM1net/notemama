#ifndef CURVEPENTOOL_H
#define CURVEPENTOOL_H

#include "tool.h"

#include <QVector2D>

class Canvas;
class VectorPathCanvasItem;
namespace VectorPath {
    struct CubicCurveSegment;
}

class CurvePenTool : public Tool {
public:
    CurvePenTool(Canvas *canvas);

    void mousePress(QVector2D position) override;
    void mouseMove(QVector2D position) override;
    void mouseRelease() override;

private:
    QList<QVector2D> m_points;
    VectorPath::CubicCurveSegment *m_segment = nullptr;
    VectorPathCanvasItem *m_pathItem = nullptr;
};

#endif // CURVEPENTOOL_H

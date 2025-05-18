#include "selecttool.h"

#include "canvas.h"
#include "items/vectorpathsceneitem.h"

bool pointInRect(QVector2D rectA, QVector2D rectB, QVector2D C) {
    return C.x() >= qMin(rectA.x(), rectB.x())
           && C.x() <= qMax(rectA.x(), rectB.x())
           && C.y() >= qMin(rectA.y(), rectB.y())
           && C.y() <= qMax(rectA.y(), rectB.y());
}

SelectTool::SelectTool(Canvas *canvas)
    : Tool(canvas) {}

void SelectTool::mousePress(QVector2D position) {
    m_firstPoint = position;
    m_secondPoint = position;
    m_item = new VectorPathSceneItem(position, {new VectorPath::LineSegment(position),
                                                new VectorPath::LineSegment(position),
                                                new VectorPath::LineSegment(position),
                                                new VectorPath::LineSegment(position)});
    m_canvas->currentScene()->addItem(m_item);
}

void SelectTool::mouseMove(QVector2D position) {
    m_secondPoint = position;
    static_cast<VectorPath::LineSegment*>(m_item->segments[0])->end.setX(position.x());
    static_cast<VectorPath::LineSegment*>(m_item->segments[1])->end = position;
    static_cast<VectorPath::LineSegment*>(m_item->segments[2])->end.setY(position.y());
    m_item->setNeedsSync();
}

QString svgPoint(QVector2D point) {
    QString ret;
    ret += QString::number(point.x()) + ", " + QString::number(point.y()) + " ";
    return ret;
}

void SelectTool::mouseRelease() {
    QList<VectorPathSceneItem*> selectedItems;
    for (SceneItem *item : std::as_const(m_canvas->currentScene()->items)) {
        if (item == m_item)
            continue;

        if (VectorPathSceneItem *pathItem = dynamic_cast<VectorPathSceneItem*>(item)) {
            if (!pointInRect(m_firstPoint, m_secondPoint, pathItem->startPoint))
                continue;

            bool inside = true;
            for (VectorPath::Segment *seg : std::as_const(pathItem->segments)) {
                if (!pointInRect(m_firstPoint, m_secondPoint, seg->lastPoint())) {
                    inside = false;
                    break;
                }
            }

            if (inside)
                selectedItems << pathItem;
        }
    }
    QString svg;
    svg += "<svg>";
    if (!selectedItems.isEmpty()) {
        for (VectorPathSceneItem *item : selectedItems) {
            svg += "<path style=\"fill:none;stroke:#000000;stroke-width:1\" d=\"";
            svg += "M " + svgPoint(item->startPoint);
            for (VectorPath::Segment *segment : item->segments) {
                if (VectorPath::CubicCurveSegment *cubicSegment = dynamic_cast<VectorPath::CubicCurveSegment*>(segment)) {
                    svg += "C " + svgPoint(cubicSegment->b) + svgPoint(cubicSegment->c) + svgPoint(cubicSegment->d);
                }
            }
            svg += "\" />";
        }
    }
    svg += "</svg>";
    qDebug() << qPrintable(svg);

    m_canvas->currentScene()->removeItem(m_item);
    delete m_item;
}

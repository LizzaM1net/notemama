#include "pentool.h"

#include "canvas.h"
#include "items/vectorpathcanvasitem.h"

PenTool::PenTool(Canvas *canvas)
    : Tool(canvas) {}

void PenTool::mousePress(QVector2D position)
{
    m_pathItem = new VectorPathCanvasItem(position, {});
    canvasItems() << m_pathItem;
    m_lastPoint = position;
    updateCanvas();
}

void PenTool::mouseMove(QVector2D position)
{
    m_pathItem->segments << new VectorPath::LineSegment(position-m_lastPoint);
    m_pathItem->changed = true;
    m_lastPoint = position;
    updateCanvas();
}

void PenTool::mouseRelease()
{
    m_pathItem = nullptr;
}

#include "pentool.h"

#include "canvas.h"
#include "scene/items/vectorpathsceneitem.h"

PenTool::PenTool(Canvas *canvas)
    : Tool(canvas) {}

void PenTool::mousePress(QVector2D position)
{
    m_pathItem = new VectorPathSceneItem(position, {});
    m_canvas->currentScene()->addItem(m_pathItem);
    m_lastPoint = position;
}

void PenTool::mouseMove(QVector2D position)
{
    m_pathItem->segments << new VectorPath::LineSegment(position-m_lastPoint);
    m_pathItem->setNeedsSync();
    m_lastPoint = position;
}

void PenTool::mouseRelease()
{
    m_pathItem = nullptr;
}

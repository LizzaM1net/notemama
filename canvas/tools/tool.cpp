#include "tool.h"

#include "canvas.h"

Tool::Tool(Canvas *canvas)
    : m_canvas(canvas) {}

QList<VectorPathCanvasItem *> &Tool::canvasItems()
{
    return m_canvas->m_items;
}

void Tool::updateCanvas()
{
    m_canvas->update();
}

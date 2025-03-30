#include "canvasitem.h"

#include "canvas.h"

CanvasItem::CanvasItem() {}

bool CanvasItem::trySync()
{
    if (m_needsSync) {
        m_needsSync = false;
        return true;
    }
    return false;
}

void CanvasItem::setNeedsSync()
{
    if (m_canvas)
        m_canvas->update();

    m_needsSync = true;
}

void CanvasItem::setCanvas(Canvas *canvas)
{
    m_canvas = canvas;
}

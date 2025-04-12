#include "tool.h"

#include "canvas.h"

Tool::Tool(Canvas *canvas)
    : m_canvas(canvas) {}

QString Tool::name()
{
    return metaObject()->className();
}

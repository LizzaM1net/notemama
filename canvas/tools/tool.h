#ifndef TOOL_H
#define TOOL_H

#include "items/vectorpathcanvasitem.h"

class Canvas;
class QVector2D;

class Tool {
public:
    Tool(Canvas *canvas);

    virtual void mousePress(QVector2D position) = 0;
    virtual void mouseMove(QVector2D position) = 0;
    virtual void mouseRelease() = 0;

protected:
    Canvas *m_canvas = nullptr;
};

#endif // TOOL_H

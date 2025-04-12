#include "fakedrawtool.h"

#include "canvas.h"
#include "items/vectorpathcanvasitem.h"

FakeDrawTool::FakeDrawTool(Canvas *canvas, Tool *realTool)
    : Tool(canvas)
    , m_realTool(realTool) {}

void FakeDrawTool::mousePress(QVector2D position)
{
    if (!m_fakePressed) {
        m_realTool->mousePress(position);

        m_pathItem = new VectorPathCanvasItem(position, {});
        m_canvas->addItem(m_pathItem);
        m_lastPoint = position;

        m_fakePressed = true;
    } else {
        m_realTool->mouseMove(position);

        m_pathItem->segments << new VectorPath::LineSegment(position-m_lastPoint);
        m_pathItem->setNeedsSync();
        m_lastPoint = position;
    }
    // tapCount++;
    // if (tapCount == 4) {
    //     m_realTool->mouseRelease();
    //     tapCount = 0;
    //     m_pathItem = nullptr;
    //     lastPoint = QVector2D();
    //     m_fakePressed = false;
    // }
}

void FakeDrawTool::mouseMove(QVector2D position) {}

void FakeDrawTool::mouseRelease() {}

#include "fakedrawtool.h"

#include "canvas.h"
#include "scene/items/vectorpathsceneitem.h"

FakeDrawTool::FakeDrawTool(Canvas *canvas, Tool *realTool)
    : Tool(canvas)
    , m_realTool(realTool) {}

void FakeDrawTool::mousePress(QVector2D position)
{
    if (m_fakePressed) {
        simulateMove(position);
    } else {
        simulatePress(position);

        if (m_clickAsMove)
            m_fakePressed = true;
    }

    m_lastPoint = position;
}

void FakeDrawTool::mouseMove(QVector2D position) {
    if (!m_clickAsMove && m_realTool) {
        simulateMove(position);
        m_lastPoint = position;
    }
}

void FakeDrawTool::mouseRelease() {
    if (!m_clickAsMove && m_realTool)
        m_realTool->mouseRelease();
}

Tool *FakeDrawTool::realTool() const {
    return m_realTool;
}

void FakeDrawTool::setRealTool(Tool *realTool) {
    if (m_realTool == realTool)
        return;

    if (realTool == this) {
        qWarning() << "Trying to set FakeDrawTool's realTool to itself";
        return;
    }

    if (m_fakePressed) {
        m_realTool->mouseRelease();
        realTool->mousePress(m_lastPoint);
    }

    m_realTool = realTool;
    emit realToolChanged();
}

bool FakeDrawTool::drawTrail() const {
    return m_drawTrail;
}

void FakeDrawTool::setDrawTrail(bool drawTrail) {
    if (m_drawTrail == drawTrail)
        return;

    if (!drawTrail && m_trailItem)
        m_trailItem = nullptr;

    m_drawTrail = drawTrail;
    emit drawTrailChanged();
}

bool FakeDrawTool::clickAsMove() const {
    return m_clickAsMove;
}

void FakeDrawTool::setClickAsMove(bool clickAsMove) {
    if (m_clickAsMove == clickAsMove)
        return;

    if (!clickAsMove && m_fakePressed) {
        m_fakePressed = false;

        if (m_realTool)
            m_realTool->mouseRelease();
    }

    m_clickAsMove = clickAsMove;
    emit clickAsMoveChanged();
}

void FakeDrawTool::simulatePress(QVector2D position) {
    m_realTool->mousePress(position);

    if (m_drawTrail)
        createTrailItem(position);
}

void FakeDrawTool::simulateMove(QVector2D position) {
    m_realTool->mouseMove(position);

    if (m_drawTrail) {
        if (m_trailItem) {
            m_trailItem->segments << new VectorPath::LineSegment(position-m_lastPoint);
            m_trailItem->setNeedsSync();
        } else
            createTrailItem(position);
    }
}

void FakeDrawTool::createTrailItem(QVector2D position) {
    m_trailItem = new VectorPathSceneItem(position, {});
    m_canvas->currentScene()->addItem(m_trailItem);
}

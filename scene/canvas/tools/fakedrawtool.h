#ifndef FAKEDRAWTOOL_H
#define FAKEDRAWTOOL_H

#include "tool.h"

#include <QVector2D>

class VectorPathSceneItem;

class FakeDrawTool : public Tool
{
public:
    FakeDrawTool(Canvas *canvas, Tool *realTool);

    void mousePress(QVector2D position) override;
    void mouseMove(QVector2D position) override;
    void mouseRelease() override;

private:
    Tool *m_realTool = nullptr;
    bool m_fakePressed = false;

    QVector2D m_lastPoint;
    // int m_tapCount = 0;

    VectorPathSceneItem *m_pathItem;
};

#endif // FAKEDRAWTOOL_H

#ifndef PENTOOL_H
#define PENTOOL_H

#include "tool.h"

#include <QVector2D>

class Canvas;
class VectorPathSceneItem;

class PenTool : public Tool {
public:
    PenTool(Canvas *canvas);

    void mousePress(QVector2D position) override;
    void mouseMove(QVector2D position) override;
    void mouseRelease() override;

private:
    VectorPathSceneItem *m_pathItem = nullptr;
    QVector2D m_lastPoint;
};

#endif // PENTOOL_H

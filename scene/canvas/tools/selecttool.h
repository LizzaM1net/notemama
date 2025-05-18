#ifndef SELECTTOOL_H
#define SELECTTOOL_H

#include "tool.h"

#include <QVector2D>

class VectorPathSceneItem;

class SelectTool : public Tool {
    Q_OBJECT

public:
    SelectTool(Canvas *canvas);

    void mousePress(QVector2D position) override;
    void mouseMove(QVector2D position) override;
    void extracted(VectorPathSceneItem *&pathItem);
    void mouseRelease() override;

private:
    VectorPathSceneItem *m_item = nullptr;
    QVector2D m_firstPoint;
    QVector2D m_secondPoint;
};

#endif // SELECTTOOL_H

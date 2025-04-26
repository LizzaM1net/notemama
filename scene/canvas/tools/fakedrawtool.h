#ifndef FAKEDRAWTOOL_H
#define FAKEDRAWTOOL_H

#include "tool.h"

#include <QVector2D>

class VectorPathSceneItem;

class FakeDrawTool : public Tool {
    Q_OBJECT

    Q_PROPERTY(Tool *realTool READ realTool WRITE setRealTool NOTIFY realToolChanged FINAL)
    Q_PROPERTY(bool drawTrail READ drawTrail WRITE setDrawTrail NOTIFY drawTrailChanged FINAL)
    Q_PROPERTY(bool clickAsMove READ clickAsMove WRITE setClickAsMove NOTIFY clickAsMoveChanged FINAL)

public:
    FakeDrawTool(Canvas *canvas, Tool *realTool);

    void mousePress(QVector2D position) override;
    void mouseMove(QVector2D position) override;
    void mouseRelease() override;

    Tool *realTool() const;
    void setRealTool(Tool *realTool);

    bool drawTrail() const;
    void setDrawTrail(bool drawTrail);

    bool clickAsMove() const;
    void setClickAsMove(bool clickAsMove);

signals:
    void realToolChanged();
    void drawTrailChanged();

    void clickAsMoveChanged();

private:
    void simulatePress(QVector2D position);
    void simulateMove(QVector2D position);

    void createTrailItem(QVector2D position);

    Tool *m_realTool = nullptr;
    bool m_drawTrail = false;
    bool m_clickAsMove = false;

    bool m_fakePressed = false;

    QVector2D m_lastPoint;
    // int m_tapCount = 0;

    VectorPathSceneItem *m_trailItem = nullptr;
};

#endif // FAKEDRAWTOOL_H

#ifndef POINTSCENEITEM_H
#define POINTSCENEITEM_H

#include "sceneitem.h"

#include <QElapsedTimer>
#include <QVector2D>


class QRhiBuffer;


class PointSceneItem : public SceneItem
{

public:

    PointSceneItem(QVector2D m_xy) : m_xy(m_xy) { }
    ~PointSceneItem();

    void synchronize(QRhi* rhi, QRhiResourceUpdateBatch* updateBatch) override;
    void render(QRhiCommandBuffer* cb) override;
    QRectF boundingRect() override;
private:

    QElapsedTimer m_timer;
    QRhiBuffer* m_buffer = nullptr;
    QVector2D m_xy{0, 0};
    int countpoint{0};
    double m_radius{0.0};
};


#endif // POINTSCENEITEM_H

#ifndef SPINNERSCENEITEM_H
#define SPINNERSCENEITEM_H

#include "sceneitem.h"

#include <QElapsedTimer>

class QRhiBuffer;

class SpinnerSceneItem : public SceneItem
{
public:
    SpinnerSceneItem();
    ~SpinnerSceneItem();

    void synchronize(QRhi *rhi, QRhiResourceUpdateBatch *updateBatch) override;
    void render(QRhiCommandBuffer *cb) override;

    QRectF boundingRect() override;

private:
    QElapsedTimer m_timer;

    QRhiBuffer *m_buffer = nullptr;
};

#endif // SPINNERSCENEITEM_H

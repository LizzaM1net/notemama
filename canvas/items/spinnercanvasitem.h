#ifndef SPINNERCANVASITEM_H
#define SPINNERCANVASITEM_H

#include "canvasitem.h"

#include <QElapsedTimer>

class QRhiBuffer;

class SpinnerCanvasItem : public CanvasItem
{
public:
    SpinnerCanvasItem();
    ~SpinnerCanvasItem();

    void synchronize(QRhi *rhi, QRhiResourceUpdateBatch *updateBatch) override;
    void render(QRhiCommandBuffer *cb) override;

    QRectF boundingRect();

private:
    QElapsedTimer m_timer;

    QRhiBuffer *m_buffer = nullptr;
};

#endif // SPINNERCANVASITEM_H

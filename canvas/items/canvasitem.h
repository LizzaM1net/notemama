#ifndef CANVASITEM_H
#define CANVASITEM_H

#include <QRectF>

class QRhiResourceUpdateBatch;
class QRhiCommandBuffer;
class QRhi;

class Canvas;

class CanvasItem
{
public:
    CanvasItem();

    virtual void synchronize(QRhi *rhi, QRhiResourceUpdateBatch *updateBatch) = 0;
    virtual void render(QRhiCommandBuffer *cb) = 0;

    bool trySync();
    void setNeedsSync();

    virtual QRectF boundingRect() {
        return QRectF();
    }

    void setCanvas(Canvas *canvas);

protected:
    Canvas *m_canvas = nullptr;

private:
    bool m_needsSync = true;
};

#endif // CANVASITEM_H

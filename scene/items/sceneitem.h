#ifndef SCENEITEM_H
#define SCENEITEM_H

#include <QRectF>
#include <QList>

class QRhiResourceUpdateBatch;
class QRhiCommandBuffer;
class QRhi;

class Scene;

class SceneItem {
public:
    SceneItem();
    virtual ~SceneItem();

    virtual void synchronize(QRhi *rhi, QRhiResourceUpdateBatch *updateBatch) = 0;
    virtual void render(QRhiCommandBuffer *cb) = 0;

    bool trySync();
    void setNeedsSync();

    virtual QRectF boundingRect() = 0;

    void setScene(Scene *scene);

protected:
    Scene *m_scene = nullptr;

private:
    bool m_needsSync = true;
};

#endif // SCENEITEM_H

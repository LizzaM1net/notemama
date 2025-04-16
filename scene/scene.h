#ifndef SCENE_H
#define SCENE_H

#include <QObject>
#include <qqmlintegration.h>

class SceneItem;

class Scene;
class SceneObserver {
    friend class Scene;
public:
    SceneObserver();
    ~SceneObserver();

protected:
    virtual void itemChanged(SceneItem *item) = 0;

    void addObservedScene(Scene *scene);
    void removeObservedScene(Scene *scene);

private:
    QList<Scene *> observedScenes;
};

class Scene : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("unless...")

    friend class SceneObserver;

public:
    Scene();
    ~Scene();

    void addItem(SceneItem *item);

    void sendItemChanged(SceneItem *item);

    QList<SceneItem*> items;

private:
    QList<SceneObserver *> sceneObservers;
};

#endif // SCENE_H

#include "scene.h"

#include "items/sceneitem.h"

#include "items/spinnersceneitem.h"
#include "items/vectorpathsceneitem.h"
#include "items/pointsceneitem.h"

#include "nddgenerator/nddgenerator.h"

SceneObserver::SceneObserver() {}

SceneObserver::~SceneObserver() {
    for (Scene *scene : std::as_const(observedScenes))
        scene->sceneObservers.removeOne(this);
}

void SceneObserver::addObservedScene(Scene *scene) {
    if (scene == nullptr || observedScenes.contains(scene))
        return;
    observedScenes.append(scene);
    scene->sceneObservers.append(this);
}

void SceneObserver::removeObservedScene(Scene *scene) {
    if (scene == nullptr || !observedScenes.contains(scene))
        return;
    observedScenes.removeOne(scene);
    scene->sceneObservers.removeOne(this);
}

Scene::Scene() {

    nddgenerator generator(10000);
    generator.generate();
    for (int i = 0; i < 10000; ++i) {
        addItem(new PointSceneItem(*generator.normaldata[i]));
    }

}

Scene::~Scene() {
    for (SceneObserver *observer : std::as_const(sceneObservers))
        observer->observedScenes.removeOne(this);
    qDeleteAll(items);
}

void Scene::sendItemChanged(SceneItem *item) {
    for (SceneObserver *observer : std::as_const(sceneObservers))
        observer->itemChanged(item);
}

void Scene::sendItemRemoved(SceneItem *item) {
    for (SceneObserver *observer : std::as_const(sceneObservers))
        observer->itemRemoved(item);
}

void Scene::addItem(SceneItem *item)
{
    if (items.contains(item))
        return;

    item->setScene(this);
    items << item;
}

void Scene::removeItem(SceneItem *item)
{
    item->setScene(nullptr);
    items.removeOne(item);
    sendItemRemoved(item);
}

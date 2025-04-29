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

    addItem(new VectorPathSceneItem(QVector2D{-10, 0}, {new VectorPath::LineSegment(QVector2D{10, 10}),
                                                                        new VectorPath::LineSegment(QVector2D{10, -10}),
                                                                        new VectorPath::LineSegment(QVector2D{-10, -10}),
                                                                        new VectorPath::LineSegment(QVector2D{-10, 10})}));

    addItem(new VectorPathSceneItem(QVector2D{10, 10+200}, {new VectorPath::QuadCurveSegment(QVector2D{0, -200}, QVector2D{200, -200}),
                                                               new VectorPath::LineSegment(QVector2D{100, 0}),
                                                               new VectorPath::LineSegment(QVector2D{20, 20}),
                                                               new VectorPath::LineSegment(QVector2D{-20, 20}),
                                                               new VectorPath::QuadCurveSegment(QVector2D{200, 0}, QVector2D{200, 200})}));

    addItem(new VectorPathSceneItem(QVector2D{60, 60+200}, {new VectorPath::CubicCurveSegment(QVector2D{0, -110}, QVector2D{90, -200}, QVector2D{200, -200}),
                                                               new VectorPath::CubicCurveSegment(QVector2D{110, 0}, QVector2D{200, 90}, QVector2D{200, 200})}));


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

void Scene::addItem(SceneItem *item)
{
    item->setScene(this);
    items << item;
}

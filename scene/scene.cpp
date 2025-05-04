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

    addItem(new VectorPathSceneItem(QVector2D{-10, 0}, {new VectorPath::LineSegment(QVector2D{0, 10}),
                                                                        new VectorPath::LineSegment(QVector2D{10, 0}),
                                                                        new VectorPath::LineSegment(QVector2D{0, -10}),
                                                                        new VectorPath::LineSegment(QVector2D{-10, 0})}));

    addItem(new VectorPathSceneItem(QVector2D{10, 10+200}, {new VectorPath::QuadCurveSegment(QVector2D{10, 10}, QVector2D{10+200, 10}),
                                                               new VectorPath::LineSegment(QVector2D{10+300, 10}),
                                                               new VectorPath::LineSegment(QVector2D{10+320, 10+20}),
                                                               new VectorPath::LineSegment(QVector2D{10+300, 10+40}),
                                                               new VectorPath::QuadCurveSegment(QVector2D{10+500, 10+40}, QVector2D{10+500, 10+240})}));

    addItem(new VectorPathSceneItem(QVector2D{60, 60+200}, {new VectorPath::CubicCurveSegment(QVector2D{60, 60+90}, QVector2D{60+90, 60}, QVector2D{60+200, 60}),
                                                               new VectorPath::CubicCurveSegment(QVector2D{60+310, 60}, QVector2D{60+400, 60+90}, QVector2D{60+400, 60+200})}));


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

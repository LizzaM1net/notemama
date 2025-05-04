#include "sceneitem.h"

#include "scene/scene.h"

SceneItem::SceneItem() {}

SceneItem::~SceneItem() {}

bool SceneItem::trySync() {
    if (m_needsSync) {
        m_needsSync = false;
        return true;
    }
    return false;
}

void SceneItem::setNeedsSync() {
    if (m_scene)
        m_scene->sendItemChanged(this);

    m_needsSync = true;
}

void SceneItem::setScene(Scene *scene) {
    m_scene = scene;
}

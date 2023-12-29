#include "scene.h"

void Scene::simulate(int) {

}

void Scene::doAction(const Action & action) {
    sDoAction(action);
}

void Scene::registerAction(const int key_code, const std::string & action_name) {
    m_actionMap[key_code] = action_name;
}

const std::map<int, std::string> & Scene::getActionMap() const {
    return m_actionMap;
}

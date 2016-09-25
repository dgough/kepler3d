#include "stdafx.h"
#include "Scene.hpp"
#include "Camera.hpp"

#include <algorithm>

namespace kepler {
    Scene::Scene() {
    }
    Scene::~Scene() noexcept {
    }

    SceneRef Scene::create() {
        return MAKE_SHARED(Scene);
    }

    void Scene::addNode(NodeRef& node) {
        if (node == nullptr) return;

        if (NodeRef parent = node->_parent.lock()) {
            Node::removeFromList(parent->_children, node);
        }
        else if (auto oldScene = node->_scene.lock()) {
            oldScene->removeChild(node);
        }
        node->_parent.reset();
        SceneRef scene = shared_from_this();
        node->_scene = scene;
        _children.push_back(node);
        node->setAllChildrenScene(scene);
        node->parentChanged();
    }

    NodeRef Scene::createChild(const std::string& name) {
        auto node = Node::create(name);
        node->_scene = shared_from_this();
        _children.push_back(node);
        return node;
    }

    void Scene::createChildren(std::initializer_list<std::string> names) {
        for (const auto& name : names) {
            createChild(name);
        }
    }

    size_t Scene::childCount() const {
        return _children.size();
    }

    NodeRef Scene::childAt(size_t index) const {
        return _children.at(index);
    }

    NodeRef Scene::lastChild() const {
        auto count = _children.size();
        if (count == 0) return nullptr;
        return _children[count - 1];
    }

    const NodeList& Scene::children() const {
        return _children;
    }

    void Scene::removeChild(size_t index) {
        Node::removeChild(_children, index);
    }

    void Scene::removeChild(const NodeRef& child) {
        if (child) {
            Node::removeFromList(_children, child);
            child->clearParent();
        }
    }

    void Scene::moveNodesFrom(const SceneRef src) {
        if (src == nullptr) return;
        while (!src->_children.empty()) {
            auto child = src->childAt(0);
            addNode(child);
        }
    }

    NodeRef Scene::findFirstNodeByName(const std::string& name, bool recursive) const {
        for (const auto& child : _children) {
            if (child->name() == name) {
                return child;
            }
        }
        if (recursive) {
            for (const auto& child : _children) {
                auto node = child->findFirstNodeByName(name, recursive);
                if (node) {
                    return node;
                }
            }
        }
        return nullptr;
    }

    CameraRef Scene::activeCamera() const {
        return _activeCamera;
    }

    void Scene::setActiveCamera(const CameraRef& camera) {
        _activeCamera = camera;
    }
}

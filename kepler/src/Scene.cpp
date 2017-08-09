#include "stdafx.h"
#include "Scene.hpp"
#include "Camera.hpp"

#include <algorithm>

namespace kepler {
    Scene::Scene() {
    }
    Scene::~Scene() noexcept {
        for (auto& node : _children) {
            node->_scene = nullptr;
            node->setAllChildrenScene(nullptr);
        }
    }

    ref<Scene> Scene::create() {
        return std::make_shared<Scene>();
    }

    void Scene::addNode(ref<Node>& node) {
        if (node == nullptr) return;

        if (ref<Node> parent = node->_parent.lock()) {
            Node::removeFromList(parent->_children, node);
        }
        else if (auto oldScene = node->_scene) {
            oldScene->removeChild(node);
        }
        node->_parent.reset();
        node->_scene = this;
        _children.push_back(node);
        node->setAllChildrenScene(this);
        node->parentChanged();
    }

    ref<Node> Scene::createChild(const std::string& name) {
        auto node = Node::create(name);
        node->_scene = this;
        _children.push_back(node);
        return node;
    }

    void Scene::createChildren(const std::initializer_list<std::string>& names) {
        for (const auto& name : names) {
            createChild(name);
        }
    }

    size_t Scene::childCount() const {
        return _children.size();
    }

    ref<Node> Scene::childAt(size_t index) const {
        return _children.at(index);
    }

    ref<Node> Scene::lastChild() const {
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

    void Scene::removeChild(const ref<Node>& child) {
        if (child) {
            Node::removeFromList(_children, child);
            child->clearParent();
        }
    }

    void Scene::moveNodesFrom(const ref<Scene> src) {
        if (src) {
            while (!src->_children.empty()) {
                auto child = src->childAt(0);
                addNode(child);
            }
        }
    }

    ref<Node> Scene::findFirstNodeByName(const std::string& name, bool recursive) const {
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

    ref<Camera> Scene::activeCamera() const {
        return _activeCamera;
    }

    void Scene::setActiveCamera(const ref<Camera>& camera) {
        _activeCamera = camera;
    }
}

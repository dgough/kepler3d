#include "stdafx.h"
#include "Node.hpp"
#include "Scene.hpp"
#include "Camera.hpp"
#include "DrawableComponent.hpp"
#include "Logging.hpp"

#include <algorithm>

namespace kepler {

    Node::Node() {
    }

    Node::Node(const char* name) : _dirtyBits(0) {
        if (name != nullptr) {
            _name.assign(name);
        }
    }

    Node::Node(const std::string& name)
        : _name(name), _dirtyBits(0) {
    }

    Node::~Node() noexcept {
    }

    NodeRef Node::create() {
        return MAKE_SHARED(Node);
    }

    NodeRef Node::create(const char* name) {
        return MAKE_SHARED(Node, name);
    }

    NodeRef Node::create(const std::string& name) {
        return MAKE_SHARED(Node, name);
    }

    NodeRef Node::createChild(const std::string& name) {
        NodeRef node = create(name);
        _children.push_back(node);
        node->setParentInner(shared_from_this());
        node->_scene = _scene;
        return node;
    }

    void Node::createChildren(std::initializer_list<std::string> names) {
        for (const auto& name : names) {
            createChild(name);
        }
    }

    void Node::addNode(const NodeRef& child) {
        if (child == nullptr) return;

        if (NodeRef parent = child->_parent.lock()) {
            // Do nothing if the node is already a child.
            if (parent.get() == this) {
                return;
            }
            // Remove the node from its old parent.
            child->removeFromParent();
        }
        _children.push_back(child);
        child->setParentInner(shared_from_this());
        child->_scene = _scene;
    }

    size_t Node::getChildCount() const {
        return _children.size();
    }

    void Node::removeChild(size_t index) {
        removeChild(_children, index);
    }

    void Node::removeChild(const NodeRef& child) {
        if (child) {
            removeFromList(_children, child);
            child->clearParent();
        }
    }

    NodeRef Node::getParent() const {
        return _parent.lock();
    }

    bool Node::hasParent() const {
        return !_parent.expired();
    }

    void Node::setParent(const NodeRef& newParent) {
        removeFromParent();
        if (newParent) {
            newParent->_children.push_back(shared_from_this());
            _parent = newParent;
            _scene = newParent->_scene;
            parentChanged();
        }
    }

    void Node::removeFromParent() {
        if (NodeRef parent = _parent.lock()) {
            parent->removeChild(shared_from_this());
        }
    }

    NodeRef Node::getRoot() {
        if (NodeRef parent = _parent.lock()) {
            while (!parent->_parent.expired()) {
                parent = parent->_parent.lock();
            }
            return parent;
        }
        return shared_from_this();
    }

    SceneRef Node::getScene() const {
        return _scene.lock();
    }

    void Node::setScene(SceneRef scene) {
        _scene = scene;
    }

    NodeRef Node::findFirstNodeByName(const std::string& name, bool recursive) const {
        for (const auto& child : _children) {
            if (child->getName() == name) {
                return child;
            }
        }
        if (recursive) {
            for (const auto& child : _children) {
                auto n = child->findFirstNodeByName(name, recursive);
                if (n) {
                    return n;
                }
            }
        }
        return nullptr;
    }

    const char* Node::getNamePtr() const {
        return _name.c_str();
    }

    const std::string& Node::getName() const {
        return _name;
    }

    void Node::setName(const std::string& name) {
        _name.assign(name);
    }

    NodeRef Node::getChildAt(size_t index) const {
        return _children.at(index);
    }

    NodeRef Node::operator[](size_t index) {
        return _children[index];
    }

    const NodeRef Node::operator[](size_t index) const {
        return _children[index];
    }

    void Node::addComponent(const std::shared_ptr<Component>& component) {
        if (component) {
            if (!containsComponent(component->getTypeName())) {
                _components.push_back(component);
                component->setNode(shared_from_this());
            }
            else {
                logw("WARN::ADD_COMPONENT::TYPE_NAME_EXISTS");
            }
        }
    }

    void Node::removeComponent(const Component* component) {
        auto it = std::remove_if(_components.begin(), _components.end(), [component](const std::shared_ptr<Component> c) {
            return c.get() == component;
        });
        _components.erase(it, _components.end());
    }

    DrawableComponentRef Node::getDrawable() {
        // TODO make this faster
        return getComponent<DrawableComponent>();
    }

    bool Node::containsComponent(const std::string& typeName) const {
        for (const auto& c : _components) {
            if (typeName == c->getTypeName()) {
                return true;
            }
        }
        return false;
    }

    void Node::translate(const glm::vec3& translation) {
        _local.translate(translation);
        setDirty(WORLD_DIRTY);
    }

    void Node::translate(float x, float y, float z) {
        _local.translate(x, y, z);
        setDirty(WORLD_DIRTY);
    }

    void Node::translateX(float x) {
        translate(x, 0, 0);
    }

    void Node::translateY(float y) {
        translate(0, y, 0);
    }

    void Node::translateZ(float z) {
        translate(0, 0, z);
    }

    void Node::scale(const glm::vec3& scale) {
        _local.scale(scale);
        setDirty(WORLD_DIRTY);
    }

    void Node::scale(float scale) {
        _local.scale(scale);
        setDirty(WORLD_DIRTY);
    }

    void Node::scale(float x, float y, float z) {
        _local.scale(x, y, z);
        setDirty(WORLD_DIRTY);
    }

    void Node::rotate(const glm::quat& rotation) {
        _local.rotate(rotation);
        setDirty(WORLD_DIRTY);
    }

    void Node::rotateX(float angle) {
        _local.rotate(glm::angleAxis(angle, glm::vec3(1, 0, 0)));
        setDirty(WORLD_DIRTY);
    }

    void Node::rotateY(float angle) {
        _local.rotate(glm::angleAxis(angle, glm::vec3(0, 1, 0)));
        setDirty(WORLD_DIRTY);
    }

    void Node::rotateZ(float angle) {
        _local.rotate(glm::angleAxis(angle, glm::vec3(0, 0, 1)));
        setDirty(WORLD_DIRTY);
    }

    void Node::setTranslation(const glm::vec3& translation) {
        _local.setTranslation(translation);
        setDirty(WORLD_DIRTY);
    }

    void Node::setTranslation(float x, float y, float z) {
        _local.setTranslation(x, y, z);
        setDirty(WORLD_DIRTY);
    }

    void Node::setScale(const glm::vec3& scale) {
        _local.setScale(scale);
        setDirty(WORLD_DIRTY);
    }

    void Node::setScale(float scale) {
        setScale(scale, scale, scale);
    }

    void Node::setScale(float x, float y, float z) {
        _local.setScale(x, y, z);
        setDirty(WORLD_DIRTY);
    }

    void Node::setRotation(const glm::quat& rotation) {
        _local.setRotation(rotation);
        setDirty(WORLD_DIRTY);
    }

    void Node::setRotationFromEuler(const glm::vec3& eulerAngles) {
        _local.setRotationFromEuler(eulerAngles);
        setDirty(WORLD_DIRTY);
    }

    void Node::setRotationFromEuler(float pitch, float yaw, float roll) {
        _local.setRotationFromEuler(pitch, yaw, roll);
        setDirty(WORLD_DIRTY);
    }

    Transform& Node::editLocalTransform() {
        setDirty(WORLD_DIRTY);
        return _local;
    }

    const Transform& Node::getLocalTransform() const {
        return _local;
    }

    const std::shared_ptr<const Transform> Node::getLocalTransformRef() const {
        return std::shared_ptr<const Transform>(shared_from_this(), &_local);
    }

    const glm::mat4& Node::getViewMatrix() const {
        if (SceneRef scene = _scene.lock()) {
            auto camera = scene->getActiveCamera();
            if (camera) {
                return camera->getViewMatrix();
            }
        }
        return IDENTITY_MATRIX;
    }

    const glm::mat4& Node::getProjectionMatrix() const {
        if (SceneRef scene = _scene.lock()) {
            auto camera = scene->getActiveCamera();
            if (camera) {
                return camera->getProjectionMatrix();
            }
        }
        return IDENTITY_MATRIX;
    }

    const Transform& Node::getWorldTransform() const {
        if ((_dirtyBits & WORLD_DIRTY) == 0) {
            return _world;
        }
        _dirtyBits &= ~WORLD_DIRTY;

        if (auto parent = _parent.lock()) {
            const Transform& parentWorldTransform = parent->getWorldTransform();
            _world = _local;
            _world.combineWithParent(parentWorldTransform);

            setDescendantsDirty(WORLD_DIRTY);
            return _world;
        }
        _world = _local;
        return _world;
    }

    const glm::mat4& Node::getWorldMatrix() const {
        // TODO don't use getWorldTransform()? Measure performance difference.
        return getWorldTransform().getMatrix();
    }

    //const glm::mat4& Node::getModelMatrix() const {
    //    return getLocalTransform().getMatrix();
    //}

    const glm::mat4 Node::getModelViewMatrix() const {
        return getViewMatrix() * getWorldMatrix();
    }

    const glm::mat4 Node::getModelViewInverseTransposeMatrix() const {
        return glm::transpose(glm::inverse(getModelViewMatrix()));
    }

    const glm::mat4 Node::getModelViewProjectionMatrix() const {
        if (SceneRef scene = _scene.lock()) {
            auto camera = scene->getActiveCamera();
            if (camera) {
                return camera->getViewProjectionMatrix() * getWorldMatrix();
            }
        }
        return getWorldMatrix();
    }

    const glm::mat4 Node::getModelInverseMatrix() const {
        return glm::inverse(getWorldMatrix());
    }

    const glm::mat4 Node::getViewInverseMatrix() const {
        return glm::inverse(getViewMatrix());
    }

    const glm::mat4 Node::getProjectionInverseMatrix() const {
        return glm::inverse(getProjectionMatrix());
    }

    const glm::mat4 Node::getModelViewInverseMatrix() const {
        return glm::inverse(getModelViewMatrix());
    }

    const glm::mat4 Node::getModelViewProjectionInverseMatrix() const {
        return glm::inverse(getModelViewProjectionMatrix());
    }

    const glm::mat4 Node::getModelInverseTransposeMatrix() const {
        return glm::transpose(glm::inverse(getWorldMatrix()));
    }

    const glm::mat4 Node::getViewportMatrix() const {
        // TODO
        return glm::mat4();
    }

    glm::vec3 Node::getForwardVectorWorld() const {
        return getWorldTransform().getRotation() * glm::vec3(0.0f, 0.0f, -1.0f);
    }

    void Node::setLocalTransform(const Transform& transform) {
        _local = transform;
        setDirty(WORLD_DIRTY);
    }

    void Node::setLocalTransform(const glm::vec3& translation, const glm::quat& rotation, const glm::vec3& scale) {
        _local.set(translation, rotation, scale);
        setDirty(WORLD_DIRTY);
    }

    bool Node::setLocalTransform(const glm::mat4& matrix) {
        if (_local.set(matrix)) {
            setDirty(WORLD_DIRTY);
            return true;
        }
        return false;
    }

    void Node::addListener(std::shared_ptr<Listener> listener) {
        if (listener) {
            createListenerList();
            _listeners->push_back(listener);
        }
    }

    void Node::removeListener(Listener* listener) {
        if (_listeners && listener != nullptr) {
            auto it = std::remove_if(_listeners->begin(), _listeners->end(), [listener](std::weak_ptr<Listener> wp) {
                // also remove expired weak references
                return (wp.lock().get() == listener || wp.expired());
            });
            _listeners->erase(it, _listeners->end());
        }
    }

    void Node::removeChild(NodeList& children, size_t index) {
        if (index < children.size()) {
            NodeRef child = children[index];
            child->clearParent();
            children.erase(children.begin() + index);
        }
    }

    void Node::removeFromList(NodeList& children, const NodeRef child) {
        children.erase(std::remove(children.begin(), children.end(), child), children.end());
    }

    void Node::setAllChildrenScene(const SceneRef& scene) {
        for (auto child : _children) {
            child->_scene = scene;
            child->setAllChildrenScene(scene);
        }
    }

    void Node::setParentInner(const NodeRef& parent) {
        _parent = parent;
        parentChanged();
    }

    void Node::clearParent() {
        _parent.reset();
        _scene.reset();
        parentChanged();
    }

    void Node::parentChanged() {
        setDirty(WORLD_DIRTY);
    }

    void Node::setDirty(unsigned char dirtyBits) const {
        _dirtyBits |= dirtyBits;
        setDescendantsDirty(dirtyBits);
        notifyTransformChanged();
    }

    void Node::setDescendantsDirty(unsigned char dirtyBits) const {
        for (const auto& child : _children) {
            child->setDirty(dirtyBits);
            //child->_dirtyBits |= dirtyBits;
            //child->setDescendantsDirty(dirtyBits);
            //child->notifyTransformChanged();
        }
    }

    void Node::createListenerList() {
        if (_listeners == nullptr) {
            _listeners = std::make_unique<NodeListenerList>();
        }
    }

    void Node::notifyTransformChanged() const {
        if (_listeners != nullptr) {
            for (auto& l : *_listeners) {
                if (auto listener = l.lock()) {
                    listener->transformChanged(this);
                }
            }
        }
    }
}

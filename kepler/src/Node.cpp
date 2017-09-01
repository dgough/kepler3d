#include "stdafx.h"
#include "Node.hpp"
#include "Scene.hpp"
#include "Camera.hpp"
#include "DrawableComponent.hpp"
#include "MeshRenderer.hpp"
#include "Mesh.hpp"
#include "Logging.hpp"
#include "Performance.hpp"

#include <algorithm>

namespace kepler {

    static constexpr unsigned char WORLD_DIRTY = 1;
    static constexpr unsigned char BOUNDS_DIRTY = 2;

    static constexpr unsigned char ALL_DIRTY = WORLD_DIRTY | BOUNDS_DIRTY;

    Node::Node() {
    }

    Node::Node(const char* name) : _dirtyBits(0) {
        if (name != nullptr) {
            setName(name);
        }
    }

    Node::Node(const std::string& name)
        : _dirtyBits(0) {
        setName(name);
    }

    Node::~Node() noexcept {
        for (auto& node : _children) {
            node->_parent = nullptr;
        }
    }

    ref<Node> Node::create() {
        return std::make_shared<Node>();
    }

    ref<Node> Node::create(const char* name) {
        return std::make_shared<Node>(name);
    }

    ref<Node> Node::create(const std::string& name) {
        return std::make_shared<Node>(name);
    }

    ref<Node> Node::createChild(const std::string& name) {
        std::unique_ptr<Node> p = nullptr;
        ref<Node> node = create(name);
        _children.push_back(node);
        node->_parent = this;
        node->_scene = _scene;
        return node;
    }

    void Node::createChildren(const std::initializer_list<std::string>& names) {
        for (const auto& name : names) {
            createChild(name);
        }
    }

    void Node::addNode(const ref<Node>& child) {
        if (child == nullptr) return;

        if (auto parent = child->_parent) {
            // Do nothing if the node is already a child.
            if (parent == this) {
                return;
            }
            // Remove the node from its old parent.
            child->removeFromParent();
        }
        _children.push_back(child);
        child->setParentInner(this);
        child->_scene = _scene;
    }

    size_t Node::childCount() const {
        return _children.size();
    }

    void Node::removeChild(size_t index) {
        removeChild(_children, index);
    }

    void Node::removeChild(const ref<Node>& child) {
        if (child) {
            removeFromList(_children, child);
            child->clearParent();
        }
    }

    Node* Node::parent() const {
        return _parent;
    }

    bool Node::hasParent() const {
        return _parent != nullptr;
    }

    void Node::setParent(const ref<Node>& newParent) { // TODO change this to Node*?
        removeFromParent();
        if (newParent) {
            newParent->_children.push_back(shared_from_this());
            _parent = newParent.get();
            _scene = newParent->_scene;
            parentChanged();
        }
    }

    void Node::removeFromParent() {
        if (auto parent = _parent) {
            parent->removeChild(shared_from_this());
        }
    }

    Node* Node::root() {
        auto node = this;
        while (node->_parent != nullptr) {
            node = node->_parent;
        }
        return node;
    }

    Scene* Node::scene() const {
        return _scene;
    }

    ref<Node> Node::findFirstNodeByName(const std::string& name, bool recursive) const {
        for (const auto& child : _children) {
            if (child->name() == name) {
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

    const char* Node::namePtr() const {
        return _name ? _name->c_str() : nullptr;
    }

    std::string Node::name() const {
        return _name ? *_name : std::string();
    }

    void Node::setName(const std::string& name) {
        if (_name) {
            _name->assign(name);
        }
        else {
            _name = std::make_unique<std::string>(name);
        }
    }

    ref<Node> Node::childAt(size_t index) const {
        return _children.at(index);
    }

    ref<Node> Node::operator[](size_t index) {
        return _children[index];
    }

    const ref<Node> Node::operator[](size_t index) const {
        return _children[index];
    }

    void Node::addComponent(const std::shared_ptr<Component>& component) {
        if (component) {
            if (!containsComponent(component->typeName())) {
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

    ref<DrawableComponent> Node::drawable() const {
        // TODO make this faster
        return component<DrawableComponent>();
    }

    bool Node::containsComponent(const std::string& typeName) const {
        for (const auto& c : _components) {
            if (typeName == c->typeName()) {
                return true;
            }
        }
        return false;
    }

    void Node::translate(const glm::vec3& translation) {
        _local.translate(translation);
        setDirty(ALL_DIRTY);
    }

    void Node::translate(float x, float y, float z) {
        _local.translate(x, y, z);
        setDirty(ALL_DIRTY);
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
        setDirty(ALL_DIRTY);
    }

    void Node::scale(float scale) {
        _local.scale(scale);
        setDirty(ALL_DIRTY);
    }

    void Node::scale(float x, float y, float z) {
        _local.scale(x, y, z);
        setDirty(ALL_DIRTY);
    }

    void Node::rotate(const glm::quat& rotation) {
        _local.rotate(rotation);
        setDirty(ALL_DIRTY);
    }

    void Node::rotateX(float angle) {
        _local.rotate(glm::angleAxis(angle, glm::vec3(1, 0, 0)));
        setDirty(ALL_DIRTY);
    }

    void Node::rotateY(float angle) {
        _local.rotate(glm::angleAxis(angle, glm::vec3(0, 1, 0)));
        setDirty(ALL_DIRTY);
    }

    void Node::rotateZ(float angle) {
        _local.rotate(glm::angleAxis(angle, glm::vec3(0, 0, 1)));
        setDirty(ALL_DIRTY);
    }

    void Node::setTranslation(const glm::vec3& translation) {
        _local.setTranslation(translation);
        setDirty(ALL_DIRTY);
    }

    void Node::setTranslation(float x, float y, float z) {
        _local.setTranslation(x, y, z);
        setDirty(ALL_DIRTY);
    }

    void Node::setScale(const glm::vec3& scale) {
        _local.setScale(scale);
        setDirty(ALL_DIRTY);
    }

    void Node::setScale(float scale) {
        setScale(scale, scale, scale);
    }

    void Node::setScale(float x, float y, float z) {
        _local.setScale(x, y, z);
        setDirty(ALL_DIRTY);
    }

    void Node::setRotation(const glm::quat& rotation) {
        _local.setRotation(rotation);
        setDirty(ALL_DIRTY);
    }

    void Node::setRotationFromEuler(const glm::vec3& eulerAngles) {
        _local.setRotationFromEuler(eulerAngles);
        setDirty(ALL_DIRTY);
    }

    void Node::setRotationFromEuler(float pitch, float yaw, float roll) {
        _local.setRotationFromEuler(pitch, yaw, roll);
        setDirty(ALL_DIRTY);
    }

    Transform& Node::editLocalTransform() {
        setDirty(ALL_DIRTY);
        return _local;
    }

    const Transform& Node::localTransform() const {
        return _local;
    }

    const std::shared_ptr<const Transform> Node::localTransformRef() const {
        return std::shared_ptr<const Transform>(shared_from_this(), &_local);
    }

    const glm::mat4& Node::viewMatrix() const {
        ProfileBlock p(0);
        if (auto scene = _scene) {
            auto camera = scene->activeCamera();
            if (camera) {
                return camera->viewMatrix();
            }
        }
        return IDENTITY_MATRIX;
    }

    const glm::mat4& Node::viewMatrix(const Camera* camera) const {
        if (camera != nullptr) {
            return camera->viewMatrix();
        }
        return IDENTITY_MATRIX;
    }

    const glm::mat4& Node::projectionMatrix() const {
        if (auto scene = _scene) {
            auto camera = scene->activeCamera();
            if (camera) {
                return camera->projectionMatrix();
            }
        }
        return IDENTITY_MATRIX;
    }

    const glm::mat4& Node::projectionMatrix(const Camera* camera) const {
        if (camera != nullptr) {
            return camera->projectionMatrix();
        }
        return IDENTITY_MATRIX;
    }

    const Transform& Node::worldTransform() const {
        if ((_dirtyBits & WORLD_DIRTY) == 0) {
            return _world;
        }
        _dirtyBits &= ~WORLD_DIRTY;
        if (auto parent = _parent) {
            const Transform& parentWorldTransform = parent->worldTransform();
            _world = _local;
            _world.combineWithParent(parentWorldTransform);

            setDescendantsDirty(WORLD_DIRTY);
            return _world;
        }
        _world = _local;
        return _world;
    }

    const glm::mat4& Node::worldMatrix() const {
        // TODO don't use getWorldTransform()? Measure performance difference.
        return worldTransform().matrix();
    }

    //const glm::mat4& Node::getModelMatrix() const {
    //    return getLocalTransform().getMatrix();
    //}

    const glm::mat4 Node::modelViewMatrix() const {
        return viewMatrix() * worldMatrix();
    }

    const glm::mat4 Node::modelViewMatrix(const Camera* camera) const {
        if (camera != nullptr) {
            return camera->viewMatrix() * worldMatrix();
        }
        return worldMatrix();
    }

    const glm::mat3 Node::modelViewInverseTransposeMatrix() const {
        return glm::transpose(glm::inverse(glm::mat3(modelViewMatrix())));
    }

    const glm::mat3 Node::modelViewInverseTransposeMatrix(const Camera* camera) const {
        if (camera != nullptr) {
            return glm::transpose(glm::inverse(glm::mat3(modelViewMatrix(camera))));
        }
        return glm::transpose(glm::inverse(glm::mat3(worldMatrix())));
    }

    const glm::mat4 Node::modelViewProjectionMatrix() const {
        if (auto scene = _scene) {
            auto camera = scene->activeCamera();
            if (camera) {
                return camera->viewProjectionMatrix() * worldMatrix();
            }
        }
        return worldMatrix();
    }

    const glm::mat4 Node::modelViewProjectionMatrix(const Camera* camera) const {
        if (camera != nullptr) {
            return camera->viewProjectionMatrix() * worldMatrix();
        }
        return worldMatrix();
    }

    const glm::mat4 Node::modelInverseMatrix() const {
        return glm::inverse(worldMatrix());
    }

    const glm::mat4 Node::viewInverseMatrix() const {
        return glm::inverse(viewMatrix());
    }

    const glm::mat4 Node::viewInverseMatrix(const Camera* camera) const {
        return glm::inverse(viewMatrix(camera));
    }

    const glm::mat4 Node::projectionInverseMatrix() const {
        return glm::inverse(projectionMatrix());
    }

    const glm::mat4 Node::projectionInverseMatrix(const Camera* camera) const {
        return glm::inverse(projectionMatrix(camera));
    }

    const glm::mat4 Node::modelViewInverseMatrix() const {
        return glm::inverse(modelViewMatrix());
    }

    const glm::mat4 Node::modelViewInverseMatrix(const Camera* camera) const {
        return glm::inverse(modelViewMatrix(camera));
    }

    const glm::mat4 Node::modelViewProjectionInverseMatrix() const {
        return glm::inverse(modelViewProjectionMatrix());
    }

    const glm::mat4 Node::modelViewProjectionInverseMatrix(const Camera* camera) const {
        return glm::inverse(modelViewProjectionMatrix(camera));
    }

    const glm::mat4 Node::modelInverseTransposeMatrix() const {
        return glm::transpose(glm::inverse(worldMatrix()));
    }

    const glm::mat4 Node::viewportMatrix() const {
        // TODO
        return glm::mat4();
    }

    glm::vec3 Node::forwardVectorWorld() const {
        return worldTransform().rotation() * glm::vec3(0.0f, 0.0f, -1.0f);
    }

    void Node::setLocalTransform(const Transform& transform) {
        _local = transform;
        setDirty(ALL_DIRTY);
    }

    void Node::setLocalTransform(const glm::vec3& translation, const glm::quat& rotation, const glm::vec3& scale) {
        _local.set(translation, rotation, scale);
        setDirty(ALL_DIRTY);
    }

    bool Node::setLocalTransform(const glm::mat4& matrix) {
        if (_local.set(matrix)) {
            setDirty(ALL_DIRTY);
            return true;
        }
        return false;
    }

    const BoundingBox& Node::boundingBox() const {
        if ((_dirtyBits & BOUNDS_DIRTY) == 0) {
            return _box;
        }
        _dirtyBits &= ~BOUNDS_DIRTY;
        bool empty = true;
        // TODO add faster way of getting mesh
        auto meshRenderer = component<MeshRenderer>();
        if (meshRenderer) {
            if (meshRenderer->getBoundingBox(_box)) {
                empty = false;
            }
        }
        if (!empty) {
            _box.transform(worldMatrix());
            
        }
        // merge with children
        for (const auto& child : _children) {
            const auto& box = child->boundingBox();
            if (!box.empty()) {
                if (empty) {
                    _box = box;
                    empty = false;
                }
                else {
                    _box.merge(box);
                }
            }
        }
        return _box;
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
            ref<Node> child = children[index];
            child->clearParent();
            children.erase(children.begin() + index);
        }
    }

    void Node::removeFromList(NodeList& children, const ref<Node>& child) {
        children.erase(std::remove(children.begin(), children.end(), child), children.end());
    }

    void Node::setAllChildrenScene(Scene* scene) {
        for (const auto& child : _children) {
            child->_scene = scene;
            child->setAllChildrenScene(scene);
        }
    }

    void Node::setParentInner(Node* parent) {
        _parent = parent;
        parentChanged();
    }

    void Node::clearParent() {
        _parent = nullptr;
        _scene = nullptr;
        parentChanged();
    }

    void Node::parentChanged() {
        setDirty(ALL_DIRTY);
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
            for (const auto& l : *_listeners) {
                if (auto listener = l.lock()) {
                    listener->transformChanged(this);
                }
            }
        }
    }

    Node::Iterator Node::begin() const {
        size_t pos = 0;
        return Iterator(_children, pos);
    }

    Node::Iterator Node::end() const {
        return Iterator(_children, _children.size());
    }

    bool Node::Iterator::operator != (const Iterator& other) const {
        return _list != other._list || _pos != other._pos;
    }

    const Node::Iterator& Node::Iterator::operator++() {
        ++_pos;
        return *this;
    }

    const Node::Iterator& Node::Iterator::operator++(int) {
        _pos++;
        return *this;
    }

    const Node::Iterator& Node::Iterator::operator--() {
        --_pos;
        return *this;
    }

    const Node::Iterator& Node::Iterator::operator--(int) {
        _pos--;
        return *this;
    }

    Node& Node::Iterator::operator* () const {
        return *(_list[_pos].get());
    }

    Node* Node::Iterator::operator->() const {
        return _list[_pos].get();
    }
}

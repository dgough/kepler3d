#pragma once

#include "Base.hpp"
#include "Transform.hpp"
#include "Component.hpp"

#include <vector>
#include <initializer_list>

namespace kepler {

    using NodeList = std::vector<std::shared_ptr<Node>>;
    using ComponentList = std::vector<std::shared_ptr<Component>>;

    /// The node class defines a hierarchical tree of objects.
    ///
    /// Each node can have at most one parent and zero or more children.
    /// Zero or more Components can be added to a node.
    /// Each node holds a strong reference to its components and its children but a weak reference to its parent.
    class Node : public std::enable_shared_from_this<Node> {
        friend class Scene;
    public:
        // Don't use constructors directly. Use Node::create().
        Node();
        explicit Node(const char* name);
        explicit Node(const std::string& name);
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;
        virtual ~Node() noexcept;

        /// Creates a node and returns a std::shared_ptr to it.
        static ref<Node> create();

        /// Creates a node with the given name.
        static ref<Node> create(const char* name);

        /// Creates a new node with the given name.
        /// Node names are not unique.
        static ref<Node> create(const std::string& name);

        /// Creates a new node and adds it as a child of this node.
        ref<Node> createChild(const std::string& name = "");

        /// Creates child nodes with the given names ands them to this node.
        void createChildren(const std::initializer_list<std::string>& names);

        /// Adds the node as a child of this node.
        void addNode(const ref<Node>& child);

        /// Returns the number of direct child nodes.
        size_t childCount() const;

        /// Removes the node at the given index.
        /// Does nothing if the index is out of bounds.
        void removeChild(size_t index);

        /// Removes the node from the list of children.
        /// This does not include grandchildren.
        void removeChild(const ref<Node>& child);

        /// Returns a pointer to the parent node. May be null.
        Node* parent() const;

        /// Returns true if this node has a parent; false otherwise.
        bool hasParent() const;

        /// Sets the parent of this node to be newParent.
        void setParent(const ref<Node>& newParent);

        /// Removes this node from its parent.
        void removeFromParent();

        /// Returns the root node.
        /// This node will be the root node if it doesn't have a parent.
        Node* root();

        /// Returns the scene that this node belongs to. May be null.
        Scene* scene() const;

        /// Finds the first descendant node that matches the given name.
        /// Immediate children are checked first before recursing.
        /// 
        /// @param[in] name The name of the node to search for.
        /// @param[in] recursive True if the search should be recursive.
        /// @return The node if found; otherwise returns nullptr.
        ref<Node> findFirstNodeByName(const std::string& name, bool recursive = true) const;

        // Node* findFirstNodeByName(const char* name, bool recursive = true);
        // unsigned int findAllNodes(const char* name, std::vector<Node*> nodes, bool recursive = true);

        /// Finds the first descendant node where the node evaluator returns true for.
        /// This method allows you to find a node using a lambda expression.
        /// @param[in] eval The node evaluator. The signature should be:
        /// ```
        /// bool eval(Node*);
        /// ```
        /// @param[in] recursive True if the search should be recursive.
        template <class NodeEval>
        ref<Node> findFirstNode(const NodeEval& eval, bool recursive = true) const;

        // isStatic
        // isEnabled

        // setTag, hasTag

        // std::unique_ptr<Node> clone() const;

        /// Returns a char pointer to this node's name.
        const char* namePtr() const;

        /// Returns the name of this node.
        std::string name() const;

        /// Sets this node's name.
        /// Node names are not unique.
        /// @param[in] name The new node name.
        void setName(const std::string& name);

        /// Returns the child node at the given index.
        /// Checks whether index is within the bounds of valid elements in the vector,
        /// throwing an out_of_range exception if it is not.
        /// Similar to std::vector::at().
        /// @throws out_of_range exception.
        ref<Node> childAt(size_t index) const;

        /// Returns the Node at specified location pos. No bounds checking is performed.
        ref<Node> operator[](size_t index);

        /// Returns the Node at specified location pos. No bounds checking is performed.
        const ref<Node> operator[](size_t index) const;

        /// Adds the component to this node.
        void addComponent(const std::shared_ptr<Component>& component);

        void removeComponent(const Component* component);

        /// Returns the component of the specified type.
        template<class T>
        std::shared_ptr<T> component();

        /// Returns the first drawable component of this node.
        /// @return Shared ref to a DrawableComponent; may be null.
        ref<DrawableComponent> drawable();

        /// Returns true if this node contains the component.
        /// You cannot search for abstract components because this is a simple string comparison.
        /// That means "DrawableComponent" will not return any results because it is an abstract class.
        /// @param[in] typeName The component type name (like "Camera").
        /// @return True if at least one component of that type was found.
        bool containsComponent(const std::string& typeName) const;

        // Transform operations

        /// Translates the node by the given vector.
        void translate(const glm::vec3& translation);
        /// Translate the node by the given x, y and z values.
        void translate(float x, float y, float z);

        void translateX(float x);
        void translateY(float y);
        void translateZ(float z);

        void scale(const glm::vec3& scale);
        /// Scales the x, y and z by the given scale value.
        void scale(float scale);
        void scale(float x, float y, float z);

        /// Rotates the node by the given quaternion.
        void rotate(const glm::quat& rotation);

        /// Rotates this node around the X axis (right handed).
        void rotateX(float angle);
        void rotateY(float angle);
        void rotateZ(float angle);

        /// Sets the local translation to the given vector.
        void setTranslation(const glm::vec3& translation);
        void setTranslation(float x, float y, float z);

        void setScale(const glm::vec3& scale);
        void setScale(float scale);
        void setScale(float x, float y, float z);

        void setRotation(const glm::quat& rotation);
        // TODO untested (setRotationFromEuler)
        void setRotationFromEuler(const glm::vec3& eulerAngles);
        void setRotationFromEuler(float pitch, float yaw, float roll);

        /// Returns the local transform.
        /// This will mark the world transform as dirty because the caller might modify the local transform.
        /// Be sure you modify the returned transform instead of making a copy.
        Transform& editLocalTransform();

        const Transform& localTransform() const;

        /// Returns a shared pointer that points to the local transform buy holds a reference to the Node.
        const std::shared_ptr<const Transform> localTransformRef() const;

        const glm::mat4& viewMatrix() const;
        const glm::mat4& viewMatrix(const Camera* camera) const;
        const glm::mat4& projectionMatrix() const;
        const glm::mat4& projectionMatrix(const Camera* camera) const;

        const Transform& worldTransform() const;
        const glm::mat4& worldMatrix() const;

        /////////////////
        const glm::mat4 modelViewMatrix() const;
        /// Returns the ModelView matrix for this node using the given camera 
        /// instead of the active camera of the scene this node belongs to.
        const glm::mat4 modelViewMatrix(const Camera* camera) const;

        const glm::mat3 modelViewInverseTransposeMatrix() const;
        const glm::mat3 modelViewInverseTransposeMatrix(const Camera* camera) const;

        const glm::mat4 modelViewProjectionMatrix() const;
        const glm::mat4 modelViewProjectionMatrix(const Camera* camera) const;
        const glm::mat4 modelInverseMatrix() const;
        const glm::mat4 viewInverseMatrix() const;
        const glm::mat4 viewInverseMatrix(const Camera* camera) const;
        const glm::mat4 projectionInverseMatrix() const;
        const glm::mat4 projectionInverseMatrix(const Camera* camera) const;
        const glm::mat4 modelViewInverseMatrix() const;
        const glm::mat4 modelViewInverseMatrix(const Camera* camera) const;
        const glm::mat4 modelViewProjectionInverseMatrix() const;
        const glm::mat4 modelViewProjectionInverseMatrix(const Camera* camera) const;
        const glm::mat4 modelInverseTransposeMatrix() const;
        const glm::mat4 viewportMatrix() const;

        /// Returns the vector (0, 0 -1) transformed by this node's world matrix.
        glm::vec3 forwardVectorWorld() const;

        void setLocalTransform(const Transform& transform);
        void setLocalTransform(const glm::vec3& translation, const glm::quat& rotation, const glm::vec3& scale);

        /// Sets the local transform using the given matrix.
        /// @return True if the matrix was successfully decompose; false if decompose failed and the transform was not changed.
        bool setLocalTransform(const glm::mat4& matrix);

        /// Node Listener
        class Listener {
        public:
            virtual ~Listener() {}
            virtual void transformChanged(const Node* node) = 0;
        };

        /// Adds a node listener.
        /// Listeners will be stored with a weak reference.
        void addListener(std::shared_ptr<Listener> listener);

        /// Removes the given listener.
        void removeListener(Listener* listener);

        /// Node iterator
        class Iterator {
        public:
            Iterator(const NodeList& list, size_t pos) : _list(list), _pos(pos) {}

            bool operator != (const Iterator& other) const;

            const Iterator& operator++();

            const Iterator& operator++(int);

            const Iterator& operator--();

            const Iterator& operator--(int);

            Node& operator* () const;

            Node* operator->() const;

        private:
            const NodeList& _list;
            size_t _pos;
        };

        /// Returns an iterator pointing to the first child node.
        /// This iterator will only iterator the children, not the grand children.
        Iterator begin() const;

        /// Returns an iterator referring to the past-the-end child (not the last child).
        Iterator end() const;

    private:
        using NodeListenerList = std::vector<std::weak_ptr<Listener>>;

        static void removeChild(NodeList& children, size_t index);

        /// Removes the child from the list of children but doesn't update its parent or scene.
        static void removeFromList(NodeList& children, const ref<Node>& child);

        void setAllChildrenScene(Scene* scene);

        void setParentInner(Node* parent);
        void clearParent();
        void parentChanged();

        void setDirty(unsigned char dirtyBits) const;
        void setDescendantsDirty(unsigned char dirtyBits) const;

        void createListenerList();
        void notifyTransformChanged() const;

    private:
        std::unique_ptr<std::string> _name;
        Node* _parent = nullptr;
        NodeList _children;
        ComponentList _components;
        Scene* _scene = nullptr;
        std::unique_ptr<NodeListenerList> _listeners;

        mutable Transform _local;
        mutable Transform _world;
        mutable unsigned char _dirtyBits;

        static constexpr unsigned char WORLD_DIRTY = 1;
    };

    // Methods

    template<class NodeEval>
    ref<Node> Node::findFirstNode(const NodeEval& eval, bool recursive) const {
        for (const auto& child : _children) {
            if (eval(child.get())) {
                return child;
            }
        }
        if (recursive) {
            for (const auto& child : _children) {
                auto n = child->findFirstNode(eval, recursive);
                if (n) {
                    return n;
                }
            }
        }
        return nullptr;
    }

    template<class T>
    std::shared_ptr<T> Node::component() {
        for (const auto& c : _components) {
            if (dynamic_cast<T*>(c.get()) != nullptr) {
                return std::dynamic_pointer_cast<T>(c);
            }
        }
        return nullptr;
    }
}

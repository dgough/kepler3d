#pragma once

#include "Node.hpp"

namespace kepler {

    class Scene : public std::enable_shared_from_this<Scene> {
        ALLOW_MAKE_SHARED(Scene);
    public:
        virtual ~Scene() noexcept;

        /// Creates a new scene object.
        static SceneRef create();

        /// Adds the node to the scene's top level list of nodes.
        /// The node will be removed from its previous parent.
        void addNode(NodeRef& node);
        NodeRef createChild(const std::string& name = "");

        /// Creates nodes with the given names and adds them to this scene.
        void createChildren(std::initializer_list<std::string> names);

        /// Returns the number of direct child nodes.
        size_t getChildCount() const;

        // Similar to std::vector::at()
        NodeRef getChildAt(size_t index) const;

        /// Returns the last child or nullptr if there are no children.
        NodeRef getLastChild() const;

        const NodeList& getChildren() const;

        /// Removes the node at the given index.
        /// Does nothing if the index is out of bounds.
        void removeChild(size_t index);

        /// Removes the node from the list of children.
        /// This does not include grandchildren.
        void removeChild(const NodeRef& child);

        /// Moves all of the nodes from src to this scene.
        void moveNodesFrom(const SceneRef src);

        /// Finds the first descendant node that matches the given name.
        /// Immediate children are checked first before recursing.
        NodeRef findFirstNodeByName(const std::string& name, bool recursive = true) const;

        template <class NodeEval>
        NodeRef findFirstNode(const NodeEval& eval, bool recursive = true) const;

        CameraRef getActiveCamera() const;
        void setActiveCamera(const CameraRef& camera);

        /// Visits each node and calls the given function pass a pointer to the current node.
        /// The expected signature is <code>void func(Node*);</code>
        template <class Func>
        void visit(const Func& func) const;

    public:
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;
    private:
        Scene();

        template <class Func>
        void visitNode(const Func& func, Node* node) const;

    private:
        NodeList _children;
        CameraRef _activeCamera;
    };

    template<class NodeEval>
    NodeRef Scene::findFirstNode(const NodeEval& eval, bool recursive) const {
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

    template<class Func>
    void Scene::visit(const Func& func) const {
        for (const auto& child : _children) {
            visitNode(func, child.get());
        }
    }

    template <class Func>
    void Scene::visitNode(const Func& func, Node* node) const {
        func(node);
        for (const auto& child : node->_children) {
            visitNode(func, child.get());
        }
    }
}

#pragma once

#include "Base.hpp"
#include <string>

namespace kepler {

    /// Base class for objects that can be attached to a node.
    /// 
    /// Components hold a weak reference to the node they are attached to because
    /// nodes hold a strong reference to components.
    class Component {
        friend Node;
    public:
        Component();
        virtual ~Component() noexcept;

        /// Returns the Node that this component belongs to.
        /// Will return nullptr if the component is not attached to a node or the node was cleaned up.
        NodeRef getNode() const;

        // TODO remove from node

        // TODO add methods for when the component is added or removed from a node?
        virtual void onNodeChanged(const NodeRef& oldNode, const NodeRef& newNode);

        /// Returns the name of the component.
        /// For example, Camera should return "Camera".
        virtual const std::string& getTypeName() const = 0;
    public:
        Component(const Component&) = delete;
        Component& operator=(const Component&) = delete;
    protected:
        /// Sets the Node that this component belongs to but does not add it to that Node.
        void setNode(const NodeRef& node);

    protected:
        std::weak_ptr<Node> _node;
    };
}

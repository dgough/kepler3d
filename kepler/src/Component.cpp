#include "stdafx.h"
#include "Component.hpp"

namespace kepler {
    Component::Component() {
    }

    Component::~Component() noexcept {
    }

    NodeRef Component::getNode() const {
        return _node.lock();
    }

    void Component::onNodeChanged(const NodeRef&, const NodeRef&) {
    }

    void Component::setNode(const NodeRef& node) {
        NodeRef oldNode = _node.lock();
        _node = node;
        onNodeChanged(oldNode, node);
    }
}

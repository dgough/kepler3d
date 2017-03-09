#include "stdafx.h"
#include "Component.hpp"

namespace kepler {
    Component::Component() {
    }

    Component::~Component() noexcept {
    }

    ref<Node> Component::node() const {
        return _node.lock();
    }

    bool Component::isDrawable() const {
        return false;
    }

    void Component::onNodeChanged(const ref<Node>&, const ref<Node>&) {
    }

    void Component::setNode(const ref<Node>& node) {
        ref<Node> oldNode = _node.lock();
        _node = node;
        onNodeChanged(oldNode, node);
    }
}

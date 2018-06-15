#include "stdafx.h"
#include "Component.hpp"

namespace kepler {
Component::Component() {
}

Component::~Component() noexcept {
}

shared_ptr<Node> Component::node() const {
    return _node.lock();
}

bool Component::isDrawable() const {
    return false;
}

void Component::onNodeChanged(const shared_ptr<Node>&, const shared_ptr<Node>&) {
}

void Component::setNode(const shared_ptr<Node>& node) {
    shared_ptr<Node> oldNode = _node.lock();
    _node = node;
    onNodeChanged(oldNode, node);
}
}

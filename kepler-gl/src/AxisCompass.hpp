#pragma once

#include <BaseGL.hpp>

namespace kepler {
namespace gl {

/// The AxisCompass draws the xyz axis in the corner. It rotates along with the camera.
class AxisCompass {
public:
    AxisCompass();
    /// Creates the AxisCompass and adds its node to the given scene.
    explicit AxisCompass(Scene* scene);

    /// Returns the node so you can add it to a scene.
    ref<Node> node() const;

private:
    ref<Node> _node;
};
}
}

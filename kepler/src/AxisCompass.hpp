#pragma once

#include "Base.hpp"

namespace kepler {

    /// The AxisCompass draws the xyz axis in the corner. It rotates along with the camera.
    class AxisCompass {
    public:
        AxisCompass();
        /// Creates the AxisCompass and adds its node to the given scene.
        explicit AxisCompass(SceneRef scene);

        /// Draws the AxisCompass.
        /// If the compass doesn't rotate, make sure you either set the scene or add the node to the scene.
        void draw();
        NodeRef getNode() const;

        /// You need to set the scene so that compass can access the camera if the node wasn't added to the scene already.
        void setScene(SceneRef scene);

    private:
        NodeRef _node;
    };
}

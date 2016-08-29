#pragma once

#include "Node.hpp"
#include "BaseMath.hpp"

namespace kepler {

    /// The camera class represents an orthographic or perspective camera.
    class Camera : public Component, public Node::Listener, public std::enable_shared_from_this<Camera> {
        ALLOW_MAKE_SHARED(Camera);
    public:

        /// Camera types.
        enum class Type {
            PERSPECTIVE = 1,
            ORTHOGRAPHIC = 2
        };

        virtual ~Camera() noexcept;

        /// Creates a perspective camera.
        /// @param[in] fov         Field of view in degrees.
        /// @param[in] aspectRatio Aspect ratio
        /// @param[in] near        Near plane distance.
        /// @param[in] far         Far plane distance.
        static CameraRef createPerspective(float fov, float aspectRatio, float near, float far);

        /// Creates a perspective camera with a FOV based on the given width and height.
        /// @param[in] fov         Field of view in degrees.
        /// @param[in] width       Width of the client display area.
        /// @param[in] height      Height of the client display area.
        /// @param[in] near        Near plane distance.
        /// @param[in] far         Far plane distance.
        static CameraRef createPerspectiveFov(float fov, float width, float height, float near, float far);

        // TODO ortho doesn't need aspect ratio
        static CameraRef createOrthographic(float zoomX, float zoomY, float aspectRatio, float near, float far);

        void onNodeChanged(const NodeRef& oldNode, const NodeRef& newNode) override;
        const std::string& getTypeName() const override;

        Camera::Type getCameraType() const;

        void transformChanged(const Node* node) override;


        const glm::mat4& getViewMatrix() const;
        const glm::mat4& getProjectionMatrix() const;
        const glm::mat4& getViewProjectionMatrix() const;

    public:
        Camera(const Camera&) = delete;
        Camera& operator=(const Camera&) = delete;
    private:
        /// Perspective camera constructor.
        Camera(float fov, float aspectRatio, float near, float far);
        /// Orthographic camera constructor.
        Camera(float zoomX, float zoomY, float aspectRatio, float near, float far);

    private:
        Camera::Type _type;
        float _fov;
        float _aspectRatio;
        float _near;
        float _far;
        float _zoomX;
        float _zoomY;

        mutable glm::mat4 _view;
        mutable glm::mat4 _projection;
        mutable glm::mat4 _viewProjection;
        mutable unsigned char _dirtyBits;

        static constexpr unsigned char WORLD_DIRTY = 1;
    };
}

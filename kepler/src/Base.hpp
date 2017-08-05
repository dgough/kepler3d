#pragma once

#include <memory>

namespace kepler {

    class Node;
    class Scene;
    class Camera;
    class Mesh;
    class MeshPrimitive;
    class MeshRenderer;
    class VertexBuffer;
    class IndexBuffer;
    class IndexAccessor;
    class VertexAttributeAccessor;
    class VertexAttributeBinding;
    class Effect;
    class Material;
    class MaterialParameter;
    class Technique;
    class RenderState;
    class Image;
    class Texture;
    class Sampler;
    class BmpFont;
    class DrawableComponent;
    class Button;

    class App;
    class AppDelegate;
    class FirstPersonController;
    class AxisCompass;

    template<class T>
    using ref = std::shared_ptr<T>; /// ref is an alias for std::shared_ptr
}

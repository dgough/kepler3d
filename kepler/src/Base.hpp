#pragma once

#include <memory>

// Macros for creating ref counted objects. Prefer to use std::make_shard when available.
#if defined(_MSC_VER) && _MSC_VER >= 1800
#define ALLOW_MAKE_SHARED(type) friend class ::std::_Ref_count_obj<type>;
#define MAKE_SHARED(type, ...) std::make_shared<type>(##__VA_ARGS__)
#else
#define ALLOW_MAKE_SHARED(x)
#define MAKE_SHARED(type, ...) std::shared_ptr<type>(new type(##__VA_ARGS__))
#endif

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

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

    class App;
    class AppDelegate;
    class FirstPersonController;
    class AxisCompass;

    /// @addtogroup ref References
    /// Reference counted types.
    /// @{
    ///
    
    using NodeRef = std::shared_ptr<Node>; ///< Shared pointer to Node.
    using NodeWeakRef = std::weak_ptr<Node>; ///< Weak pointer to Node.
    using SceneRef = std::shared_ptr<Scene>; ///< Shared pointer to Scene.
    using CameraRef = std::shared_ptr<Camera>; ///< Shared pointer to Camera.
    using MeshRef = std::shared_ptr<Mesh>; ///< Shared pointer to Mesh.
    using MeshPrimitiveRef = std::shared_ptr<MeshPrimitive>; ///< Shared pointer to MeshPrimitive.
    using MeshRendererRef = std::shared_ptr<MeshRenderer>; ///< Shared pointer to MeshRenderer.
    using VertexBufferRef = std::shared_ptr<VertexBuffer>; ///< Shared pointer to VertexBuffer.
    using IndexBufferRef = std::shared_ptr<IndexBuffer>; ///< Shared pointer to IndexBuffer.
    using IndexAccessorRef = std::shared_ptr<IndexAccessor>; ///< Shared pointer to IndexAccessor.
    using VertexAttributeAccessorRef = std::shared_ptr<VertexAttributeAccessor>; ///< Shared pointer to VertexAttributeAccessor.
    using VertexAttributeBindingRef = std::shared_ptr<VertexAttributeBinding>; ///< Shared pointer to VertexAttributeBinding.
    using EffectRef = std::shared_ptr<Effect>; ///< Shared pointer to Effect.
    using MaterialRef = std::shared_ptr<Material>; ///< Shared pointer to Material.
    using MaterialParameterRef = std::shared_ptr<MaterialParameter>; ///< Shared pointer to MaterialParameter.
    using TechniqueRef = std::shared_ptr<Technique>; ///< Shared pointer to Technique.
    using RenderStateRef = std::shared_ptr<RenderState>; ///< Shared pointer to RenderState.
    using ImageRef = std::shared_ptr<Image>; ///< Shared pointer to Image.
    using TextureRef = std::shared_ptr<Texture>; ///< Shared pointer to Texture.
    using SamplerRef = std::shared_ptr<Sampler>; ///< Shared pointer to Sampler.
    using BmpFontRef = std::shared_ptr<BmpFont>; ///< Shared pointer to BmpFont.
    using DrawableComponentRef = std::shared_ptr<DrawableComponent>; ///< Shared pointer to DrawableComponent.

    using AppDelegateRef = std::shared_ptr<AppDelegate>; ///< Shared pointer to AppDelegate.
    ///
    /// @}

}

#pragma once

#include <Base.hpp>
#include "BaseGL.hpp"

#include <string>
#include <map>
#include <functional>

namespace kepler {

/// RenderState controls the fixed-function GL states and functions when rendering a primitive.
/// 
/// RenderState remembers the previous global state and will make changes only when needed.
/// That means you shouldn't change GL states without using RenderState.
/// 
/// For example, if you call glDepthMask(GL_FALSE), the next primitive to draw won't know that depth writing was disabled.
/// GL_DITHER is not managed by RenderState so you can call glDisable(GL_DITHER).
class RenderState {
public:
    RenderState();
    virtual ~RenderState() noexcept = default;

    /// Binds the RenderState and applies its changes to the global GL state.
    void bind() const noexcept;

    /// Sets the blend color.
    void setBlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

    /// Sets the equation used for both the RGB blend equation and the Alpha blend equation.
    /// @param[in] mode Specifies how source and destination colors are combined. Must be GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT, GL_MIN, GL_MAX.
    void setBlendEquation(GLenum mode);
    /// Sets the RGB blend equation and the alpha blend equation separately.
    void setBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);

    /// Sets pixel arithmetic for blending.
    /// @param[in] sfactor Source RGBA factor.
    /// @param[in] dfactor Destination RGBA factor.
    void setBlendFunc(GLenum sfactor, GLenum dfactor);

    /// Sets the pixel arithmetic for RGB and alpha components separately.
    /// @param[in] srcRGB   Source RGB.
    /// @param[in] dstRGB   Destination RGB.
    /// @param[in] srcAlpha Source alpha.
    /// @param[in] dstAlpha Destination alpha.
    void setBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);

    /// Sets which color channels are writable.
    void setColorMask(bool red, bool green, bool blue, bool alpha);

    /// Sets which polygon face should be culled if culling is enabled.
    /// @param[in] mode GL_FRONT, GL_BACK, or GL_FRONT_AND_BACK.
    void setCullFace(GLenum mode);

    /// Sets the depth comparison function.
    /// GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL, GL_ALWAYS.
    /// @param[in] func
    void setDepthFunc(GLenum func);

    /// Enables or disables the writing into the depth buffer.
    /// @param[in] flag True to enable the depth buffer; false to disable.
    void setDepthMask(bool flag);

    void setDepthRange(GLfloat near, GLfloat far);

    /// Sets which winding is the front face of then polygon.
    void setFrontFace(GLenum mode);

    /// Sets the line width. 
    /// Line widths greather than 1 may not be supported. Query GL_ALIASED_LINE_WIDTH_RANGE to find the supported line widths.
    void setLineWidth(GLfloat width);
    void setPolygonOffset(GLfloat factor, GLfloat units);

    /// Sets the scissor box.
    /// Be sure to enable the scissor test.
    void setScissor(GLint x, GLint y, GLsizei width, GLsizei height);

    // Togglable states

    bool isBlendEnabled() const noexcept;
    bool isCullingEnabled() const noexcept;
    bool isDepthTestEnabled() const noexcept;
    bool isPolygonOffsetFillEnabled() const noexcept;
    bool isPrimitiveRestartFixedIndexEnabled() const noexcept;
    bool isRasterizerDiscardEnabled() const noexcept;
    bool isSampleAlphaToCoverageEnabled() const noexcept;
    bool isSampleCoverageEnabled() const noexcept;
    bool isSampleMaskEnabled() const noexcept;
    bool isScissorTestEnabled() const noexcept;
    bool isStencilTestEnabled() const noexcept;

    void setBlend(bool value);
    void setCulling(bool value);
    void setDepthTest(bool value);
    void setPolygonOffsetFill(bool value);
    void setPrimitiveRestartFixedIndex(bool value);
    void setRasterizerDiscard(bool value);
    void setSampleAlphaToCoverage(bool value);
    void setSampleCoverage(bool value);
    void setSampleMask(bool value);
    void setScissorTest(bool value);
    void setStencilTest(bool value);

    /// Sets the global depth mask. 
    /// This is useful for setting the depth mask before clearing the depth buffer.
    static void setGlobalDepthMask(bool flag);

private:
    RenderState(const RenderState&) = delete;
    RenderState& operator=(const RenderState&) = delete;
private:

    void bindStateBlock() const noexcept;

    struct BlendState {
        GLenum modeRGB;
        GLenum modeAlpha;
        GLenum srcRGB;
        GLenum dstRGB;
        GLenum srcAlpha;
        GLenum dstAlpha;
        GLfloat red;
        GLfloat green;
        GLfloat blue;
        GLfloat alpha;
        BlendState();
        void bind() const noexcept;
    };

    struct DepthState {
        GLenum func;
        GLfloat near;
        GLfloat far;
        GLboolean mask;
        DepthState();
        void bind() const noexcept;
    };

    struct CullState {
        GLenum cullFace;
        GLenum frontFace;
        CullState();
        void bind() const noexcept;
    };

    struct PolygonOffset {
        GLfloat factor;
        GLfloat units;
        PolygonOffset();
        void bind() const noexcept;
    };

    struct Scissor {
        GLint x;
        GLint y;
        GLsizei width;
        GLsizei height;
        Scissor();
    };

private:
    int _state;
    GLubyte _colorMask;
    GLfloat _lineWidth;

    BlendState _blend;
    DepthState _depth;
    CullState _cull;
    PolygonOffset _polygonOffset;
    Scissor _scissor;
};
}

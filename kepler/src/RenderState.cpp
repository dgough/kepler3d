#include "stdafx.h"
#include "RenderState.hpp"
#include "MaterialParameter.hpp"

namespace kepler {

    static constexpr GLubyte RED_MASK = 1;
    static constexpr GLubyte GREEN_MASK = 1 << 1;
    static constexpr GLubyte BLUE_MASK = 1 << 2;
    static constexpr GLubyte ALPHA_MASK = 1 << 3;
    static constexpr GLubyte DEFAULT_COLOR_MASK = RED_MASK | GREEN_MASK | BLUE_MASK | ALPHA_MASK;

    static constexpr int BLEND = 1;
    static constexpr int CULL_FACE = 1 << 1;
    static constexpr int DEPTH_TEST = 1 << 2;
    static constexpr int POLYGON_OFFSET_FILL = 1 << 3;
    static constexpr int PRIMITIVE_RESTART_FIXED_INDEX = 1 << 4;
    static constexpr int RASTERIZER_DISCARD = 1 << 5;
    static constexpr int SAMPLE_ALPHA_TO_COVERAGE = 1 << 6;
    static constexpr int SAMPLE_COVERAGE = 1 << 7;
    static constexpr int SAMPLE_MASK = 1 << 8;
    static constexpr int SCISSOR_TEST = 1 << 9;
    static constexpr int STENCIL_TEST = 1 << 10;

    static constexpr int DEFAULT_STATE = 0;
    static constexpr int INFREQUENT = POLYGON_OFFSET_FILL | PRIMITIVE_RESTART_FIXED_INDEX | RASTERIZER_DISCARD | SAMPLE_ALPHA_TO_COVERAGE | SAMPLE_COVERAGE | SAMPLE_MASK | SCISSOR_TEST | STENCIL_TEST;

    static int g_current_state = DEFAULT_STATE;
    static RenderState g_renderState;

    static inline void bindState(const int& state, const int flag, const int cap) {
        if ((g_current_state & flag) != (state & flag)) {
            if ((state & flag) != 0) {
                glEnable(cap);
                g_current_state |= flag;
            }
            else {
                glDisable(cap);
                g_current_state &= ~flag;
            }
        }
    }

    static inline GLboolean isSet(GLubyte bits, GLubyte mask) {
        return (bits & mask) != 0;
    }

    template<typename Bits, typename Flag>
    static inline void setBit(Bits& bits, Flag flag, bool value) {
        bits = value ? bits | flag : flag & ~flag;
    }

    RenderState::BlendState::BlendState() :
        modeRGB(GL_FUNC_ADD),
        modeAlpha(GL_FUNC_ADD),
        srcRGB(GL_ONE),
        dstRGB(GL_ZERO),
        srcAlpha(GL_ONE),
        dstAlpha(GL_ZERO),
        red(0.f),
        blue(0.f),
        green(0.f),
        alpha(0.f) {}

    void RenderState::BlendState::bind() const noexcept {
        auto& gBlend = g_renderState._blend;
        if (modeRGB != gBlend.modeRGB || modeAlpha != gBlend.modeAlpha) {
            glBlendEquationSeparate(modeRGB, modeAlpha);
            gBlend.modeRGB = modeRGB;
            gBlend.modeAlpha = modeAlpha;
        }
        if (srcRGB != gBlend.srcRGB || dstRGB != gBlend.dstRGB || srcAlpha != gBlend.srcAlpha || dstAlpha != gBlend.dstAlpha) {
            glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
            gBlend.srcRGB = srcRGB;
            gBlend.dstRGB = dstRGB;
            gBlend.srcAlpha = srcAlpha;
            gBlend.dstAlpha = dstAlpha;
        }
        if (red != gBlend.red || green != gBlend.green || blue != gBlend.blue || alpha != gBlend.alpha) {
            glBlendColor(red, green, blue, alpha);
            gBlend.red = red;
            gBlend.green = green;
            gBlend.blue = blue;
            gBlend.alpha = alpha;
        }
    }

    RenderState::DepthState::DepthState() : func(GL_LESS), near(0.f), far(0.f), mask(GL_TRUE) {}

    void RenderState::DepthState::bind() const noexcept {
        if (g_renderState._depth.mask != mask) {
            glDepthMask(mask);
            g_renderState._depth.mask = mask;
        }
        if (g_renderState._depth.func != func) {
            glDepthFunc(func);
            g_renderState._depth.func = func;
        }
        if (g_renderState._depth.near != near || g_renderState._depth.far != far) {
            glDepthRange(near, far);
            g_renderState._depth.near = near;
            g_renderState._depth.far = far;
        }
    }

    RenderState::CullState::CullState() : cullFace(GL_BACK), frontFace(GL_CCW) {}

    void RenderState::CullState::bind() const noexcept {
        if (cullFace != g_renderState._cull.cullFace) {
            glCullFace(cullFace);
            g_renderState._cull.cullFace = cullFace;
        }
        if (frontFace != g_renderState._cull.frontFace) {
            glFrontFace(frontFace);
            g_renderState._cull.frontFace = frontFace;
        }
    }

    RenderState::PolygonOffset::PolygonOffset() : factor(0.f), units(0.f) {}

    void RenderState::PolygonOffset::bind() const noexcept {
        auto& gPoly = g_renderState._polygonOffset;
        if (factor != gPoly.factor || units != gPoly.units) {
            glPolygonOffset(factor, units);
            gPoly = *this;
        }
    }

    RenderState::Scissor::Scissor() : x(0), y(0), width(0), height(0) {}

    ///////////

    RenderState::RenderState() :
        _state(DEFAULT_STATE),
        _colorMask(DEFAULT_COLOR_MASK),
        _lineWidth(1.f) {}

    RenderState::~RenderState() noexcept {
    }

    void RenderState::bind() const noexcept {
        // apply StateBlock first
        bindStateBlock();

        if (isBlendEnabled()) {
            _blend.bind();
        }
        if (isDepthTestEnabled()) {
            _depth.bind();
        }
        if (isCullingEnabled()) {
            _cull.bind();
        }
        if (isPolygonOffsetFillEnabled()) {
            _polygonOffset.bind();
        }
        if (isScissorTestEnabled()) {
            glScissor(_scissor.x, _scissor.y, _scissor.width, _scissor.height);
            g_renderState._scissor = _scissor;
        }
        if (_colorMask != g_renderState._colorMask) {
            glColorMask(isSet(_colorMask, RED_MASK), isSet(_colorMask, BLUE_MASK), isSet(_colorMask, GREEN_MASK), isSet(_colorMask, ALPHA_MASK));
        }
        if (_lineWidth != g_renderState._lineWidth) {
            glLineWidth(_lineWidth);
        }
    }

    void RenderState::setBlendColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) {
        _blend.red = red;
        _blend.green = green;
        _blend.blue = blue;
        _blend.alpha = alpha;
    }

    void RenderState::setBlendEquation(GLenum mode) {
        setBlendEquationSeparate(mode, mode);
    }

    void RenderState::setBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha) {
        _blend.modeRGB = modeRGB;
        _blend.modeAlpha = modeAlpha;
    }
    void RenderState::setBlendFunc(GLenum sfactor, GLenum dfactor) {
        setBlendFuncSeparate(sfactor, dfactor, sfactor, dfactor);
    }
    void RenderState::setBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha) {
        _blend.srcRGB = srcRGB;
        _blend.dstRGB = dstRGB;
        _blend.srcAlpha = srcAlpha;
        _blend.dstAlpha = dstAlpha;
    }

    void RenderState::setColorMask(bool red, bool green, bool blue, bool alpha) {
        setBit(_colorMask, RED_MASK, red);
        setBit(_colorMask, GREEN_MASK, green);
        setBit(_colorMask, BLUE_MASK, blue);
        setBit(_colorMask, ALPHA_MASK, alpha);
    }
    void RenderState::setCullFace(GLenum mode) {
        _cull.cullFace = mode;
    }
    void RenderState::setDepthFunc(GLenum func) {
        _depth.func = func;
    }
    void RenderState::setDepthMask(bool flag) {
        _depth.mask = flag;
    }
    void RenderState::setDepthRange(GLfloat near, GLfloat far) {
        _depth.near = near;
        _depth.far = far;
    }
    void RenderState::setFrontFace(GLenum mode) {
        _cull.frontFace = mode;
    }
    void RenderState::setLineWidth(GLfloat width) {
        _lineWidth = width;
    }
    void RenderState::setPolygonOffset(GLfloat factor, GLfloat units) {
        _polygonOffset.factor = factor;
        _polygonOffset.units = units;
    }
    void RenderState::setScissor(GLint x, GLint y, GLsizei width, GLsizei height) {
        _scissor.x = x;
        _scissor.y = y;
        _scissor.width = width;
        _scissor.height = height;
    }
    bool RenderState::isBlendEnabled() const noexcept {
        return (_state & BLEND) != 0;
    }

    bool RenderState::isCullingEnabled() const noexcept {
        return (_state & CULL_FACE) != 0;
    }

    bool RenderState::isDepthTestEnabled() const noexcept {
        return (_state & DEPTH_TEST) != 0;
    }

    bool RenderState::isPolygonOffsetFillEnabled() const noexcept {
        return (_state & POLYGON_OFFSET_FILL) != 0;
    }

    bool RenderState::isPrimitiveRestartFixedIndexEnabled() const noexcept {
        return (_state & PRIMITIVE_RESTART_FIXED_INDEX) != 0;
    }

    bool RenderState::isRasterizerDiscardEnabled() const noexcept {
        return (_state & RASTERIZER_DISCARD) != 0;
    }

    bool RenderState::isSampleAlphaToCoverageEnabled() const noexcept {
        return (_state & SAMPLE_ALPHA_TO_COVERAGE) != 0;
    }

    bool RenderState::isSampleCoverageEnabled() const noexcept {
        return (_state & SAMPLE_COVERAGE) != 0;
    }

    bool RenderState::isSampleMaskEnabled() const noexcept {
        return (_state & SAMPLE_MASK) != 0;
    }

    bool RenderState::isScissorTestEnabled() const noexcept {
        return (_state & SCISSOR_TEST) != 0;
    }

    bool RenderState::isStencilTestEnabled() const noexcept {
        return (_state & STENCIL_TEST) != 0;
    }
    void RenderState::setBlend(bool value) {
        setBit(_state, BLEND, value);
    }
    void RenderState::setCulling(bool value) {
        setBit(_state, CULL_FACE, value);
    }
    void RenderState::setDepthTest(bool value) {
        setBit(_state, DEPTH_TEST, value);
    }
    void RenderState::setPolygonOffsetFill(bool value) {
        setBit(_state, POLYGON_OFFSET_FILL, value);
    }
    void RenderState::setPrimitiveRestartFixedIndex(bool value) {
        setBit(_state, PRIMITIVE_RESTART_FIXED_INDEX, value);
    }
    void RenderState::setRasterizerDiscard(bool value) {
        setBit(_state, RASTERIZER_DISCARD, value);
    }
    void RenderState::setSampleAlphaToCoverage(bool value) {
        setBit(_state, SAMPLE_ALPHA_TO_COVERAGE, value);
    }
    void RenderState::setSampleCoverage(bool value) {
        setBit(_state, SAMPLE_COVERAGE, value);
    }
    void RenderState::setSampleMask(bool value) {
        setBit(_state, SAMPLE_MASK, value);
    }
    void RenderState::setScissorTest(bool value) {
        setBit(_state, SCISSOR_TEST, value);
    }
    void RenderState::setStencilTest(bool value) {
        setBit(_state, STENCIL_TEST, value);
    }
    void RenderState::setGlobalDepthMask(bool flag) {
        if ((g_renderState._depth.mask != GL_FALSE) != flag) {
            glDepthMask(flag);
            g_renderState._depth.mask = flag;
        }
    }
    void RenderState::bindStateBlock() const noexcept {
        if (_state != g_current_state) {
            bindState(_state, BLEND, GL_BLEND);
            bindState(_state, CULL_FACE, GL_CULL_FACE);
            bindState(_state, DEPTH_TEST, GL_DEPTH_TEST);
            if ((_state & INFREQUENT) != (g_current_state & INFREQUENT)) {
                bindState(_state, POLYGON_OFFSET_FILL, GL_POLYGON_OFFSET_FILL);
                bindState(_state, PRIMITIVE_RESTART_FIXED_INDEX, GL_PRIMITIVE_RESTART_FIXED_INDEX);
                bindState(_state, RASTERIZER_DISCARD, GL_RASTERIZER_DISCARD);
                bindState(_state, SAMPLE_ALPHA_TO_COVERAGE, GL_SAMPLE_ALPHA_TO_COVERAGE);
                bindState(_state, SAMPLE_COVERAGE, GL_SAMPLE_COVERAGE);
                bindState(_state, SAMPLE_MASK, GL_SAMPLE_MASK);
                bindState(_state, SCISSOR_TEST, GL_SCISSOR_TEST);
                bindState(_state, STENCIL_TEST, GL_STENCIL_TEST);
            }
        }
    }
}

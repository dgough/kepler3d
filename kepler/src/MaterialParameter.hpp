#pragma once

#include "Base.hpp"
#include "Effect.hpp"
#include "Texture.hpp"

#include <string>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace kepler {

    class Uniform;

    class MaterialParameter {
    public:

        using FunctionBinding = std::function<void(Effect&, const Uniform* uniform)>;

        enum class Semantic {
            NONE,
            LOCAL,
            MODEL,
            VIEW,
            PROJECTION,
            MODELVIEW,
            MODELVIEWPROJECTION,
            MODELINVERSE,
            VIEWINVERSE,
            PROJECTIONINVERSE,
            MODELVIEWINVERSE,
            MODELVIEWPROJECTIONINVERSE,
            MODELINVERSETRANSPOSE,
            MODELVIEWINVERSETRANSPOSE,
            VIEWPORT,
        };

        enum class Type {
            BYTE = 5120,
            UNSIGNED_BYTE = 5121,
            SHORT = 5122,
            UNSIGNED_SHORT = 5123,
            INT = 5124,
            UNSIGNED_INT = 5125,
            FLOAT = 5126,
            FLOAT_VEC2 = 35664,
            FLOAT_VEC3 = 35665,
            FLOAT_VEC4 = 35666,
            INT_VEC2 = 35667,
            INT_VEC3 = 35668,
            INT_VEC4 = 35669,
            BOOL = 35670,
            BOOL_VEC2 = 35671,
            BOOL_VEC3 = 35672,
            BOOL_VEC4 = 35673,
            FLOAT_MAT2 = 35674,
            FLOAT_MAT3 = 35675,
            FLOAT_MAT4 = 35676,
            SAMPLER_2D = 35678,
        };

        explicit MaterialParameter(const std::string& name);
        virtual ~MaterialParameter() noexcept;
        static MaterialParameterRef create(const std::string& name);

        const std::string& getName() const;

        void setValue(float value);
        void setValue(int value);
        void setValue(const glm::mat4& value);
        void setValue(const glm::vec2& value);
        void setValue(const glm::vec3& value);
        void setValue(const glm::vec4& value);

        void setTexture(TextureRef texture);

        Semantic getSemantic() const;
        void setSemantic(Semantic semantic, Type type);

        Uniform* getUniform() const;
        void setUniform(Uniform* uniform);

        void setFunction(const FunctionBinding& func);

        void bind(EffectRef effect);

    private:
        MaterialParameter(const MaterialParameter&) = delete;
        MaterialParameter& operator=(const MaterialParameter&) = delete;

    private:

        std::string _name;
        Semantic _semantic; // TODO move to its own class?
        Type _type;
        Uniform* _uniform;

        std::function<void(Effect&, const Uniform* uniform)> _function;
    };
}

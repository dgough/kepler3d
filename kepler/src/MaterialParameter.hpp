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

        explicit MaterialParameter(const std::string& name);
        virtual ~MaterialParameter() noexcept;
        static ref<MaterialParameter> create(const std::string& name);

        template<typename T>
        static ref<MaterialParameter> create(const std::string& name, const T& type);


        const std::string& name() const;

        void setValue(float value);
        void setValue(int value);
        void setValue(const glm::mat4& value);
        void setValue(const glm::vec2& value);
        void setValue(const glm::vec3& value);
        void setValue(const glm::vec4& value);
        void setValue(const FunctionBinding& func);
        void setValue(ref<Texture> texture);

        Semantic semantic() const;
        void setSemantic(Semantic semantic);

        Uniform* uniform() const;
        void setUniform(Uniform* uniform);

        void bind(Effect& effect) const;

    private:
        MaterialParameter(const MaterialParameter&) = delete;
        MaterialParameter& operator=(const MaterialParameter&) = delete;

    private:

        std::string _name;
        Semantic _semantic; // TODO move to its own class?
        Uniform* _uniform;

        std::function<void(Effect&, const Uniform* uniform)> _function;
    };

    template<typename T>
    inline ref<MaterialParameter> MaterialParameter::create(const std::string& name, const T& type) {
        auto param = MaterialParameter::create(name);
        param->setValue(type);
        return param;
    }
}

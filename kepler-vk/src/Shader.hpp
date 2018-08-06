#pragma once

#include <BaseVk.hpp>

namespace kepler {
namespace vulkan {

enum class ShaderType {
    Vertex,
    Fragment,
};

class Shader {
public:
    using SharedPtr = std::shared_ptr<Shader>;

    Shader(ShaderType type, vk::ShaderModule shaderModule);
    virtual ~Shader();
    static Shader::SharedPtr createFromFile(ShaderType type, const char* path);

    ShaderType type() const;
    vk::ShaderStageFlagBits stage() const;
    vk::ShaderModule shaderModule() const;

    vk::PipelineShaderStageCreateInfo pipelineCreateInfo(const char* name);

public:
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
protected:
    ShaderType _type;
    vk::ShaderModule _module;
};
}
}

#include "stdafx.h"
#include "Shader.hpp"
#include <FileSystem.hpp>

namespace kepler {
namespace vulkan {

Shader::Shader(ShaderType type, vk::ShaderModule shaderModule) 
    : _type(type), _module(shaderModule) {
}

Shader::~Shader() {
    if (_module) {
        g_device.destroyShaderModule(_module);
    }
}

Shader::SharedPtr Shader::createFromFile(ShaderType type, const char* path) {
    std::vector<char> code;
    if (!readBinaryFile(path, code)) {
        return nullptr;
    }
    vk::ShaderModuleCreateInfo createInfo;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    vk::ShaderModule shaderModule = g_device.createShaderModule(createInfo);
    return std::make_shared<Shader>(type, shaderModule);
}

ShaderType Shader::type() const {
    return _type;
}

vk::ShaderStageFlagBits Shader::stage() const {
    switch (_type) {
    case ShaderType::VERTEX:
        return vk::ShaderStageFlagBits::eVertex;
    case ShaderType::FRAGMENT:
    default:
        return vk::ShaderStageFlagBits::eFragment;
    }
}

vk::ShaderModule Shader::shaderModule() const {
    return _module;
}

vk::PipelineShaderStageCreateInfo Shader::pipelineCreateInfo(const char* name) {
    vk::PipelineShaderStageCreateInfo shaderStageInfo;
    shaderStageInfo.stage = stage();
    shaderStageInfo.module = _module;
    shaderStageInfo.pName = name;
    return shaderStageInfo;
}

}
}

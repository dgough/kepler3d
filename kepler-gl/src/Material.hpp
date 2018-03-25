#pragma once

#include <BaseGL.hpp>
#include "MaterialParameter.hpp"

#include <string>
#include <map>

namespace kepler {
namespace gl {

/// A material is defined as an instance of a shading technique along with parameterized values.
class Material : public std::enable_shared_from_this<Material> {
public:
    /// Use Material::create()
    Material() = default;
    virtual ~Material() noexcept = default;
    static ref<Material> create();
    static ref<Material> create(const ref<Technique>& technique);
    Material(const Material&) = delete;
    Material& operator=(const Material&) = delete;

    /// Returns the Technique in this Material.
    ref<Technique> technique() const;

    const std::string& name() const;
    void setName(const std::string& name);
    void setTechnique(const ref<Technique>& technique);

    /// Adds a MaterialParameter to this Material.
    void addParam(const ref<MaterialParameter>& param);

    /// Returns the MaterialParameter matching the given name.
    ref<MaterialParameter> param(const std::string& name) const;

    void bind();

private:
    std::string _name;
    ref<Technique> _technique; // TODO only 1 for now?
    std::map<std::string, ref<MaterialParameter>> _parameters;
};
}
}

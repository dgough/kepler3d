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
    static shared_ptr<Material> create();
    static shared_ptr<Material> create(const shared_ptr<Technique>& technique);
    Material(const Material&) = delete;
    Material& operator=(const Material&) = delete;

    /// Returns the Technique in this Material.
    shared_ptr<Technique> technique() const;

    const std::string& name() const;
    void setName(const std::string& name);
    void setTechnique(const shared_ptr<Technique>& technique);

    /// Adds a MaterialParameter to this Material.
    void addParam(const shared_ptr<MaterialParameter>& param);

    /// Returns the MaterialParameter matching the given name.
    shared_ptr<MaterialParameter> param(const std::string& name) const;

    void bind();

private:
    std::string _name;
    shared_ptr<Technique> _technique; // TODO only 1 for now?
    std::map<std::string, shared_ptr<MaterialParameter>> _parameters;
};
}
}

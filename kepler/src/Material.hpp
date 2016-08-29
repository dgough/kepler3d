#pragma once

#include "Base.hpp"
#include "MaterialParameter.hpp"

#include <string>
#include <map>

namespace kepler {

    /// A material is defined as an instance of a shading technique along with parameterized values.
    class Material : public std::enable_shared_from_this<Material> {
        ALLOW_MAKE_SHARED(Material);
    public:
        virtual ~Material() noexcept;
        static MaterialRef create();
        static MaterialRef create(TechniqueRef technique);

        /// Returns the Technique in this Material.
        TechniqueRef getTechnique() const;

        const std::string& getName() const;
        void setName(const std::string& name);
        void setTechnique(const TechniqueRef& technique);

        /// Adds a MaterialParameter to this Material.
        void addParam(const MaterialParameterRef& param);

        /// Returns the MaterialParameter matching the given name.
        MaterialParameterRef getParam(const std::string& name) const;

        void bind();

    public:
        Material(const Material&) = delete;
        Material& operator=(const Material&) = delete;
    private:
        Material();
    private:
        std::string _name;
        TechniqueRef _technique; // TODO only 1 for now?
        std::map<std::string, MaterialParameterRef> _parameters;
    };
}

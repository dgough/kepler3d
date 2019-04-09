#pragma once

#include <BaseGL.hpp>
#include "BaseMath.hpp"

#include <vector>
#include <functional>

namespace kepler {
namespace gl {

class Material;
class Effect;
class MaterialParameter;

/// Stores the uniform binding for a MeshPrimitive.
class MaterialBinding final {
public:
    MaterialBinding() = default;
    ~MaterialBinding() = default;

    MaterialBinding(const MaterialBinding&) = delete;
    MaterialBinding& operator=(const MaterialBinding&) = delete;

    void bind(const Node& node, const Material& material);

    void updateBindings(const Material& material);

private:
    void updateValues(const Material& material);

    std::vector<std::function<void(const Effect& effect, const Node&, const Camera*)>> _functions;
    std::vector<shared_ptr<MaterialParameter>> _values;
};

}
}

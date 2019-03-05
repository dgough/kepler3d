#pragma once

#include <BaseGL.hpp>
#include "MaterialParameter.hpp"

namespace kepler {
namespace gl {

enum class AttributeSemantic {
    POSITION,
    NORMAL,
    TANGENT,
    COLOR, // removed in gltf 2.0
    COLOR_0,
    JOINT, // removed in gltf 2.0
    JOINTS_0,
    JOINTMATRIX, // removed in gltf 2.0
    WEIGHT, // removed in gltf 2.0
    WEIGHTS_0,
    TEXCOORD_0,
    TEXCOORD_1,
    TEXCOORD_2,
    TEXCOORD_3,
    TEXCOORD_4,
    TEXCOORD_5,
    TEXCOORD_6,
    TEXCOORD_7,
    TEXCOORD_8,
    TEXCOORD_9,
    TEXCOORD_10,
    TEXCOORD_11,
    TEXCOORD_12,
    TEXCOORD_13,
    TEXCOORD_14,
    TEXCOORD_15,
    TEXCOORD_16,
    TEXCOORD_17,
    TEXCOORD_18,
    TEXCOORD_19,
    TEXCOORD_20,
    TEXCOORD_21,
    TEXCOORD_22,
    TEXCOORD_23,
    TEXCOORD_24,
    TEXCOORD_25,
    TEXCOORD_26,
    TEXCOORD_27,
    TEXCOORD_28,
    TEXCOORD_29,
    TEXCOORD_30,
    TEXCOORD_31,
};
}
}

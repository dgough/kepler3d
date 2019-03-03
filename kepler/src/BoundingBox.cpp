#include "stdafx.h"
#include "BoundingBox.hpp"

namespace kepler {
bool BoundingBox::empty() const {
    return min == max;
}

vec3 BoundingBox::center() const {
    return (max - min) * 0.5f + min;
}

void BoundingBox::corners(vec3* p) const {
    // Left-top-front
    p[0] = vec3(min.x, max.y, max.z);
    // Left-bottom-front
    p[1] = vec3(min.x, min.y, max.z);
    // Right-bottom-front
    p[2] = vec3(max.x, min.y, max.z);
    // Right-top-front
    p[3] = vec3(max.x, max.y, max.z);

    // Far face, specified counter-clockwise looking towards the origin from the negative z-axis.
    // Right-top-back
    p[4] = vec3(max.x, max.y, min.z);
    // Right-bottom-back
    p[5] = vec3(max.x, min.y, min.z);
    // Left-bottom-back
    p[6] = vec3(min.x, min.y, min.z);
    // Left-top-back
    p[7] = vec3(min.x, max.y, min.z);
}

void BoundingBox::merge(const BoundingBox& box) {
    min = glm::min(min, box.min);
    max = glm::max(max, box.max);
}
void BoundingBox::transform(const mat4& matrix) {
    BoundingBox b;
    const float *m = (const float*)glm::value_ptr(matrix);
    vec3 t(m[12], m[13], m[14]);

    for (int i = 0; i < 3; ++i) {
        b.min[i] = b.max[i] = t[i];
        for (int j = 0; j < 3; j++) {
            const int k = i + j * 4;
            float e = m[k] * min[j];
            float f = m[k] * max[j];
            if (e < f) {
                b.min[i] += e;
                b.max[i] += f;
            }
            else {
                b.min[i] += f;
                b.max[i] += e;
            }
        }
    }
    *this = b;
}
}

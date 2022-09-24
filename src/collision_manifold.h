#ifndef COLLISION_MANIFOLD_H_
#define COLLISION_MANIFOLD_H_

#include <cstdint>
#include <glm/glm.hpp>

#define CONTANT_POINT_COUNT 16
struct sCollisionManifold {
    uint16_t obj1;
    uint16_t obj2;

    glm::vec3 normal = {};

    glm::vec3 contanct_points[CONTANT_POINT_COUNT];
    float     contanct_depth[CONTANT_POINT_COUNT];
    uint8_t   contanct_count = 0;
};

#endif // COLLISION_MANIFOLD_H_

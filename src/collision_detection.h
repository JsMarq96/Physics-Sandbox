#ifndef COLLISION_DETECTION_H_
#define COLLISION_DETECTION_H_

#include "trasform.h"
#include "collision_manifold.h"

#define EPSILON 0.001f

namespace CollDet {
    inline bool sphere_sphere_collision(const sTransform &transform1,
                                        const sTransform &transform2,
                                        sCollisionManifold *manifold) {
        glm::vec3 center_to_center = transform2.position - transform1.position;
        float intercenter_distance = center_to_center.length();

        float margin = intercenter_distance - (transform1.scale.x + transform2.scale.x);

        if (margin > EPSILON) {
            return false;
        }

        manifold->contanct_count = 1;
        manifold->normal = glm::normalize(center_to_center);
        manifold->contanct_points[0] = transform2.position + (manifold->normal * -1.0f * transform2.scale.x);
        manifold->contanct_depth[0] = margin;

        return true;
    };
};

#endif // COLLISION_DETECTION_H_

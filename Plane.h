#ifndef PLANE_H
#define PLANE_H

#include "glm/geometric.hpp"
#include "vec3.h"
#include <glm/glm.hpp>

/* from https://www.peroxide.dk/papers/collision/collision.pdf */
class Plane {
public:
    float equation[4];
    glm::vec3 normal;
    glm::vec3 origin;

    Plane(const glm::vec3& origin, const glm::vec3& normal) {
        this->normal = normal;
        this->origin = origin;
        equation[0] = normal.x;
        equation[1] = normal.y;
        equation[2] = normal.z;
        equation[3] = -(normal.x * origin.x 
                      + normal.y * origin.y 
                      + normal.z * origin.z);
    }

    Plane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
        normal = glm::cross(p2 - p1, p3 - p1);
        normal = glm::normalize(normal);
        origin = p1;

        equation[0] = normal.x;
        equation[1] = normal.y;
        equation[2] = normal.z;
        equation[3] = -(normal.x * origin.x 
                      + normal.y * origin.y 
                      + normal.z * origin.z);
    }

    Plane() {}

    bool isFrontFacingTo(const glm::vec3& direction) const {
        double dot = glm::dot(normal, direction);
        return dot <= 0;
    }

    double signedDistanceTo(const glm::vec3& point) {
        return dot(point, normal) + equation[3];
    }
};
#endif

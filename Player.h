#ifndef PLAYER_H
#define PLAYER_H

#include "Plane.h"
#include "Wall.h"
#include "camera.h"
#include "glm/fwd.hpp"
#include "vec3.h"
#include <vector>
#include <cmath>

struct Collision {
    bool success;
    glm::vec3 point;
    Plane plane;
};

class Player {
    public:
        Player(point3 position, double height, double radius) : 
            position(position), 
            velocity(vec3(0.0f, 0.0f, 0.0f)),
            hitbox(radius, height / 2.0, radius), 
            camera() {}

        void tick(double deltaTime) {
            if(velocity.length() > 0) velocity = (velocity / velocity.length()) * maxVelocity;
            glm::vec3 posDelta = glm::vec3(velocity.x(), velocity.y(), velocity.z()) * (float)deltaTime;
            glm::vec3 newPos = collideWithWorld(glm::vec3(position.x(), position.y(), position.z()), posDelta);

            auto preGravPos = position + vec3(newPos.x, newPos.y, newPos.z);
            auto gravDelta = glm::vec3(0.0, -3.0, 0.0) * (float)deltaTime;
            newPos += collideWithWorld(glm::vec3(preGravPos.x(), preGravPos.y(), preGravPos.z()), gravDelta);


            position += vec3(newPos.x, newPos.y, newPos.z);

            camera.SetPosition(position);
            velocity = vec3(); 
        }

        glm::vec3 projectVelocity(glm::vec3 origin, glm::vec3 vel, glm::vec3 normal) {
            Plane slidingPlane = Plane(origin, normal);
            auto dest = origin + vel;
            float dist = slidingPlane.signedDistanceTo(dest);
            glm::vec3 newDest = dest - slidingPlane.normal * dist;
            return (newDest - origin);

        }

        glm::vec3 collideWithWorld(glm::vec3 pos, glm::vec3 vel) {
            glm::vec3 testVelocity = vel; 
            // check for collisions
            double tol = 1e-4;
            while(true && testVelocity.length() > tol) {
                bool is_collision = false;
                for(Wall obj : colliders) {
                    auto collision = collides(obj,
                                              pos, 
                                              testVelocity );

                    if(!collision.success) continue;
                    is_collision = true;

                    // respond 
                    testVelocity = projectVelocity(pos, testVelocity, collision.plane.normal);
                }
                if(!is_collision) break;
            }
            return testVelocity;
        }

        void movePlayer(Camera_Movement direction, double deltaTime) {
            vec3 vDir;
            if (direction == FORWARD)
                vDir = vec3(camera.Front.x, 0, camera.Front.z);
            if (direction == BACKWARD)
                vDir = -vec3(camera.Front.x, 0, camera.Front.z);
            if (direction == LEFT)
                vDir = -vec3(camera.Right.x, 0, camera.Right.z);
            if (direction == RIGHT)
                vDir = vec3(camera.Right.x, 0, camera.Right.z);
            vDir /= vDir.length();
            velocity += vDir;
        }

        void deltaY(double d) {
            velocity += vec3(0, d, 0.0) / glm::abs(d);
            camera.SetPosition(position);
        }

        Camera& getCamera() { 
            return camera;
        }

        void addCollider(Wall w) {
            colliders.push_back(w);
        }
private:
point3 position;
vec3 velocity;
double maxVelocity = 8.0;

vec3 hitbox = vec3(1.0, 2.0, 1.0);

Camera camera;       

std::vector<Wall> colliders;


bool pointInsideTriangle(const glm::vec3 point, const glm::vec3 normal, const glm::vec3 p1, const glm::vec3 p2, const glm::vec3 p3); 


Collision noCollision() {
    return Collision{
        false,
        glm::vec3(),
        Plane()
    };
}

Plane nearestRightPlane(glm::vec3 vec, glm::vec3 point) {
    double x = vec.x, y = vec.y, z = vec.z;
    double ax = glm::abs(x), ay = glm::abs(y), az = glm::abs(z);
    auto maxComponent = glm::max(ax, glm::max(ay, az)); 

    if(maxComponent == ax) {
        return Plane(point, glm::normalize(glm::vec3(-vec.x, 0, 0)));
    }
    else if(maxComponent == ay) {
        return Plane(point, glm::normalize(glm::vec3(0, -vec.y, 0)));
    }
    else if(maxComponent == az) {
        return Plane(point, glm::normalize(glm::vec3(0, 0, -vec.z)));
    }

}

Collision collides(Wall wall, const glm::vec3 origin, const glm::vec3 velocity) {
    Collision collision = noCollision();

    Plane plane = wall.getPlane();
    double signedDist = plane.signedDistanceTo(origin);
    
    double t0, t1;
    bool embeddedInPlane = false;

    float normDotVel = glm::dot(plane.normal, velocity);

    // velocity is parallel to plane
    if(normDotVel == 0.0f) {
        if(signedDist >= 1.0f) return noCollision();

        embeddedInPlane = true;
        t0 = 0.0;
        t1 = 1.0;
    } else {
        t0 = (-1.0-signedDist)/normDotVel;
        t1 = ( 1.0-signedDist)/normDotVel;

        if (t0 > t1) {
            double tmp = t1;
            t1 = t0;
            t0 = tmp;
        }

        // outside range, no collision
        if(t0 > 1.0f || t1 < 0.0f) {
            return noCollision();
        }

        // clamp
        t0 = glm::max(0.0, glm::min(1.0, t0));
        t1 = glm::max(0.0, glm::min(1.0, t1));
    }

    float t = 1.0;
    if(!embeddedInPlane) {
        glm::vec3 planeIntersectPoint = 
            origin - plane.normal
            + velocity * (float)t0;
        bool inside = wall.pointInside(planeIntersectPoint);
        if(inside) {
            t = t0;
            collision = Collision{
                true,
                planeIntersectPoint,
                plane
            };
        }
    }

    // early return as face collision always occurs first
    if(collision.success) {
        return collision;
    }

    double velSq = glm::length(velocity); 
    velSq *= velSq;

    float a, b, c;
    float newT;
    for(auto point : wall.getPoints()) {
        auto to = origin - point;
        a = velSq;
        b = 2.0 * glm::dot(velocity, to); 
        c = glm::length(to) * glm::length(to) - 1.0;
        if(getLowestRoot(a, b, c, t, &newT)) {
            t = newT;
            auto collisionPoint = origin + velocity * newT;
            auto slidePlane = nearestRightPlane(point - origin, point);
            // collision = Collision {
            //     true,
            //     collisionPoint,
            //     plane  
            // };
        }
    }

    // Edge Collision
    for(int i = 0; i < 4; i++) { 
        auto points = wall.getPoints();
        auto p1 = points.at(i);
        auto p2 = points.at((i + 1) % 4);
        auto edge = p2 -p1;

        auto baseToVertex = p1 - origin;
        double edgeSq = glm::length(edge) * glm::length(edge);
        double edgeDotVel = glm::dot(edge, velocity);
        double edgeDotBaseToVertex = glm::dot(edge, baseToVertex);

        a = edgeSq * -velSq + edgeDotVel * edgeDotVel;
        b = edgeSq * (2.0*glm::dot(velocity, baseToVertex)) - 2.0 * edgeDotVel * edgeDotBaseToVertex;
        c = edgeSq * (1.0 - baseToVertex.length() * baseToVertex.length()) + edgeDotBaseToVertex * edgeDotBaseToVertex;
        if(getLowestRoot(a, b, c, t, &newT)) {
            float f = (edgeDotVel * newT - edgeDotBaseToVertex) / edgeSq;
            if(f >= 0.0 && f <= 1.0) {
                auto intersection = origin + velocity * newT;
                t = newT;
                auto collisionPoint = p1 + f * edge;
                collision = Collision{
                    false,
                    collisionPoint,
                    Plane(collisionPoint, glm::normalize(origin - collisionPoint)) 
                };
            }
        }

        // glm::vec3 edge = p2-p1;
        // glm::vec3 edgeToOrigin = (origin + velocity) - p1;
        // float cosAng = glm::dot(edge, edgeToOrigin) / glm::length(edge) / glm::length(edgeToOrigin);
        // glm::vec3 projOrigin = p1 + glm::normalize(edge) * cosAng;
        // double distToEdge = glm::length((origin + velocity) - projOrigin);
        // std::cout << distToEdge << std::endl;
        // if(distToEdge <= 1.0) {
            // collision = Collision{
            //     true,
            //     origin + velocity,
            //     Plane(p1 - projOrigin, glm::normalize((origin + velocity) - projOrigin))
            // };
        // }
        // glm::vec3 start = origin;
        // glm::vec3 end = origin + velocity;
        // glm::vec3 lineDirection = glm::normalize(edge); // Direction of the line segment
        //
        // // Calculate the closest points on the line segment to the start and end points
        // glm::vec3 closestPointOnLineStart = p1 + glm::dot(start- p1, lineDirection) * lineDirection;
        // glm::vec3 closestPointOnLineEnd = p1 + glm::dot(end - p1, lineDirection) * lineDirection;
        //
        // // Calculate the distances between the start and end points and their closest points on the line segment
        // float distanceStart = glm::length(start - closestPointOnLineStart);
        // float distanceEnd = glm::length(end - closestPointOnLineEnd);
        //
        // // Check if the distances are less than or equal to 1.0
        // if (distanceStart <= 1.0f || distanceEnd <= 1.0f) {
        //     // collision = Collision{
        //     //     true,
        //     //     origin,
        //     //     nearestRightPlane(velocity, p1)
        //     // };
        //     continue;
        // }
    }
    return collision;
}

bool getLowestRoot(float a, float b, float c, float maxR,
float* root) {
    // Check if a solution exists
    float determinant = b*b - 4.0f*a*c;
    // If determinant is negative it means no solutions.
    if (determinant < 0.0f) return false;
    // calculate the two roots: (if determinant == 0 then
    // x1==x2 but let’s disregard that slight optimization)
    float sqrtD = std::sqrt(determinant);
    float r1 = (-b - sqrtD) / (2*a);
    float r2 = (-b + sqrtD) / (2*a);
    // Sort so x1 <= x2
    if (r1 > r2) {
        float temp = r2;
        r2 = r1;
        r1 = temp;
    }
    // Get lowest root:
    if (r1 > 0 && r1 < maxR) {
        *root = r1;
        return true;
    }
    // It is possible that we want x2 - this can happen
    // if x1 < 0
    if (r2 > 0 && r2 < maxR) {
        *root = r2;
        return true;
    }
    // No (valid) solutions
    return false;
}
};
#endif

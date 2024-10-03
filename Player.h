#ifndef PLAYER_H
#define PLAYER_H

#include "Plane.h"
#include "Wall.h"
#include "camera.h"
#include "glm/fwd.hpp"
#include <vector>
#include <cmath>

struct Collision {
    bool success;
    glm::vec3 point;
    Plane plane;
};

class Player {
    public:
        Player(glm::vec3 position, double height, double radius) : 
            position(position), 
            velocity(glm::vec3(0.0f, 0.0f, 0.0f)),
            hitbox(radius, height / 2.0, radius), 
            camera() {}

        void tick(double deltaTime) {

            if(glm::length(velocity) > 0) {
                velocity /= glm::length(velocity);
                velocity *=  (float)maxVelocity;
            }
            glm::vec3 posDelta = velocity * (float)deltaTime;
            glm::vec3 newVelocity = collideWithWorld(position, posDelta);

            auto preGravPos = position + newVelocity;
            auto gravDelta = glm::vec3(0.0, -5.0, 0.0) * (float)deltaTime;
            auto gravVel = collideWithWorld(preGravPos, gravDelta);  

            position = preGravPos + gravVel;

            // view bobbing
            // float time = glfwGetTime();
            // float bobOscillate = sin(time / 6.0 * glm::length(velocity) * (2 * M_PI));
            // glm::vec3 cameraPos = position;
            // if(glm::length(velocity) > 0) {
                // cameraPos = position + glm::vec3(0.0, bobOscillate / 5.0 + 0.3, 0.0); 
            // }

            camera.SetPosition(position);
            velocity = glm::vec3(); 
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
            while(true && glm::length(testVelocity) > tol) {
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
            glm::vec3 vDir;
            if (direction == FORWARD)
                vDir = glm::vec3(camera.Front.x, 0, camera.Front.z);
            if (direction == BACKWARD)
                vDir = -glm::vec3(camera.Front.x, 0, camera.Front.z);
            if (direction == LEFT)
                vDir = -glm::vec3(camera.Right.x, 0, camera.Right.z);
            if (direction == RIGHT)
                vDir = glm::vec3(camera.Right.x, 0, camera.Right.z);
            vDir /= glm::length(vDir);
            velocity += vDir;
        }

        void deltaY(double d) {
            velocity += glm::vec3(0, d, 0.0) / (float)glm::abs(d);
            camera.SetPosition(position);
        }

        Camera& getCamera() { 
            return camera;
        }

        void addCollider(Wall w) {
            colliders.push_back(w);
        }
private:
    glm::vec3 position;
    glm::vec3 velocity;
    double maxVelocity = 12.0;

    glm::vec3 hitbox = glm::vec3(1.0, 2.0, 1.0);

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
                collision = Collision {
                    true,
                    collisionPoint,
                    Plane(collisionPoint, glm::normalize(origin - collisionPoint)) 
                };
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
            c = edgeSq * (1.0 - glm::length(baseToVertex) * glm::length(baseToVertex)) + edgeDotBaseToVertex * edgeDotBaseToVertex;
            if(getLowestRoot(a, b, c, t, &newT)) {
                float f = (edgeDotVel * newT - edgeDotBaseToVertex) / edgeSq;
                if(f >= 0.0 && f <= 1.0) {
                    t = newT;
                    auto collisionPoint = p1 + f * edge;
                    collision = Collision{
                        true,
                        collisionPoint,
                        Plane(collisionPoint, glm::normalize(origin - collisionPoint)) 
                    };
                }
            }
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

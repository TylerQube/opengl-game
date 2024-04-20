#ifndef PLAYER_H
#define PLAYER_H

#include "Wall.h"
#include "camera.h"
#include "vec3.h"
#include <vector>

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
            velocity = velocity + vec3(0.0, -0.1, 0.0);
            glm::vec3 posDelta = glm::vec3(velocity.x(), velocity.y(), velocity.z()) * (float)deltaTime;
            glm::vec3 newPos = collideWithWorld(glm::vec3(position.x(), position.y(), position.z()), posDelta);

            auto preGravPos = position + vec3(newPos.x, newPos.y, newPos.z);
            auto gravDelta = glm::vec3(0.0, -3.0, 0.0) * (float)deltaTime;
            newPos += collideWithWorld(glm::vec3(preGravPos.x(), preGravPos.y(), preGravPos.z()), gravDelta);


            position += vec3(newPos.x, newPos.y, newPos.z);
            std::cout << "Position: " << this->position << std::endl;

            camera.SetPosition(position);
            velocity = vec3(); 
        }

        glm::vec3 projectVelocity(glm::vec3 origin, glm::vec3 vel, glm::vec3 normal) {
            Plane slidingPlane = Plane(origin, normal);
            auto dest = origin + vel;
            float dist = slidingPlane.signedDistanceTo(dest);
            glm::vec3 newDest = dest - slidingPlane.normal * dist;
            std::cout << "Project: " << newDest.x << std::endl;
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
                    else std::cout << "colliding!" << std::endl;
                    is_collision = true;

                    // respond 
                    testVelocity = projectVelocity(pos, testVelocity, obj.getPlane().normal);
                }
                if(!is_collision) break;
            }
            std::cout << "Post-collision: " << testVelocity.x << ' ' << testVelocity.y << ' ' << testVelocity.z << std::endl;
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

Collision collides(Wall wall, const glm::vec3 origin, const glm::vec3 velocity) {
    Plane plane = wall.getPlane();
    if(true || plane.isFrontFacingTo(glm::normalize(velocity))) {
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
            if(t0 > 1.0f || t1 < 0.0f) return noCollision();

            // clamp
            t0 = glm::max(0.0, glm::min(1.0, t0));
            t1 = glm::max(0.0, glm::min(1.0, t1));

            glm::vec3 collisionPoint;
            bool foundCollision = false;
            float t = 1.0;

            if(!embeddedInPlane) {
                glm::vec3 planeIntersectPoint = 
                    origin - plane.normal
                    + velocity * (float)t0;
                bool inside = wall.pointInside(planeIntersectPoint);
                if(inside) return Collision{
                    true,
                    planeIntersectPoint,
                    plane
                };
            }
        }
    }
    return noCollision();
}

};
#endif

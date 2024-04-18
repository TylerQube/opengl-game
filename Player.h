#ifndef PLAYER_H
#define PLAYER_H

#include "camera.h"
#include "vec3.h"

class Player {
    public:
        Player(point3 position, double height, double radius) : 
            position(position), 
            velocity(vec3(0.0f, 0.0f, 0.0f)),
            height(height), 
            radius(radius), 
            camera() {}

        void tick(double deltaTime) {
            if(velocity.length() > 0) velocity = (velocity / velocity.length()) * maxVelocity;
            position += velocity * deltaTime;
            camera.SetPosition(position);
            velocity = vec3(); 
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



        Camera& getCamera() { 
            return camera;
        }
    private:
        point3 position;
        vec3 velocity;
        double maxVelocity = 5.0;

        double height;
        double radius;

        Camera camera;       


};
#endif

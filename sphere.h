#ifndef SPHERE_H
#define SPHERE_H

#include "vec3.h"
#include "math.h"
#include <vector>

#define PI 3.14159265

class sphere {
    public:
        sphere(glm::vec3 center, double radius, int stacks, int slices) : 
            center(center), 
            radius(radius), 
            stacks(stacks), 
            slices(slices) {}

        void setCenter(glm::vec3 newCenter) {
            this->center = newCenter;
        }

        std::vector<float> *generateVertices() {
            auto vertices = new std::vector<float>;

            float x, y, z, xy;                              // vertex position

            float sectorStep = 2 * PI / slices;
            float stackStep = PI / stacks;
            float sectorAngle, stackAngle;
        
            for(int i = 0; i <= stacks; ++i) {
                stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
                xy = radius * cosf(stackAngle);             // r * cos(u)
                z = radius * sinf(stackAngle);
                for(int j = 0; j <= slices; ++j) {

                    sectorAngle = j * sectorStep;           // starting from 0 to 2pi

                    // vertex position (x, y, z)
                    x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
                    y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
                    auto p = glm::vec3(x, y, z);
                    p += this->center;
                    vertices->push_back(p.x);
                    vertices->push_back(p.y);
                    vertices->push_back(p.z); 

                }
            }
            
            return vertices;
        }

        std::vector<int> *generateIndices() {
            auto indices = new std::vector<int>;

            int k1, k2;
            for(int i = 0; i < stacks; ++i)
            {
                k1 = i * (slices + 1);     // beginning of current stack
                k2 = k1 + slices + 1;      // beginning of next stack

                for(int j = 0; j < slices; ++j, ++k1, ++k2)
                {
                    // 2 triangles per sector excluding first and last stacks
                    // k1 => k2 => k1+1
                    if(i != 0)
                    {
                        indices->push_back(k1);
                        indices->push_back(k2);
                        indices->push_back(k1 + 1);
                    }

                    // k1+1 => k2 => k2+1
                    if(i != (stacks-1))
                    {
                        indices->push_back(k1 + 1);
                        indices->push_back(k2);
                        indices->push_back(k2 + 1);
                    }
                }
            }
            return indices;
        }

        float* tempRect() {
            float *vertices = new float[24];
            vertices[0] = 0.5f;
            vertices[1] = 0.5f;
            vertices[2] = 0.0f;
            vertices[3] = 0.0f;
            vertices[4] = 0.0f;
            vertices[5] = 1.0f;

            vertices[6] = 0.5f;
            vertices[7] = -0.5f;
            vertices[8] = 0.0f;
            vertices[9] = 0.0f;
            vertices[10] = 0.0f;
            vertices[11] = 1.0f;

            vertices[12] = -0.5f;
            vertices[13] = -0.5f;
            vertices[14] = 0.0f;
            vertices[15] = 0.0f;
            vertices[16] = 0.0f;
            vertices[17] = 1.0f;

            vertices[18] = -0.5f;
            vertices[19] = 0.5f;
            vertices[20] = 0.0f;
            vertices[21] = 0.0f;
            vertices[22] = 0.0f;
            vertices[23] = 1.0f;
            return vertices;
        }

        float* tempCube() {
            float *cube_vertices = new float[36 * 6]{
                -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
                0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
                0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
                0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
                -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
                -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

                -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
                0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
                -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
                -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

                -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
                -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
                -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
                -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
                -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
                -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

                0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
                0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
                0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
                0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
                0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
                0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

                -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
                0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
                0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
                0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
                -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

                -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
                0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
                0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
                -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
                -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
            };
            
            return cube_vertices;
        }
    private:
        glm::vec3 center;
        double radius;
        int stacks;
        int slices;
};
#endif

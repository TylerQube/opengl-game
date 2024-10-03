#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H 

#include <vector>
#include "Plane.h"
#include "vec3.h"
#include "shader.h"
#include "stb_image.h"
#include "Mesh.h"

class Wall {
    public:
        Wall(Shader shader, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) : shader(shader), mesh(std::vector<Vertex>(), std::vector<unsigned int>()) {
            plane = Plane(p1, p2, p3); 
            glm::vec3 p4 = p3 - (p2 - p1);
            glm::vec3 points[] = {p1, p2, p3, p4};


            for(int i = 0; i < 4; i++) {
                glm::vec3 p = points[i];
                this->points.push_back(p);

                Vertex vertex;
                vertex.Position = p;
                vertex.Normal = plane.normal;
                
                switch(i) {
                    case 0:
                        vertex.TexCoords = glm::vec2(0.0, 0.0);
                        break;
                    case 1:
                        vertex.TexCoords = glm::vec2(1.0, 0.0);
                        break;
                    case 2:
                        vertex.TexCoords = glm::vec2(1.0, 1.0);
                        break;
                    case 3:
                        vertex.TexCoords = glm::vec2(0.0, 1.0);
                }
                this->meshVertices.push_back(vertex);
            }

            int arr[6] = {0, 1, 3, 1, 2, 3}; 
            for(int i : arr) {
                indices.push_back((unsigned int)i);
            }

            this->mesh = Mesh(this->meshVertices, indices);
        }

        void draw() {
            this->mesh.draw(shader);
        }

        void setColor(float r, float g, float b) {
            // this->mesh.setColorvec3(r, g, b);
        }

        void setTexture(const char* url) {
            this->mesh.setTexture(url); 
            // loadTexture();
        }

        Plane& getPlane() { return plane; }

        std::vector<glm::vec3>& getPoints() { return points; }

        bool pointInside(const glm::vec3 point) {
            bool inside1 = pointInsideTriangle(point, plane.normal, points[0], points[1], points[3]);
            bool inside2 = pointInsideTriangle(point, plane.normal, points[1], points[2], points[3]);
            return inside1 || inside2;
        }
    private:
        Shader shader; 
        Mesh mesh;
        std::vector<Vertex> meshVertices;

        Plane plane;

        std::vector<unsigned int> indices;
        std::vector<glm::vec3> points;

        bool pointInsideTriangle(const glm::vec3 point, const glm::vec3 normal, const glm::vec3 p1, const glm::vec3 p2, const glm::vec3 p3) {
            glm::vec3 edge1 = p2 - p1;
            glm::vec3 edge2 = p3 - p2;
            glm::vec3 edge3 = p1 - p3;

            glm::vec3 c1 = point - p1;
            glm::vec3 c2 = point - p2;
            glm::vec3 c3 = point - p3;

            return glm::dot(normal, glm::cross(edge1, c1)) > 0 &&
                   glm::dot(normal, glm::cross(edge2, c2)) > 0 &&
                   glm::dot(normal, glm::cross(edge3, c3)) > 0; 
        }

};
#endif

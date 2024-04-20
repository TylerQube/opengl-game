#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H 

#include <vector>
#include "Plane.h"
#include "vec3.h"
#include "shader.h"

class Wall {
    public:
        Wall(Shader shader, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) : shader(shader), plane(Plane(p1, p2, p3)) {
            glm::vec3 p4 = p3 - (p2 - p1);
            glm::vec3 points[] = {p1, p2, p3, p4};
            for(glm::vec3 p : points) {
                this->points.push_back(p);
                vertices.push_back((float)p.x);
                vertices.push_back((float)p.y);
                vertices.push_back((float)p.z);
                vertices.push_back((float)plane.normal.x);
                vertices.push_back((float)plane.normal.y);
                vertices.push_back((float)plane.normal.z);
            }

            int arr[6] = {0, 1, 3, 1, 2, 3}; 
            for(int i : arr) {
                indices.push_back(i);
            }
            

            genBuffers();
            loadData();
        }

        void genBuffers() {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &EBO);
            glGenBuffers(1, &VBO);
        }

        void loadData() {
            glBindVertexArray(VAO);

            std::cout << "Loading vertices into VBO..." << std::endl;
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

            std::cout << "Loading indices into EBO..." << std::endl;
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW);

            std::cout << "Setting position attribute..." << std::endl;
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

        }


        void draw() {
            shader.use();
            shader.setVec3(colorUniform, color.x(), color.y(), color.z());
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);  
        }

        void setColor(float r, float g, float b) {
            this->color = vec3(r, g, b);
        }

        Plane& getPlane() { return plane; }

        bool pointInside(const glm::vec3 point) {
            bool inside1 = pointInsideTriangle(point, plane.normal, points[0], points[1], points[3]);
            bool inside2 = pointInsideTriangle(point, plane.normal, points[1], points[2], points[3]);
            return inside1 || inside2;
        }
    private:
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
        Shader shader; 

        Plane plane;

        std::string colorUniform = "objectColor";
        vec3 color = vec3(0.0, 0.9, 0.2);

        std::vector<float> vertices;
        std::vector<int> indices;
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

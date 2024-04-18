#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H 

#include <vector>
#include "vec3.h"
#include "shader.h"

class Wall {
    public:
        Wall(Shader shader, point3 p1, point3 p2, point3 p3) : shader(shader) {
            point3 p4 = p3 - (p2 - p1);
            point3 points[] = {p1, p2, p3, p4};
            for(point3 p : points) {
                vertices.push_back((float)p.x());
                vertices.push_back((float)p.y());
                vertices.push_back((float)p.z());
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
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        }

        void draw() {
            shader.use();
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);  
        }
    private:
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
        Shader shader; 

        std::vector<float> vertices;
        std::vector<int> indices;

};
#endif

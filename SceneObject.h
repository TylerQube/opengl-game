#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H 

#include <vector>
#include "vec3.h"
#include "shader.h"

class SceneObject {
    public:
        void genBuffers() {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &EBO);
            glGenBuffers(1, &VBO);
        }
        virtual std::vector<point3> updateVertices() const = 0;
        virtual std::vector<point3> updateIndices() const = 0;

        void bind() {
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->vertices->size(), this->vertices->data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices->size(), indices->data(), GL_STATIC_DRAW);
        }
    private:
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
        Shader shader; 

        std::vector<point3> vertices;
        std::indices<point3> indices;
};
#endif

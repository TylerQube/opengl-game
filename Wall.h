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
        Wall(Shader shader, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) : mesh(std::vector<Vertex>(), std::vector<unsigned int>()), shader(shader) {
            shader = Shader(shader);
            plane = Plane(p1, p2, p3); 
            glm::vec3 p4 = p3 - (p2 - p1);
            glm::vec3 points[] = {p1, p2, p3, p4};


            for(int i = 0; i < 4; i++) {
                glm::vec3 p = points[i];
                this->points.push_back(p);


                Vertex vertex;
                vertex.Position = p;
                vertex.Normal = plane.normal;

                vertices.push_back((float)p.x);
                vertices.push_back((float)p.y);
                vertices.push_back((float)p.z);
                vertices.push_back((float)plane.normal.x);
                vertices.push_back((float)plane.normal.y);
                vertices.push_back((float)plane.normal.z);
                
                switch(i) {
                    case 0:
                        vertex.TexCoords = glm::vec2(0.0, 0.0);
                        vertices.push_back(0.0);
                        vertices.push_back(0.0);
                        break;
                    case 1:
                        vertex.TexCoords = glm::vec2(1.0, 0.0);
                        vertices.push_back(1.0);
                        vertices.push_back(0.0);
                        break;
                    case 2:
                        vertex.TexCoords = glm::vec2(1.0, 1.0);
                        vertices.push_back(1.0);
                        vertices.push_back(1.0);
                        break;
                    case 3:
                        vertex.TexCoords = glm::vec2(0.0, 1.0);
                        vertices.push_back(0.0);
                        vertices.push_back(1.0);
                        break;
                }
                this->meshVertices.push_back(vertex);
            }

            int arr[6] = {0, 1, 3, 1, 2, 3}; 
            for(int i : arr) {
                indices.push_back((unsigned int)i);
            }


            this->mesh = Mesh(this->meshVertices, indices);

            genBuffers();
            loadTexture();
            loadData();
        }

        void genBuffers() {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &EBO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &texture);
        }

        void loadData() {
            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),(void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);
        }

        void loadTexture() {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            int width, height, nrChannels;
            unsigned char* data = stbi_load(this->textureUrl, &width, &height, &nrChannels, 0);
            if(data) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            } else {
                std::cout << "Failed to load texture" << std::endl;
            }
            stbi_image_free(data);

            float wallWidth = glm::length(this->points.at(1) - this->points.at(0));  
            float wallHeight = glm::length(this->points.at(2) - this->points.at(1));  
            float xScale, yScale;
            if(wallWidth > wallHeight) {
                yScale = 1.0 ;
                xScale = wallWidth / wallHeight;
            } else {
                yScale = wallHeight / wallWidth;
                xScale = 1.0 ;
            }
            // set texture coordinates
            this->vertices.at(14) = xScale;
            this->vertices.at(31) = yScale;
            this->vertices.at(22) = xScale;
            this->vertices.at(23) = yScale;
        }

        void draw() {
            this->mesh.draw(shader);
            // shader.use();
            // shader.setVec3(colorUniform, color.x(), color.y(), color.z());
            // glBindVertexArray(VAO);
            // glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);  
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
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
        Shader shader; 
        Mesh mesh;
        std::vector<Vertex> meshVertices;

        unsigned int texture;
        const char* textureUrl;

        Plane plane;

        std::vector<float> vertices;
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

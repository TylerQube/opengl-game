#ifndef MESH_H 
#define MESH_H 

#include <vector>
#include <glm/glm.hpp>
#include "shader.h"
#include "stb_image.h"

using namespace std;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords; 
};

class Mesh {

public:
    vector<Vertex> vertices;
    vector<unsigned int> indices;

    Mesh(std::vector<Vertex> vertices, vector<unsigned int> indices) {
        this->vertices = vertices;
        this->indices = indices;
        setup();
    }

    void Draw(Shader& shader) {
        shader.use();
        // shader.setVec3(colorUniform, 0.5, 0.5, 0.5);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);  

    }

    void setup() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),(void*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void draw(Shader& shader) {
        shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);  
    }

    void loadTexture() {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        int width, height, nrChannels;
        unsigned char* data = stbi_load(this->textureUrl, &width, &height, &nrChannels, 0);
        if(data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

	    double side1 = getSide1();
	    double side2 = getSide2();

	    double ratio = side1 / side2;
	    if(ratio > 1) {
		vertices[1].TexCoords.x = ratio;
		vertices[2].TexCoords.x = ratio;
	    } else {
		vertices[2].TexCoords.y = 1 / ratio;
		vertices[3].TexCoords.y = 1 / ratio;
	    }
	    setup();
        } else {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }

    void setTexture(const char* url) {
        this->textureUrl = url;
        loadTexture();
    }
private:
    unsigned int VAO, VBO, EBO;

    const char* textureUrl;

    string colorUniform = "objectColor";
    string textureUniform = "myTexture";

    	double getSide1() {
		double side1 = glm::length(vertices[1].Position - vertices[0].Position);	
		return side1;
	}

    	double getSide2() {
		double side2 = glm::length(vertices[2].Position - vertices[1].Position);	
		return side2;
	}

};
#endif

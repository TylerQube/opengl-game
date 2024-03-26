#ifdef __APPLE__
// Defined before OpenGL and GLUT includes to avoid deprecation messages
#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>

#include "shader.h"

const size_t WIDTH = 640;
const size_t HEIGHT = 480;
const char* WINDOW_NAME = "Learn OpenGL";

std::string loadShaderSource(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return 0;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string shaderSource = buffer.str();
    return shaderSource; 
}

// const char *vertexShaderSource = "#version 330 core\n"
//     "layout (location = 0) in vec3 aPos;\n"
//     "void main()\n"
//     "{\n"
//     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
//     "}\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
} 

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void setupShape(unsigned int VAO, unsigned int VBO, unsigned int EBO, float vertices[], int vsize, unsigned int indices[], int isize) 
{
    glBindVertexArray(VAO);
    // copy vertices array into buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vsize, vertices, GL_STATIC_DRAW);
    // copy index array into element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, isize, indices, GL_STATIC_DRAW);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
  
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Create shader
    Shader myShader("./shaders/vertex.glsl", "./shaders/fragment.glsl");

    // specify how to interpret vertex data
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);

    float vertices1[] = {
         0.5f,  0.7f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.3f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.0f,  0.5f, 0.0f, 0.5f, 0.0f, 0.5f
    };
    unsigned int indices1[] = {
        0, 1, 3,
        1, 2, 3
    };

    float vertices2[] = {
        -0.5f,  0.7f, 0.0f, 1.0f, 0.0f, 0.5f,
        -0.5f, -0.3f, 0.0f, 1.0f, 0.7f, 0.1f,
         0.0f, -0.5f, 0.0f, 0.2f, 0.6f, 0.9f,
         0.0f,  0.5f, 0.0f, 0.3f, 0.0f, 0.0f,
    };
    unsigned int indices2[] = {
        1, 2, 3,
        0, 1, 3
    };

    float vertices3[] = {
         0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.7f,
         0.0f, 0.9f, 0.0f, 0.8f, 0.8f, 0.2f,
        -0.5f, 0.7f, 0.0f, 0.2f, 0.2f, 0.0f,
         0.5f, 0.7f, 0.0f, 0.5f, 0.1f, 0.5f,
    };
    unsigned int indices3[] = {
        0, 1, 2,
        0, 1, 3
    };

    // Generate buffers
    unsigned int num_shapes = 3;
    unsigned int VBOs[num_shapes], VAOs[num_shapes], EBOs[num_shapes];
    glGenBuffers(num_shapes, VBOs);
    glGenVertexArrays(num_shapes, VAOs);
    glGenBuffers(num_shapes, EBOs);

    // SETUP SHAPES
    setupShape(VAOs[0], VBOs[0], EBOs[0], vertices1, sizeof(vertices1), indices1, sizeof(indices1));
    setupShape(VAOs[1], VBOs[1], EBOs[1], vertices2, sizeof(vertices2), indices2, sizeof(indices2));
    setupShape(VAOs[2], VBOs[2], EBOs[2], vertices3, sizeof(vertices3), indices3, sizeof(indices3));

    //unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // draw in wireframe mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    while(!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // set background
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        myShader.use();

        float timeValue = glfwGetTime();
        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;

        myShader.set4Float("ourColor", 0.0f, greenValue, 0.0f, 0.0f);

        glBindVertexArray(VAOs[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(VAOs[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(VAOs[2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        // call events + swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }
    glfwTerminate();
    return 0;
}
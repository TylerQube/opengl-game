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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


const size_t WIDTH = 640;
const size_t HEIGHT = 480;
const char* WINDOW_NAME = "Learn OpenGL";
float alphaVal = 0.5f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
} 

void processInput(GLFWwindow *window)
{
    float delta = 0.01f;
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        alphaVal += delta;
        if(alphaVal > 1.0f) alphaVal = 1.0f;
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        alphaVal -= delta;
        if(alphaVal < 0.0f) alphaVal = 0.0f;
    }
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
    char stride = 8;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(6* sizeof(float)));
    glEnableVertexAttribArray(2);
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

    float vertices1[] = {
                                                // texture coords
         0.5f,  0.7f, 0.0f, 1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
         0.5f, -0.3f, 0.0f, 0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
         0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
         0.0f,  0.5f, 0.0f, 0.5f, 0.0f, 0.5f,   0.0f, 1.0f
    };
    unsigned int indices1[] = {
        0, 1, 3,
        1, 2, 3
    };

    float vertices2[] = {
        -0.5f,  0.7f, 0.0f, 1.0f, 0.0f, 0.5f,   1.0f, 1.0f,
        -0.5f, -0.3f, 0.0f, 1.0f, 0.7f, 0.1f,   1.0f, 0.0f,
         0.0f, -0.5f, 0.0f, 0.2f, 0.6f, 0.9f,   0.0f, 0.0f,
         0.0f,  0.5f, 0.0f, 0.3f, 0.0f, 0.0f,   0.0f, 1.0f,
    };
    unsigned int indices2[] = {
        1, 2, 3,
        0, 1, 3
    };

    float vertices3[] = {
         0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.7f,   1.0f, 1.0f,
         0.0f, 0.9f, 0.0f, 0.8f, 0.8f, 0.2f,   0.0f, 0.0f,
        -0.5f, 0.7f, 0.0f, 0.2f, 0.2f, 0.0f,   1.0f, 0.0f,
         0.5f, 0.7f, 0.0f, 0.5f, 0.1f, 0.5f,   0.0f, 1.0f,
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

    // generate texture
    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set wrapping / filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // load texture
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load("./textures/container.jpg", &width, &height, &nrChannels, 0);
    if(data) {
        // generate texture from loaded image
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    // free image memory
    stbi_image_free(data);
    

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set wrapping / filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // load happy face
    data = stbi_load("./textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if(data) {
        // generate texture from loaded image
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    // free image memory
    stbi_image_free(data);

    myShader.use();
    myShader.setInt("texture1", 0);
    myShader.setInt("texture2", 1);

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
        myShader.setFloat("alpha", alphaVal);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        for(int i = 0; i < num_shapes; i++)
        {
            glBindVertexArray(VAOs[i]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        // call events + swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }
    glfwTerminate();
    return 0;
}
#include <cstdlib>
#include <cstring>
#ifdef __APPLE__
// Defined before OpenGL and GLUT includes to avoid deprecation messages
#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "vec3.h"
#include "shader.h"
#include "camera.h"
#include "Player.h"
#include "sphere.h"
#include "Wall.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


const size_t WIDTH = 800;
const size_t HEIGHT = 600;
const char* WINDOW_NAME = "Learn OpenGL";

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
} 

std::vector<Wall> worldColliders;

Player player(point3(2.0001, 5.0, 0.0), 5.0, 0.5);
bool phong = true;
bool capture_mouse = true;

auto walls = std::vector<Wall>();

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
        
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        player.movePlayer(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        player.movePlayer(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        player.movePlayer(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        player.movePlayer(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        player.deltaY(0.3);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        player.deltaY(-0.3);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    {
        phong = !phong;
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS) 
    {
        capture_mouse = !capture_mouse;
        if(capture_mouse) 
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    player.getCamera().mouse_callback(window, xposIn, yposIn);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    player.getCamera().ProcessMouseScroll(static_cast<float>(yoffset));
}

int randomNum(int min, int max) {
    int range = max - min + 1;
    return rand() % range + min;
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

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);  

    player.getCamera().setLastMouse(WIDTH / 2.0, HEIGHT / 2.0);

    /*float vertices[] = {
         0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 
        -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f 
    };*/
    /*unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };*/

    float cube_vertices[] = {
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


    // container VAO
    unsigned int sVBO, cVBO, EBO, sVAO, cVAO;
    glGenVertexArrays(1, &sVAO);
    glGenVertexArrays(1, &cVAO);
    glGenBuffers(1, &EBO);

    glGenBuffers(1, &sVBO);
    glGenBuffers(1, &cVBO);

    glBindVertexArray(sVAO);

    auto sph = sphere(point3(0, 0, 0), 1, 200, 400);

    std::vector<float> *vertices = sph.generateVertices();
    glBindBuffer(GL_ARRAY_BUFFER, sVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices->size(), vertices->data(), GL_STATIC_DRAW);

    std::vector<int> *indices = sph.generateIndices();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices->size(), indices->data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);

    // LIGHT CUBE SETUP
    glBindVertexArray(cVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // draw in wireframe mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Create shader
    Shader lightingShader("./shaders/cont_vertex.glsl", "./shaders/cont_fragment.glsl");
    Shader lightCubeShader("./shaders/light_vertex.glsl", "./shaders/light_fragment.glsl");

    glm::vec3 lightPos(0.0f, 3.0f, 0.0f);


    auto floor = Wall(
        lightingShader,
        glm::vec3(-5, 0, -5),
        glm::vec3(-5, 0,  5),
        glm::vec3( 5, 0,  5)
    );
    auto wall1 = Wall(
        lightingShader,
        glm::vec3(-5, 0, -5),
        glm::vec3(-5, 0,  5),
        glm::vec3(-5, 3,  5)
    );
    auto wall2 = Wall(
        lightingShader,
        glm::vec3(-5, 0,  5),
        glm::vec3( 5, 0,  5),
        glm::vec3( 5, 3,  5)
    );
    auto wall3 = Wall(
        lightingShader,
        glm::vec3( 5, 0, -5),
        glm::vec3(-5, 0, -5),
        glm::vec3(-5, 3, -5)
    );

    auto gapWall1 = Wall(
        lightingShader,
        glm::vec3( 5, 0,  5),
        glm::vec3( 5, 0,  2),
        glm::vec3( 5, 3,  2)
    );
    auto gapWall2 = Wall(
        lightingShader,
        glm::vec3( 5, 0, -5),
        glm::vec3( 5, 0, -2),
        glm::vec3( 5, 3, -2)
    );

    auto floor2 = Wall(
        lightingShader,
        glm::vec3(10, 2, -5),
        glm::vec3(10, 2,  5),
        glm::vec3(30, 2,  5)
    );
    auto wall4 = Wall(
        lightingShader,
        glm::vec3(30, 2, -5),
        glm::vec3(30, 2,  5),
        glm::vec3(30, 5,  5)
    );
    auto wall5 = Wall(
        lightingShader,
        glm::vec3(10, 2,  5),
        glm::vec3(30, 2,  5),
        glm::vec3(30, 5,  5)
    );
    auto wall6 = Wall(
        lightingShader,
        glm::vec3(10, 2, -5),
        glm::vec3(30, 2, -5),
        glm::vec3(30, 5, -5)
    );

    auto gapWall3 = Wall(
        lightingShader,
        glm::vec3(10, 2,  5),
        glm::vec3(10, 2,  2),
        glm::vec3(10, 5,  2)
    );
    auto gapWall4 = Wall(
        lightingShader,
        glm::vec3(10, 2, -5),
        glm::vec3(10, 2, -2),
        glm::vec3(10, 5, -2)
    );


    auto ramp = Wall(
        lightingShader,
        glm::vec3(5, 0, 2),
        glm::vec3(10, 2, 2),
        glm::vec3(10, 2, -2)
    );
    auto tunnelWall1 = Wall(
        lightingShader,
        glm::vec3(5,  0, 2),
        glm::vec3(10, 2, 2),
        glm::vec3(10, 5, 2)
    );
    auto tunnelWall2 = Wall(
        lightingShader,
        glm::vec3(5,  0, -2),
        glm::vec3(10, 2, -2),
        glm::vec3(10, 5, -2)
    );

    walls.push_back(floor);
    walls.push_back(wall1);
    walls.push_back(wall2);
    walls.push_back(wall3);
    walls.push_back(floor2);
    walls.push_back(wall4);
    walls.push_back(wall5);
    walls.push_back(wall6);
    walls.push_back(gapWall1);
    walls.push_back(gapWall2);
    walls.push_back(gapWall3);
    walls.push_back(gapWall4);
    walls.push_back(ramp);
    walls.push_back(tunnelWall1);
    walls.push_back(tunnelWall2);

    for(Wall& w: walls) {
        std::cout << "Adding" << std::endl;
        player.addCollider(w);
        float r = randomNum(0, 255) / 255.0;
        float g = randomNum(0, 255) / 255.0;
        float b = randomNum(0, 255) / 255.0;
        std::cout << r << ' ' << g << ' ' << b << std::endl;
        w.setColor(r, g, b);  
    }
    // render loop
    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        processInput(window);
        player.tick(deltaTime);

        // set background
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader 
        lightingShader.use();
        lightingShader.setVec3("objectColor", 0.0f, 0.9f, 0.3f);
        lightingShader.setVec3("lightColor", 1.0f, 0.5f, 0.3f);
        lightingShader.setVec3("lightPos", lightPos);

        unsigned int phongLoc = glGetUniformLocation(lightingShader.ID, "phong");
        glUniform1i(phongLoc, phong);

        lightingShader.setVec3("viewPos", player.getCamera().Position);

        glm::mat4 view       = glm::mat4(1.0f);
        glm::mat4 model      = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        model = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(player.getCamera().Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        view = player.getCamera().GetViewMatrix();

        glUniformMatrix4fv(glGetUniformLocation(lightingShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(sVAO);

        float newX = sinf(glfwGetTime());
        sph.setCenter(point3(newX, 0, 0));
        vertices = sph.generateVertices();
        glBindBuffer(GL_ARRAY_BUFFER, sVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices->size(), vertices->data(), GL_STATIC_DRAW);

        //glDrawElements(GL_TRIANGLES, indices->size(), GL_UNSIGNED_INT, 0);

        for(auto w: walls) {
            w.draw(); 
        }

        lightCubeShader.use();
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(cVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // call events + swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }
    glfwTerminate();
    return 0;
}

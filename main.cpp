#ifdef __APPLE__
// Defined before OpenGL and GLUT includes to avoid deprecation messages
#define GL_SILENCE_DEPRECATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif

#include <iostream>

const size_t WIDTH = 640;
const size_t HEIGHT = 480;
const char* WINDOW_NAME = "Learn OpenGL";

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.7f, 0.0f, 1.0f);\n"
    "}\0";
const char *fragmentShaderSource2 = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(0.0f, 0.7f, 0.1f, 1.0f);\n"
    "}\0";

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

unsigned int compileShader(const char *source, int shader_type, const std::string& name) 
{
    unsigned int shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::" << name << "::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader)
{
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for failure
    int  success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    }
    return shaderProgram;
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

    // Compile shaders
    unsigned int fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER, "FRAGMENT");
    unsigned int fragmentShader2 = compileShader(fragmentShaderSource2, GL_FRAGMENT_SHADER, "FRAGMENT2");
    unsigned int vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER, "VERTEX");

    // create shader program
    unsigned int rectShader = createShaderProgram(vertexShader, fragmentShader);
    unsigned int triangleShader = createShaderProgram(vertexShader, fragmentShader2);


    // delete shaders after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // specify how to interpret vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    float rect_vertices[] = {
         0.5f,  0.7f, 0.0f,
         0.5f, -0.7f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    unsigned int rect_indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    float tri_vertices[] = {
        -0.6f, -0.4f, 0.0f,
        0.6f, -0.4f, 0.0f,
        0.0f, 0.8f, 0.0f
    };
    unsigned int tri_indices[] = {
        0, 1, 2
    };

    // send vertices to buffer
    unsigned int VBOs[2], VAOs[2], EBOs[2];
    glGenBuffers(2, VBOs);
    // generate VAO - Vertex Array Object
    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, EBOs);

    // SETUP RECTANGLE
    setupShape(VAOs[0], VBOs[0], EBOs[0], rect_vertices, sizeof(rect_vertices), rect_indices, sizeof(rect_indices));
    // SETUP TRIANGLE
    setupShape(VAOs[1], VBOs[1], EBOs[1], tri_vertices, sizeof(tri_vertices), tri_indices, sizeof(tri_indices));

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

        glUseProgram(rectShader);
        glBindVertexArray(VAOs[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(triangleShader);
        glBindVertexArray(VAOs[1]);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        // call events + swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }
    glfwTerminate();
    return 0;
}
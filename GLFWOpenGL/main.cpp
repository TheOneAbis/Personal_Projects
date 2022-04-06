// GLFWOpenGL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>


const GLuint WIDTH = 800, HEIGHT = 600;
// Shaders
// vertexShaderSource handles positioning and location of triangle
const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
"}\0";

// Fragment shader source handles color and texture of triangle
const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

int main()
{
    // Init GLFW
    glfwInit();
    //Set required options for GLFW window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions 
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGLTest", nullptr, nullptr);

    // What this line of code actually does is it gets the actual width of the screen window itself, relative to the density of the screen.
    // Prevents compatibility issues w/ different platforms, screen sizes, and pixel densities as this gets the actual window width and height with those changes
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    // Failed to Create window
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions 
    glewExperimental = GL_TRUE;

    // Initialize GLEW to setup the OpenGL Function pointers 
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }
    // Define the viewport dimensions 
    glViewport(0, 0, screenWidth, screenHeight);

    // Build and compile our shader program

    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // source of the shader
    glCompileShader(vertexShader);
    // Check for compile time errors - these 2 variables used in all shader compilations
    GLint success;
    GLchar infoLog[512];
    // Return values for shader object via params
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    // Check if shader was successfully compiled
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog); // get info log, passing it into infoLog char array
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment shader - similar compilation sequence
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // GAME LOOP
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activated (key pressed, mouse moved, etc.) and call corresponding response functions
        glfwPollEvents();

        // Render and clear the color buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw OpenGL
        glfwSwapBuffers(window);
    }
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();

    return EXIT_SUCCESS;
}
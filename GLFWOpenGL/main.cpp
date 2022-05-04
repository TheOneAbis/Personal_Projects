// GLFWOpenGL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>


const GLuint WIDTH = 800, HEIGHT = 600;
// Shaders
// vertexShaderSource handles positioning and location of triangle

// Fragment shader source handles color and texture of triangle


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
    // Link shaders
    // a Program object is an object by which shaders can be attached.
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram); // link program w/ newly attached shaders
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKED_FAILED\n" << infoLog << std::endl;
    }
    // Shaders have been successfully linked and are part of the shader program, so can delete them here
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Set up vertex data (and buffer(s)) and attribute pointers
    // The values of the screen coords range between -1 and 1 by default is not explicitely set
    GLfloat vertices[] =
    {
        -0.5f, -0.5f, 0.0f, // Left
         0.5f, -0.5f, 0.0f,  // Right
         0.0f, 0.5f, 0.0f    // Top
    }; // creates a triangle
    GLuint VBO, VAO; // Vertex Buffer Object, Vertex Array Object
    glGenVertexArrays(1, &VAO); // these 2 functions generate the vertex array and buffers in VAO and VBO
    glGenBuffers(1, &VBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // Create and enable the vertex pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Note that this is allowed, the call to glVertexAttribPointer 
    // registered VBO as the currently bound vertex buffer object so 
    // afterwards we can safely unbind

    glBindVertexArray(0);
    // Unbind VAO (it's always a good thing to unbind any buffer/array 
    // to prevent strange bugs)
    // (Passing in 0 unbinds)

    // GAME LOOP
    while (!glfwWindowShouldClose(window))
    {
        // Check if any events have been activated (key pressed, mouse moved, etc.) and call corresponding response functions
        glfwPollEvents();

        // Render and clear the color buffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw Stuff HERE
        glUseProgram(shaderProgram); // indicate what shader program we are using to draw
        glBindVertexArray(VAO); // bind vertex array (our triangle)
        glDrawArrays(GL_TRIANGLES, 0, 3); // Draw the vertices to the screen
        glBindVertexArray(0); // unbind array

        // Draw OpenGL
        glfwSwapBuffers(window);
    }
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();

    return EXIT_SUCCESS;
}
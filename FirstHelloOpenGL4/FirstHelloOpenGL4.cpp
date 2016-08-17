// FirstHelloOpenGL4.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <gl\glew.h>
#include <gl\glfw3.h>

#include <iostream>
#include <fstream>

namespace
{
    template<typename T, std::size_t N>
    constexpr std::size_t arraySize(T(&)[N]) noexcept //reference to array of type T
    {
        return N;
    }

    void AssertOnError()
    {
        GLenum errCode1 = glGetError();
        if (errCode1 != GL_NO_ERROR)
            _ASSERTE(errCode1 == GL_NO_ERROR);
    }

    std::string readContentsOfFile(std::string filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return std::string();
        }
        std::istreambuf_iterator<char> eos;
        std::string s(std::istreambuf_iterator<char>(file), eos);
        return s;
    }
};
int main()
{
    //Start GL context and a window using GLFW helper library
    if (!glfwInit())
    {
        std::cerr << "ERROR: could not start GLFW3\n";
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello Srinadh's Triangle", NULL, NULL);
    if (!window)
    {
        std::cerr << "ERROR: Could not open window in GLFW3\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    //Start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();

    //Get version info
    std::string renderer = std::string(reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
    std::string version  = std::string(reinterpret_cast<const char *>(glGetString(GL_VERSION)));

    std::cout << "Renderer: " << renderer.c_str() << std::endl;
    std::wcout << "Version: " << version.c_str() << std::endl;

    //Now lets render something
    glEnable(GL_DEPTH_TEST); //enable depth testing.
    glDepthFunc(GL_LESS); // depth testing interprets a smaller values as closer to the eye that are projected onto same pixel.

    //add a simple triangle into VBO unit in GPU.
    GLfloat tinPts[] = {
         0.0f,  1.0f, 0.0f,
         1.0f,  0.0f, 0.0f,
        -1.0f,  0.0f, 0.0f,
         0.0f, -1.0f, 0.0f,
    };

    std::size_t tinSize = arraySize(tinPts) * sizeof(GLfloat);

    //Create a Vertex buffer object
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, tinSize, tinPts, GL_STATIC_DRAW);

    //Use of Vertex attribute object (VAO)
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0); //First attribute of the mesh
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL); //Defines layout of the mesh. 3 mean vec3

    //Setup the vertex shader and compile and use.
    std::string vertexShader = readContentsOfFile("simple.vert");
    const char* vertShader   = vertexShader.c_str();
    const GLint vertShaderSz = vertexShader.size();
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertShader, &vertShaderSz);
    glCompileShader(vs);

    //Setup the fragment shader and compile and use.
    std::string fragmentShader = readContentsOfFile("simple.frag");
    const char* fragShader = fragmentShader.c_str();
    const GLint fragShaderSz = fragmentShader.size();
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragShader, &fragShaderSz);
    glCompileShader(fs);

    //Execute the shaders in GPU.
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, fs);
    glAttachShader(shaderProgram, vs);
    glLinkProgram(shaderProgram);

    //Let GPI draw.
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);

        // Draw triangles (3 points 0-2) from the currently bound VAO with current in-use shader
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        // Update other events like input handling
        glfwPollEvents();
        // Put the stuff we've been drawing onto the display.
        glfwSwapBuffers(window);
    }

    //Finally done.
    glfwTerminate();

    return 0;
}


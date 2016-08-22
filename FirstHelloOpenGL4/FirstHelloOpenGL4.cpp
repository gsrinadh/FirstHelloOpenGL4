// FirstHelloOpenGL4.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <gl\glew.h>
#include <gl\glfw3.h>

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>

namespace
{
    std::ofstream g_infoLogger, g_errorLogger;
    int g_WindowsWt = 640, g_WindowsHt = 480;

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

    std::string ReadContentsOfFile(std::string filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return std::string();
        }
        std::istreambuf_iterator<char> eos;
        std::string s(std::istreambuf_iterator<char>(file), eos);
        return s;
    }

    //Variadic template usage for typesafe form of printf.
    void TPrintf(std::ostream &file, const char* format) // base function
    {
        file << format << std::endl;
    }

    template<typename T, typename... Targs>
    void TPrintf(std::ostream &file, const char* format, T value, Targs... Fargs) // recursive variadic function
    {
        //Now print the message with arguments
        for (; *format != '\0'; format++) {
            if (*format == '%') {
                ++format;
                file << value;
                TPrintf(file, format + 1, Fargs...); // recursive call
                return;
            }
            file << *format;
        }
    }

    template<class... T>
    bool LogInfo(const char *message, T... params)
    {
        //Get time from system
        //std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
        //time_t asciiTime = std::chrono::system_clock::to_time_t(currentTime);
        //g_infoLogger << std::endl << ctime(&asciiTime);

        TPrintf(g_infoLogger, message, params...);

        return true;
    }

    template<class... T>
    bool LogError(const char *message, T... params)
    {
        //Get time from system
        std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
        time_t asciiTime = std::chrono::system_clock::to_time_t(currentTime);
        g_errorLogger << ctime(&asciiTime) << std::endl;

        TPrintf(g_errorLogger, message, params...);
        TPrintf(std::cerr, message, params...);

        return true;
    }

    void LogGLParams()
    {
        GLenum params[] = {
            GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
            GL_MAX_CUBE_MAP_TEXTURE_SIZE,
            GL_MAX_DRAW_BUFFERS,
            GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
            GL_MAX_TEXTURE_IMAGE_UNITS,
            GL_MAX_TEXTURE_SIZE,
            GL_MAX_VARYING_FLOATS,
            GL_MAX_VERTEX_ATTRIBS,
            GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
            GL_MAX_VERTEX_UNIFORM_COMPONENTS,
            GL_MAX_VIEWPORT_DIMS,
            GL_STEREO,
        };
        const char* names[] = {
            "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
            "GL_MAX_CUBE_MAP_TEXTURE_SIZE",
            "GL_MAX_DRAW_BUFFERS",
            "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
            "GL_MAX_TEXTURE_IMAGE_UNITS",
            "GL_MAX_TEXTURE_SIZE",
            "GL_MAX_VARYING_FLOATS",
            "GL_MAX_VERTEX_ATTRIBS",
            "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS",
            "GL_MAX_VERTEX_UNIFORM_COMPONENTS",
            "GL_MAX_VIEWPORT_DIMS",
            "GL_STEREO",
        };

        LogInfo("GL Context Params:\n");
        size_t len = arraySize(params);
        for (size_t i = 0; i < len-2; ++i)
        {
                int v = 0;
                glGetIntegerv(params[i], &v);
                LogInfo("%s %i\n", names[i], v);
        }

        // others
        int v[] = { 0, 0 };
        glGetIntegerv(params[len-2], v);
        LogInfo("%s %i %i\n", names[len - 2], v[0], v[1]);
        unsigned char s = 0;
        glGetBooleanv(params[len - 1], &s);
        LogInfo("%s %i\n", names[len - 1], (unsigned int)s);
        LogInfo("-----------------------------\n");
    }

    bool StartLoggers()
    {
        std::string infoFileName = "C:\\Temp\\ogl4.log";
        //std::ofstream file(fileName);
        g_infoLogger.open(infoFileName);
        if (!g_infoLogger.is_open())
        {
            std::cerr << "ERROR: Could not open info logger " << infoFileName.c_str() << " for writing" << std::endl;
            return false;
        }

        std::string errorFileName = "C:\\Temp\\errors.log";
        g_errorLogger.open(errorFileName);
        if (!g_errorLogger.is_open())
        {
            std::cerr << "ERROR: Could not open error logger " << errorFileName.c_str() << " for writing" << std::endl;
            return false;
        }

        return true;
    }

    void GlfwErrorCallback(int error, const char* desc)
    {
        LogError("GLFW_ERROR: code %i msg: %s\n", error, desc);
    }

    void GlfwWindowSizeCallback(GLFWwindow *window, int width, int height)
    {
        g_WindowsHt = height;
        g_WindowsWt = width;

        LogInfo( "Window size changed to %i %i", width, height);
    }
};

int main()
{
    StartLoggers();

    LogInfo("Starting GLFW\n%s\n", glfwGetVersionString());
    glfwSetErrorCallback(GlfwErrorCallback);

    //Start GL context and a window using GLFW helper library
    if (!glfwInit())
    {
        std::cerr << "ERROR: could not start GLFW3\n";
        return 1;
    }

    //Full screen rendering on primary monitor
    //GLFWmonitor* mon = glfwGetPrimaryMonitor ();
    //const GLFWvidmode* vmode = glfwGetVideoMode (mon);
    //GLFWwindow* window = glfwCreateWindow (
    //vmode->width, vmode->height, "Extended GL Init", mon, NULL
    //);
    GLFWwindow* window = glfwCreateWindow(g_WindowsWt, g_WindowsHt, "Hello Srinadh's Triangle", NULL, NULL);
    if (!window)
    {
        std::cerr << "ERROR: Could not open window in GLFW3\n";
        glfwTerminate();
        return 1;
    }

    //Set callback when windows size changes.
    glfwSetWindowSizeCallback(window, GlfwWindowSizeCallback);

    glfwMakeContextCurrent(window);

    //Limit Anti-aliasing sample size to 4x4 to minimize expensive smoothing operation.
    glfwWindowHint(GLFW_SAMPLES, 4);
    //Start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();

    //Get version info
    std::string renderer = std::string(reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
    std::string version  = std::string(reinterpret_cast<const char *>(glGetString(GL_VERSION)));

    LogInfo("Renderer is %s and GL Version is %s", renderer.c_str(), version.c_str());

    LogGLParams();

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
    std::string vertexShader = ReadContentsOfFile("simple.vert");
    const char* vertShader   = vertexShader.c_str();
    const GLint vertShaderSz = static_cast<GLint>(vertexShader.size());
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertShader, &vertShaderSz);
    glCompileShader(vs);

    //Setup the fragment shader and compile and use.
    std::string fragmentShader = ReadContentsOfFile("simple.frag");
    const char* fragShader = fragmentShader.c_str();
    const GLint fragShaderSz = static_cast<GLint>(fragmentShader.size());
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

        glViewport(0, 0, g_WindowsWt, g_WindowsHt);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);

        // Draw triangles (3 points 0-2) from the currently bound VAO with current in-use shader
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        // Update other events like input handling
        glfwPollEvents();
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) 
        {
            glfwSetWindowShouldClose(window, 1);
        }
        // Put the stuff we've been drawing onto the display.
        glfwSwapBuffers(window);
    }

    //Finally done.
    glfwTerminate();

    return 0;
}


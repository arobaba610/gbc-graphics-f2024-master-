﻿#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Math.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

GLuint CreateShader(GLint type, const char* path);
GLuint CreateProgram(GLuint vs, GLuint fs);

std::array<int, GLFW_KEY_LAST> gKeysCurr{}, gKeysPrev{};
bool IsKeyDown(int key);
bool IsKeyUp(int key);
bool IsKeyPressed(int key);

int main(void)
{
    // Lines 20-40 are all window creation. You can ignore this if you want ;)
    assert(glfwInit() == GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#ifdef NDEBUG
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Graphics 1", NULL, NULL);
    glfwMakeContextCurrent(window);
    assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));
    glfwSetKeyCallback(window, key_callback);

#ifdef NDEBUG
#else
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(glDebugOutput, nullptr);
#endif

    // Create a vertex shader, a fragment shader, and a shader program (vs + fs)
    GLuint vs = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/default.vert");
    GLuint fsUniformColor = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/uniform_color.frag");
    GLuint vsVertexColor = CreateShader(GL_VERTEX_SHADER, "./assets/shaders/vertex_color.vert");
    GLuint fsVertexColor = CreateShader(GL_FRAGMENT_SHADER, "./assets/shaders/vertex_color.frag");
    GLuint shaderUniformColor = CreateProgram(vs, fsUniformColor);
    GLuint shaderVertexColor = CreateProgram(vsVertexColor, fsVertexColor);

    // Positions of our triangle's (3D) vertices (CCW winding-order)
    float positions[] = {
        // positions        // colors (RGB)
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Vertex 1: Kırmızı (Sağ alt)
        0.0f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // Vertex 2: Yeşil (Üst orta)
       -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // Vertex 3: Mavi (Sol alt)
    };

    // vao = "Vertex Array Object", vbo = "Vertex Buffer Object"
    // A vao is a collection of vbos.
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao); // Allocate a vao handle
    glBindVertexArray(vao);     // Bind the vao - tells the GPU we want to work with this vao!
    glGenBuffers(1, &vbo);      // Allocate a vbo handle
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // Bind the vbo - tells the gpu which data we want in our vao.

    // Uploads data to the bound vbo - 9 3d floating-points as positions!
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);    

    // Describes the data of the bound vbo - attribute 0, 3-component floating-point number
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Pozisyon verisi

    // Enable the attribute we just bound (all attributes are disabled by default)!
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Renk verisi
    glEnableVertexAttribArray(1);

    // In summary, we need 3 things to render:
    // 1. Vertex data -- right now just positions.
    // 2. Shader -- vs forwards input, fs colours.
    // 3. Draw call -- draw 3 vertices interpreted as a triangle (GL_TRIANGLES)
    // *** Everything is just data and behaviour ***
    // *** vao & vbo describe data, shaders describe behaviour ***

    GLint u_world = glGetUniformLocation(shaderUniformColor, "u_world");
    GLint u_color = glGetUniformLocation(shaderUniformColor, "u_color");
    GLint u_intensity = glGetUniformLocation(shaderUniformColor, "u_intensity");

    int object = 0;
    printf("Object %i\n", object + 1);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float time = glfwGetTime(); 
        Matrix world = MatrixIdentity();

        switch (object + 1)
        {
        // Hint: Change the colour to white
        case 1: // Beyaz üçgen
            glUseProgram(shaderUniformColor);
            world = MatrixIdentity();
            glUniformMatrix4fv(u_world, 1, GL_FALSE, ToFloat16(world).v);
            glUniform3f(u_color, 1.0f, 1.0f, 1.0f);
            glUniform1f(u_intensity, 1.0f);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;

        // Hint: Switch the shader to colour based on vertex positions
        // If you get errors in the console, comment out all unused uniforms
        case 2: // Rainbow üçgen
            glUseProgram(shaderVertexColor);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;

        // Hint: Make intensity change from 0 to 1 using a periodic function (sin or cos)
        case 3: // Zamanla renk değişimi
        {
            glUseProgram(shaderUniformColor);
            glUniformMatrix4fv(u_world, 1, GL_FALSE, ToFloat16(world).v);
            float intensity = (sin(glfwGetTime()) * 0.5f) + 0.5f; // Hata çözüldü
            glUniform3f(u_color, intensity, 0.0f, 1.0f - intensity);
            glUniform1f(u_intensity, 1.0f);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;
        }


        // Hint: Use the Translate function
        case 4: // X ekseninde ileri geri hareket eden üçgen
        {
            glUseProgram(shaderUniformColor);
            float xPosition = sin(glfwGetTime()) * 1.0f; // Hata çözüldü
            Matrix world = Translate(xPosition, 0.0f, 0.0f);
            glUniformMatrix4fv(u_world, 1, GL_FALSE, ToFloat16(world).v);
            glUniform3f(u_color, 1.0f, 0.0f, 1.0f);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;
        }

        // Hint: Use the RotateZ function
        case 5: // Sürekli dönen üçgen
            glUseProgram(shaderUniformColor);
            world = RotateZ(glfwGetTime());
            glUniformMatrix4fv(u_world, 1, GL_FALSE, ToFloat16(world).v);
            glUniform3f(u_color, 0.0f, 1.0f, 1.0f);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;
        }

        // Change object when space is pressed
        if (IsKeyPressed(GLFW_KEY_SPACE))
        {
            ++object %= 5;
            printf("Object %i\n", object + 1);
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll and process events */
        memcpy(gKeysPrev.data(), gKeysCurr.data(), GLFW_KEY_LAST * sizeof(int));
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Disable repeat events so keys are either up or down
    if (action == GLFW_REPEAT) return;

    if (key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    gKeysCurr[key] = action;

    // Key logging
    //const char* name = glfwGetKeyName(key, scancode);
    //if (action == GLFW_PRESS)
    //    printf("%s\n", name);
}

// Compile a shader
GLuint CreateShader(GLint type, const char* path)
{
    GLuint shader = GL_NONE;
    try
    {
        // Load text file
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open(path);

        // Interpret the file as a giant string
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();

        // Verify shader type matches shader file extension
        const char* ext = strrchr(path, '.');
        switch (type)
        {
        case GL_VERTEX_SHADER:
            assert(strcmp(ext, ".vert") == 0);
            break;

        case GL_FRAGMENT_SHADER:
            assert(strcmp(ext, ".frag") == 0);
            break;
        default:
            assert(false, "Invalid shader type");
            break;
        }

        // Compile text as a shader
        std::string str = stream.str();
        const char* src = str.c_str();
        shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, NULL);
        glCompileShader(shader);

        // Check for compilation errors
        GLint success;
        GLchar infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cout << "Shader failed to compile: \n" << infoLog << std::endl;
        }
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "Shader (" << path << ") not found: " << e.what() << std::endl;
        assert(false);
    }

    return shader;
}

// Combine two compiled shaders into a program that can run on the GPU
GLuint CreateProgram(GLuint vs, GLuint fs)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        program = GL_NONE;
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    return program;
}

// Graphics debug callback
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

bool IsKeyDown(int key)
{
    return gKeysCurr[key] == GLFW_PRESS;
}

bool IsKeyUp(int key)
{
    return gKeysCurr[key] == GLFW_RELEASE;
}

bool IsKeyPressed(int key)
{
    return gKeysPrev[key] == GLFW_PRESS && gKeysCurr[key] == GLFW_RELEASE;
}

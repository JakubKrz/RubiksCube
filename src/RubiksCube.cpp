#include <windows.h>
#include <GL/gl.h>
#include "GL/wglext.h"
#include "Window.h"
#include "OpenGLContext.h"
#include "Shader.h"
#include "GLLoader.h"
        
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    Window win(800, 600, L"Rubic's Cube");
    OpenGLContext gl;
    
    if (!gl.Init(win.GetHDC())) {
        MessageBox(NULL, L"Failed to initialize OpenGL 4.6!", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    Shader myShader("Shaders/VertexShader.vert", "Shaders/FragmentShader.frag");

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (win.IsOpen()) {
        win.ProcessMessages();

        glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myShader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        win.Swap();
    }

    return 0;
}
#include <windows.h>
#include <GL/gl.h>
#include "GL/wglext.h"
#include "Window.h"
#include "OpenGLContext.h"
        
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    Window win(800, 600, L"Rubic's Cube");
    OpenGLContext gl;
    
    if (!gl.Init(win.GetHDC())) {
        MessageBox(NULL, L"Failed to initialize OpenGL 4.6!", L"Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    while (win.IsOpen()) {
        win.ProcessMessages();

        glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        win.Swap();
    }

    return 0;
}
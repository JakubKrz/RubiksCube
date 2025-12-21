#include "OpenGLContext.h"
#include "gl/GL.h"
#include "GL/wglext.h"
#include "GL/glcorearb.h"

#define LOAD_PROC(type, name) name = (type)wglGetProcAddress(#name);

PFNGLGENVERTEXARRAYSPROC    glGenVertexArrays = nullptr;
PFNGLBINDVERTEXARRAYPROC    glBindVertexArray = nullptr;
PFNGLGENBUFFERSPROC         glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC         glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC         glBufferData = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC    glVertexAttribPointer = nullptr;

PFNGLCREATESHADERPROC       glCreateShader = nullptr;
PFNGLSHADERSOURCEPROC       glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC      glCompileShader = nullptr;
PFNGLCREATEPROGRAMPROC      glCreateProgram = nullptr;
PFNGLATTACHSHADERPROC       glAttachShader = nullptr;
PFNGLLINKPROGRAMPROC        glLinkProgram = nullptr;
PFNGLUSEPROGRAMPROC         glUseProgram = nullptr;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr;
PFNGLUNIFORMMATRIX4FVPROC   glUniformMatrix4fv = nullptr;

bool OpenGLContext::Init(HDC hdc) {
    HGLRC tempContext = wglCreateContext(hdc);
    wglMakeCurrent(hdc, tempContext);

    auto wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    if (wglCreateContextAttribsARB) {
        int attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 6,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };
        renderingContext = wglCreateContextAttribsARB(hdc, NULL, attribs);

        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(tempContext);
        wglMakeCurrent(hdc, renderingContext);
    }

    LOAD_PROC(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
    LOAD_PROC(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
    LOAD_PROC(PFNGLGENBUFFERSPROC, glGenBuffers);
    LOAD_PROC(PFNGLBINDBUFFERPROC, glBindBuffer);
    LOAD_PROC(PFNGLBUFFERDATAPROC, glBufferData);
    LOAD_PROC(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
    LOAD_PROC(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);

    LOAD_PROC(PFNGLCREATESHADERPROC, glCreateShader);
    LOAD_PROC(PFNGLSHADERSOURCEPROC, glShaderSource);
    LOAD_PROC(PFNGLCOMPILESHADERPROC, glCompileShader);
    LOAD_PROC(PFNGLCREATEPROGRAMPROC, glCreateProgram);
    LOAD_PROC(PFNGLATTACHSHADERPROC, glAttachShader);
    LOAD_PROC(PFNGLLINKPROGRAMPROC, glLinkProgram);
    LOAD_PROC(PFNGLUSEPROGRAMPROC, glUseProgram);
    LOAD_PROC(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
    LOAD_PROC(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);

    return renderingContext != nullptr;
}

OpenGLContext::~OpenGLContext() {
    if (renderingContext) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(renderingContext);
    }
}
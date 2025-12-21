#include "OpenGLContext.h"
#include "gl/GL.h"
#include "GL/wglext.h"
#include "GL/glcorearb.h"
    
#define X(type, name) type name = nullptr;
#include "GLFunctionsList.h"
#undef X

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

    #define X(type, name) name = (type)wglGetProcAddress(#name);
    #include "GLFunctionsList.h"
    #undef X

    return renderingContext != nullptr;
}

OpenGLContext::~OpenGLContext() {
    if (renderingContext) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(renderingContext);
    }
}
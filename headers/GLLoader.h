#pragma once
#include <Windows.h>
#include <GL/gl.h>
#include "GL/glcorearb.h"
#include "GL/wglext.h"

#define X(type, name) extern type name;
#include "GLFunctionsList.h"
#undef X
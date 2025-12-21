#pragma once
#include <Windows.h>
#include "GLLoader.h"

class OpenGLContext {
public:
	OpenGLContext() : renderingContext(nullptr) {}
	~OpenGLContext();

	bool Init(HDC hdc);
private:
	HGLRC renderingContext;
};
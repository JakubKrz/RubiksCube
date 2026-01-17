#pragma once
#include <Windows.h>
#include <string>

struct MouseInput {
	float moveX;
	float moveY;
	float scroll;
	bool isLeftBtnDown;
};

class Window {
public:
	Window(int width, int height, const std::wstring& title);
	~Window();
	void ProcessMessages();
	bool IsOpen() const { return isOpen; }
	void Swap();
	MouseInput GetAndResetMouseInput();

	HWND GetHWND() const { return windowHandle; }
	HDC  GetHDC()  const { return deviceContext; }

private:
	HWND windowHandle;
	HDC deviceContext;
	HINSTANCE instanceHandle;
	HGLRC renderingContext;
	std::wstring title;
	
	bool isOpen;
	int width;
	int height;
	float aspectRation;

	bool isDragging = false;
	int lastMouseX = 0;
	int lastMouseY = 0;

	float deltaX = 0.0f;
	float deltaY = 0.0f;
	float deltaScroll = 0.0f;

	static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
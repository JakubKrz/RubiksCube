#pragma once
#include <Windows.h>
#include <string>

class Window {
public:
	Window(int width, int height, const std::wstring& title);
	~Window();
	void ProcessMessages();
	bool IsOpen() const { return isOpen; }
	void Swap();

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

	static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
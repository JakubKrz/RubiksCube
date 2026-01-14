#include "Window.h"

Window::Window(int width, int height, const std::wstring& title) 
    : title(title), isOpen(true), width(width), height(height), 
      aspectRation(static_cast<float>(width)/static_cast<float>(height)), renderingContext(nullptr)
{
	instanceHandle = GetModuleHandle(NULL);
	WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
	wc.lpfnWndProc = StaticWindowProc;
	wc.hInstance = instanceHandle;
	wc.lpszClassName = L"RubikCubeWindow";
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.style = CS_OWNDC;

	RegisterClassEx(&wc);

	windowHandle = CreateWindowEx(
		0, wc.lpszClassName, title.c_str(),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height,
		NULL, NULL, instanceHandle, this
	);
	deviceContext = GetDC(windowHandle);

	PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1 };
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
    pfd.cAlphaBits = 8;

	int format = ChoosePixelFormat(deviceContext, &pfd);
	SetPixelFormat(deviceContext, format, &pfd);
}

LRESULT CALLBACK Window::StaticWindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    Window* pWindow = nullptr;
    if (msg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lp;
        pWindow = (Window*)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pWindow);
    }
    else {
        pWindow = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pWindow) return pWindow->WindowProc(hwnd, msg, wp, lp);
    return DefWindowProc(hwnd, msg, wp, lp);
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_SIZE:
        width = LOWORD(lp);
        height = HIWORD(lp);
        aspectRation = (float)width / height;
        break;
    case WM_CLOSE:
        isOpen = false;
        break;
    default:
        return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

void Window::ProcessMessages() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Window::Swap() {
    SwapBuffers(deviceContext);
}

Window::~Window() {
    ReleaseDC(windowHandle, deviceContext);
    DestroyWindow(windowHandle);
    UnregisterClass(L"RubikCubeWindow", instanceHandle);
}
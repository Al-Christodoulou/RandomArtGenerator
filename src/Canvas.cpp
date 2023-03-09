#include "Canvas.h"

bool Canvas::initialize(HINSTANCE hInstance, int nShowCmd)
{
	if (!registerWindowClass(hInstance))
	{
		MessageBox(NULL, L"Failed to initialize window class!", L"Error", MB_OK);
		return false;
	}
	if (!createWindow(hInstance))
	{
		MessageBox(NULL, L"Failed to initialize the window!", L"Error", MB_OK);
		return false;
	}
	m_nShowCmd = nShowCmd;
	m_successfully_initialized = true;
	return true;
}

void Canvas::beginDrawing()
{
	runMessageLoop(m_nShowCmd);
}

void Canvas::paintWindow()
{

}

void Canvas::click()
{

}

bool Canvas::registerWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEX wc;

	// registering the window class
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = Canvas::windowProcedure;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = cWndClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
		return false;
	return true;
}

bool Canvas::createWindow(HINSTANCE hInstance)
{
	// the creation of the window happens here. the last parameter is custom parameter
	// data we pass to the window, in this case a pointer to "this" canvas object. this
	// is needed because Canvas::windowProcedure() must be static (because lpfnWndProc
	// of WNDCLASSEX is a standard function pointer, look at registerWindowClass) and
	// it must also have access to a canvas object (to avoid making everything else static)
	m_windowHandle = CreateWindowEx(WS_EX_CLIENTEDGE, cWndClassName,
		L"Random Art Generator", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1024,
		576, NULL, NULL, hInstance, reinterpret_cast<LPVOID>(this));

	if (m_windowHandle == nullptr)
		return false;
	return true;
}

UINT_PTR Canvas::runMessageLoop(int nShowCmd)
{
	ShowWindow(m_windowHandle, nShowCmd);
	UpdateWindow(m_windowHandle);

	MSG msg;
	while (GetMessage(&msg, m_windowHandle, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		// calls windowProcedure behind the scenes
		DispatchMessage(&msg);
		// forces the window to be re-drawn, sending a WM_PAINT event
		RedrawWindow(m_windowHandle, NULL, NULL, RDW_INVALIDATE);
	}
	return msg.wParam;
}

LRESULT CALLBACK Canvas::windowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Canvas* canvas{ nullptr };

	// WM_NCCREATE is a message that comes just before WM_CREATE
	if (msg == WM_NCCREATE)
	{
		CREATESTRUCT* createstruct{ reinterpret_cast<CREATESTRUCT*>(lParam) };
		if (!createstruct->lpCreateParams)
			return FALSE;

		// the custom user data we've passed to CreateWindowEx is a pointer to the
		// canvas object
		canvas = static_cast<Canvas*>(createstruct->lpCreateParams);

		// required for proper error handling of SetWindowLongPtr
		SetLastError(0);
		// set the user data of the window to store a pointer to a canvas object
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(canvas));
		if (GetLastError() != 0)
			return FALSE;
		return TRUE;
	}
	else // the window's already been created, get the canvas pointer through the
		// user data
		canvas = reinterpret_cast<Canvas*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (msg)
	{
	case WM_PAINT:
		canvas->paintWindow();
		break;
	case WM_LBUTTONDOWN:
		canvas->click();
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
#include <cmath>
#include "Canvas.h"

//#define U8(n) static_cast<uint8_t>(n)
template <typename T>
constexpr uint8_t toU8(T n) { return static_cast<uint8_t>(n); }

constexpr unsigned int CBUFFER_WIDTH{ 512 };
constexpr unsigned int CBUFFER_HEIGHT{ 512 };

constexpr float CDIVISOR_WIDTH{ static_cast<float>(CBUFFER_WIDTH) / 255 };
constexpr float CDIVISOR_HEIGHT{ static_cast<float>(CBUFFER_HEIGHT) / 255 };

Canvas::Canvas()
	: m_renderer{ nullptr, CBUFFER_WIDTH, CBUFFER_HEIGHT }
{}

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
	if (!initRenderer())
		return false;
	return true;
}

void Canvas::beginDrawing()
{
	runMessageLoop(m_nShowCmd);
}

void Canvas::paintWindow()
{
	Pixel redPixel{ 255, 0, 0 };
	Pixel greenPixel{ 0, 255, 0 };
	for (unsigned int i{ 0 }; i < m_renderer.m_bufferdims.width; i++)
	{
		for (unsigned int j{ 0 }; j < m_renderer.m_bufferdims.height; j++)
		{
			//m_renderer.setPixel({ U8(i), U8(j), U8(U8(i) - U8(j)) }, j, i);
			//m_renderer.setPixel({ U8(i), U8(j), getSum(U8(i), U8(j)) }, j, i);
			m_renderer.setPixel({ toU8(i * sqrt(j)), toU8(j / CDIVISOR_HEIGHT), toU8(sqrt(i) * j) }, j, i);

			//m_renderer.setPixel(greenPixel, j + 50, i);
			//m_renderer.setPixel(redPixel, j + 100, i);
			//m_renderer.setPixel(greenPixel, j + 150, i);
			//m_renderer.setPixel({ U8(i), U8(j), U8(U8(i) + U8(j) / 2) }, i, j);
		}
	}
	m_renderer.render();
}

bool Canvas::initRenderer()
{
	if (!m_successfully_initialized)
		return false;
	m_renderer.attachWindowHandle(m_windowHandle);
	return true;
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
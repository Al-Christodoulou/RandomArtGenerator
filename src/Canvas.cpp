#include <cmath>
#include <string>
#include "Random.h"
#include "Canvas.h"

//#define U8(n) static_cast<uint8_t>(n)
template <typename T>
constexpr uint8_t toU8(T n) { return static_cast<uint8_t>(n); }

constexpr unsigned int CBUFFER_WIDTH{ 960 };
constexpr unsigned int CBUFFER_HEIGHT{ 540 };

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
	initRenderer();
	return true;
}

void Canvas::beginDrawing()
{
	reSeedChoices();
	runMessageLoop(m_nShowCmd);
}

void Canvas::generateImage()
{
	for (unsigned int i{ 0 }; i < m_renderer.m_bufferdims.width; i++)
	{
		for (unsigned int j{ 0 }; j < m_renderer.m_bufferdims.height; j++)
		{
			Pixel pixel{
				toU8(getRandExpression(i, j, ColorIndex::Red)),
				toU8(getRandExpression(i, j, ColorIndex::Green)),
				toU8(getRandExpression(i, j, ColorIndex::Blue))
			};
			m_renderer.setPixel(pixel, j, i);
		}
	}
	m_regenerateImage = false;
}

void Canvas::paintWindow()
{
	if (m_regenerateImage)
		generateImage();
	m_renderer.render();
}

// receives i,j and returns a random expression after the two
// parameters are passed through a random filter. the ends
// results can look like:
// i * sqrt(j), sqrt(i) * log(j), i * j * j, etc.
unsigned int Canvas::getRandExpression(unsigned int i, unsigned int j, ColorIndex colorIndex)
{
	unsigned int filteredI{ getRandFilter(i, colorIndex, 1) };
	unsigned int filteredJ{ getRandFilter(j, colorIndex, 2) };
	switch (m_choices[static_cast<unsigned int>(colorIndex)][0])
	{
	case 0:
		return filteredI + filteredJ;
	case 1:
		return filteredI - filteredJ;
	case 2:
		return filteredI * filteredJ;
	case 3:
		if (filteredJ == 0)
			return filteredI;
		else
			return filteredI / filteredJ;
	default: // shouldn't happen
		return 0;
	}
}

// receives an int and returns a random filter passed through it,
// such as the square root of input, the logarithm, the input
// squared and so on
unsigned int Canvas::getRandFilter(unsigned int input, ColorIndex colorIndex, unsigned int secondIndex)
{
	switch (m_choices[static_cast<unsigned int>(colorIndex)][secondIndex])
	{
	case 0:
		return input;
	case 1:
		return static_cast<unsigned int>(sqrt(input));
	case 2:
		return static_cast<unsigned int>(log(input));
	case 3:
		return static_cast<unsigned int>(log10(input));
	case 4:
		return input * input;
	default: // shouldn't happen
		return 0;
	}
}

void Canvas::reSeedChoices()
{
	for (int i{ 0 }; i < 3; i++)
	{
		for (int j{ 0 }; j < 3; j++)
		{
			// if j == 0, then this is the first random choice in
			// getRandExpression(). the other two are getRandFilter's choices
			if (j == 0)
				m_choices[i][j] = static_cast<unsigned int>(Random::get(0, 3));
			else
				m_choices[i][j] = static_cast<unsigned int>(Random::get(0, 4));
		}
	}
	InvalidateRgn(m_windowHandle, NULL, TRUE);
	m_regenerateImage = true;
}

void Canvas::initRenderer()
{
	m_renderer.attachWindowHandle(m_windowHandle);
}

void Canvas::leftClick()
{
	reSeedChoices();
}

void Canvas::rightClick()
{
	m_renderer.invertAllPixels();
	InvalidateRgn(m_windowHandle, NULL, TRUE);
}

void Canvas::constructFormulaString(std::wstring& str, ColorIndex colorIndex)
{
	switch (colorIndex)
	{
	case ColorIndex::Red:
		str.append(L"Red pixel: ");
		break;
	case ColorIndex::Green:
		str.append(L"Green pixel: ");
		break;
	case ColorIndex::Blue:
		str.append(L"Blue pixel: ");
		break;
	default:
		str.append(L"ERROR: Garbage pixel index!");
		return;
	}

	// in m_choices[I][x], I is the pixel index:
	// 0 == red, 1 == green, 2 == blue
	switch (m_choices[static_cast<unsigned int>(colorIndex)][1])
	{
	case 0:
		str.append(L"X ");
		break;
	case 1:
		str.append(L"Sqrt(X) ");
		break;
	case 2:
		str.append(L"Log(X) ");
		break;
	case 3:
		str.append(L"Log10(X) ");
		break;
	case 4:
		str.append(L"X * X ");
		break;
	}

	switch (m_choices[static_cast<unsigned int>(colorIndex)][0])
	{
	case 0:
		str.append(L"+ ");
		break;
	case 1:
		str.append(L"- ");
		break;
	case 2:
		str.append(L"* ");
		break;
	case 3:
		str.append(L"/ ");
		break;
	}

	switch (m_choices[static_cast<unsigned int>(colorIndex)][2])
	{
	case 0:
		str.append(L"Y");
		break;
	case 1:
		str.append(L"Sqrt(Y)");
		break;
	case 2:
		str.append(L"Log(Y)");
		break;
	case 3:
		str.append(L"Log10(Y)");
		break;
	case 4:
		str.append(L"Y * Y");
		break;
	}
	str.append(L"\n");
}

void Canvas::FKeyPress()
{
	std::wstring message{};
	message.reserve(60);

	constructFormulaString(message, ColorIndex::Red);
	constructFormulaString(message, ColorIndex::Green);
	constructFormulaString(message, ColorIndex::Blue);

	MessageBox(m_windowHandle, message.c_str(), L"Formula", NULL);
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
		//RedrawWindow(m_windowHandle, NULL, NULL, RDW_INVALIDATE);
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
	case WM_SIZE:
	case WM_SIZING:
		InvalidateRgn(canvas->m_windowHandle, NULL, TRUE);
		break;
	case WM_LBUTTONDOWN:
		canvas->leftClick();
		break;
	case WM_RBUTTONDOWN:
		canvas->rightClick();
		break;
	case WM_KEYDOWN:
		if (wParam == 0x46 /* F */)
			canvas->FKeyPress();
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
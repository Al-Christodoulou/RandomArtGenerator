#pragma once
#include <Windows.h>
#include "Renderer.h"

class Canvas
{
private:
	bool m_successfully_initialized{ false };
	HWND m_windowHandle{ nullptr };
	static constexpr inline LPCWSTR cWndClassName{ L"CanvasWindow" };
	int m_nShowCmd{};
	Renderer m_renderer;
public:
	Canvas();
	bool initialize(HINSTANCE hInstance, int nShowCmd);
	void beginDrawing();

private:
	bool initRenderer();
	void paintWindow();
	void click();

	// WinAPI wrapper functions
	bool registerWindowClass(HINSTANCE hInstance);
	bool createWindow(HINSTANCE hInstance);
	UINT_PTR runMessageLoop(int nShowCmd);

	static LRESULT CALLBACK windowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
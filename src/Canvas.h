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

	// used by getRandExpression & getRandFilter
	unsigned int m_choices[3][3]{};
	bool m_regenerateImage{ false };
public:
	Canvas();
	bool initialize(HINSTANCE hInstance, int nShowCmd);
	void beginDrawing();

private:
	unsigned int getRandExpression(unsigned int i, unsigned int j, unsigned int choiceIndex);
	unsigned int getRandFilter(unsigned int input, unsigned int choiceIndex, unsigned int secondIndex);
	void reSeedChoices();

	bool initRenderer();
	void generateImage();
	void paintWindow();
	void leftClick();
	void rightClick();

	// WinAPI wrapper functions
	bool registerWindowClass(HINSTANCE hInstance);
	bool createWindow(HINSTANCE hInstance);
	UINT_PTR runMessageLoop(int nShowCmd);

	static LRESULT CALLBACK windowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
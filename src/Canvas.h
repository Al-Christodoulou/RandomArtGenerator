#pragma once
#include <Windows.h>
#include <string>
#include "Renderer.h"

class Canvas
{
private:
	bool m_successfully_initialized{ false };
	HWND m_windowHandle{ nullptr };
	static constexpr inline LPCWSTR cWndClassName{ L"CanvasWindow" };
	int m_nShowCmd{};
	Renderer m_renderer;

	// these below are used by getRandExpression & getRandFilter
	enum class ColorIndex
	{
		Red = 0,
		Green,
		Blue,
		ColorIndexMaxCount
	};

	/*
	* m_choices is a random lookup table, updated by reSeedChoices, and it's used
	* to randomly generate the image. it's a 3x3 array, containing the following
	* content:
	*   +------------- 0 -----------------+---------------- 1 ------------------+---------------- 2 ------------------+
	* 0 | <Red pixel random expression>   | <Red pixel random É filter value>   | <Red pixel random J filter value>   |
	* 1 | <Green pixel random expression> | <Green pixel random É filter value> | <Green pixel random J filter value> |
	* 2 | <Blue pixel random expression>  | <Blue pixel random É filter value>  | <Blue pixel random J filter value>  |
	*   +-----------------------------------------------------------------------+-------------------------------------+
	*/
	unsigned int m_choices[(unsigned int)ColorIndex::ColorIndexMaxCount][3]{};
	bool m_regenerateImage{ false };
public:
	Canvas();
	bool initialize(HINSTANCE hInstance, int nShowCmd);
	void beginDrawing();

private:
	unsigned int getRandExpression(unsigned int i, unsigned int j, ColorIndex colorIndex);
	unsigned int getRandFilter(unsigned int input, ColorIndex colorIndex, unsigned int secondIndex);
	void reSeedChoices();

	bool initRenderer();
	void generateImage();
	void paintWindow();
	void leftClick();
	void rightClick();
	void constructFormulaString(std::wstring&, ColorIndex colorIndex);
	void FKeyPress();

	// WinAPI wrapper functions
	bool registerWindowClass(HINSTANCE hInstance);
	bool createWindow(HINSTANCE hInstance);
	UINT_PTR runMessageLoop(int nShowCmd);

	static LRESULT CALLBACK windowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
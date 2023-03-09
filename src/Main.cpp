#include <Windows.h>
#include "Canvas.h"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	[[maybe_unused]] _In_opt_ HINSTANCE hPrevInstance,
	[[maybe_unused]] _In_ LPSTR lpCmdLine,
	[[maybe_unused]] _In_ int nShowCmd)
{
	Canvas canvas{};
	if (canvas.initialize(hInstance, nShowCmd))
		canvas.beginDrawing();
}
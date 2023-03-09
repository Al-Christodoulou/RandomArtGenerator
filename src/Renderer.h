#pragma once
#include <inttypes.h>
#include <Windows.h>

struct Pixel
{
	uint8_t red{}, green{}, blue{};
};

class Renderer
{
private:
	HBITMAP m_hBitmap{ nullptr };
	RGBQUAD* m_bitmapData{ nullptr };
	HWND m_attachedWndHandle{ nullptr };

	void initBitmap();
public:
	// the renderer needs a window handle to "attach" to (otherwise the rendering
	// & bitmap creation can't happen)
	Renderer(HWND hWnd, unsigned int width, unsigned int height);
	void setPixel(const Pixel& pixel, unsigned int row, unsigned int column);
	void render();

	struct BufferDimensions
	{
		unsigned int width{}, height{};
	} const m_bufferdims;
};
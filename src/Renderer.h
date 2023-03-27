#pragma once
#include <stdint.h>
#include <Windows.h>

struct Pixel
{
	uint8_t red{}, green{}, blue{};
};

class Renderer
{
private:
	HBITMAP m_hBitmap{ nullptr };
	BITMAPINFO m_bitmapInfo{};
	RGBQUAD* m_bitmapData{ nullptr };
	HWND m_attachedWndHandle{ nullptr };

	void initBitmap();
public:
	// the renderer needs a window handle to "attach" to (otherwise the rendering
	// & bitmap creation can't happen)
	Renderer(HWND hWnd, unsigned int buffer_width, unsigned int buffer_height);
	Renderer(unsigned int buffer_width, unsigned int buffer_height);
	~Renderer();
	void setPixel(const Pixel& pixel, unsigned int row, unsigned int column);
	Pixel getPixel(unsigned int row, unsigned int column);
	void invertAllPixels();
	void render();
	void attachWindowHandle(HWND hWnd);

	void operator=(const Renderer&) = delete;

	struct BufferDimensions
	{
		unsigned int width{}, height{};
	} const m_bufferdims;
};
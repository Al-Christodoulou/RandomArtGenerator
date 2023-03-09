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

	void initBitmap();
public:
	Renderer(unsigned int width, unsigned int height);
	void setPixel(const Pixel& pixel, unsigned int row, unsigned int column);
	void render(HWND hWnd);

	struct BufferDimensions
	{
		unsigned int width{}, height{};
	} const m_bufferdims;
};
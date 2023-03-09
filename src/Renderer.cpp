#include "Renderer.h"

void Renderer::initBitmap()
{
	BITMAPINFOHEADER bmpInfoHdr{};
	bmpInfoHdr.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfoHdr.biWidth = m_bufferdims.width;
	// when the height is negative, the pixels start from the top
	// left instead of the bottom left of the window
	bmpInfoHdr.biHeight = -static_cast<LONG>(m_bufferdims.height);
	bmpInfoHdr.biPlanes = 1; // must be set to 1
	bmpInfoHdr.biBitCount = sizeof(RGBQUAD) << 3; // average number of bits per pixel
	bmpInfoHdr.biCompression = BI_RGB;
	bmpInfoHdr.biSizeImage = 0; // 0 for uncompressed RGB bitmaps
	// the next two don't matter for CreateDIBSection
	bmpInfoHdr.biXPelsPerMeter = 0;
	bmpInfoHdr.biYPelsPerMeter = 0;
	bmpInfoHdr.biClrUsed = FALSE;
	bmpInfoHdr.biClrImportant = 0;

	// BITMAPINFO contains a BITMAPINFOHEADER and a RGBQUAD, but the latter isn't
	// really needed, so we just do a pointer cast to bypass it
	BITMAPINFO bmpInfo{ *reinterpret_cast<LPBITMAPINFO>(&bmpInfoHdr) };

	/*m_hBitmap = CreateDIBSection(
		hdc
	)*/
}

Renderer::Renderer(HWND hWnd, unsigned int buffer_width, unsigned int buffer_height)
	: m_bufferdims{ buffer_width, buffer_height }, m_attachedWndHandle{ hWnd }
{
	initBitmap();
}

void Renderer::setPixel(const Pixel& pixel, unsigned int row, unsigned int column)
{
	m_bitmapData[column + row * m_bufferdims.width].rgbRed = pixel.red;
	m_bitmapData[column + row * m_bufferdims.width].rgbGreen = pixel.green;
	m_bitmapData[column + row * m_bufferdims.width].rgbBlue = pixel.blue;
}

void Renderer::render()
{
	PAINTSTRUCT ps;
	HDC hdc{ BeginPaint(m_attachedWndHandle, &ps) };
	EndPaint(m_attachedWndHandle, &ps);
}
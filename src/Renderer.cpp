#include "Renderer.h"


Renderer::Renderer(HWND hWnd, unsigned int buffer_width, unsigned int buffer_height)
	: m_bufferdims{ buffer_width, buffer_height }, m_attachedWndHandle{ hWnd }
{
	initBitmap();
}

Renderer::Renderer(Renderer&& renderer) noexcept
	: m_bufferdims{ renderer.m_bufferdims }
{
	m_hBitmap = renderer.m_hBitmap;
	renderer.m_hBitmap = nullptr;
	m_bitmapInfo = renderer.m_bitmapInfo;
	m_bitmapData = renderer.m_bitmapData;
	renderer.m_bitmapData = nullptr;
	m_attachedWndHandle = renderer.m_attachedWndHandle;
	renderer.m_attachedWndHandle = nullptr;
}

Renderer::~Renderer()
{
	DeleteObject(m_hBitmap);
}

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
	m_bitmapInfo = *reinterpret_cast<LPBITMAPINFO>(&bmpInfoHdr);

	HDC hdc{ GetDC(nullptr) };

	m_hBitmap = CreateDIBSection(
		hdc, &m_bitmapInfo, DIB_RGB_COLORS, reinterpret_cast<void**>(&m_bitmapData), nullptr, 0
	);

	ReleaseDC(nullptr, hdc);
}

void Renderer::setPixel(const Pixel& pixel, unsigned int row, unsigned int column)
{
	if (row <= m_bufferdims.height - 1 && column <= m_bufferdims.width - 1)
	{
		m_bitmapData[column + row * m_bufferdims.width].rgbRed = pixel.red;
		m_bitmapData[column + row * m_bufferdims.width].rgbGreen = pixel.green;
		m_bitmapData[column + row * m_bufferdims.width].rgbBlue = pixel.blue;
	}
}

Pixel Renderer::getPixel(unsigned int row, unsigned int column)
{
	return Pixel{
		m_bitmapData[column + row * m_bufferdims.width].rgbRed,
		m_bitmapData[column + row * m_bufferdims.width].rgbGreen,
		m_bitmapData[column + row * m_bufferdims.width].rgbBlue,
	};
}

void Renderer::render()
{
	PAINTSTRUCT ps;
	HDC hdc{ BeginPaint(m_attachedWndHandle, &ps) };

	if (hdc)
	{
		RECT winRect{};
		GetClientRect(m_attachedWndHandle, &winRect);
		StretchDIBits(hdc, 0, 0, winRect.right, winRect.bottom, 0, 0,
			m_bufferdims.width, m_bufferdims.height,
			m_bitmapData, &m_bitmapInfo, DIB_RGB_COLORS, SRCCOPY
		);
	}

	EndPaint(m_attachedWndHandle, &ps);
}

// only allowed if the renderer doesn't already have a window attached to it
void Renderer::attachWindowHandle(HWND hWnd)
{
	if (!m_attachedWndHandle)
		m_attachedWndHandle = hWnd;
}
#include "DisplayWin32.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_KEYDOWN:
	{
		if (static_cast<unsigned int>(wparam) == 27) PostQuitMessage(0);
		return 0;
	}
	default:
	{
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
	}
}

DisplayWin32::DisplayWin32()
{
	InitWNDCLASSEX();
	RECT windowRect = { 0, 0, static_cast<LONG>(pWidth), static_cast<LONG>(pHeight) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;

	auto posX = (GetSystemMetrics(SM_CXSCREEN) - pWidth) / 2;
	auto posY = (GetSystemMetrics(SM_CYSCREEN) - pHeight) / 2;

	phWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName,
		dwStyle,
		posX, posY,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInstance, nullptr);
	ShowWindow(phWnd, SW_SHOW);
	SetForegroundWindow(phWnd);
	SetFocus(phWnd);
	ShowCursor(true);
}

DisplayWin32::DisplayWin32(int Width, int Height) : pHeight(Height), pWidth(Width)
{
	InitWNDCLASSEX();
	RECT windowRect = { 0, 0, static_cast<LONG>(pWidth), static_cast<LONG>(pHeight) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;

	auto posX = (GetSystemMetrics(SM_CXSCREEN) - pWidth) / 2;
	auto posY = (GetSystemMetrics(SM_CYSCREEN) - pHeight) / 2;

	phWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName,
		dwStyle,
		posX, posY,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInstance, nullptr);
	ShowWindow(phWnd, SW_SHOW);
	SetForegroundWindow(phWnd);
	SetFocus(phWnd);
	ShowCursor(true);
}

int DisplayWin32::GetHeight()
{
	return pHeight;
}

int DisplayWin32::GetWidth()
{
	return pWidth;
}

void DisplayWin32::Release()
{

}

void DisplayWin32::InitWNDCLASSEX()
{
	pWC.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	pWC.lpfnWndProc = WndProc;
	pWC.cbClsExtra = 0;
	pWC.cbWndExtra = 0;
	pWC.hInstance = hInstance;
	pWC.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	pWC.hIconSm = pWC.hIcon;
	pWC.hCursor = LoadCursor(nullptr, IDC_ARROW);
	pWC.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	pWC.lpszMenuName = nullptr;
	pWC.lpszClassName = applicationName;
	pWC.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&pWC);
}


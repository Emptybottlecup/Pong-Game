#pragma once
#include <windows.h>
#include <WinUser.h>

class DisplayWin32
{

public:

	DisplayWin32();

	DisplayWin32(int Width, int Height);

	int GetHeight();

	int GetWidth();

	HWND& GetWindow()
	{
		return phWnd;
	}

	void Release();

private:
	void InitWNDCLASSEX();

private:
	int pHeight = 800;
	int pWidth = 800;
	WNDCLASSEX pWC;
	HWND phWnd;
	LPCWSTR applicationName = L"DirectX";
	HINSTANCE hInstance = GetModuleHandle(nullptr);
};

#include <windows.h>
#include "resource.h"
#include "MainWindow.h"
#include "Ellipses.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	MainWindow win;

	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCEL1));

	win.Register();

	if (!win.Create(L"Ellipses Window", WS_OVERLAPPEDWINDOW))
	{
		return 0;
	}

	ShowWindow(win.Window(), nCmdShow);

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(win.Window(), hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}

#include "Direct2DWindow.h"
#include "resource.h"
#pragma comment(lib, "d2d1")

BOOL CALLBACK ChooseClrDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

float DPIScale::scaleX = 1.0f;
float DPIScale::scaleY = 1.0f;

// Current color
int iCurrentColor;

Direct2DWindow::Direct2DWindow() : pFactory(NULL), pRenderTarget(NULL)
{
}


Direct2DWindow::~Direct2DWindow()
{
}

HRESULT Direct2DWindow::CreateGraphicsResources()
{
	HRESULT hr = S_OK;
	if (pRenderTarget == NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&pRenderTarget);
		if (SUCCEEDED(hr))
		{
			hr = CreateAdditionalGraphicsResources();
		}
	}
	return hr;
}

HRESULT Direct2DWindow::CreateAdditionalGraphicsResources()
{
	return S_OK;
}

void Direct2DWindow::DiscardGraphicsResources()
{
	SafeRelease(&pRenderTarget);
}

void Direct2DWindow::OnPaint()
{
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);

		pRenderTarget->BeginDraw();

		// LightBlue - background color
		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::LightBlue));
		OnPaintScene();
		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}
		EndPaint(m_hwnd, &ps);
	}
}

void Direct2DWindow::Resize()
{
	if (pRenderTarget != NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		pRenderTarget->Resize(size);
		CalculateLayout();
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
}

LRESULT Direct2DWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		if (FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
		{
			return -1;  // Failure a CreateWindowEx
		}
		DPIScale::Initialize(pFactory);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_CHOOSECOLOR:
			if (DialogBox(NULL, MAKEINTRESOURCE(IDD_DIALOG1), m_hwnd, ChooseClrDlgProc))
				InvalidateRect(m_hwnd, NULL, TRUE);
			shape_color = iCurrentColor;
			return 0;
		}
		break;

	case WM_DESTROY:
		DiscardGraphicsResources();
		SafeRelease(&pFactory);
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
		OnPaint();
		return 0;


	case WM_SIZE:
		Resize();
		return 0;
	}
	LRESULT Result = 0;
	if (HandleAdditionalMessage(uMsg, wParam, lParam, &Result)) return Result;
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

bool Direct2DWindow::HandleAdditionalMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *pResult)
{
	return false;
}

BOOL CALLBACK ChooseClrDlgProc(HWND hDlg, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	static HWND hCtrlBlock;
	static int  iColor, iFigure;

	switch (message)
	{
	case WM_INITDIALOG:
		iColor = iCurrentColor;
		switch (iCurrentColor)
		{
		case 0:
			CheckRadioButton(hDlg, IDC_BLUE, IDC_VIOLET, IDC_RED);
			return TRUE;
		case 1:
			CheckRadioButton(hDlg, IDC_BLUE, IDC_VIOLET, IDC_GREEN);
			return TRUE;
		case 2:
			CheckRadioButton(hDlg, IDC_BLUE, IDC_VIOLET, IDC_YELLOW);
			return TRUE;
		case 3:
			CheckRadioButton(hDlg, IDC_BLUE, IDC_VIOLET, IDC_BLUE);
			return TRUE;
		case 4:
			CheckRadioButton(hDlg, IDC_BLUE, IDC_VIOLET, IDC_ORANGE);
			return TRUE;
		case 5:
			CheckRadioButton(hDlg, IDC_BLUE, IDC_VIOLET, IDC_CYAN);
			return TRUE;
		case 6:
			CheckRadioButton(hDlg, IDC_BLUE, IDC_VIOLET, IDC_VIOLET);
			return TRUE;
		}
		return FALSE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			iCurrentColor = iColor;
			EndDialog(hDlg, TRUE);
			return TRUE;

		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			return TRUE;

		case IDC_RED:
			iColor = 0;
			CheckRadioButton(hDlg, IDC_BLUE, IDC_VIOLET, IDC_RED);
			return TRUE;
		case IDC_GREEN:
			iColor = 1;
			CheckRadioButton(hDlg, IDC_BLUE, IDC_VIOLET, IDC_GREEN);
			return TRUE;
		case IDC_YELLOW:
			iColor = 2;
			CheckRadioButton(hDlg, IDC_BLUE, IDC_VIOLET, IDC_YELLOW);
			return TRUE;
		case IDC_BLUE:
			iColor = 3;
			CheckRadioButton(hDlg, IDC_BLUE, IDC_VIOLET, IDC_BLUE);
			return TRUE;
		case IDC_ORANGE:
			iColor = 4;
			CheckRadioButton(hDlg, IDC_BLUE, IDC_VIOLET, IDC_ORANGE);
			return TRUE;
		case IDC_CYAN:
			iColor = 5;
			CheckRadioButton(hDlg, IDC_BLUE, IDC_VIOLET, IDC_CYAN);
			return TRUE;
		case IDC_VIOLET:
			iColor = 6;
			CheckRadioButton(hDlg, IDC_RED, IDC_VIOLET, IDC_VIOLET);
			return TRUE;
		}
		break;

	case WM_PAINT:
		break;
	}
	return FALSE;
}

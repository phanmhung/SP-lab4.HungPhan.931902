#include <windows.h>
#include <windowsX.h>
#include "resource.h"
#include "MainWindow.h"

D2D1::ColorF MainWindow::colors[7] = { D2D1::ColorF::Red,D2D1::ColorF::Green,D2D1::ColorF::Yellow,
D2D1::ColorF::Blue,D2D1::ColorF::Orange,D2D1::ColorF::Cyan,D2D1::ColorF::Violet };

void MainWindow::InsertEllipse(float x, float y, int cur_color)
{
	shared_ptr<MyEllipse> pme(new MyEllipse);
	D2D1_POINT_2F pt = { x,y };
	pme->ellipse.point = ptMouse = pt;
	pme->ellipse.radiusX = pme->ellipse.radiusY = 1.0f;
	pme->color = colors[shape_color];
	ellipses.push_front(pme);
	selection = ellipses.begin();
}

void MainWindow::DeleteEllipse()
{
	ellipses.erase(selection);
	ClearSelection();
}

BOOL MainWindow::HitTest(float x, float y)
{
	list<shared_ptr<MyEllipse>>::iterator iter;
	for (iter = ellipses.begin(); iter != ellipses.end(); iter++)
		if ((*iter)->HitTest(x, y)) { selection = iter; return TRUE; }
	return 0;
}

// Recalculation when changing the window size
HRESULT MainWindow::CreateAdditionalGraphicsResources()
{
	const D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 0);
	HRESULT hr = pRenderTarget->CreateSolidColorBrush(color, &pBrush);
	if (SUCCEEDED(hr))
	{
		CalculateLayout();
	}
	return hr;
}

void MainWindow::DiscardGraphicsResources()
{
	Direct2DWindow::DiscardGraphicsResources();
	SafeRelease(&pBrush);
}

void MainWindow::OnPaintScene()
{
	list<shared_ptr<MyEllipse>>::reverse_iterator riter;
	for (riter = ellipses.rbegin(); riter != ellipses.rend(); riter++)
		(*riter)->Draw(pRenderTarget, pBrush, (Selection() && (*riter == *selection)));
}

void MainWindow::OnLButtonDown(int pixelX, int pixelY, DWORD flags)
{
	const float dipX = DPIScale::PixelsToDipsX(pixelX);
	const float dipY = DPIScale::PixelsToDipsY(pixelY);

	if (mode == DrawMode)
	{
		POINT pt = { pixelX, pixelY };

		if (DragDetect(m_hwnd, pt))
		{
			SetCapture(m_hwnd);

			// Start a new ellipse
			InsertEllipse(dipX, dipY, shape_color);
		}
	}
	else
	{
		ClearSelection();

		if (HitTest(dipX, dipY))
		{
			SetCapture(m_hwnd);

			ptMouse = Selection()->ellipse.point;
			ptMouse.x -= dipX;
			ptMouse.y -= dipY;

			SetMode(DragMode);
		}
	}
	InvalidateRect(m_hwnd, NULL, FALSE);
}

void MainWindow::OnMouseMove(int pixelX, int pixelY, DWORD flags)
{
	const float dipX = DPIScale::PixelsToDipsX(pixelX);
	const float dipY = DPIScale::PixelsToDipsY(pixelY);

	if ((flags & MK_LBUTTON) && Selection())
	{
		if (mode == DrawMode)
		{
			// Resize the ellipse
			const float width = (dipX - ptMouse.x) / 2;
			const float height = (dipY - ptMouse.y) / 2;
			const float x1 = ptMouse.x + width;
			const float y1 = ptMouse.y + height;

			Selection()->ellipse = D2D1::Ellipse(D2D1::Point2F(x1, y1), width, height);
		}
		else if (mode == DragMode)
		{
			// Move the ellipse
			Selection()->ellipse.point.x = dipX + ptMouse.x;
			Selection()->ellipse.point.y = dipY + ptMouse.y;
		}
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
}

void MainWindow::OnLButtonUp()
{
	if ((mode == DrawMode) && Selection())
	{
		ClearSelection();
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
	else if (mode == DragMode)
	{
		SetMode(SelectMode);
	}
	ReleaseCapture();
}

bool MainWindow::HandleAdditionalMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *pResult)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_DRAW_MODE:
			SetMode(DrawMode);
			return true;

		case ID_SELECT_MODE:
			SetMode(SelectMode);
			return true;

		case ID_TOGGLE_MODE:
			if (mode == DrawMode)
			{
				SetMode(SelectMode);
			}
			else
			{
				SetMode(DrawMode);
			}
			return true;

		case ID_DELETE:
			if (Selection())
			{
				DeleteEllipse();
				InvalidateRect(m_hwnd, NULL, FALSE);
			}
			return true;
		}
		return false;

	case WM_LBUTTONDOWN:
		OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return true;

	case WM_LBUTTONUP:
		OnLButtonUp();
		return true;

	case WM_MOUSEMOVE:
		OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return true;

	case WM_SETCURSOR:
		if (LOWORD(lParam) == HTCLIENT)
		{
			SetCursor(hCursor);
			*pResult = TRUE;
			return true;
		}
		return false;
	}
	return false;
}
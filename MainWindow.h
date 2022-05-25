#pragma once
#include <list>
#include <memory>
#include "Direct2DWindow.h"
#include "MyEllipse.h"

using namespace std;

class MainWindow : public Direct2DWindow
{
	ID2D1SolidColorBrush    *pBrush;
	list<shared_ptr<MyEllipse>>             ellipses;
	enum Mode { DrawMode, SelectMode, DragMode } mode;
	D2D1_POINT_2F           ptMouse;
	HCURSOR hCursor;
	list<shared_ptr<MyEllipse>>::iterator   selection;
	int numColor;
	static D2D1::ColorF colors[7];


	shared_ptr<MyEllipse> Selection()
	{
		if (selection == ellipses.end())
		{
			return nullptr;
		}
		else
		{
			return (*selection);
		}
	}

	void    ClearSelection() { selection = ellipses.end(); }
	void	SetMode(Mode m)
	{
		mode = m;
		// Update the cursor
		LPWSTR cursor{ IDC_CROSS };
		switch (mode)
		{
		case DrawMode:
			cursor = IDC_CROSS;
			break;

		case SelectMode:
			cursor = IDC_HAND;
			break;

		case DragMode:
			cursor = IDC_SIZEALL;
			break;
		}

		hCursor = LoadCursor(NULL, cursor);
		SetCursor(hCursor);
	}
	void	InsertEllipse(float x, float y, int cur_color);
	void	DeleteEllipse();
	BOOL	HitTest(float x, float y);
	HRESULT CreateAdditionalGraphicsResources();
	void    DiscardGraphicsResources();
	void OnPaintScene();
	void    OnLButtonDown(int pixelX, int pixelY, DWORD flags);
	void    OnLButtonUp();
	void    OnMouseMove(int pixelX, int pixelY, DWORD flags);
public:

	MainWindow() : pBrush(NULL),
		ptMouse(D2D1::Point2F()),
		numColor(0)
	{
		ClearSelection();
		SetMode(DrawMode);
	}

	PCWSTR  ClassName() const { return L"Ellipses Class"; }
	bool HandleAdditionalMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *);
};


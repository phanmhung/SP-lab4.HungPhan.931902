#pragma once
#include <d2d1.h>
#include "BaseWindow.h"
#include "DPIScale.h"


template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

class Direct2DWindow :
	public BaseWindow
{
	ID2D1Factory            *pFactory;
protected:
	ID2D1HwndRenderTarget   *pRenderTarget;

public:
	Direct2DWindow();
	~Direct2DWindow();
	virtual void    CalculateLayout() {}
	HRESULT CreateGraphicsResources();
	virtual HRESULT CreateAdditionalGraphicsResources();
	virtual void    DiscardGraphicsResources();
	void    OnPaint();
	void    Resize();
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnPaintScene() {}
	virtual bool HandleAdditionalMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *);
};


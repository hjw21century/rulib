#include "StdAfx.h"
#include "ClockControl.h"
#include "WinTime.h"

BEGIN_MESSAGE_MAP(CClockControl, CStatic)
    ON_WM_PAINT()
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CClockControl, CStatic)

CClockControl::CClockControl(void)
{
}

CClockControl::~CClockControl(void)
{
}


void CClockControl::OnPaint()
{
	CStatic::OnPaint();

    CDC* dc = GetDC();
	CRect rect;
    GetClientRect(&rect);
   
    SYSTEMTIME st;
    GetSystemTime(&st);
    CWinTime::DrawClock(dc->GetSafeHdc(), &rect, &st, 1, RGB(0, 0, 255), RGB(255, 255, 255));
}

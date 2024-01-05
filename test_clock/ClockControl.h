#pragma once
#include "afxwin.h"

class CClockControl :
    public CStatic
{
DECLARE_DYNAMIC(CClockControl)

public:
    CClockControl(void);
public:
    ~CClockControl(void);

protected:
    afx_msg void OnPaint();

DECLARE_MESSAGE_MAP()
};

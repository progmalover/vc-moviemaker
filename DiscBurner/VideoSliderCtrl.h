#pragma once
#include "CustomSliderCtrl.h"

// CVideoSliderCtrl

class CVideoSliderCtrl : public CCustomSliderCtrl
{
	DECLARE_DYNAMIC(CVideoSliderCtrl)

public:
	CVideoSliderCtrl();
	virtual ~CVideoSliderCtrl();

protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual void GetChannelRect(LPRECT pRect) const;
	virtual void GetThumbRect(LPRECT pRect) const;
	virtual void GetSelectionMarkerRect(LPRECT pRect1, LPRECT pRect2) const;
	virtual void DrawChannel(CDC *pDC) const;
	virtual void DrawThumb(CDC *pDC) const;
	virtual void DrawSelectionMarker(CDC *pDC) const;
	virtual int PointToPos(POINT point) const;
	virtual CPoint PosToPoint(int nPos) const;
};



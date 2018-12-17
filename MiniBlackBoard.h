#pragma once
#include "DrawItem.h"
class MiniBlackBoard : public CDrawItem
{
public:
	using CDrawItem::CDrawItem;
	void SetBBStatus(EBBStatus status);
	EBBStatus GetBBStatus() { return m_status; }
	void SetCurBBIndex(int);
	void SetCurAnchorIndex(int);
	void FeedLocation(CRect p);
	void ReSet(CRect (*rects)[4]);
	void ShowErrorAndWarning(bool b) { m_bShowError = b; }
	CRect(* GetPoints())[4];
protected:
	virtual void OnPaint(CDC* dc)override;
private:
	EBBStatus m_status = e4;
	CRect m_anchors[4][4] = {};
	int m_curBBIndex = -1;
	int m_curAnchorIndex = -1;
	bool m_bShowError = false;
};


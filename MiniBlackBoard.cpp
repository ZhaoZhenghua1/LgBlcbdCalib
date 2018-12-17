#include "stdafx.h"
#include "MiniBlackBoard.h"
#include "resource.h"

enum EAnchors{eTL, eTR, eBL, eBR};
void MiniBlackBoard::SetBBStatus(EBBStatus status)
{
	if (status != m_status)
	{
		m_status = status;
		m_curBBIndex = -1;
		m_curAnchorIndex = -1;
		Update();
	}
}

void MiniBlackBoard::SetCurBBIndex(int i)
{
	if (i >= 0 && i < m_status && m_curBBIndex != i)
	{
		m_curBBIndex = i;
		Update();
	}
}

void MiniBlackBoard::SetCurAnchorIndex(int i)
{
	if (i >= 0 && i <= eBR && m_curAnchorIndex != i)
	{
		m_curAnchorIndex = i;
	}
}

void MiniBlackBoard::FeedLocation(CRect p)
{
	if (m_curAnchorIndex >= 0 && m_curAnchorIndex <= eBR && m_curBBIndex >= 0 && m_curBBIndex < e4)
	{
		m_anchors[m_curBBIndex][m_curAnchorIndex] = p;
		Update();
	}
}

void MiniBlackBoard::ReSet(CRect(*rects)[4])
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			m_anchors[i][j] = rects[i][j];
		}
	}
	
	m_curBBIndex = -1;
	m_curAnchorIndex = -1;
	Update();
}

CRect(*MiniBlackBoard::GetPoints())[4]
{
	return m_anchors;
}

extern bool Load(CImage *pImage, UINT nResID);
void MiniBlackBoard::OnPaint(CDC * pDC)
{
	CBrush bbBrush(RGB(1, 90, 72));
	CBrush* pOldBrush = pDC->SelectObject(&bbBrush);

	// create and select a thick, black pen
	CPen bbPen;
	bbPen.CreatePen(PS_SOLID, 2, RGB(180, 148, 85));
	CPen* pOldPen = pDC->SelectObject(&bbPen);

	CPen selPen;
	selPen.CreatePen(PS_SOLID, 2, RGB(13, 190, 2));
	CRect rec(Rect());
	int oneW = rec.Width() / m_status;
	for (int i = 1; i <= m_status; i++)
	{
		if (i - 1 == m_curBBIndex)
		{
			pDC->SelectObject(&selPen);
		}
		else
		{
			pDC->SelectObject(&bbPen);
		}
		CRect(&PCur )[4] = m_anchors[i - 1];
		bool anchored = true;
		for (int i = 0; i < 4; ++i)
		{
			if (PCur[i].left == 0 || PCur[i].top == 0)
			{
				anchored = false;
			}
		}
		RECT dst = { rec.left + (i - 1)*oneW , rec.top, rec.left + i*oneW, rec.BottomRight().y };
		pDC->Rectangle(dst.left, dst.top, dst.right, dst.bottom);

		if (m_bShowError)
		{
			CImage Image;
			Load(&Image, anchored ? IDB_RIGHT : IDB_WARNING);
			RECT src = { 0,0, Image.GetWidth(), Image.GetHeight() };
			RECT imgDst = { dst.right - 25, dst.bottom - 47, dst.right - 5, dst.bottom - 27 };
			Image.Draw(pDC->m_hDC, imgDst, src);
			Image.Destroy();
		}
	}
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
}

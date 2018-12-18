#include "stdafx.h"
#include "resource.h"
#include "ComboBox.h"

const int ITEM_H = 28;
extern bool Load(CImage *pImage, UINT nResID);
void ComboBox::SetIndexChangeInvoker(const std::function<void(int)>& fun)
{
	m_funIndexChanged = fun;
}

void ComboBox::SetCurrentIndex(int cur)
{
	if (cur != m_current)
	{
		m_current = cur;
		m_funIndexChanged(cur);
	}
}

void ComboBox::OnPaint(CDC * pDC)
{
	CImage Image;
	DWORD borderID = StatusBtnItem::Status() == eBtnNormal ? IDB_COMNORMAL : IDB_COMBOOVER;
	if (Load(&Image, borderID))
	{
		RECT src = { 0,0, Image.GetWidth(), Image.GetHeight() };
		Image.Draw(pDC->m_hDC, StatusBtnItem::Rect(), src);
		Image.Destroy();
	}
	CImage ImageArr;
	if (Load(&ImageArr, IDB_COMBOARR))
	{
		RECT dst = { StatusBtnItem::Rect().right - 14 - 3, StatusBtnItem::Rect().top + 10,  StatusBtnItem::Rect().right - 6 , StatusBtnItem::Rect().bottom - 10};
		RECT src = { 0,0, ImageArr.GetWidth(), ImageArr.GetHeight() };
		ImageArr.Draw(pDC->m_hDC, dst, src);
		ImageArr.Destroy();
	}

	pDC->SetTextColor(RGB(255, 255, 255));
	pDC->SetBkMode(TRANSPARENT);
	CFont font;
	font.CreatePointFont(100, L"Î¢ÈíÑÅºÚ");
	pDC->SelectObject(font);
	RECT r = StatusBtnItem::Rect();
	r.left -= 10;
	r.right -= 10;
	r.top += 5;
	r.bottom += 5;
	std::wstring text = m_items[m_current];
	pDC->DrawText(text.c_str(), &r, DT_CENTER);

	DrawItems(pDC);
}

void ComboBox::DrawItems(CDC * dc)
{
	if (m_status == eUnFold)
	{
		int top = StatusBtnItem::Rect().bottom;
		for (std::wstring& item : m_items)
		{
			RECT r = { StatusBtnItem::Rect().left, top, StatusBtnItem::Rect().right, top + ITEM_H };
			CBrush bk;
			bk.CreateSolidBrush(RGB(102,102,102));
			dc->FillRect(&r, &bk);
			CBrush br;
			br.CreateSolidBrush(RGB(22, 168, 224));
			RECT ir = { r.left + 1, r.top + 1,r.right - 1,r.bottom - 1 };
			dc->FillRect(&ir, &br);
			r.left -= 10;
			r.right -= 10;
			r.top += 5;
			r.bottom += 5;
			dc->DrawText(item.c_str(), &r, DT_CENTER);
			top += ITEM_H;
		}
	}
}

bool ComboBox::MouseMove(UINT u, CPoint p)
{
	return StatusBtnItem::MouseMove(u, p);
}

bool ComboBox::MouseDown(UINT u, CPoint p)
{
	if (IsDisabled())
	{
		return false;
	}
	bool ret = false;
	StatusBtnItem::MouseDown(u, p);
	if (StatusBtnItem::Status() == eBtnPressed)
	{
		m_status = eUnFold;
		Update();
	}
	else
	{
		if (m_status == eUnFold)
		{
			if (p.x >= StatusBtnItem::Rect().left && p.x <= StatusBtnItem::Rect().right)
			{
				int index = (p.y - StatusBtnItem::Rect().bottom + ITEM_H - 1) / ITEM_H;
				if (index >= 1 && index <= (int)m_items.size())
				{
					int cur = index - 1;
					if (cur != m_current)
					{
						SetCurrentIndex(cur);
						ret = true;
					}
				}
			}
		}
		m_status = eNormal;
		Update();
	}
	return ret;
}

bool ComboBox::MouseUp(UINT u, CPoint p)
{
	return StatusBtnItem::MouseUp(u, p);
}

RECT ComboBox::Rect()
{
	RECT baseRect = StatusBtnItem::Rect();
//	if (m_status == eUnFold)
	{
		baseRect.bottom += m_items.size() * ITEM_H;
	}
	return baseRect;
}

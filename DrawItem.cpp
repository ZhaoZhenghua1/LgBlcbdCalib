#include "stdafx.h"
#include "DrawItem.h"


CDrawItem::CDrawItem(CWnd* window, RECT& rec):m_wnd(window),m_rect(rec)
{
}


CDrawItem::~CDrawItem()
{
}

inline bool CDrawItem::Contain(CPoint & p) 
{ 
	return (m_rect.left <= p.x) && (m_rect.top <= p.y) && (m_rect.bottom >= p.y) && (m_rect.right >= p.x); 
}

inline void CDrawItem::Update(bool all) 
{ 
	if (all)
	{
		m_wnd->Invalidate();
	}
	else
	{
		m_wnd->InvalidateRect(&Rect(), false); 
	}
}

void CDrawItem::SetVisible(bool visible)
{
	m_bVisible = visible;
}

void StatusBtnItem::OnPaint(CDC * dc)
{
}

bool StatusBtnItem::MouseMove(UINT c, CPoint p)
{
	auto old = m_status;
	if (0==c && Contain(p))
	{
		m_status = eBtnHover;
	}
	else
	{
		m_status = eBtnNormal;
	}
	if (old != m_status)
	{
		Update();
	}
	return false;
}

bool StatusBtnItem::MouseDown(UINT c, CPoint p)
{
	if (MK_LBUTTON&c && Contain(p))
	{
		m_status = eBtnPressed;
	}
	else
	{
		m_status = eBtnNormal;
	}
	Update();
	return false;
}

bool StatusBtnItem::MouseUp(UINT c, CPoint p)
{
	bool update = false;
	if (Contain(p) && m_status == eBtnPressed)
	{
		update = true;
		//do something	
		m_funCkChanged();
	}
	m_status = eBtnNormal;
	if (update)
	{
		Update();
	}
	return false;
}

BOOL LoadImageFromResource(CImage *pImage, UINT nResID, LPCTSTR lpTyp)
{
	if (pImage == NULL)
		return false;
	pImage->Destroy();
	// 查找资源
	HRSRC hRsrc = ::FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(nResID), lpTyp);
	if (hRsrc == NULL)
		return false;
	// 加载资源
	HGLOBAL hImgData = ::LoadResource(AfxGetResourceHandle(), hRsrc);
	if (hImgData == NULL)
	{
		::FreeResource(hImgData);
		return false;
	}
	// 锁定内存中的指定资源
	LPVOID lpVoid = ::LockResource(hImgData);
	LPSTREAM pStream = NULL;
	DWORD dwSize = ::SizeofResource(AfxGetResourceHandle(), hRsrc);
	HGLOBAL hNew = ::GlobalAlloc(GHND, dwSize);
	LPBYTE lpByte = (LPBYTE)::GlobalLock(hNew);
	::memcpy(lpByte, lpVoid, dwSize);
	// 解除内存中的指定资源
	::GlobalUnlock(hNew);
	// 从指定内存创建流对象
	HRESULT ht = ::CreateStreamOnHGlobal(hNew, TRUE, &pStream);
	if (ht != S_OK)
	{
		GlobalFree(hNew);
	}
	else
	{
		// 加载图片
		pImage->Load(pStream);
		GlobalFree(hNew);
	}
	// 释放资源
	::FreeResource(hImgData);
	return true;
}
bool Load(CImage *pImage, UINT nResID)
{
	if (!LoadImageFromResource(pImage, nResID, _T("PNG")))
	{
		return false;
	}
	if (pImage->IsNull())
	{
		return false;
	}
	if (pImage->GetBPP() == 32) //确认该图像包含Alpha通道
	{
		int i;
		int j;
		for (i = 0; i < pImage->GetWidth(); i++)
		{
			for (j = 0; j < pImage->GetHeight(); j++)
			{
				byte *pByte = (byte *)pImage->GetPixelAddress(i, j);
				pByte[0] = pByte[0] * pByte[3] / 255;
				pByte[1] = pByte[1] * pByte[3] / 255;
				pByte[2] = pByte[2] * pByte[3] / 255;
			}
		}
	}
	return true;
}
//void MapPushButton::OnPaint(CDC * pDC)
//{
//	CImage Image;
//	Load(&Image, mapID);
//	RECT src = { 0,0, Image.GetWidth(), Image.GetHeight() };
//	Image.Draw(pDC->m_hDC, Rect(), src);
//	Image.Destroy();
//}

void CheckButton::SetChecked(bool checked)
{
	if (checked != m_checked)
	{
		m_checked = checked;
		m_funCkChanged(this);
		Update();
	}
}

bool CheckButton::MouseUp(UINT ui, CPoint p)
{
	bool pressed = StatusBtnItem::Status() == eBtnPressed;
	StatusBtnItem::MouseUp(ui, p);
	if (pressed && Contain(p) && !m_checked)
	{
		m_checked = true;
		m_funCkChanged(this);
		Update();
	}
	return false;
}

void CheckButton::OnPaint(CDC * pDC)
{
	if (0 != m_resIDUnchecked)
	{
		CImage Image;
		Load(&Image, m_checked&&m_resIDChecked ? m_resIDChecked : m_resIDUnchecked);
		RECT src = { 0,0, Image.GetWidth(), Image.GetHeight() };
		Image.Draw(pDC->m_hDC, Rect(), src);
		Image.Destroy();
	}
	if (!m_text.empty())
	{
		DWORD color = m_checked ? RGB(0, 153, 255) : RGB(153, 153, 153);
		CBrush brushBlue(color);
		CBrush* pOldBrush = pDC->SelectObject(&brushBlue);

		// create and select a thick, black pen
		CPen penBlack;
		penBlack.CreatePen(PS_NULL, 0, color);
		CPen* pOldPen = pDC->SelectObject(&penBlack);


		POINT p = { 7,7 };
		pDC->RoundRect(&Rect(), p);
		pDC->SelectObject(pOldBrush);
		pDC->SelectObject(pOldPen);

		CPen penBlack2;
		penBlack2.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
		pDC->SelectObject(&penBlack2);
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkMode(TRANSPARENT);
		CFont font;
		font.CreatePointFont(100, L"微软雅黑");
		pDC->SelectObject(font);
		RECT r = Rect();
		r.top += 5;
		r.bottom += 5;
		pDC->DrawText(m_text.c_str(), &r, DT_CENTER);
	}
}

CheckGroup::CheckGroup(std::initializer_list<CheckButton*> list) :m_btns(list) 
{
	for (CheckButton* ct : m_btns)
	{
		ct->SetInvocker([=](CheckButton* cbtn)
		{
			if (cbtn->Checked())
			{
				for (CheckButton* ct : m_btns)
				{
					if (ct != cbtn)
					{
						ct->SetChecked(false);
					}
				}
				m_funCkChanged(cbtn);
			}
		});
	}
}

bool BlankPushButton::MouseMove(UINT u, CPoint p)
{
	StatusBtnItem::MouseMove(u, p);
	if (Status() == eBtnHover)
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
	}
	else
	{
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}
	return false;
}

bool BlankPushButton::MouseDown(UINT u, CPoint p)
{
	return StatusBtnItem::MouseDown(u, p);
}

bool BlankPushButton::MouseUp(UINT u, CPoint p)
{
	return StatusBtnItem::MouseUp(u, p);
}

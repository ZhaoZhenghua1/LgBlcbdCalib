#include "stdafx.h"
#include "CameraItem.h"

extern std::wstring s2ws(const std::string& str);
CString CameraItem::Init()
{
	try
	{
		m_cameraThings.Init();
	}
	catch (const std::string& err)
	{
		AfxMessageBox(s2ws(err).c_str());
	}
	catch (const std::wstring& err)
	{
		AfxMessageBox(err.c_str());
	}
	catch (...)
	{
		AfxMessageBox(L"≥ı ºªØ ß∞‹£°");
	}
	
	return L"";
}

CString CameraItem::GetCameraIP()
{
	return m_cameraThings.GetCameraIP();
}

void CameraItem::StartCamera(bool b)
{
	m_cameraThings.StartCamera(b);
}
extern void MatToCImage(Mat& mat, CImage& cimage);
CRect GetDrawRect(CRect dst, CRect src)
{
	double ratio = (double)src.Width() / src.Height();
	double dstRatio = (double)dst.Width() / dst.Height();
	double dstH = 0, dstW = 0;
	if (ratio < dstRatio)
	{
		dstH = dst.Height();
		dstW = (double)dstH / src.Height() * src.Width();
	}
	else
	{
		dstW = dst.Width();
		dstH = (double)dstW / src.Width() * src.Height();
	}
	int left = int(0.5 + dst.left + (dst.Width() - dstW) / 2);
	int top = int(0.5 + dst.top + (dst.Height() - dstH) / 2);
	int right = left + int(0.5 + dstW);
	int bottom = top + int(0.5 + dstH);
	return{ left, top, right, bottom };
}
void CameraItem::OnPaint(CDC * dc)
{
	CBrush br;
	br.CreateSolidBrush(RGB(22, 168, 224));
	dc->FillRect(&Rect(), &br);
	
	CImage image;
	Mat imageMat = m_cameraThings.GetImage();
	MatToCImage(imageMat, image);
	
	if (!image.IsNull())
	{
		CRect src = { 0,0, image.GetWidth(), image.GetHeight() };
		CRect dst = Rect();//GetDrawRect(Rect(), src);
		SetStretchBltMode(dc->m_hDC, src.Width() > dst.Width() ? HALFTONE : COLORONCOLOR);
		image.StretchBlt(dc->m_hDC, dst);
	}
	image.Destroy();

	dc->SelectObject(GetStockObject(HOLLOW_BRUSH));
	if (m_rects)
	{
		COLORREF colors[4] = { RGB(255,128,128),RGB(255,255,128), RGB(128,255,128), RGB(128,255,255) };
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; ++j)
			{
				CRect rect = m_rects[i][j];
				if (rect.left > 0 && rect.top > 0)
				{
					rect += CPoint(Rect().left, Rect().top);
					
					CPen penBlack;
					penBlack.CreatePen(PS_SOLID, 0, colors[i]);
					CPen* pOldPen = dc->SelectObject(&penBlack);
					dc->Rectangle(rect);
				}
			}
		}
	}
	CPen penWhite;
	penWhite.CreatePen(PS_SOLID, 1, RGB(255,255,255));
	dc->SelectObject(&penWhite);
	if (m_pos0.x != 0 && m_pos0.y != 0)
	{
		CRect r = { std::min(m_pos0.x, m_pos1.x) ,std::min(m_pos0.y, m_pos1.y),  std::max(m_pos0.x, m_pos1.x) ,std::max(m_pos0.y,m_pos1.y) };
		dc->Rectangle(r);
	}
}

bool CameraItem::MouseDown(UINT u, CPoint p)
{
	if (Contain(p))
	{
		m_pos0 = p;
	}
	return false;
}

bool CameraItem::MouseMove(UINT, CPoint p)
{
	if (m_pos0.x != 0 && m_pos0.y != 0)
	{
		m_pos1 = p;
		this->Update();	
	}
	return false;
}

bool CameraItem::MouseUp(UINT, CPoint)
{
	if (m_pos0.x > 0 && m_pos0.y > 0 && m_pos1.x > 0 && m_pos1.y > 0)
	{
		RECT r = { std::min(m_pos0.x, m_pos1.x) - Rect().left ,std::min(m_pos0.y, m_pos1.y) - Rect().top,  std::max(m_pos0.x, m_pos1.x) - Rect().left ,std::max(m_pos0.y,m_pos1.y) - Rect().top };
		m_funAnchor(r);
	}
	m_pos1 = m_pos0 = CPoint();
	return false;
}

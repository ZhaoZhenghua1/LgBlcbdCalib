#pragma once
#include "DrawItem.h"
#include ".\camera\CameraThings.h"
class CameraItem : public CDrawItem
{
public:
	using CDrawItem::CDrawItem;
public:
	CString Init();
	CString GetCameraIP();
	void StartCamera(bool = true);
	void SetPoints(CRect(*points)[4]) { m_rects = points; }
	void SetAnchorInvocker(const std::function<void(CRect)>& fun) { m_funAnchor = fun; }
	CameraThings& GetCameraThings() { return m_cameraThings; }
	bool IsCameraStarted() { return m_cameraThings.IsRunning(); }
protected:
	virtual void OnPaint(CDC* dc);
	virtual bool MouseDown(UINT, CPoint);
	virtual bool MouseMove(UINT, CPoint)override;
	virtual bool MouseUp(UINT, CPoint)override;
private:
	CameraThings m_cameraThings;
	std::function<void(CRect)> m_funAnchor = [](CRect) {};
	CRect (*m_rects)[4] = nullptr;
	CPoint m_pos0;
	CPoint m_pos1;
};


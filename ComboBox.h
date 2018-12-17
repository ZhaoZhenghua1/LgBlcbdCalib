#pragma once
#include <vector>
#include "DrawItem.h"

class ComboBox : public StatusBtnItem
{
	enum EStatus{eNormal, eUnFold};
public:
	using StatusBtnItem::StatusBtnItem;
public:
	void SetIndexChangeInvoker(const std::function<void(int)>& fun);
	void SetCurrentIndex(int i);
protected:
	virtual void OnPaint(CDC* dc);
	virtual bool MouseMove(UINT, CPoint);
	virtual bool MouseDown(UINT, CPoint);
	virtual bool MouseUp(UINT, CPoint);
	virtual RECT Rect() override;
private:
	void DrawItems(CDC* dc);
private:
	std::vector<std::wstring> m_items = {L"2ÁªºÚ°å", L"4ÁªºÚ°å"};
	int m_current = 1;
	EStatus m_status = eNormal;
	std::function<void(int)> m_funIndexChanged = [](int) {};
};


#pragma once
#include <string>
#include <functional>
#include <vector>

class CDrawItem
{
public:
	CDrawItem(CWnd* window, RECT& rec);
	virtual ~CDrawItem();
public:
	virtual RECT Rect() { return m_rect; }
	bool Contain(CPoint& p);
	virtual void Update(bool all = false);
	void SetZOrder(int z) { m_zorder = z; }
	int ZOrder() { return m_zorder; }
	void SetVisible(bool visible);
	bool Visible() { return m_bVisible; }
	void SetDisabled(bool b) { m_bDistabled = b; }
	bool IsDisabled() { return m_bDistabled; }
protected:
	virtual void OnPaint(CDC* dc) {}
	virtual bool MouseMove(UINT, CPoint) { return false; }
	virtual bool MouseDown(UINT, CPoint) { return false; }
	virtual bool MouseUp(UINT, CPoint) { return false; }
	friend class CCameraDealingDlg;
private:
	RECT m_rect;
	CWnd* m_wnd;
	int m_zorder = 0;
	bool m_bVisible = true;
	bool m_bDistabled = false;
};

enum ButtonStatus{eBtnNormal, eBtnHover, eBtnPressed};
class StatusBtnItem : public CDrawItem
{
public:
	using CDrawItem::CDrawItem;
	void SetInvoker(const std::function<void()>& fun) { m_funCkChanged = fun; }
protected:
	virtual void OnPaint(CDC* dc) override;
	virtual bool MouseMove(UINT, CPoint) override;
	virtual bool MouseDown(UINT, CPoint) override;
	virtual bool MouseUp(UINT, CPoint) override;
protected:
	ButtonStatus Status() { return m_status; }
	ButtonStatus m_status = eBtnNormal;
private:
	std::function<void()> m_funCkChanged = []() {};
};

class BlankPushButton : public StatusBtnItem
{
public:
	using StatusBtnItem::StatusBtnItem;
protected:
	virtual void Update(bool all = false)override {};
};

class CheckButton : public StatusBtnItem
{
public:
	using StatusBtnItem::StatusBtnItem;
	void SetText(const std::wstring& text) { m_text = text; }
	void SetResource(DWORD res0, DWORD res1 = 0) { m_resIDUnchecked = res0, m_resIDChecked = res1; }
	void SetChecked(bool checked);
	bool Checked() { return m_checked; }
	void SetData(int data) { m_data = data; }
	int Data() { return m_data; }
	void SetInvocker(const std::function<void(CheckButton*)>& fun) { m_funCkChanged = fun; }
protected:
	virtual bool MouseUp(UINT, CPoint) override;
	virtual void OnPaint(CDC* dc) override;
private:
	bool m_checked = false;
	std::wstring m_text;
	DWORD m_resIDUnchecked = 0, m_resIDChecked = 0;
	int m_data = 0;
	std::function<void(CheckButton*)> m_funCkChanged = [](CheckButton*) {};
};

class CheckGroup
{
public:
	CheckGroup(std::initializer_list<CheckButton*> list);
	void SetCheckGroupInvoker(const std::function<void(CheckButton*)>& fun) { m_funCkChanged = fun; }
private:
	std::vector<CheckButton*> m_btns;
	std::function<void(CheckButton*)> m_funCkChanged = [](CheckButton*) {};
};

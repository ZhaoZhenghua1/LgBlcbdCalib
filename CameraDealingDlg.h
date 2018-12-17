
// CameraDealingDlg.h : 头文件
//

#pragma once
#include <list>

class CDrawItem;
class CameraItem;
// CCameraDealingDlg 对话框
class CCameraDealingDlg : public CDialogEx
{
// 构造
public:
	CCameraDealingDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMERADEALING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNcPaint();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnMouseMove(UINT, CPoint);
	afx_msg void OnLButtonDown(UINT, CPoint);
	afx_msg void OnLButtonUp(UINT, CPoint);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
private:
	template<class T>
	T* CreateT(RECT& r)
	{
		auto ret = new T(this, r);
		m_drawItems.push_back(ret);
		return ret;
	}
private:
	std::list<CDrawItem*> m_drawItems;
	CameraItem* m_camera = nullptr;
	CDrawItem* m_minimum = nullptr;
	CDrawItem* m_close = nullptr;
	CDrawItem* m_play = nullptr;
	CString m_cameraIP;
};

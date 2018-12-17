
// CameraDealingDlg.h : ͷ�ļ�
//

#pragma once
#include <list>

class CDrawItem;
class CameraItem;
// CCameraDealingDlg �Ի���
class CCameraDealingDlg : public CDialogEx
{
// ����
public:
	CCameraDealingDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMERADEALING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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

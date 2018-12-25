
// CameraDealingDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "CameraDealing.h"
#include "CameraDealingDlg.h"
#include "afxdialogex.h"
#include "DrawItem.h"
#include "ComboBox.h"
#include "MiniBlackBoard.h"
#include "CameraItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCameraDealingDlg �Ի���


CCameraDealingDlg::CCameraDealingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CAMERADEALING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	RECT comboRect = { 118,10,198,38 };
	ComboBox* combox = CreateT<ComboBox>(comboRect);
	combox->SetZOrder(1);

	CRect minibbRECT = { 93,102, 327, 153 };
	MiniBlackBoard* minibb = CreateT<MiniBlackBoard>(minibbRECT);
	m_miniBB = minibb;

	CRect checkRECT = {92,55, 92+ 63, 55 + 30};
	CheckButton* checkBtn1 = CreateT<CheckButton>(checkRECT);
	checkBtn1->SetText(L"��1��");
	checkRECT.MoveToX(checkRECT.left + 80);
	CheckButton* checkBtn2 = CreateT<CheckButton>(checkRECT);
	checkBtn2->SetText(L"��2��");
	checkRECT.MoveToX(checkRECT.left + 80);
	CheckButton* checkBtn3 = CreateT<CheckButton>(checkRECT);
	checkBtn3->SetText(L"��3��");
	checkRECT.MoveToX(checkRECT.left + 80);
	CheckButton* checkBtn4 = CreateT<CheckButton>(checkRECT);
	checkBtn4->SetText(L"��4��");
	CheckGroup* cg0 = new CheckGroup({ checkBtn1, checkBtn2, checkBtn3, checkBtn4 });


	checkRECT = { 505,84, 505 + 14, 84 + 14 };
	CheckButton* radiocheckBtn1 = CreateT<CheckButton>(checkRECT);
	radiocheckBtn1->SetResource(IDB_CHECKEDNOT, IDB_CHECKED);
	checkRECT.MoveToX(checkRECT.left + 72);
	CheckButton* radiocheckBtn2 = CreateT<CheckButton>(checkRECT);
	radiocheckBtn2->SetResource(IDB_CHECKEDNOT, IDB_CHECKED);
	checkRECT.MoveToY(checkRECT.top + 37);
	CheckButton* radiocheckBtn3 = CreateT<CheckButton>(checkRECT);
	radiocheckBtn3->SetResource(IDB_CHECKEDNOT, IDB_CHECKED);
	checkRECT.MoveToXY(checkRECT.left - 72, checkRECT.top);
	CheckButton* radiocheckBtn4 = CreateT<CheckButton>(checkRECT);
	radiocheckBtn4->SetResource(IDB_CHECKEDNOT, IDB_CHECKED);
	CheckGroup* cg = new CheckGroup({ radiocheckBtn1, radiocheckBtn2, radiocheckBtn3, radiocheckBtn4 });
	//�궨λ�ð�ť���µĻص�
	cg->SetCheckGroupInvoker([=](CheckButton* cb)
	{
		if (cb == radiocheckBtn1)
		{
			minibb->SetCurAnchorIndex(0);
		}
		else if (cb == radiocheckBtn2)
		{
			minibb->SetCurAnchorIndex(1);
		}
		else if (cb == radiocheckBtn3)
		{
			minibb->SetCurAnchorIndex(3);
		}
		else if (cb == radiocheckBtn4)
		{
			minibb->SetCurAnchorIndex(2);
		}
	});
	
	//�ڼ����ڰ屻���µĻص�
	cg0->SetCheckGroupInvoker([=](CheckButton* cb) {
		if (cb == checkBtn1)
		{
			minibb->SetCurBBIndex(0);
		}
		else if (cb == checkBtn2)
		{
			minibb->SetCurBBIndex(1);
		}
		else if (cb == checkBtn3)
		{
			minibb->SetCurBBIndex(2);
		}
		else if (cb == checkBtn4)
		{
			minibb->SetCurBBIndex(3);
		}
		radiocheckBtn1->SetChecked(true);
	});

	combox->SetIndexChangeInvoker([=](int i)
	{
		checkBtn3->SetVisible(i == 1);
		checkBtn4->SetVisible(i == 1);
		minibb->SetBBStatus(i == 1 ? EBBStatus::e4 : EBBStatus::e2);

		CheckButton* arr[] = { checkBtn1,checkBtn2,checkBtn3,checkBtn4,radiocheckBtn1,radiocheckBtn2,radiocheckBtn3,radiocheckBtn4 };
		for (CheckButton* a : arr)
		{
			a->SetChecked(false);
		}

		CRect cameraRect = m_camera->Rect();
		minibb->ReSet(m_camera->GetCameraThings().GetDataFromRegedit(i == 1 ? EBBStatus::e4 : EBBStatus::e2, cameraRect.Size()));
		if (this->m_hWnd)
		{
			checkBtn1->SetChecked(true);
			this->Invalidate(false);
		}
	});
	

	CRect closeRECT = { 976,5, 998, 27 };
	BlankPushButton* blankBtnClose = CreateT<BlankPushButton>(closeRECT);
	m_close = blankBtnClose;
	blankBtnClose->SetInvoker([=]() {this->Close();});

	CRect miniRECT = { 943,5, 967, 27 };
	BlankPushButton* blankBtnMinimum = CreateT<BlankPushButton>(miniRECT);
	m_minimum = blankBtnMinimum;
	blankBtnMinimum->SetInvoker([=]() {
		this->ShowWindow(SW_MINIMIZE);
	});

	CRect cameraRect = { 13,174,988,677 };
	m_camera = CreateT<CameraItem>(cameraRect);
	m_camera->SetVisible(false);
	m_camera->SetPoints(minibb->GetPoints());
	m_camera->SetAnchorInvocker([=](CRect p) {
		minibb->FeedLocation(p);
	});

	CRect cameraBtnRECT = { 443,467, 562, 504 };
	BlankPushButton* blankBtnCanera = CreateT<BlankPushButton>(cameraBtnRECT);
	m_play = blankBtnCanera;
	blankBtnCanera->SetInvoker([=]() 
	{
		for (auto ite = m_drawItems.begin(); ite != m_drawItems.end(); ++ite)
		{
			CDrawItem* item = (*ite);
			//δ���ţ����ܵ���������ť
			item->SetDisabled(false);
		}
		blankBtnCanera->SetVisible(false);
		m_camera->SetVisible(true);
		m_camera->StartCamera(true);
		combox->SetCurrentIndex(0);
		minibb->ShowErrorAndWarning(true);
		checkBtn1->SetChecked(true);
		this->Invalidate(false);
	});

	RECT r = { 648,88,699,139 };
	BlankPushButton* blankBtnReset = CreateT<BlankPushButton>(r);
	blankBtnReset->SetInvoker([=]()
	{
		CRect empty[4][4];
		m_camera->GetCameraThings().Save(minibb->GetBBStatus(), empty, CSize(m_camera->Rect().right - m_camera->Rect().left, m_camera->Rect().bottom - m_camera->Rect().top));
		minibb->ReSet(empty);
		checkBtn1->SetChecked(true);
		Invalidate(false);
	});

	r = { 904,123,988,159 };
	BlankPushButton* blankBtnSave = CreateT<BlankPushButton>(r);
	blankBtnSave->SetInvoker([=]()
	{
		//Save
		m_camera->GetCameraThings().Save(minibb->GetBBStatus(), minibb->GetPoints(), CSize(m_camera->Rect().right - m_camera->Rect().left, m_camera->Rect().bottom - m_camera->Rect().top));
		AfxMessageBox(L"����ɹ�!");
	});
	combox->SetCurrentIndex(0);
	for (auto ite = m_drawItems.begin(); ite != m_drawItems.end(); ++ite)
	{
		CDrawItem* item = (*ite);
		//δ���ţ����ܵ���������ť
		if (item != m_play && item != m_minimum && item != m_close)
		{
			item->SetDisabled(true);
		}
	}
}

void CCameraDealingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCameraDealingDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_NCPAINT()
	ON_WM_NCHITTEST()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CCameraDealingDlg ��Ϣ�������

BOOL CCameraDealingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	AfxSetWindowText(this->m_hWnd, L"LgBlcbdCalib");
//	this->SetWindowTextW(L"LgBlcbdCalib");
	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	SetTimer(1, 333, NULL);
	m_camera->Init();
	m_cameraIP =  m_camera->GetCameraIP();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CCameraDealingDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCameraDealingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������
		RECT rect;
		this->GetClientRect(&rect);


		CDC MemDC; //���ȶ���һ����ʾ�豸����
		CBitmap MemBitmap;//����һ��λͼ����

		//���������Ļ��ʾ���ݵ��ڴ���ʾ�豸
		MemDC.CreateCompatibleDC(NULL);
		//��ʱ�����ܻ�ͼ����Ϊû�еط��� ^_^
		//���潨��һ������Ļ��ʾ���ݵ�λͼ������λͼ�Ĵ�С������ô��ڵĴ�С
		MemBitmap.CreateCompatibleBitmap(&dc, rect.right - rect.left, rect.bottom - rect.top);

		//��λͼѡ�뵽�ڴ���ʾ�豸��
		//ֻ��ѡ����λͼ���ڴ���ʾ�豸���еط���ͼ������ָ����λͼ��
		CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);

//		MemDC.FillSolidRect(0, 0, rect.right - rect.left, rect.bottom - rect.top, RGB(255, 255, 255));
		CBrush br;
		br.CreateSolidBrush(RGB(0, 0, 255));
		MemDC.FillRect(&rect, &br);
		
		CBitmap bitmap;
		if (bitmap.LoadBitmapW(IDB_BITMAPBKG))
		{
			HDC memdc = ::CreateCompatibleDC(MemDC.m_hDC);
			::SelectObject(memdc, bitmap);
			BitBlt(MemDC.m_hDC, 0, 0, rect.right, rect.bottom, memdc, 0, 0, SRCCOPY);
			DeleteObject(memdc);
		}

		CPen penBlack2;
		penBlack2.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
		MemDC.SelectObject(&penBlack2);
		MemDC.SetTextColor(RGB(255, 255, 255));
		MemDC.SetBkMode(TRANSPARENT);
		CFont font;
		VERIFY(font.CreateFont(
			20,                        // nHeight
			0,                         // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			FW_BOLD,                   // nWeight
			FALSE,                     // bItalic
			FALSE,                     // bUnderline
			0,                         // cStrikeOut
			DEFAULT_CHARSET,           // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
			_T("΢���ź�")));           // lpszFacename

		//font.CreatePointFont(100, L"΢���ź�");
		MemDC.SelectObject(font);
		RECT r = {887,57,995,118};
		CString ip;
		int atSig = m_cameraIP.Find(L"@");
		if (atSig >= 0)
		{
			ip = m_cameraIP.Mid(atSig + 1);
		}
		atSig = ip.Find(L":");
		if (atSig >= 0)
		{
			ip = ip.Mid(0, atSig);
		}
		MemDC.DrawText(ip, &r, DT_LEFT | DT_WORDBREAK);
		font.DeleteObject();

		m_drawItems.sort([](CDrawItem* l, CDrawItem* r) {return l->ZOrder() < r->ZOrder();});
		
		for (auto ite : m_drawItems)
		{
			if (ite->Visible())
				ite->OnPaint(&MemDC);
		}
		//���ڴ��е�ͼ��������Ļ�Ͻ�����ʾ
		dc.BitBlt(0, 0, rect.right - rect.left, rect.bottom - rect.top, &MemDC, 0, 0, SRCCOPY);

		MemBitmap.DeleteObject();
		MemDC.DeleteDC();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CCameraDealingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCameraDealingDlg::OnNcPaint()
{
	CDialogEx::OnNcPaint();
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialogEx::OnNcPaint()
}

LRESULT CCameraDealingDlg::OnNcHitTest(CPoint point)
{
	ScreenToClient(&point);
	for (auto ite : m_drawItems)
	{
		if (ite->Contain(point))
		{
			return CDialogEx::OnNcHitTest(point);
		}
	}

	if (point.y < 44)
		return HTCAPTION;

	return CDialogEx::OnNcHitTest(point);
}

void CCameraDealingDlg::OnMouseMove(UINT ui, CPoint p)
{
	for (auto ite = m_drawItems.rbegin(); ite != m_drawItems.rend(); ++ite)
	{
		if ((*ite)->Visible())
		{
			if ((*ite)->MouseMove(ui, p))
				break;
		}
	}
}

void CCameraDealingDlg::OnLButtonDown(UINT ui, CPoint p)
{
	for (auto ite = m_drawItems.rbegin(); ite != m_drawItems.rend(); ++ite)
	{
		if ((*ite)->Visible())
		{
			if ((*ite)->MouseDown(ui, p))
				break;
		}
	}
}

void CCameraDealingDlg::OnLButtonUp(UINT ui, CPoint p)
{
	for (auto ite = m_drawItems.rbegin(); ite != m_drawItems.rend(); ++ite)
	{
		if ((*ite)->Visible())
			if ((*ite)->MouseUp(ui, p))
				break;
	}
}

void CCameraDealingDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (m_camera->Visible())
	{
		m_camera->Update();
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CCameraDealingDlg::Close()
{
	//�Ƿ��Ѿ�������ͷ
	if (!m_camera->Visible())
	{
		::MessageBox(this->m_hWnd, L"���ȴ�����ͷ!", L"�������ͷ", 0);
		return;
	}
	//�Ƿ�ȫ���궨
	if (m_camera->GetCameraThings().IsAllMarked(m_miniBB->GetBBStatus()))
	{
		PostQuitMessage(0);
	}
	else
	{
		::MessageBox(this->m_hWnd, L"��궨���е��������,Ȼ�����˳�!", L"���ȱ궨", 0);
	}
}


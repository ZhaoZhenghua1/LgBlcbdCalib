
// CameraDealing.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCameraDealingApp: 
// �йش����ʵ�֣������ CameraDealing.cpp
//

class CCameraDealingApp : public CWinApp
{
public:
	CCameraDealingApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCameraDealingApp theApp;
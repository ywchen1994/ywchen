
// RTEXTool.h : RTEXTool ���ε{�����D���Y��
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

#include "resource.h"       // �D�n�Ÿ�


// CRTEXToolApp:
// �аѾ\��@�����O�� RTEXTool.cpp
//

class CRTEXToolApp : public CWinApp
{
public:
	CRTEXToolApp();


// �мg
public:
	virtual BOOL InitInstance();

// �{���X��@
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CRTEXToolApp theApp;

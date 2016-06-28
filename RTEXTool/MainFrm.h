
// MainFrm.h : CMainFrame ���O������
//

#pragma once
#include "MySplitterWnd.h"

const UINT uifuncForm_W = 500;
const UINT uidebugForm_H = 300;
#define  _Cmd_NOShow_Status_Bar
#define NoDebugViewForm

class CMainFrame : public CFrameWnd
{
	
protected: // �ȱq�ǦC�ƫإ�
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// �ݩ�
public:

// �@�~
public:

// �мg
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// �{���X��@
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // ����C���O������
	CStatusBar        m_wndStatusBar, m_wndStatusBar1,  m_wndStatusBar2;
protected:
	CSplitterWnd    m_splitter;
#ifdef NoDebugViewForm
#else
	CSplitterWnd    m_subsplitter;
#endif
	//CMySplitterWnd    m_splitter;
	//CMySplitterWnd    m_subsplitter;

// ���ͪ��T�������禡
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	bool isInit;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	bool m_fgRejectClosedByQCmd;
	bool m_fgRejectClosedByShowPos;
	CWnd* m_myFormView;
};



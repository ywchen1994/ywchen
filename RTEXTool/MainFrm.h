
// MainFrm.h : CMainFrame 類別的介面
//

#pragma once
#include "MySplitterWnd.h"

const UINT uifuncForm_W = 500;
const UINT uidebugForm_H = 300;
#define  _Cmd_NOShow_Status_Bar
#define NoDebugViewForm

class CMainFrame : public CFrameWnd
{
	
protected: // 僅從序列化建立
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 屬性
public:

// 作業
public:

// 覆寫
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 程式碼實作
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // 控制列內嵌的成員
	CStatusBar        m_wndStatusBar, m_wndStatusBar1,  m_wndStatusBar2;
protected:
	CSplitterWnd    m_splitter;
#ifdef NoDebugViewForm
#else
	CSplitterWnd    m_subsplitter;
#endif
	//CMySplitterWnd    m_splitter;
	//CMySplitterWnd    m_subsplitter;

// 產生的訊息對應函式
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



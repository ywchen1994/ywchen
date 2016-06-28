
// MainFrm.cpp : CMainFrame 類別的實作
//

#include "stdafx.h"
#include "RTEXTool.h"

#include "MainFrm.h"
#include "RTEXToolDoc.h"
#include "RTEXToolView.h"
#include "RTEXFormView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_INDICATOR_Pos,
//	ID_SEPARATOR,           // 狀態列指示器
//	ID_INDICATOR_CAPS,
	//ID_INDICATOR_NUM,
	//ID_INDICATOR_SCRL,
};

static UINT indicators1[] =
{
	ID_INDICATOR_RecieveCmd,
};

static UINT indicators2[] =
{
	ID_INDICATOR_SendCmd,
};



// CMainFrame 建構/解構

CMainFrame::CMainFrame()
	: isInit(false)
	, m_fgRejectClosedByQCmd(FALSE)
	, m_fgRejectClosedByShowPos(FALSE)
	, m_myFormView(nullptr)
{
	// TODO: 在此加入成員初始化程式碼
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

  #ifndef _Cmd_NOShow_Status_Bar
	if (!m_wndStatusBar1.Create(this))
	{
		TRACE0("無法建立狀態列\n");
		return -1;      // 無法建立
	}

	if (!m_wndStatusBar2.Create(this))
	{
		TRACE0("無法建立狀態列\n");
		return -1;      // 無法建立
	}
  #endif
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("無法建立狀態列\n");
		return -1;      // 無法建立
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	m_wndStatusBar.SetPaneInfo(0,ID_INDICATOR_Pos,SBPS_NORMAL,175);
  #ifndef _Cmd_NOShow_Status_Bar
	m_wndStatusBar1.SetIndicators(indicators1, sizeof(indicators1)/sizeof(UINT));
	m_wndStatusBar1.SetPaneInfo(0,ID_INDICATOR_RecieveCmd,SBPS_STRETCH,1000);
	m_wndStatusBar2.SetIndicators(indicators2, sizeof(indicators2)/sizeof(UINT));
	m_wndStatusBar2.SetPaneInfo(0,ID_INDICATOR_SendCmd,SBPS_STRETCH,1000);
  #endif
	this->SetWindowText("RTEXTool V30 OPEN");
	SetMenu(NULL);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此經由修改 CREATESTRUCT cs 
	// 達到修改視窗類別或樣式的目的
	const long nScrnW =GetSystemMetrics(SM_CXSCREEN);
	const long nScrnH =GetSystemMetrics(SM_CYSCREEN);
	TRACE("screen: %d, %d\n",nScrnW, nScrnH);
	cs.x=10;
	cs.y =10; 	

        cs.cy =(nScrnH * 9/10);
#ifdef NoDebugViewForm
		cs.cx =(nScrnW * 9 /10);
#else
		cs.cx =(nScrnH * 9 /10) -uidebugForm_H + uifuncForm_W;		
#endif

	cs.style &= ~FWS_ADDTOTITLE;
	cs.style &= ~WS_SIZEBOX;
	return TRUE;
}

// CMainFrame 診斷

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 訊息處理常式


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	if(!m_splitter.CreateStatic(this,1,2))
	{
		return FALSE;
	}
#ifdef NoDebugViewForm
#else
	if(!m_subsplitter.CreateStatic(&m_splitter,2,1,WS_CHILD|WS_VISIBLE, m_splitter.IdFromRowCol(0, 1)))
	{
				return FALSE;
	}
#endif

	CRect cr;
	GetClientRect(&cr);
	if(!m_splitter.CreateView(0,0,RUNTIME_CLASS(CRTEXFormView),CSize(uifuncForm_W,cr.Height()),pContext))
	{
		return FALSE;
	}

#ifdef NoDebugViewForm
	if(!m_splitter.CreateView(0,1,RUNTIME_CLASS(CRTEXToolView),CSize(cr.Width()-uifuncForm_W,cr.Height()),pContext))
	{
		return FALSE;
	}
#else
	if(!m_subsplitter.CreateView(0,0,RUNTIME_CLASS(CRTEXToolView),CSize(cr.Width()-uifuncForm_W,cr.Height()-uidebugForm_H),pContext))
	{
		return FALSE;
	}	
	if(!m_subsplitter.CreateView(1,0,RUNTIME_CLASS(CRTEXDebugFormView),CSize(cr.Width()-uifuncForm_W,uidebugForm_H),pContext))
	{
		return FALSE;
	}	
#endif
	isInit =TRUE;	
	return TRUE;
//	return CFrameWnd::OnCreateClient(lpcs, pContext);
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// TODO: 在此加入您的訊息處理常式程式碼
	if (isInit && (nType != SIZE_MINIMIZED))
	{
		CRect cr;
		GetClientRect(&cr);
		TRACE("size: %d , %d\n",cr.Width(),cr.Height());
		m_splitter.SetColumnInfo(0,uifuncForm_W,10);
		m_splitter.SetColumnInfo(1,cr.Width()-uifuncForm_W,10);
		m_splitter.RecalcLayout();	
#ifdef NoDebugViewForm
#else
		m_subsplitter.SetRowInfo(0,cr.Height()-uidebugForm_H,10);
		m_subsplitter.SetRowInfo(1,uidebugForm_H,10);
		m_subsplitter.RecalcLayout();	
#endif
	}

}


void CMainFrame::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	TRACE("wm_close\n");
	if(m_fgRejectClosedByQCmd || m_fgRejectClosedByShowPos)
	{
		CMainFrame* pMainWnd = (CMainFrame*) AfxGetApp()->m_pMainWnd;  
		if(m_myFormView)
		{              
			::PostMessage(m_myFormView->m_hWnd,WM_SendviewStop,0,0);
		}
		TRACE("Set to reject closed \n");
		return;
	}
	CFrameWnd::OnClose();
}

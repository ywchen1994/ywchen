// SocketClient.cpp : 實作檔
//

#include "stdafx.h"
#include "RTEXTool.h"
#include "SocketClient.h"
#include "RTEXFormView.h"

// CSocketClient

CSocketClient::CSocketClient()
{
}

CSocketClient::~CSocketClient()
{
}


// CSocketClient 成員函式

//自定義事件，設置與對話框的關聯
void CSocketClient::SetParent(CWnd* pWnd)
{
	m_pWnd = pWnd;
}

//下列的Socket事件，設置其與對話框相關聯，重載代碼如下：
void CSocketClient::OnClose(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class

	if (0==nErrorCode)
	{
		((CRTEXFormView*)m_pWnd)->ClientOnSocketClose();
	}
}

void CSocketClient::OnConnect(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class

	if(nErrorCode==0)
	{
		((CRTEXFormView*)m_pWnd)->ClientOnConnect();
	}
}

void CSocketClient::OnReceive(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class

	if(0==nErrorCode)
	{
		//2選1，parent function or post Private Window Message  to parent
		((CRTEXFormView*)m_pWnd)->ClientOnReceive();
		//2選1，parent function or post Private Window Message  to parent
		//::PostMessage(m_pWnd->m_hWnd,WM_Socket_Receive,0,0);
	}
}

void CSocketClient::OnSend(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class

	if (0==nErrorCode)
	{
		((CRTEXFormView*)m_pWnd)->ClientOnSend();
	}
}

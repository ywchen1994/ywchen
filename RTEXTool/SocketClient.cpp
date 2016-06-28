// SocketClient.cpp : ��@��
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


// CSocketClient �����禡

//�۩w�q�ƥ�A�]�m�P��ܮت����p
void CSocketClient::SetParent(CWnd* pWnd)
{
	m_pWnd = pWnd;
}

//�U�C��Socket�ƥ�A�]�m��P��ܮج����p�A�����N�X�p�U�G
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
		//2��1�Aparent function or post Private Window Message  to parent
		((CRTEXFormView*)m_pWnd)->ClientOnReceive();
		//2��1�Aparent function or post Private Window Message  to parent
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

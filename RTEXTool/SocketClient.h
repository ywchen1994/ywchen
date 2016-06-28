#pragma once

// CSocketClient 命令目標

class CSocketClient : public CAsyncSocket
{
public:
	//自定義函數，用來設置該類與對話框關聯
	void SetParent(CWnd* pWnd);
	CSocketClient();
	virtual ~CSocketClient();
private:
	CWnd* m_pWnd;
protected:
	virtual void OnClose(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
};



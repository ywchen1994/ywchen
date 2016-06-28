#pragma once

// CSocketClient �R�O�ؼ�

class CSocketClient : public CAsyncSocket
{
public:
	//�۩w�q��ơA�Ψӳ]�m�����P��ܮ����p
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



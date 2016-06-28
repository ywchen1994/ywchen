#pragma once
#include "RTEXFormView.h"

// COpenJogDialog ��ܤ��

class COpenJogDialog : public CDialogEx
{
	DECLARE_DYNAMIC(COpenJogDialog)

public:
	COpenJogDialog(CWnd* pParent = NULL);   // �зǫغc�禡
	virtual ~COpenJogDialog();
	CRTEXFormView* m_parent;
// ��ܤ�����
	enum { IDD = IDD_JOGDIALOG };

protected:
	CWinThread* m_GetPostionthread;
	bool m_fgStopGetPos;
	static UINT GetPosthreadFun(LPVOID LParam);
	
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
protected:
	afx_msg LRESULT OnSendjogdialogupdatepos(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRadioClicked(UINT uiID);
	afx_msg void OnUPDWNClicked(UINT uiID);
public:
	afx_msg void OnEnKillfocusEditJogOffset(UINT uiID);
	afx_msg void OnEnKillfocusEditJogSpeed();
};

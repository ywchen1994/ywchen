#pragma once
#include "RTEXFormView.h"
#include "afxwin.h"

// CSystemID 對話方塊
const unsigned char ucCmdType[] = {0x01,0x05,0x06,0x12,0x13,0x14,0x15,0x22,0x23};
class CSystemID : public CDialogEx
{
	DECLARE_DYNAMIC(CSystemID)

public:
	CSystemID(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CSystemID();

// 對話方塊資料
	enum { IDD = IDD_SystemID };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	CRTEXFormView* m_parent;
	DECLARE_MESSAGE_MAP()
public:
	UINT m_uiSelectedSlaves;
	afx_msg void OnBnClickedCheckMacid();
	virtual BOOL OnInitDialog();
	CComboBox m_CtrlCmdSet;
	afx_msg void OnBnClickedButtonExecutecmd();
	CListBox m_ctrllistShowdata;
};

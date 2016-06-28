#pragma once
#include "RTEXFormView.h"
#include "afxwin.h"

// CDriverRegister 對話方塊
const unsigned char uctypecode[] ={0x01  ,0x02      ,0x04  ,0x05  ,0x06 ,0x07  ,0x08  ,0x09   ,0x0A   ,0x0c  ,0x0d};
const CString  strTypecode[] =    {"PERR","Enc Rev" ,"MPOS","ASPD","TRQ","APOS","IPOS","LPOS1","LPOS2","MSPD","EXPOS"};


class CDriverRegister : public CDialogEx
{
	DECLARE_DYNAMIC(CDriverRegister)

public:
	CDriverRegister(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CDriverRegister();

// 對話方塊資料
	enum { IDD = IDD_DriverRegDialog };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	CRTEXFormView* m_parent;
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox m_ctrlAxisMacID;
	afx_msg void OnBnClickedButtonReadRegister();
	afx_msg void OnBnClickedButtonWriteRegister();
	afx_msg void OnBnClickedButtonEepromWriteRegister();
	afx_msg void OnClose();
	CComboBox m_ctrlTypecode;
	afx_msg void OnBnClickedButtonMonitor();
};

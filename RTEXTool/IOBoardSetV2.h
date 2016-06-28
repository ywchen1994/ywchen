#pragma once
#include "RTEXFormView.h"
#include "afxcmn.h"
#include "afxwin.h"

// CIOBoardSetV2 對話方塊
const UINT IOBtnDlgItemNum[32]={
		    IDC_CHECK_BIT0, IDC_CHECK_BIT1, IDC_CHECK_BIT2, IDC_CHECK_BIT3, IDC_CHECK_BIT4, IDC_CHECK_BIT5, IDC_CHECK_BIT6, IDC_CHECK_BIT7,
			IDC_CHECK_BIT8, IDC_CHECK_BIT9, IDC_CHECK_BIT10, IDC_CHECK_BIT11, IDC_CHECK_BIT12, IDC_CHECK_BIT13, IDC_CHECK_BIT14, IDC_CHECK_BIT15,
			IDC_CHECK_BIT16, IDC_CHECK_BIT17, IDC_CHECK_BIT18, IDC_CHECK_BIT19, IDC_CHECK_BIT20, IDC_CHECK_BIT21, IDC_CHECK_BIT22, IDC_CHECK_BIT23,
			IDC_CHECK_BIT24, IDC_CHECK_BIT25, IDC_CHECK_BIT26, IDC_CHECK_BIT27, IDC_CHECK_BIT28, IDC_CHECK_BIT29, IDC_CHECK_BIT30, IDC_CHECK_BIT31
	};
class CIOBoardSetV2 : public CDialogEx
{
	DECLARE_DYNAMIC(CIOBoardSetV2)

public:
	CIOBoardSetV2(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CIOBoardSetV2();
	CRTEXFormView* m_parent;

// 對話方塊資料
	enum { IDD = IDD_IOBoardSetV2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CTabCtrl m_ctrlIOTabctrl;
	int m_icurTabSel;
	afx_msg void OnTcnSelchangeTabSwitchioboard(NMHDR *pNMHDR, LRESULT *pResult);
	void SetDlgitemStatus(int icurSel);
	CComboBox m_ctrlMacID;
	BOOL m_fgenableIO_borad;
	BOOL m_fgInput;
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClose();
	afx_msg void OnBnClickedCheckEnableio();
	afx_msg void OnCbnSelchangeComboIoMacid();
	void UpdateIOstatus(int icurSelIO);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCHKClicked0_31(UINT uiCmdID);
	afx_msg void OnRadioBtnClickedIO(UINT uiCmdID);
	bool fgboard0btnChg;
	
	CListCtrl m_ctrlListctrl;
	void ListCtrlInit(void);
	CFont ListFont;
	void ListCtrlInforUpdate(void);
	void UpdateIOstatusInList(void);

	int CurTabSel2IOdeviceSeq(int icurTabSel);
};

// SystemID.cpp : 實作檔
//

#include "stdafx.h"
#include "RTEXTool.h"
#include "SystemID.h"
#include "afxdialogex.h"
#include "RTEXFormView.h"

// CSystemID 對話方塊

IMPLEMENT_DYNAMIC(CSystemID, CDialogEx)

CSystemID::CSystemID(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSystemID::IDD, pParent)
	, m_uiSelectedSlaves(0)
{
	m_parent = (CRTEXFormView*)pParent;
}

CSystemID::~CSystemID()
{
}

void CSystemID::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CMD, m_CtrlCmdSet);
	DDX_Control(pDX, IDC_LIST_Show_data, m_ctrllistShowdata);
}


BEGIN_MESSAGE_MAP(CSystemID, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_MaCID_0, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_1, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_2, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_3, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_4, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_5, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_6, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_7, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_8, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_9, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_10, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_11, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_12, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_13, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_14, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_15, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_16, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_17, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_18, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_19, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_20, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_21, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_22, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_23, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_24, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_25, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_26, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_27, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_28, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_29, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_30, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_CHECK_MaCID_31, &CSystemID::OnBnClickedCheckMacid)
	ON_BN_CLICKED(IDC_Button_ExecuteCmd, &CSystemID::OnBnClickedButtonExecutecmd)
END_MESSAGE_MAP()


// CSystemID 訊息處理常式

BOOL CSystemID::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此加入額外的初始化
	if(m_parent)
		m_uiSelectedSlaves = m_parent->m_uiSelectedSlaves;
	for (UINT i = 0 ; i < 32 ; i++)
	{
		CButton* pbtn=(CButton*)GetDlgItem(macID[i]);
		pbtn->SetCheck(FALSE);
		if(m_uiSelectedSlaves & (1 << i))
		{
			pbtn->SetCheck(TRUE);
		}
		else
		{
            pbtn->EnableWindow(FALSE);
		}
	}
	m_CtrlCmdSet.ResetContent();
	m_CtrlCmdSet.AddString("Vendor name");
	m_CtrlCmdSet.AddString("Device type");
	m_CtrlCmdSet.AddString("Manufacturer use ");
	m_CtrlCmdSet.AddString("Driver model No.");
	m_CtrlCmdSet.AddString("Driver serial No.");
	m_CtrlCmdSet.AddString("Servo Driver Software Version");
	m_CtrlCmdSet.AddString("Driver Type");
	m_CtrlCmdSet.AddString("Motor model No.");
	m_CtrlCmdSet.AddString("Motor serial No.");  //8	
	m_CtrlCmdSet.SetCurSel(0);
	m_ctrllistShowdata.ResetContent();

	    OnBnClickedCheckMacid();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CSystemID::OnBnClickedCheckMacid()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	m_uiSelectedSlaves = 0;
	for (UINT i = 0 ; i< 32 ; i++)
	{
		CButton* pbtn=(CButton*)GetDlgItem(macID[i]);
		if(pbtn->GetCheck())
		{
			m_uiSelectedSlaves += (1 << i);
		}
	}
	TRACE("selcect slave = %u\n",m_uiSelectedSlaves);
	CString str;
	str.Format("0x%08X",m_uiSelectedSlaves);
	SetDlgItemText(IDC_EDIT_SelectedSlaves,str);
}
void CSystemID::OnBnClickedButtonExecutecmd()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	TRACE("m_uiSelectedSlaves = %08X, type = 0x%02X\n",m_uiSelectedSlaves, ucCmdType[m_CtrlCmdSet.GetCurSel()]);
    m_parent->RTEX_CMD_SystemID(m_uiSelectedSlaves,ucCmdType[m_CtrlCmdSet.GetCurSel()]);	
	m_parent->SetWaitCMDAcking(1);
	if(m_parent->WaitCMDAcking())
	{
		MessageBox("CMD Fail");
		return;
	}
	CString strtype;
	int n = m_CtrlCmdSet.GetLBTextLen(m_CtrlCmdSet.GetCurSel());
	m_CtrlCmdSet.GetLBText(m_CtrlCmdSet.GetCurSel(),strtype.GetBuffer(n));
	strtype.ReleaseBuffer();
	CString str,str1;
	TRACE("strtype : %s\n",strtype);
	unsigned char* pdata;
	pdata = m_parent->m_ucSystemIDreturndata;
	for(int i = 6 ; i < pdata[5]+6; i++)
	{
		if((i % 17) == 6)
		{
			TRACE("macid = %d\n",pdata[i]);
			str.Format("show %s of MAC ID %d:",strtype,pdata[i]);
				m_ctrllistShowdata.AddString(str);
				str="";
		}
		else
		{
			//TRACE("%c\n",pdata[i]);
			//TRACE("%d ",i);
			str1.Format("%c",pdata[i]);
			str += pdata[i];
		}
		if((i % 17) == 5) m_ctrllistShowdata.AddString(str);;
	}
	//TRACE("show\n");
	//TRACE(str);
	m_ctrllistShowdata.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
}

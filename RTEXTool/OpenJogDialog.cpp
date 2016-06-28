// OpenJogDialog.cpp : 實作檔
//

#include "stdafx.h"
#include "RTEXTool.h"
#include "OpenJogDialog.h"
#include "afxdialogex.h"


// COpenJogDialog 對話方塊

IMPLEMENT_DYNAMIC(COpenJogDialog, CDialogEx)

COpenJogDialog::COpenJogDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(COpenJogDialog::IDD, pParent)
	, m_GetPostionthread(nullptr)
	, m_fgStopGetPos(FALSE)
{
	m_parent = (CRTEXFormView*)pParent;
}

COpenJogDialog::~COpenJogDialog()
{
}

void COpenJogDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COpenJogDialog, CDialogEx)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_SendJogDialogUpdatePos, &COpenJogDialog::OnSendjogdialogupdatepos)
    ON_CONTROL_RANGE(BN_CLICKED,IDC_RADIO_XYZYAW,IDC_RADIO_J1J2ZJ4,OnRadioClicked)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_BUTTON_Para1,IDC_BUTTON_Para8,OnUPDWNClicked)
	ON_CONTROL_RANGE(EN_KILLFOCUS,IDC_EDIT_Jog_Offset1,IDC_EDIT_Jog_Offset4,OnEnKillfocusEditJogOffset)
	//ON_EN_KILLFOCUS(IDC_EDIT_Jog_Offset, &COpenJogDialog::OnEnKillfocusEditJogOffset)
	ON_EN_KILLFOCUS(IDC_EDIT_Jog_Speed, &COpenJogDialog::OnEnKillfocusEditJogSpeed)
END_MESSAGE_MAP()


// COpenJogDialog 訊息處理常式


void COpenJogDialog::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	m_fgStopGetPos = TRUE;
	if(::WaitForSingleObject(m_GetPostionthread->m_hThread,1000) == WAIT_TIMEOUT)
	{
		TRACE("timeout wait GetPos thread over\n");
	}
	CDialogEx::OnClose();
}
void COpenJogDialog::OnOK()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	m_fgStopGetPos = TRUE;
	if(::WaitForSingleObject(m_GetPostionthread->m_hThread,1000) == WAIT_TIMEOUT)
	{
		TRACE("timeout wait GetPos thread over\n");
	}
	CDialogEx::OnOK();
}
void COpenJogDialog::OnCancel()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	m_fgStopGetPos = TRUE;
	if(::WaitForSingleObject(m_GetPostionthread->m_hThread,1000) == WAIT_TIMEOUT)
	{
		TRACE("timeout wait GetPos thread over\n");
	}
	CDialogEx::OnCancel();
}


BOOL COpenJogDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_GetPostionthread = AfxBeginThread(&COpenJogDialog::GetPosthreadFun, (LPVOID)this);
	// TODO:  在此加入額外的初始化
	CButton* pbtn = (CButton*)GetDlgItem(IDC_RADIO_J1J2ZJ4);
	pbtn->SetCheck(1);
	OnRadioClicked(IDC_RADIO_J1J2ZJ4);
	SetDlgItemText(IDC_EDIT_Jog_Offset1,"5");
	SetDlgItemText(IDC_EDIT_Jog_Offset2,"5");
	SetDlgItemText(IDC_EDIT_Jog_Offset3,"5");
	SetDlgItemText(IDC_EDIT_Jog_Offset4,"5");
	SetDlgItemText(IDC_EDIT_Jog_Speed,"10");
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}
UINT COpenJogDialog::GetPosthreadFun(LPVOID LParam)
{
	COpenJogDialog* pdialog = (COpenJogDialog*)LParam;
    //pdialog->GetPos();
	while(!(pdialog->m_fgStopGetPos))
	{
		Sleep(100);
		::PostMessage(pdialog->m_hWnd,WM_SendJogDialogUpdatePos,0,0);
	}
	return 0x55AA;
}
afx_msg LRESULT COpenJogDialog::OnSendjogdialogupdatepos(WPARAM wParam, LPARAM lParam)
{
	CString str;
	CButton* pbtn = (CButton*)GetDlgItem(IDC_RADIO_XYZYAW);
	if(pbtn->GetCheck())
	{
		str.Format("%.3f",m_parent->m_fx);
		SetDlgItemText(IDC_EDIT_Jog_Para1,str);
		str.Format("%.3f",m_parent->m_fy);
		SetDlgItemText(IDC_EDIT_Jog_Para2,str);
		str.Format("%.3f",m_parent->m_fz);
		SetDlgItemText(IDC_EDIT_Jog_Para3,str);
		str.Format("%.3f",m_parent->m_fYaw);
		SetDlgItemText(IDC_EDIT_Jog_Para4,str);
	}
	else
	{
		str.Format("%.3f",m_parent->m_ftheta1);
		SetDlgItemText(IDC_EDIT_Jog_Para1,str);
		str.Format("%.3f",m_parent->m_ftheta2);
		SetDlgItemText(IDC_EDIT_Jog_Para2,str);
		str.Format("%.3f",m_parent->m_fz);
		SetDlgItemText(IDC_EDIT_Jog_Para3,str);
		str.Format("%.3f",m_parent->m_ftheta4);
		SetDlgItemText(IDC_EDIT_Jog_Para4,str);
	}

	return 0;
}
void COpenJogDialog::OnRadioClicked(UINT uiID)
{
	TRACE("radio button click\n");
	CButton* pbtn = (CButton*)GetDlgItem(IDC_RADIO_XYZYAW);
	if(pbtn->GetCheck())
	{
		SetDlgItemText(IDC_STATIC_Para1,"X");
		SetDlgItemText(IDC_STATIC_Para2,"Y");
		SetDlgItemText(IDC_STATIC_Para3,"Z");
		SetDlgItemText(IDC_STATIC_Para4,"YAW");
	}
	else
	{
		SetDlgItemText(IDC_STATIC_Para1,"Theta1");
		SetDlgItemText(IDC_STATIC_Para2,"Theta2");
		SetDlgItemText(IDC_STATIC_Para3,"Z");
		SetDlgItemText(IDC_STATIC_Para4,"Theta4");
	}
	OnSendjogdialogupdatepos(0,0);
}

void COpenJogDialog::OnEnKillfocusEditJogOffset(UINT uiID)
{
	// TODO: 在此加入控制項告知處理常式程式碼
	TRACE("kill focus\n");
	float fvalue =0;
	CString str;
	GetDlgItemText(uiID,str);
	fvalue = (float)atof(str);
	if(fvalue > 600) fvalue = (float)600;
	if(fvalue < 0.005)   fvalue = (float)0.05;
	str.Format("%.3f",fvalue);
	SetDlgItemText(uiID,str);
	
}


void COpenJogDialog::OnEnKillfocusEditJogSpeed()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	UINT uivalue = 0;
	uivalue = GetDlgItemInt(IDC_EDIT_Jog_Speed,NULL,0);
	if(uivalue > 100) uivalue = 100;
	if(uivalue < 1)   uivalue = 1;
	SetDlgItemInt(IDC_EDIT_Jog_Speed,uivalue,0);
}
void COpenJogDialog::OnUPDWNClicked(UINT uiID)
{
	CButton* pbtn = (CButton*)GetDlgItem(IDC_RADIO_XYZYAW);
	float Para[4];
	CString str;
	UINT uibtnNumber =(uiID-IDC_BUTTON_Para1);
	TRACE("button = %d\n",uibtnNumber);
	GetDlgItemText(IDC_EDIT_Jog_Speed,str);
	float fJogspd = (float)atof((LPCSTR)str);
	while(m_parent->m_WaitCmdAck);
	m_parent->SetWaitCMDAcking(1);
	m_parent->RTEX_CMD_Set_Jog_Speed(fJogspd);	
	if(m_parent->WaitCMDAcking())
	{
		MessageBox("can't set Jog Speed");
		return;
	}
	//return;
	
	FLOAT fJogoffset[4];
	for(int i = 0; i < 4 ; i++)
	{
		GetDlgItemText(IDC_EDIT_Jog_Offset1 +i,str);
		fJogoffset[i] = (float)atof((LPCSTR)str);
		//TRACE("Ofst = %.3f\n",fJogoffset[i]);
	}
	GetDlgItemText(IDC_EDIT_Jog_Para1,str);
	Para[0] = (float)atof((LPCSTR)str);
	GetDlgItemText(IDC_EDIT_Jog_Para2,str);
	Para[1] = (float)atof((LPCSTR)str);
	GetDlgItemText(IDC_EDIT_Jog_Para3,str);
	Para[2] = (float)atof((LPCSTR)str);
	GetDlgItemText(IDC_EDIT_Jog_Para4,str);
	Para[3] = (float)atof((LPCSTR)str);
	if(uibtnNumber >= 4)
	{
		uibtnNumber -=4;
		for(int i = 0 ; i < 4 ; i++)		fJogoffset[i] = -fJogoffset[i]; 
	}
	Para[uibtnNumber] += fJogoffset[uibtnNumber];	
	while(m_parent->m_WaitCmdAck);
	m_parent->SetWaitCMDAcking(1);
	if(pbtn->GetCheck())	m_parent->RTEX_CMD_SET_XYZ_MotionJOG(Para[0],Para[1],Para[2],Para[3]);
	else  m_parent->RTEX_CMD_SET_J1234_MotionJOG(Para[0],Para[1],Para[2],Para[3]);
	if(m_parent->WaitCMDAcking())
	{
		MessageBox("can't set Jog motion");
		return;
	}
}

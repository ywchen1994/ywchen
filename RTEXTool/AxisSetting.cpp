// AxisSetting.cpp : 實作檔
//

#include "stdafx.h"
#include "RTEXTool.h"
#include "AxisSetting.h"
#include "afxdialogex.h"


// CAxisSetting 對話方塊

IMPLEMENT_DYNAMIC(CAxisSetting, CDialogEx)

CAxisSetting::CAxisSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAxisSetting::IDD, pParent)
	, m_fVmax(0)
	, m_uiTa(0)
	, m_uiEnableAxis(0)
	,m_CurSelTab(0)
	, m_intcmbxSpdType(0)
	, m_boolRTEX_Configurated(false)
	, m_uiShowPosTimeSlot(0)
	, m_fScaraRobot_L1(0)
	, m_fScaraRobot_L2(0)
	, m_fScaraRobot_Height(0)
	, m_iScaraRobot_Arm_dir(0)
	, m_uiCheckErrTimeSlot(0)
	, m_iJog_MaxRPM(0)
	, m_fJog_Speed(0)
	, m_HomeJ1(0)
	, m_HomeJ2(0)
	, m_HomeJ3(0)
	, m_HomeJ4(0)
{
	for(int i = 0 ; i < 6; i++)
	{
		m_MacID_axis[i] = i +12 ;
		m_Axis_Pitch[i] = 1;
		m_Axis_GearRatio[i] = 10000;
		m_Axis_EncoderOffset[i] = 0;
	}
        m_parent = (CRTEXFormView*)pParent;
}

CAxisSetting::~CAxisSetting()
{
}

void CAxisSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_Vmax, m_fVmax);
	DDX_Text(pDX, IDC_EDIT_Ta, m_uiTa);
	DDX_Control(pDX, IDC_COMBO_SpdType, m_ctrlSpdType);
	DDX_Text(pDX, IDC_EDIT_EnableAxis, m_uiEnableAxis);	
	DDX_Control(pDX, IDC_TAB_SwitchAxis, m_ctrlSwitchAxis);
	DDX_CBIndex(pDX, IDC_COMBO_SpdType, m_intcmbxSpdType);	
	DDX_Text(pDX, IDC_EDIT_ShowPosTimeSlot, m_uiShowPosTimeSlot);
	DDX_Text(pDX, IDC_EDIT_L1, m_fScaraRobot_L1);
	DDX_Text(pDX, IDC_EDIT_L2, m_fScaraRobot_L2);
	DDX_Text(pDX, IDC_EDIT_Height, m_fScaraRobot_Height);
	DDX_CBIndex(pDX, IDC_COMBO_Arm_Type, m_iScaraRobot_Arm_dir);
	DDX_Control(pDX, IDC_COMBO_Arm_Type, m_ctrlArm_Type);
	DDX_Text(pDX, IDC_EDIT_ChkErrTimeSlot, m_uiCheckErrTimeSlot);
	DDX_Text(pDX, IDC_EDIT_Jog_MaxRPM, m_iJog_MaxRPM);
	DDX_Text(pDX, IDC_EDIT_Jog_Speed, m_fJog_Speed);
	DDX_Text(pDX, IDC_EDIT_HomeJ1, m_HomeJ1);
	DDX_Text(pDX, IDC_EDIT_HomeJ2, m_HomeJ2);
	DDX_Text(pDX, IDC_EDIT_HomeJ3, m_HomeJ3);
	DDX_Text(pDX, IDC_EDIT_HomeJ4, m_HomeJ4);

}


BEGIN_MESSAGE_MAP(CAxisSetting, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SwitchAxis, &CAxisSetting::OnTcnSelchangeTabSwitchaxis)
	ON_BN_CLICKED(IDC_BUTTON_Modify_Spd_Parameter, &CAxisSetting::OnBnClickedButtonModifySpdParameter)
	ON_BN_CLICKED(IDC_BUTTON_Modify_Axis_Parameter, &CAxisSetting::OnBnClickedButtonModifyAxisParameter)
	ON_BN_CLICKED(IDC_BUTTON_Set_Scara_Mech, &CAxisSetting::OnBnClickedButtonSetScaraMech)
	ON_BN_CLICKED(IDC_BUTTON_Jog_MaxRPM, &CAxisSetting::OnBnClickedButtonJogMaxrpm)
	ON_BN_CLICKED(IDC_BUTTON_Jog_Speed, &CAxisSetting::OnBnClickedButtonJogSpeed)
	ON_BN_CLICKED(IDC_BUTTON_HomePointChange, &CAxisSetting::OnBnClickedButtonHomepointchange)
END_MESSAGE_MAP()


// CAxisSetting 訊息處理常式


BOOL CAxisSetting::OnInitDialog()
{
	//The framework automatically calls UpdateData(FALSE)
	//when a modal dialog box is created in the default implementation of CDialog::OnInitDialog. 
	CDialogEx::OnInitDialog();

	// TODO:  在此加入額外的初始化
	if(m_parent->m_boolRTEX_Configurated)
		GetDlgItem(IDC_BUTTON_Modify_Axis_Parameter)->ShowWindow(TRUE);
	else
		GetDlgItem(IDC_BUTTON_Modify_Axis_Parameter)->ShowWindow(FALSE);
	m_ctrlSpdType.ResetContent();
	m_ctrlSpdType.AddString("T Curve Type");
	m_ctrlSpdType.AddString("S Curve Type");
	m_ctrlArm_Type.ResetContent();
	m_ctrlArm_Type.AddString("Arm_R");
	m_ctrlArm_Type.AddString("Arm_L");
	//m_ctrlSpdType.SetCurSel(1);
	//m_intcmbxSpdType = 1;
	//m_fVmax = 100;
	//m_uiTa = 500;
	UpdateData(FALSE);
	m_ctrlSwitchAxis.InsertItem(0, "axis X");
	m_ctrlSwitchAxis.InsertItem(1, "axis Y");
	m_ctrlSwitchAxis.InsertItem(2, "axis Z");
	m_ctrlSwitchAxis.InsertItem(3, "axis U");
	m_ctrlSwitchAxis.InsertItem(4, "axis V");
	m_ctrlSwitchAxis.InsertItem(5, "axis W");

	for(int i = 0 ; i< 6 ;i++)
	{
	  m_page[i].Create(IDD_OneAxisSetting, &m_ctrlSwitchAxis);
	}

	CRect rc;
	m_ctrlSwitchAxis.GetClientRect(rc);
	rc.top += 20;
	rc.bottom -= 0;
	rc.left += 0;
	rc.right -= 0;
	for(UINT i = 0 ; i< 6 ;i++)	
	{
		m_page[i].MoveWindow(&rc);		
		m_page[i].m_ctrlSelectedMacID.SetCurSel(12+i);
		m_page[i].m_axis = i;
		m_page[i].m_ctrlSelectedMacID.SetCurSel(m_MacID_axis[i]);
		m_page[i].m_Pitch = m_Axis_Pitch[i];
		m_page[i].m_GearRatio = m_Axis_GearRatio[i];
		m_page[i].m_POTLimit = m_Axis_POTLimit[i];
		m_page[i].m_NOTLimit = m_Axis_NOTLimit[i];
		m_page[i].m_GearRatio_Mech = m_Axis_GearRatio_Mech[i];
		m_page[i].m_EncoderOffset = m_Axis_EncoderOffset[i];
		m_page[i].UpdateData(FALSE);
		pDialog[i] = &m_page[i];
		pDialog[i]->ShowWindow(SW_HIDE);
		if(m_Axis_Type[i] == 0)
		{
			m_page[i].GetDlgItem(IDC_EDIT_Type)->SetWindowText("R to R");
			m_page[i].GetDlgItem(IDC_STATIC_POTLimit_Unit)->SetWindowText("deg");
			m_page[i].GetDlgItem(IDC_STATIC_NOTLimit_Unit)->SetWindowText("deg");
		}
		else if (m_Axis_Type[i] == 1)
		{
			m_page[i].GetDlgItem(IDC_EDIT_Type)->SetWindowText("R to L");
			m_page[i].GetDlgItem(IDC_STATIC_POTLimit_Unit)->SetWindowText("mm");
			m_page[i].GetDlgItem(IDC_STATIC_NOTLimit_Unit)->SetWindowText("mm");
		}
		if(m_boolRTEX_Configurated)
		{
			GetDlgItem(IDC_EDIT_EnableAxis)->EnableWindow(FALSE);
			m_page[i].m_ctrlSelectedMacID.EnableWindow(FALSE);
		}
		else
		{
			GetDlgItem(IDC_EDIT_EnableAxis)->EnableWindow(TRUE);
			m_page[i].m_ctrlSelectedMacID.EnableWindow(TRUE);
		}

		if((m_boolRTEX_Configurated) && (i < m_uiEnableAxis))
		{
			m_page[i].GetDlgItem(IDC_STATIC_Pitch)->ShowWindow(TRUE);
			m_page[i].GetDlgItem(IDC_STATIC_GearRatio)->ShowWindow(TRUE);
			m_page[i].GetDlgItem(IDC_STATIC_GearRatio_Mech)->ShowWindow(TRUE);
			m_page[i].GetDlgItem(IDC_STATIC_Pitch_Uint)->ShowWindow(TRUE);
			m_page[i].GetDlgItem(IDC_STATIC_GearRatio_Unit)->ShowWindow(TRUE);
			m_page[i].GetDlgItem(IDC_EDIT_Pitch)->ShowWindow(TRUE);
			m_page[i].GetDlgItem(IDC_EDIT_GearRatio)->ShowWindow(TRUE);
			m_page[i].GetDlgItem(IDC_EDIT_GearRatio_Mech)->ShowWindow(TRUE);
			m_page[i].GetDlgItem(IDC_STATIC_POTLimit)->ShowWindow(TRUE);
			m_page[i].GetDlgItem(IDC_STATIC_POTLimit_Unit)->ShowWindow(TRUE);
			m_page[i].GetDlgItem(IDC_EDIT_POTLimit)->ShowWindow(TRUE);
			m_page[i].GetDlgItem(IDC_STATIC_NOTLimit)->ShowWindow(TRUE);
			m_page[i].GetDlgItem(IDC_STATIC_NOTLimit_Unit)->ShowWindow(TRUE);
			m_page[i].GetDlgItem(IDC_EDIT_NOTLimit)->ShowWindow(TRUE);
			m_page[i].GetDlgItem(IDC_STATIC_Type)->ShowWindow(TRUE);
			m_page[i].GetDlgItem(IDC_EDIT_Type)->ShowWindow(TRUE);
			m_page[i].GetDlgItem(IDC_STATIC_EncoderOfst)->ShowWindow(TRUE);
			m_page[i].GetDlgItem(IDC_EDIT_EncoderOfst)->ShowWindow(TRUE);
		}
		else
		{		
			m_page[i].GetDlgItem(IDC_STATIC_Pitch)->ShowWindow(FALSE);
			m_page[i].GetDlgItem(IDC_STATIC_GearRatio)->ShowWindow(FALSE);
			m_page[i].GetDlgItem(IDC_STATIC_GearRatio_Mech)->ShowWindow(FALSE);
			m_page[i].GetDlgItem(IDC_STATIC_Pitch_Uint)->ShowWindow(FALSE);
			m_page[i].GetDlgItem(IDC_STATIC_GearRatio_Unit)->ShowWindow(FALSE);
			m_page[i].GetDlgItem(IDC_EDIT_Pitch)->ShowWindow(FALSE);
			m_page[i].GetDlgItem(IDC_EDIT_GearRatio)->ShowWindow(FALSE);
			m_page[i].GetDlgItem(IDC_EDIT_GearRatio_Mech)->ShowWindow(FALSE);
			m_page[i].GetDlgItem(IDC_STATIC_POTLimit)->ShowWindow(FALSE);
			m_page[i].GetDlgItem(IDC_STATIC_POTLimit_Unit)->ShowWindow(FALSE);
			m_page[i].GetDlgItem(IDC_EDIT_POTLimit)->ShowWindow(FALSE);
			m_page[i].GetDlgItem(IDC_STATIC_NOTLimit)->ShowWindow(FALSE);
			m_page[i].GetDlgItem(IDC_STATIC_NOTLimit_Unit)->ShowWindow(FALSE);
			m_page[i].GetDlgItem(IDC_EDIT_NOTLimit)->ShowWindow(FALSE);
			m_page[i].GetDlgItem(IDC_STATIC_Type)->ShowWindow(FALSE);
			m_page[i].GetDlgItem(IDC_EDIT_Type)->ShowWindow(FALSE);
			m_page[i].GetDlgItem(IDC_STATIC_EncoderOfst)->ShowWindow(FALSE);
			m_page[i].GetDlgItem(IDC_EDIT_EncoderOfst)->ShowWindow(FALSE);
		}

	}
	pDialog[0]->ShowWindow(SW_SHOW);
	m_CurSelTab = 0;

	if(m_parent->m_fgScaraRobot)
	{
		for (int i = 0 ; i < (sizeof(uiScaraParameter) / sizeof(uiScaraParameter[0])); i++)
		{
			GetDlgItem(uiScaraParameter[i])->ShowWindow(TRUE);
			if(m_parent->m_boolRTEX_Configurated)
				GetDlgItem(uiScaraParameter[i])->EnableWindow(FALSE);
			else
				GetDlgItem(uiScaraParameter[i])->EnableWindow(TRUE);
		}
		GetDlgItem(IDC_COMBO_Arm_Type)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_Arm_Type)->ShowWindow(FALSE);
	}
	else
	{
		for (int i = 0 ; i < (sizeof(uiScaraParameter) / sizeof(uiScaraParameter[0])); i++)
			GetDlgItem(uiScaraParameter[i])->ShowWindow(FALSE);

	}
	if(m_parent->m_boolRTEX_Configurated)
	{
		GetDlgItem(IDC_STATIC_Jog_MaxRPM)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_Jog_MaxRPM)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_Unit_Jog_MaxRPM)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_Jog_MaxRPM)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_STATIC_Jog_MaxRPM)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_Jog_MaxRPM)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC_Unit_Jog_MaxRPM)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_Jog_MaxRPM)->EnableWindow(TRUE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}


void CAxisSetting::OnTcnSelchangeTabSwitchaxis(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此加入控制項告知處理常式程式碼
	pDialog[m_CurSelTab]->ShowWindow(SW_HIDE);
	m_CurSelTab = m_ctrlSwitchAxis.GetCurSel();
	pDialog[m_CurSelTab]->ShowWindow(SW_SHOW);
	*pResult = 0;
}


void CAxisSetting::OnOK()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別


	for (int i = 0; i < 6 ; i++)
	{
		m_page[i].UpdateData(TRUE);
		//TRACE("maid[%d]= %d  ",i, m_MacID_axis[i]);

	}
	//TRACE("\n");
    //TRACE("axis number = %d\n ",m_uiEnableAxis);
	UpdateData(TRUE);
	CDialogEx::OnOK();
}


void CAxisSetting::OnBnClickedButtonModifySpdParameter()
{
	// TODO: 在此加入控制項告知處理常式程式碼
    UpdateData(TRUE);
	m_parent->m_intcmbxSpdType = m_intcmbxSpdType;
	m_parent->m_fVmax = m_fVmax;
	m_parent->m_uiTa = m_uiTa;
	m_parent->Setspeedparameter();
}
void CAxisSetting::OnBnClickedButtonModifyAxisParameter()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	for(UINT i = 0 ; i < m_uiEnableAxis; i++)
	{
		m_page[i].UpdateData(TRUE);
		m_parent->m_Axis_Pitch[i] = m_page[i].m_Pitch;
		m_parent->m_Axis_GearRatio[i] = m_page[i].m_GearRatio;
		m_parent->m_Axis_POTLimit[i] = m_page[i].m_POTLimit;
		m_parent->m_Axis_NOTLimit[i] = m_page[i].m_NOTLimit;
		m_parent->m_Axis_GearRatio_Mech[i] = m_page[i].m_GearRatio_Mech;
		m_parent->m_Axis_Encoder_OFS[i] = m_page[i].m_EncoderOffset;
	}
	m_parent->Setaxisparameter();
}


void CAxisSetting::OnBnClickedButtonSetScaraMech()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	UpdateData(TRUE);
	m_parent->RTEX_CMD_SET_Scara_Robot_mech(m_fScaraRobot_L1,m_fScaraRobot_L2,m_fScaraRobot_Height);

	m_parent->SetWaitCMDAcking(1);
	if(m_parent->WaitCMDAcking())
	{	
		MessageBox("RTEX_SetDRIVER_NON_CYCLINC_MODE mode 0 NG");
		return;
	}	
	m_parent->m_fScaraRobot_L1 = m_fScaraRobot_L1;
	m_parent->m_fScaraRobot_L2 = m_fScaraRobot_L2;
	m_parent->m_fScaraRobot_Height = m_fScaraRobot_Height;
	//m_parent->m_ucScaraRobot_Arm_dir = (unsigned char)m_iScaraRobot_Arm_dir;
}


void CAxisSetting::OnBnClickedButtonJogMaxrpm()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	UpdateData(TRUE);
	if(m_iJog_MaxRPM <= 0)
	{
		MessageBox("請設置適當的Max RPM");
		return;
	}
	CString str;
	str.Format("Set Max RPM of JOG Mode = % d RPM ",m_iJog_MaxRPM);
	MessageBox(str);
	m_parent->RTEX_CMD_Set_Jog_MaxRPM(m_iJog_MaxRPM);
	m_parent->SetWaitCMDAcking(1);
	if(m_parent->WaitCMDAcking())
	{	
		MessageBox("Set Jog Max RPM NG");
		return;
	}	
	m_parent->m_iJog_MaxRPM = m_iJog_MaxRPM;
}


void CAxisSetting::OnBnClickedButtonJogSpeed()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	UpdateData(TRUE);
	if((m_fJog_Speed < 0) || (m_fJog_Speed >100))
	{
		MessageBox("請設置適當的速度百分比");
		return;
	}

	CString str;
	str.Format("Set Speed of JOG Mode = %.2f %% ",m_fJog_Speed);
	MessageBox(str);
	m_parent->RTEX_CMD_Set_Jog_Speed(m_fJog_Speed);
	m_parent->SetWaitCMDAcking(1);
	if(m_parent->WaitCMDAcking())
	{	
		MessageBox("Set Jog Speed NG");
		return;
	}	
	m_parent->m_fJog_Speed = m_fJog_Speed;

}


void CAxisSetting::OnBnClickedButtonHomepointchange()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	UpdateData(TRUE);
		if((m_HomeJ1 * m_HomeJ2) > 0)
		{
			MessageBox("第一軸與第二軸最好一正一負");
			m_HomeJ2 = -m_HomeJ2;
		}
   UpdateData(FALSE);
   m_parent->m_HomeJ[0] = m_HomeJ1;
   m_parent->m_HomeJ[1] = m_HomeJ2;
   m_parent->m_HomeJ[2] = m_HomeJ3;
   m_parent->m_HomeJ[3] = m_HomeJ4;
   TRACE("J1 = %.2f, J2 = %.2f, J3 = %.2f , J4 = %.2f\n",m_parent->m_HomeJ[0],m_parent->m_HomeJ[1],m_parent->m_HomeJ[2],m_parent->m_HomeJ[3]);
}

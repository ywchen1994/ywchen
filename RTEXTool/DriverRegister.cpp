// DriverRegister.cpp : ��@��
//

#include "stdafx.h"
#include "RTEXTool.h"
#include "DriverRegister.h"
#include "afxdialogex.h"


// CDriverRegister ��ܤ��

IMPLEMENT_DYNAMIC(CDriverRegister, CDialogEx)

CDriverRegister::CDriverRegister(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDriverRegister::IDD, pParent)
{
	m_parent = (CRTEXFormView*)pParent;
}

CDriverRegister::~CDriverRegister()
{
}

void CDriverRegister::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_Axis_Mac_ID, m_ctrlAxisMacID);
	DDX_Control(pDX, IDC_COMBO_TypeCode, m_ctrlTypecode);
}


BEGIN_MESSAGE_MAP(CDriverRegister, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_Read_Register, &CDriverRegister::OnBnClickedButtonReadRegister)
	ON_BN_CLICKED(IDC_BUTTON_Write_Register, &CDriverRegister::OnBnClickedButtonWriteRegister)
	ON_BN_CLICKED(IDC_BUTTON_EEPROM_Write_Register, &CDriverRegister::OnBnClickedButtonEepromWriteRegister)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_Monitor, &CDriverRegister::OnBnClickedButtonMonitor)
END_MESSAGE_MAP()


// CDriverRegister �T���B�z�`��


BOOL CDriverRegister::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �b���[�J�B�~����l��
	m_ctrlAxisMacID.ResetContent();
	SetDlgItemInt(IDC_EDIT_Para_Class,0);
	SetDlgItemInt(IDC_EDIT_Para_Register,0);
	SetDlgItemInt(IDC_EDIT_Para_Value,0);
	if(m_parent)
	{

		CString str;
		for(UINT i = 0 ; i < m_parent->m_uiEnableAxis ; i++)
		{
			str.Format("%d",m_parent->m_Axis[i]);
			m_ctrlAxisMacID.AddString(str);
		}
		m_ctrlAxisMacID.SetCurSel(0);

	}
	m_ctrlTypecode.ResetContent();
	for(UINT i = 0 ; i < (sizeof(uctypecode) / sizeof(uctypecode[0])); i++)
	{
		m_ctrlTypecode.AddString(strTypecode[i]);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX �ݩʭ����Ǧ^ FALSE
}


void CDriverRegister::OnBnClickedButtonReadRegister()
{
	// TODO: �b���[�J����i���B�z�`���{���X
	UpdateData(TRUE);
	int i = m_ctrlAxisMacID.GetCurSel();
	unsigned char ucParaClass = (unsigned char) GetDlgItemInt(IDC_EDIT_Para_Class);
	unsigned char ucParanum = (unsigned char) GetDlgItemInt(IDC_EDIT_Para_Register);
	TRACE("class = %d , register = %d \n",ucParaClass,ucParanum);
	m_parent->DriverRegDialog_GetAxisDriverPara(m_parent->m_Axis[i],ucParaClass,ucParanum);

	SetDlgItemInt(IDC_EDIT_Para_Value,m_parent->m_intDvrReturnData);

}


void CDriverRegister::OnBnClickedButtonWriteRegister()
{
	// TODO: �b���[�J����i���B�z�`���{���X
	UpdateData(TRUE);
	int i = m_ctrlAxisMacID.GetCurSel();
	unsigned char ucParaClass = (unsigned char) GetDlgItemInt(IDC_EDIT_Para_Class);
	unsigned char ucParanum = (unsigned char) GetDlgItemInt(IDC_EDIT_Para_Register);
	int value = GetDlgItemInt(IDC_EDIT_Para_Value);
	TRACE("class = %d , register = %d \n",ucParaClass,ucParanum);
	m_parent->DriverRegDialog_SetAxisDriverPara(m_parent->m_Axis[i],ucParaClass,ucParanum,value);
	
	SetDlgItemInt(IDC_EDIT_Para_Value,m_parent->m_intDvrReturnData);

}


void CDriverRegister::OnBnClickedButtonEepromWriteRegister()
{
	// TODO: �b���[�J����i���B�z�`���{���X
	UpdateData(TRUE);
	int i = m_ctrlAxisMacID.GetCurSel();
	m_parent->DriverRegDialog_EEPROMWriteDriverPara(m_parent->m_Axis[i]);
    
	SetDlgItemInt(IDC_EDIT_Para_Value,m_parent->m_intDvrReturnData);
}


void CDriverRegister::OnClose()
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��

	//CDialogEx::OnClose();
	CDialogEx::OnOK();
}


void CDriverRegister::OnBnClickedButtonMonitor()
{
	// TODO: �b���[�J����i���B�z�`���{���X
	CString str;
	UpdateData(TRUE);
	int i = m_ctrlAxisMacID.GetCurSel();
	unsigned char  value =  uctypecode[m_ctrlTypecode.GetCurSel()];
	m_parent->DriverRegDialog_Monitor_CMD(m_parent->m_Axis[i],value);
	m_parent->m_MonitorSingal[i] = value;
	str.Format("MacID = %d, addtional monitor singal = %s" , m_parent->m_Axis[i], strTypecode[m_ctrlTypecode.GetCurSel()]);
	MessageBox(str);

}

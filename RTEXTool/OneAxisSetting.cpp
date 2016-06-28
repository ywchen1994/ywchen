// OneAxisSetting.cpp : 實作檔
//

#include "stdafx.h"
#include "RTEXTool.h"
#include "OneAxisSetting.h"
#include "afxdialogex.h"
#include "AxisSetting.h"

// COneAxisSetting 對話方塊

IMPLEMENT_DYNAMIC(COneAxisSetting, CDialogEx)

COneAxisSetting::COneAxisSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(COneAxisSetting::IDD, pParent)
	, m_axis(0)
	, m_Pitch(0)
	, m_GearRatio(0)
	, m_POTLimit(0)
	, m_NOTLimit(0)
	, m_GearRatio_Mech(0)
	, m_EncoderOffset(0)
{

}

COneAxisSetting::~COneAxisSetting()
{
}

void COneAxisSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ComboBox_MacID_for_Axis, m_ctrlSelectedMacID);
	DDX_Text(pDX, IDC_EDIT_Pitch, m_Pitch);
	DDX_Text(pDX, IDC_EDIT_GearRatio, m_GearRatio);
	DDX_Text(pDX, IDC_EDIT_POTLimit, m_POTLimit);
	DDX_Text(pDX, IDC_EDIT_NOTLimit, m_NOTLimit);
	DDX_Text(pDX, IDC_EDIT_GearRatio_Mech, m_GearRatio_Mech);
	DDX_Text(pDX, IDC_EDIT_EncoderOfst, m_EncoderOffset);
}


BEGIN_MESSAGE_MAP(COneAxisSetting, CDialogEx)
	ON_CBN_SELCHANGE(IDC_ComboBox_MacID_for_Axis, &COneAxisSetting::OnCbnSelchangeComboboxMacidforAxis)
END_MESSAGE_MAP()


// COneAxisSetting 訊息處理常式


BOOL COneAxisSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此加入額外的初始化
	CString str;
	m_ctrlSelectedMacID.ResetContent();
	for (int i = 0 ; i < 32 ; i++)	{		str.Format("%d", i);		m_ctrlSelectedMacID.AddString(str);	}	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}


void COneAxisSetting::OnCbnSelchangeComboboxMacidforAxis()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	UpdateData(TRUE);
	CAxisSetting* a = (CAxisSetting*)this->GetParentOwner();
	//TRACE("m_axis = %d\n",m_axis);
	int ibkMacID = a->m_MacID_axis[m_axis];
	a->m_MacID_axis[m_axis] =  m_ctrlSelectedMacID.GetCurSel();
	for(int i = 0 ; i < 6 ; i++)
	{
		if (i == m_axis) continue;
		if(a->m_MacID_axis[m_axis] == a->m_MacID_axis[i])
		{
			MessageBox("Same Mac ID with other driver");
			m_ctrlSelectedMacID.SetCurSel(ibkMacID);
			a->m_MacID_axis[m_axis] = ibkMacID;
			m_ctrlSelectedMacID.SetFocus();
		}
	}	
}
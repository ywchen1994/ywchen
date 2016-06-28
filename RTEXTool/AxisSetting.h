#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "OneAxisSetting.h"
#include "RTEXFormView.h"
// CAxisSetting 對話方塊
const UINT uiScaraParameter[] = {IDC_EDIT_L1, IDC_STATIC_L1, IDC_STATIC_Unit_L1, IDC_EDIT_L2, IDC_STATIC_L2, IDC_STATIC_Unit_L2, IDC_EDIT_Height, 
	IDC_STATIC_Height, IDC_STATIC_Unit_Height, IDC_STATIC_Arm_Type, IDC_COMBO_Arm_Type, IDC_BUTTON_Set_Scara_Mech};
class CAxisSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CAxisSetting)

public:
	CAxisSetting(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CAxisSetting();

// 對話方塊資料
	enum { IDD = IDD_RTEX_Configuration };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	float m_fVmax;
	UINT m_uiTa;
	CComboBox m_ctrlSpdType;
	UINT m_uiEnableAxis;
	virtual BOOL OnInitDialog();
	CTabCtrl m_ctrlSwitchAxis;
	int m_CurSelTab;
	COneAxisSetting m_page[6];
	CDialog* pDialog[6];
	afx_msg void OnTcnSelchangeTabSwitchaxis(NMHDR *pNMHDR, LRESULT *pResult);
	int m_MacID_axis[6];
	float m_Axis_Pitch[6];
	int m_Axis_GearRatio[6];
	float m_Axis_POTLimit[6];
	float m_Axis_NOTLimit[6];
	float m_Axis_GearRatio_Mech[6];
	int m_Axis_EncoderOffset[6];
	unsigned char m_Axis_Type[6];
	virtual void OnOK();
	int m_intcmbxSpdType;
	bool m_boolRTEX_Configurated;
	UINT m_uiShowPosTimeSlot;
	CRTEXFormView* m_parent;
	afx_msg void OnBnClickedButtonModifySpdParameter();
	afx_msg void OnBnClickedButtonModifyAxisParameter();
	float m_fScaraRobot_L1;
	float m_fScaraRobot_L2;
	float m_fScaraRobot_Height;
	int m_iScaraRobot_Arm_dir;
	CComboBox m_ctrlArm_Type;
	afx_msg void OnBnClickedButtonSetScaraMech();
	UINT m_uiCheckErrTimeSlot;
	int m_iJog_MaxRPM;
	float m_fJog_Speed;
	afx_msg void OnBnClickedButtonJogMaxrpm();
	afx_msg void OnBnClickedButtonJogSpeed();
	float m_HomeJ1;
	float m_HomeJ2;
	float m_HomeJ3;
	float m_HomeJ4;

	afx_msg void OnBnClickedButtonHomepointchange();
};

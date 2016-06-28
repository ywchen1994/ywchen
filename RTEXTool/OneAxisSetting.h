#pragma once
#include "afxwin.h"


// COneAxisSetting 對話方塊

class COneAxisSetting : public CDialogEx
{
	DECLARE_DYNAMIC(COneAxisSetting)

public:
	COneAxisSetting(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~COneAxisSetting();

// 對話方塊資料
	enum { IDD = IDD_OneAxisSetting };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_ctrlSelectedMacID;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboboxMacidforAxis();
	UINT m_axis;
	float m_Pitch;
	int m_GearRatio;
	float m_POTLimit;
	float m_NOTLimit;
	float m_GearRatio_Mech;
	int m_EncoderOffset;
};

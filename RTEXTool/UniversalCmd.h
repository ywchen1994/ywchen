#pragma once
#include "afxwin.h"
#include <vector>

// CUniversalCmd ��ܤ��

class CMyParameter
{
public:
	inline CMyParameter(CString str, int type)
	{
		m_strParameter = str;
		m_iDataType = type;
	}
	CString m_strParameter;
	int m_iDataType;
};


class CUniversalCmd : public CDialogEx
{
	DECLARE_DYNAMIC(CUniversalCmd)

public:
	CUniversalCmd(CWnd* pParent = NULL);   // �зǫغc�禡
	virtual ~CUniversalCmd();

// ��ܤ�����
	enum { IDD = IDD_UniversalCmd };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	CString m_strCmd;
	CString m_strParameter;
	CListBox m_ctrlParameterList;
	CComboBox m_ctrParameterType;
public:
protected:
	void UpdateParaListbox(void);
	BOOL Check_Value_and_Type(CString str, int datatype);
	BOOL Check_CmdFail(CString str, unsigned char* uschar);
	afx_msg void OnBnClickedButtonAddparameter();
	CComboBox m_ctrlParameterListNum;
	std::vector<CMyParameter*> m_MyParameterList;
	afx_msg void OnCbnSelchangeComboParameterlistnum();

public:
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBnClickedButtonTransfer();
	unsigned char*m_TransferBuff;
	union {
		unsigned char uchar_number[4];
		float float_number;
	}nTypeTrans;

};

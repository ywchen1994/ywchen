// UniversalCmd.cpp : 龟郎
//

#include "stdafx.h"
#include "RTEXTool.h"
#include "UniversalCmd.h"
#include "afxdialogex.h"


// CUniversalCmd 癸杠よ遏

IMPLEMENT_DYNAMIC(CUniversalCmd, CDialogEx)

CUniversalCmd::CUniversalCmd(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUniversalCmd::IDD, pParent)
	, m_strCmd(_T(""))
	, m_strParameter(_T(""))
	, m_TransferBuff(NULL)
{
	m_TransferBuff = new unsigned char[512];
}

CUniversalCmd::~CUniversalCmd()
{
	for ( auto iter = m_MyParameterList.begin( ) ; iter != m_MyParameterList.end( ) ; iter++ )
	{
		delete *iter;
	}
	delete [] m_TransferBuff;
}

void CUniversalCmd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_Command, m_strCmd);
	DDX_Text(pDX, IDC_EDIT_Parameter, m_strParameter);
	DDX_Control(pDX, IDC_COMBO_ParameterType, m_ctrParameterType);
	DDX_Control(pDX, IDC_LIST_ParameterList, m_ctrlParameterList);
	DDX_Control(pDX, IDC_COMBO_ParameterListNum, m_ctrlParameterListNum);
}


BEGIN_MESSAGE_MAP(CUniversalCmd, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_AddParameter, &CUniversalCmd::OnBnClickedButtonAddparameter)
	ON_CBN_SELCHANGE(IDC_COMBO_ParameterListNum, &CUniversalCmd::OnCbnSelchangeComboParameterlistnum)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_Transfer, &CUniversalCmd::OnBnClickedButtonTransfer)
END_MESSAGE_MAP()


// CUniversalCmd 癟矪瞶盽Α


BOOL CUniversalCmd::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// TODO:  肂﹍て
	
	m_ctrParameterType.ResetContent();
	m_ctrParameterType.AddString("unsigned char");
	m_ctrParameterType.AddString("unsigned short");
	m_ctrParameterType.AddString("int");
	m_ctrParameterType.AddString("float");
	m_ctrlParameterListNum.ResetContent();
	m_ctrlParameterListNum.AddString("Add Parameter");

	m_ctrlParameterListNum.SetCurSel(m_ctrlParameterListNum.GetCount()-1);
	SetDlgItemText(IDC_EDIT_Command,"0xFF");
	OnCbnSelchangeComboParameterlistnum();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 妮┦莱肚 FALSE
}


void CUniversalCmd::OnBnClickedButtonAddparameter()
{
	// TODO: 北兜矪瞶盽Α祘Α絏
	UpdateData(TRUE);
	CString str;
	int listCnt = m_ctrlParameterListNum.GetCount();
	int listNumcurrentset = m_ctrlParameterListNum.GetCurSel();

	GetDlgItemText(IDC_EDIT_Parameter,str);
	int Datatype_curset = m_ctrParameterType.GetCurSel();
	CMyParameter* pMyPara;

	if(Check_Value_and_Type(str,Datatype_curset))
	{
		// value or Type is wrong
		GetDlgItem(IDC_EDIT_Parameter)->SetFocus();
		CEdit* pedit=(CEdit*)GetDlgItem(IDC_EDIT_Parameter);
		pedit->SetSel(0,pedit->GetWindowTextLength());
	}
    else if(listNumcurrentset == (listCnt -1))  //ADD parameter
	{
		pMyPara = new CMyParameter(str,Datatype_curset);
		m_MyParameterList.push_back(pMyPara);
		str.Format("%d", listCnt);
        m_ctrlParameterListNum.InsertString((listCnt -1),str);
		m_ctrParameterType.SetCurSel(0);  
		SetDlgItemText(IDC_EDIT_Parameter,"0");
		GetDlgItem(IDC_EDIT_Parameter)->SetFocus();
		CEdit* pedit=(CEdit*)GetDlgItem(IDC_EDIT_Parameter);
		pedit->SetSel(0,pedit->GetWindowTextLength());
	}
	else  //modify parameter
	{
		pMyPara = m_MyParameterList[listNumcurrentset];
		pMyPara->m_strParameter = str;
		pMyPara->m_iDataType = Datatype_curset;
        GetDlgItem(IDC_COMBO_ParameterListNum)->SetFocus();
	}
    UpdateParaListbox();
}


void CUniversalCmd::OnCbnSelchangeComboParameterlistnum()
{
	// TODO: 北兜矪瞶盽Α祘Α絏
	UpdateData(TRUE);
	if(m_ctrlParameterListNum.GetCurSel() == (m_ctrlParameterListNum.GetCount() -1))
	{
		SetDlgItemText(IDC_BUTTON_AddParameter,"穝糤把计");
		m_ctrParameterType.SetCurSel(0);
		SetDlgItemInt(IDC_EDIT_Parameter,0);
	}
	else
	{
		int currentset = m_ctrlParameterListNum.GetCurSel();
		SetDlgItemText(IDC_BUTTON_AddParameter,"э把计");
		CMyParameter* pMyPara;
		pMyPara = m_MyParameterList[currentset];
		m_ctrParameterType.SetCurSel(pMyPara->m_iDataType);
		SetDlgItemText(IDC_EDIT_Parameter,pMyPara->m_strParameter);
	}
}


void CUniversalCmd::UpdateParaListbox(void)
{
	CString str,strDatatype;
	m_ctrlParameterList.ResetContent();
	int i = 0;
	for ( auto iter = m_MyParameterList.begin( ) ; iter != m_MyParameterList.end( ) ; iter++ )
	{
		i++;
		m_ctrParameterType.GetLBText((*iter)->m_iDataType, strDatatype);
        str.Format("把计抖 = %2d, 把计 = %s,   把计篈 = %s",i ,(*iter)->m_strParameter, strDatatype);
	    m_ctrlParameterList.AddString(str);
	}
}


BOOL CUniversalCmd::Check_Value_and_Type(CString str, int datatype)
{
	UpdateData(TRUE);
	unsigned char ucCheckPass = 1;
	unsigned char ucCheckstrHex = 0;
	char* chkstr;
	

	if((str.Left(2) == "0x") || (str.Left(2) == "0X"))	
	{
		if(datatype == 3)
		{
			MessageBox("把计Τ粇\n疊翴计ぃや穿hex よΑ块");
			OnCbnSelchangeComboParameterlistnum();
			return TRUE;
		}
		ucCheckstrHex = 1;
		MessageBox("HEX Value");
	}

	long value = 0;
	float fvalue = 0;

	TRACE("check 把计 篈 = %d\n",datatype);

	if(datatype == 3)
	{
	   fvalue = (float)strtod((LPCSTR)str,&chkstr);
  	   TRACE("疊翴计 = %.6f, chkstr len = %d\n" ,fvalue,strlen(chkstr));
	}
	if(datatype != 3)
	{

		if(ucCheckstrHex == 1)	value = strtol((LPCSTR)str,&chkstr,16);
		else    				value = strtol((LPCSTR)str,&chkstr,10);
		
		TRACE("16秈 = 0x%08X,  10秈 = %d, chkstr len = %d\n", value,value,strlen(chkstr));
	}

	switch (datatype)
	{
		case 0 :  //unsigned char
			if ((value > 0xff) || (value < 0) || (strlen(chkstr) != 0))
			{
				ucCheckPass = 0;
			}
			break;
		case 1 :  //unsigned short
			if ((value > 0xffff) || (value < 0) || (strlen(chkstr) != 0))
			{
				ucCheckPass = 0;
			}
			break;
		case 2 :  //int
		case 3 :  //float
			if (strlen(chkstr) != 0)
			{
				ucCheckPass = 0;
			}
			break;
		default :
			ucCheckPass = 0;
			break;
	}

	if(ucCheckPass)
	{
    	return FALSE;
	}
	else
	{
	  MessageBox("把计Τ粇\n叫穝块続讽");
	  OnCbnSelchangeComboParameterlistnum();
	  return TRUE;
	}
}

BOOL CUniversalCmd::Check_CmdFail(CString str, unsigned char* uschar)
{
	long value;
	char* chkstr;
	if((str.Left(2) == "0x") || (str.Left(2) == "0X"))	
		value = strtol((LPCSTR)str,&chkstr,16);
	else
		value = strtol((LPCSTR)str,&chkstr,10);
	TRACE("check CMD\n");
    TRACE("16秈 = 0x%08X,  10秈 = %d, chkstr len = %d\n", value,value,strlen(chkstr));

	if ((value > 0xff) || (value < 0) || (strlen(chkstr) != 0) || (str.GetLength()== 0))
	{
		MessageBox("CommandΤ粇");
    return TRUE;
	}
	*uschar = (unsigned char) value;
	return FALSE;
}



void CUniversalCmd::OnClose()
{
	// TODO: 眤癟矪瞶盽Α祘Α絏㎝ (┪) ㊣箇砞
	CDialogEx::OnCancel();
	//CDialogEx::OnClose();
}
void CUniversalCmd::OnCancel()
{
	// TODO: 疭﹚祘Α絏㎝ (┪) ㊣膀┏摸

	CDialogEx::OnCancel();
}

void CUniversalCmd::OnOK()
{
	// TODO: 疭﹚祘Α絏㎝ (┪) ㊣膀┏摸

	//CDialogEx::OnOK();
}


void CUniversalCmd::OnBnClickedButtonTransfer()
{
	// TODO: 北兜矪瞶盽Α祘Α絏
	CString str;
	m_TransferBuff[0] = 'Q';
	m_TransferBuff[1] = 'S';
	m_TransferBuff[2] = 'I';
	m_TransferBuff[3] = 'C';
	
	GetDlgItemText(IDC_EDIT_Command,str);
	if(Check_CmdFail(str, &m_TransferBuff[4]))
	{
		SetDlgItemText(IDC_EDIT_Command,"0xFF");
		GetDlgItem(IDC_EDIT_Command)->SetFocus();
		return;
	}
	
	int i = 6;
	CMyParameter* pMypara;
	long value;
	float fvalue;
	for ( auto iter = m_MyParameterList.begin( ) ; iter != m_MyParameterList.end( ) ; iter++ )
	{
		pMypara = *iter;
		str = pMypara->m_strParameter;
		if(pMypara->m_iDataType == 3)
		{
			fvalue = (float)strtod((LPCSTR)str,NULL);
		}
		else
		{
			if((str.Left(2) == "0x") || (str.Left(2) == "0X"))	value = strtol((LPCSTR)str,NULL,16);
			else value = strtol((LPCSTR)str,NULL,10);
		}

		switch (pMypara->m_iDataType)
		{
		case 0 :  //unsigned char 
			m_TransferBuff[i] = (unsigned char) value;
			i++;
			break;
		case 1 :  // unsigned short
			m_TransferBuff[i] = (unsigned char)((value & 0x0000FF00) >> 8);
			m_TransferBuff[i +1] = (unsigned char)(value & 0x000000FF);
			i= i+ 2;
			break;
		case 2 : // int
			m_TransferBuff[i] =     (unsigned char)((value & 0xFF000000) >> 24);
			m_TransferBuff[i + 1] = (unsigned char)((value & 0x00FF0000) >> 16);
			m_TransferBuff[i + 2] = (unsigned char)((value & 0x0000FF00) >> 8);
			m_TransferBuff[i + 3] = (unsigned char)((value & 0x000000FF));
			i= i+ 4;
			break;
		case 3 : // float
			nTypeTrans.float_number = fvalue;
			m_TransferBuff[i] =     nTypeTrans.uchar_number[3];
			m_TransferBuff[i + 1] = nTypeTrans.uchar_number[2];
			m_TransferBuff[i + 2] = nTypeTrans.uchar_number[1];
			m_TransferBuff[i + 3] = nTypeTrans.uchar_number[0];
			i= i+ 4;
			break;
		}
	}
	m_TransferBuff[5] = (i - 6);
	for(int j = 0 ; j < i ; j++)    TRACE("m_TransferBuff[%d] = 0x%02X\n",j,m_TransferBuff[j]);
	CDialogEx::OnOK();

}



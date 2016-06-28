// IOBoardSetV2.cpp : 實作檔
//

#include "stdafx.h"
#include "RTEXTool.h"
#include "IOBoardSetV2.h"
#include "afxdialogex.h"


// CIOBoardSetV2 對話方塊

IMPLEMENT_DYNAMIC(CIOBoardSetV2, CDialogEx)

CIOBoardSetV2::CIOBoardSetV2(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIOBoardSetV2::IDD, pParent)
	, m_icurTabSel(0)
	, m_fgenableIO_borad(FALSE)
	, fgboard0btnChg(false)
	, m_fgInput(FALSE)
{
	m_parent = (CRTEXFormView*) pParent;
}

CIOBoardSetV2::~CIOBoardSetV2()
{
}

void CIOBoardSetV2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_SwitchIOBoard, m_ctrlIOTabctrl);
	DDX_Control(pDX, IDC_COMBO_IO_MacID, m_ctrlMacID);
	DDX_Check(pDX, IDC_CHECK_EnableIO, m_fgenableIO_borad);
	DDX_Radio(pDX, IDC_RADIO_OutputBoard, m_fgInput);
	DDX_Control(pDX, IDC_LIST_IO_STATUS, m_ctrlListctrl);
}


BEGIN_MESSAGE_MAP(CIOBoardSetV2, CDialogEx)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SwitchIOBoard, &CIOBoardSetV2::OnTcnSelchangeTabSwitchioboard)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK_EnableIO, &CIOBoardSetV2::OnBnClickedCheckEnableio)
	ON_CBN_SELCHANGE(IDC_COMBO_IO_MacID, &CIOBoardSetV2::OnCbnSelchangeComboIoMacid)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_CHECK_BIT0,IDC_CHECK_BIT31,OnCHKClicked0_31)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_RADIO_InputBoard,IDC_RADIO_OutputBoard,OnRadioBtnClickedIO)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CIOBoardSetV2 訊息處理常式


BOOL CIOBoardSetV2::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此加入額外的初始化
	CString str;
	if(m_parent)
	{
		m_ctrlMacID.ResetContent();
		for(int i = 0 ; i < 32 ;i++)
		{
			str.Format("%d",i);
			m_ctrlMacID.AddString(str);
		}

		if(m_parent->m_boolRTEX_Configurated)
		{
			for(UINT i = 0 ; i < (sizeof(m_parent->m_fgenableIO_borad) / sizeof(m_parent->m_fgenableIO_borad[0]));i++)
			{
				if((m_parent->m_fgenableIO_borad[i] == TRUE))
				{
					str.Format("I/O MACID = %d : %s",m_parent->m_ucIO_MacID[i],(m_parent->m_fgInput[i])?"Input":"Output");
					m_ctrlIOTabctrl.InsertItem(i,str);
				}
			}
		}
		else
		{
			for(UINT i = 0 ; i < (sizeof(m_parent->m_fgenableIO_borad) / sizeof(m_parent->m_fgenableIO_borad[0]));i++)
			{
					str.Format("I/O Board %d",i);
					m_ctrlIOTabctrl.InsertItem(i,str);
			}

		}
	}	
	m_icurTabSel = CurTabSel2IOdeviceSeq(0);
	SetDlgitemStatus(m_icurTabSel);
	ListCtrlInit();
	SetTimer(0,200,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}
int CIOBoardSetV2::CurTabSel2IOdeviceSeq(int icurTabSel)
{
	if(!m_parent->m_boolRTEX_Configurated) return icurTabSel;

	int i = 0;
	int value = 0;
	for(i = 0 ; i < (sizeof(m_parent->m_fgenableIO_borad) / sizeof(m_parent->m_fgenableIO_borad[0]));i++)
	{
		if(m_parent->m_fgenableIO_borad[i] == TRUE)
		{
			value++;
		}
		if(value == (icurTabSel+1)) return i;
	}
	return 0;
}
void CIOBoardSetV2::OnTcnSelchangeTabSwitchioboard(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此加入控制項告知處理常式程式碼
	m_icurTabSel = m_ctrlIOTabctrl.GetCurSel();
	//TRACE("Tab sel = %d\n",m_icurTabSel);
	m_icurTabSel = CurTabSel2IOdeviceSeq(m_icurTabSel);
	SetDlgitemStatus(m_icurTabSel);
	*pResult = 0;
}
void CIOBoardSetV2::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if(nIDEvent == 0)
	{
		  UpdateIOstatus(m_icurTabSel);
		  UpdateIOstatusInList();
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CIOBoardSetV2::SetDlgitemStatus(int icurSel)
{
	GetDlgItem(IDC_CHECK_EnableIO)->EnableWindow(!m_parent->m_boolRTEX_Configurated);
	GetDlgItem(IDC_STATIC_IO_MACID)->EnableWindow(!m_parent->m_boolRTEX_Configurated);
	GetDlgItem(IDC_COMBO_IO_MacID)->EnableWindow(!m_parent->m_boolRTEX_Configurated);
	GetDlgItem(IDC_STATIC_IO_TYPE)->EnableWindow(!m_parent->m_boolRTEX_Configurated);
	GetDlgItem(IDC_RADIO_InputBoard)->EnableWindow(!m_parent->m_boolRTEX_Configurated);
	GetDlgItem(IDC_RADIO_OutputBoard)->EnableWindow(!m_parent->m_boolRTEX_Configurated);
	m_fgenableIO_borad = m_parent->m_fgenableIO_borad[icurSel];
	m_ctrlMacID.SetCurSel(m_parent->m_ucIO_MacID[icurSel]);
	m_fgInput = m_parent->m_fgInput[icurSel];
	CButton * pbtn;
    for(int i = 0 ; i < 32 ;i++)
	{
		pbtn = (CButton*)GetDlgItem(IOBtnDlgItemNum[i]);
		pbtn->ShowWindow(m_parent->m_boolRTEX_Configurated);
		pbtn->EnableWindow(!m_parent->m_fgInput[icurSel]);
	}
	UpdateIOstatus(icurSel);
	UpdateData(FALSE);
}
void CIOBoardSetV2::UpdateIOstatus(int icurSelIO)
{
	if(fgboard0btnChg == TRUE)		return;
	CButton * pbtn;
	for(int i = 0 ; i < 32 ;i++)
	{
		pbtn = (CButton*)GetDlgItem(IOBtnDlgItemNum[i]);
		if(m_parent->m_uiIOStatus[icurSelIO] & (1 << i)) pbtn->SetCheck(1);
		else pbtn->SetCheck(0);
	}
}
void CIOBoardSetV2::OnCHKClicked0_31(UINT uiCmdID)
{
	//TRACE("btn click = %d\n",uiCmdID);
	fgboard0btnChg = TRUE;
	unsigned char ucbitchg = uiCmdID-IDC_CHECK_BIT0;
	UINT  uicurrentstatus =  m_parent->m_uiIOStatus[m_icurTabSel];
	//TRACE("board %d -> ID: %d is clicked\n ",m_icurTabSel,ucbitchg);
		
	//MSG msg;
	while(m_parent->m_blWaiitCmdPROGRESS || m_parent->m_WaitCmdAck)
	{
		Sleep(2);
		//GetMessage(&msg,NULL,0,0);
		//TranslateMessage(&msg);
		//DispatchMessage(&msg);
	};	
	m_parent->SetWaitCMDAcking(1);	
	uicurrentstatus ^= (UINT)(1 << ucbitchg);
	m_parent->RTEX_CMD_SET_OUTPUT(m_parent->m_ucIO_MacID[m_icurTabSel],uicurrentstatus);
	//m_parent->RTEX_CMD_SET_OUTPUT(m_parent->m_ucIO_MacID[m_icurTabSel],ucbitchg,(uicurrentstatus & (1 << ucbitchg))? TRUE : FALSE);	
	//TRACE("set output %d\n",ucbitchg);	
	if(m_parent->WaitCMDAcking())
	{
		MessageBox("CMD Fail");
		return;
	}
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime;
	LARGE_INTEGER nEndTime;
	double dtime;
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nBeginTime);
	do 
	{
		Sleep(2);
		if(m_parent->m_uiIOStatus[m_icurTabSel] == uicurrentstatus)
		{
			//TRACE("ok\n");
			break;
		}
		//GetMessage(&msg,NULL,0,0);
		//TranslateMessage(&msg);
		//DispatchMessage(&msg);
		QueryPerformanceCounter(&nEndTime);
		dtime=(double)(nEndTime.QuadPart-nBeginTime.QuadPart)/(double)nFreq.QuadPart;
	} while (dtime < (double)(m_parent->m_uiShowPosTimeSlot * 4 * 0.001)); //delay 2 times m_uiShowPosTimeSlot
	fgboard0btnChg = FALSE;

}
void CIOBoardSetV2::OnRadioBtnClickedIO(UINT uiCmdID)
{
	TRACE("btn click = %d\n",uiCmdID);
	UpdateData(TRUE);
	m_parent->m_fgInput[m_icurTabSel] = m_fgInput;
	TRACE("get I/O %d %s \n" ,m_icurTabSel, m_parent->m_fgInput[m_icurTabSel]?"Input":"Output");
	ListCtrlInforUpdate();
}
void CIOBoardSetV2::OnBnClickedCheckEnableio()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	UpdateData(TRUE);
	m_parent->m_fgenableIO_borad[m_icurTabSel] = m_fgenableIO_borad;
	TRACE("get I/O %d %s \n" ,m_icurTabSel, m_parent->m_fgenableIO_borad[m_icurTabSel]?"Enable":"disable");
	ListCtrlInforUpdate();
}
void CIOBoardSetV2::OnCbnSelchangeComboIoMacid()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	UpdateData(TRUE);
	m_parent->m_ucIO_MacID[m_icurTabSel] = m_ctrlMacID.GetCurSel();
	TRACE("get I/O %d macID = %d \n" ,m_icurTabSel, m_parent->m_ucIO_MacID[m_icurTabSel]);
	ListCtrlInforUpdate();
}
void CIOBoardSetV2::OnOK()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	CDialogEx::OnOK();
}
void CIOBoardSetV2::OnCancel()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	//CDialogEx::OnCancel();
}
void CIOBoardSetV2::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	//CDialogEx::OnClose();
}
void CIOBoardSetV2::ListCtrlInit(void)
{
	ListFont.CreatePointFont(120,"Arial");
	m_ctrlListctrl.SetFont(&ListFont,1);
	DWORD dwStyle=GetWindowLong(m_ctrlListctrl,GWL_STYLE);
	dwStyle|=LVS_REPORT;	
	SetWindowLong(m_ctrlListctrl,GWL_STYLE,dwStyle); 
	DWORD dwExstyle = m_ctrlListctrl.GetExtendedStyle();
	dwExstyle |= LVS_EX_GRIDLINES;
	m_ctrlListctrl.SetExtendedStyle(dwExstyle);
	m_ctrlListctrl.Invalidate();
	int width = 80;
	m_ctrlListctrl.InsertColumn(0,"MACID",LVCFMT_CENTER, width);
	m_ctrlListctrl.InsertColumn(1,"IO TYPE",LVCFMT_LEFT, width);
	m_ctrlListctrl.InsertColumn(2,"Active",LVCFMT_LEFT, width);
	m_ctrlListctrl.InsertColumn(3,"BYTE3",LVCFMT_CENTER, width);
	m_ctrlListctrl.InsertColumn(4,"BYTE2",LVCFMT_CENTER, width);
	m_ctrlListctrl.InsertColumn(5,"BYTE1",LVCFMT_CENTER, width);
	m_ctrlListctrl.InsertColumn(6,"BYTE0",LVCFMT_CENTER, width);
	ListCtrlInforUpdate();
}
void CIOBoardSetV2::ListCtrlInforUpdate(void)
{
	CString str;
	m_ctrlListctrl.DeleteAllItems();
	int listcnt = 0;
	for(UINT i = 0 ; i < (sizeof(m_parent->m_fgenableIO_borad) / sizeof(m_parent->m_fgenableIO_borad[0]));i++)
	{
		if((m_parent->m_boolRTEX_Configurated) && (m_parent->m_fgenableIO_borad[i] == FALSE)) continue;
		str.Format("%d",m_parent->m_ucIO_MacID[i]);
		m_ctrlListctrl.InsertItem(listcnt,str);
		str.Format("%s",m_parent->m_fgInput[i]? "Input":"Output");
		m_ctrlListctrl.SetItemText(listcnt,1,str);
		str.Format("%s",m_parent->m_fgenableIO_borad[i]? "Enable":"Disable");
		m_ctrlListctrl.SetItemText(listcnt,2,str);
		listcnt++;
	}
	UpdateIOstatusInList();
}
void CIOBoardSetV2::UpdateIOstatusInList(void)
{
	if(!m_parent->m_boolRTEX_Configurated) return;
	CString str;
	unsigned char ch;
	int listcnt = 0;
	for(UINT i = 0 ; i < (sizeof(m_parent->m_fgenableIO_borad) / sizeof(m_parent->m_fgenableIO_borad[0]));i++)
	{
		if((m_parent->m_boolRTEX_Configurated) && (m_parent->m_fgenableIO_borad[i] == TRUE))
		{
			
			ch = (unsigned char)((m_parent->m_uiIOStatus[i] & 0xFF000000) >> 24);
			str.Format("%02X",ch);
			m_ctrlListctrl.SetItemText(listcnt,3,str);
			ch = (unsigned char)((m_parent->m_uiIOStatus[i] & 0x00FF0000) >> 16);
			str.Format("%02X",ch);
			m_ctrlListctrl.SetItemText(listcnt,4,str);
			ch = (unsigned char)((m_parent->m_uiIOStatus[i] & 0x0000FF00) >> 8);
			str.Format("%02X",ch);
			m_ctrlListctrl.SetItemText(listcnt,5,str);
			ch = (unsigned char)(m_parent->m_uiIOStatus[i] & 0x000000FF);
			str.Format("%02X",ch);			
			m_ctrlListctrl.SetItemText(listcnt,6,str);
			listcnt++;
		}
	}
}




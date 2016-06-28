// IOBoardSet.cpp : 實作檔
//

#include "stdafx.h"
#include "RTEXTool.h"
#include "IOBoardSet.h"
#include "afxdialogex.h"


// CIOBoardSet 對話方塊

IMPLEMENT_DYNAMIC(CIOBoardSet, CDialogEx)

CIOBoardSet::CIOBoardSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIOBoardSet::IDD, pParent)
	, m_input_threading(FALSE)
	, m_threadEnd(TRUE)
{
    m_parent = (CRTEXFormView*)pParent;
	for(UINT i = 0 ; i < (sizeof(m_fgenableIO_borad) / sizeof(m_fgenableIO_borad[0]));i++)
		m_fgenableIO_borad[i] = FALSE;
	for(UINT i = 0 ; i < (sizeof(m_fgInput) / sizeof(m_fgInput[0]));i++)
		m_fgInput[i] = FALSE;

	if(m_parent)
	{
		for(UINT i = 0 ; i < (sizeof(m_fgenableIO_borad) / sizeof(m_fgenableIO_borad[0]));i++)
		  m_fgenableIO_borad[i] = m_parent->m_fgenableIO_borad[i];
		for(UINT i = 0 ; i < (sizeof(m_fgInput) / sizeof(m_fgInput[0]));i++)
		  m_fgInput[i] = m_parent->m_fgInput[i];
	}
	fgboard0btnChg[0] = FALSE;
	fgboard0btnChg[1] = FALSE;
	fgboard0btnChg[2] = FALSE;
	fgboard0btnChg[3] = FALSE;

}

CIOBoardSet::~CIOBoardSet()
{
}

void CIOBoardSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_IO_MacID, m_ctrl_IO_MacID[0]);
	DDX_Control(pDX, IDC_COMBO_IO_MacID1, m_ctrl_IO_MacID[1]);
	DDX_Control(pDX, IDC_COMBO_IO_MacID2, m_ctrl_IO_MacID[2]);
	DDX_Control(pDX, IDC_COMBO_IO_MacID3, m_ctrl_IO_MacID[3]);

	DDX_Check(pDX, IDC_CHECK_EnableIO, m_fgenableIO_borad[0]);
	DDX_Check(pDX, IDC_CHECK_EnableIO1, m_fgenableIO_borad[1]);
	DDX_Check(pDX, IDC_CHECK_EnableIO2, m_fgenableIO_borad[2]);
	DDX_Check(pDX, IDC_CHECK_EnableIO3, m_fgenableIO_borad[3]);

	DDX_Check(pDX, IDC_CHECK_Input, m_fgInput[0]);
	DDX_Check(pDX, IDC_CHECK_Input1, m_fgInput[1]);
	DDX_Check(pDX, IDC_CHECK_Input2, m_fgInput[2]);
	DDX_Check(pDX, IDC_CHECK_Input3, m_fgInput[3]);

}


BEGIN_MESSAGE_MAP(CIOBoardSet, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_IO_MacID, &CIOBoardSet::OnCbnSelchangeComboIoMacid)
	ON_CBN_SELCHANGE(IDC_COMBO_IO_MacID1, &CIOBoardSet::OnCbnSelchangeComboIoMacid1)
	ON_CBN_SELCHANGE(IDC_COMBO_IO_MacID2, &CIOBoardSet::OnCbnSelchangeComboIoMacid2)
	ON_CBN_SELCHANGE(IDC_COMBO_IO_MacID3, &CIOBoardSet::OnCbnSelchangeComboIoMacid3)

	ON_WM_CLOSE()

	ON_CONTROL_RANGE(BN_CLICKED,IDC_CHECK_BIT0,IDC_CHECK_BIT31,OnCHKClicked0_31)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_CHECK_BIT32,IDC_CHECK_BIT63,OnCHKClicked32_63)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_CHECK_BIT64,IDC_CHECK_BIT95,OnCHKClicked64_95)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_CHECK_BIT96,IDC_CHECK_BIT127,OnCHKClicked96_127)

	ON_BN_CLICKED(IDExit, &CIOBoardSet::OnBnClickedExit)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CIOBoardSet 訊息處理常式


BOOL CIOBoardSet::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此加入額外的初始化
	CString str;
	for(UINT i = 0 ; i < (sizeof(m_ctrl_IO_MacID) / sizeof(m_ctrl_IO_MacID[0]));i++)
	{
		m_ctrl_IO_MacID[i].ResetContent();

	}
	for (int i = 0 ; i < 32 ; i++)	
	{	
		str.Format("%d", i);		
		m_ctrl_IO_MacID[0].AddString(str);	
		m_ctrl_IO_MacID[1].AddString(str);	
		m_ctrl_IO_MacID[2].AddString(str);	
		m_ctrl_IO_MacID[3].AddString(str);	
	}	
    if(m_parent)
	{
	  for(UINT i = 0 ; i < (sizeof(m_ctrl_IO_MacID) / sizeof(m_ctrl_IO_MacID[0]));i++)
	  {
	    m_ctrl_IO_MacID[i].SetCurSel(m_parent->m_ucIO_MacID[i]);
	  }
	  

	  if(m_parent->m_boolRTEX_Configurated)
	  {
		  GetDlgItem(IDC_CHECK_EnableIO)->EnableWindow(FALSE);
		  GetDlgItem(IDC_CHECK_EnableIO1)->EnableWindow(FALSE);
		  GetDlgItem(IDC_CHECK_EnableIO2)->EnableWindow(FALSE);
		  GetDlgItem(IDC_CHECK_EnableIO3)->EnableWindow(FALSE);

		  GetDlgItem(IDC_CHECK_Input)->EnableWindow(FALSE);
		  GetDlgItem(IDC_CHECK_Input1)->EnableWindow(FALSE);
		  GetDlgItem(IDC_CHECK_Input2)->EnableWindow(FALSE);
		  GetDlgItem(IDC_CHECK_Input3)->EnableWindow(FALSE);

		  GetDlgItem(IDC_COMBO_IO_MacID)->EnableWindow(FALSE);
		  GetDlgItem(IDC_COMBO_IO_MacID1)->EnableWindow(FALSE);
		  GetDlgItem(IDC_COMBO_IO_MacID2)->EnableWindow(FALSE);
		  GetDlgItem(IDC_COMBO_IO_MacID3)->EnableWindow(FALSE);

		  CButton * pbtn;
		  for(UINT i = 0 ; i < (sizeof(m_fgenableIO_borad) / sizeof(m_fgenableIO_borad[0]));i++)
		  {
			      if(m_parent->m_fgenableIO_borad[i])
				  {
					for(int j = 0 ; j < 32 ; j++)
					{
						GetDlgItem(IOBoardstatus[i][j])->ShowWindow(TRUE);
						if(m_parent->m_fgInput[i])
						{ 
							GetDlgItem(IOBoardstatus[i][j])->EnableWindow(FALSE);
						}
						else
						{
							GetDlgItem(IOBoardstatus[i][j])->EnableWindow(TRUE);
						}

						pbtn = (CButton *) GetDlgItem(IOBoardstatus[i][j]);
						if(m_parent->m_uiIOStatus[i] & (1 << j)) pbtn->SetCheck(1);
						else pbtn->SetCheck(0);
					}
					CString str;
					if(m_parent->m_fgInput[i])
					{
					  str.Format("I / O Borad %d :     INPUT Board", i);
                      SetDlgItemText(IOStatusText[i],str);
					}
					else
					{
						str.Format("I / O Borad %d :     OUTPUT Board", i);
						SetDlgItemText(IOStatusText[i],str);
					}
				  }
				  else
				  {
				    for(int j = 0 ; j < 32 ; j++) 
					{
						GetDlgItem(IOBoardstatus[i][j])->ShowWindow(FALSE);
					}
				  }

		  }          
		  						
// 		  if((m_parent->m_fgInput[0] && m_parent->m_fgenableIO_borad[0]) || (m_parent->m_fgInput[1] && m_parent->m_fgenableIO_borad[1]))
// 		  {
// 			  m_input_threading = TRUE;
// 			  m_threadEnd = FALSE;
// 			  AfxBeginThread(&CIOBoardSet::InputBoardthreadFun, (LPVOID)this);
// 		  }
		  SetTimer(0,200,NULL);
	  }
	  else
	  {
		  for(UINT i = 0 ; i < (sizeof(m_fgenableIO_borad) / sizeof(m_fgenableIO_borad[0]));i++)
		  {
			  for(int j = 0 ; j < 32 ; j++)
			  {
			    GetDlgItem(IOBoardstatus[i][j])->ShowWindow(FALSE);
			  }
		  }
	  }
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}


void CIOBoardSet::OnCbnSelchangeComboIoMacid()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	UpdateData(TRUE);
	m_parent->m_ucIO_MacID[0] = m_ctrl_IO_MacID[0].GetCurSel();
	TRACE("get I/O 0 macID = %d \n" ,m_parent->m_ucIO_MacID[0]);
}


void CIOBoardSet::OnCbnSelchangeComboIoMacid1()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	UpdateData(TRUE);
	m_parent->m_ucIO_MacID[1] = m_ctrl_IO_MacID[1].GetCurSel();
	TRACE("get I/O 1 macID = %d \n" ,m_parent->m_ucIO_MacID[1]);
}
void CIOBoardSet::OnCbnSelchangeComboIoMacid2()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	UpdateData(TRUE);
	m_parent->m_ucIO_MacID[2] = m_ctrl_IO_MacID[2].GetCurSel();
	TRACE("get I/O 0 macID = %d \n" ,m_parent->m_ucIO_MacID[2]);
}


void CIOBoardSet::OnCbnSelchangeComboIoMacid3()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	UpdateData(TRUE);
	m_parent->m_ucIO_MacID[3] = m_ctrl_IO_MacID[3].GetCurSel();
	TRACE("get I/O 1 macID = %d \n" ,m_parent->m_ucIO_MacID[3]);
}
void CIOBoardSet::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if(nIDEvent == 0)
	{
		for(UINT i = 0 ; i < (sizeof(m_fgenableIO_borad) /sizeof(m_fgenableIO_borad[0])) ; i++)  // modify 
		{
			if(m_parent->m_fgenableIO_borad[i])
			{
				if(!fgboard0btnChg[i])
				{
					SetInputButton(i, m_parent->m_uiIOStatus[i]);
				}
			}
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}

UINT CIOBoardSet::InputBoardthreadFun(LPVOID LParam)
{
	CIOBoardSet* pioset = (CIOBoardSet*)LParam;	
	pioset->DoInuputEvent();
	TRACE("end thread\n");
	return 0;
}
void CIOBoardSet::DoInuputEvent(void)
{
		LARGE_INTEGER nFreq;
		LARGE_INTEGER nBeginTime;
		LARGE_INTEGER nEndTime;
		double dtime;
	
		QueryPerformanceFrequency(&nFreq);
		while(m_input_threading)
		{
			QueryPerformanceCounter(&nBeginTime);
			//if((!m_parent->m_blWaiitCmdPROGRESS) && (!m_parent->m_WaitCmdAck))
			{
				for(UINT i = 0 ; i < (sizeof(m_fgenableIO_borad) /sizeof(m_fgenableIO_borad[0])) ; i++)  // modify 
				{
					if(m_parent->m_fgenableIO_borad[i] && m_parent->m_fgInput[i])
					{
// 						m_parent->SetWaitCMDAcking(1);
// 						m_parent->RTEX_CMD_GET_INPUT(m_parent->m_ucIO_MacID[i]);
// 						if(m_parent->WaitCMDAcking())
// 						{
// 							MessageBox("CMD Fail");
// 							break;
// 						}
						if(!fgboard0btnChg[i])
						{
 						  SetInputButton(i, m_parent->m_uiIOStatus[i]);
						}
					}
				}
			}
			do 
			{
				if(!m_input_threading) break;
			    QueryPerformanceCounter(&nEndTime);
			    dtime=(double)(nEndTime.QuadPart-nBeginTime.QuadPart)/(double)nFreq.QuadPart;
			} while (dtime < (double)(0.2)); //while 直到間隔時間到
		}
		TRACE("DoInuputEvent end\n");
		m_threadEnd = TRUE;

}
void CIOBoardSet::SetInputButton(UINT number, UINT value)
{
	//TRACE("I/O Board = %d , value = %d \n", number, value);
	m_parent->m_uiIOStatus[number] = value;
	for (UINT i = 0 ; i < 32 ; i++)
	{
		CButton* ptn = (CButton*) GetDlgItem(IOBoardstatus[number][i]);
		if(value & (1 << i))		ptn->SetCheck(1);
		else ptn->SetCheck(0);
	}
}
void CIOBoardSet::OnOK()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	//CDialogEx::OnOK();
}
void CIOBoardSet::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	TRACE("on closed \n");
	OnBnClickedExit();
	//CDialogEx::OnClose();
}
void CIOBoardSet::OnCancel()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別

	//CDialogEx::OnCancel();
}


void CIOBoardSet::OnBnClickedExit()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	MSG msg;
	m_input_threading = FALSE;			
	TRACE("threadend while \n");
	while(!m_threadEnd)
	{
		GetMessage(&msg,NULL,0,0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	};
	TRACE("on exit \n");
	CDialogEx::OnOK();
}
void  CIOBoardSet::OnCHKClicked0_31(UINT uiCmdID)
{
	unsigned char ucbitchg = uiCmdID-IDC_CHECK_BIT0;
	SetOutBoardFunction(0,ucbitchg);
}
void  CIOBoardSet::OnCHKClicked32_63(UINT uiCmdID)
{
	unsigned char ucbitchg = uiCmdID-IDC_CHECK_BIT32;
	SetOutBoardFunction(1,ucbitchg);
}
void  CIOBoardSet::OnCHKClicked64_95(UINT uiCmdID)
{
	unsigned char ucbitchg = uiCmdID-IDC_CHECK_BIT64;
	SetOutBoardFunction(2,ucbitchg);
}
void  CIOBoardSet::OnCHKClicked96_127(UINT uiCmdID)
{
	unsigned char ucbitchg = uiCmdID-IDC_CHECK_BIT96;
	SetOutBoardFunction(3,ucbitchg);
}

void CIOBoardSet::SetOutBoardFunction(unsigned char ucBoardNum, unsigned char ucbitchg)
{
	UINT  uicurrentstatus =  m_parent->m_uiIOStatus[ucBoardNum];
	fgboard0btnChg[ucBoardNum] = TRUE;
	//TRACE("board %d -> ID: %d is clicked\n ",ucBoardNum,ucbitchg);

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
	m_parent->RTEX_CMD_SET_OUTPUT(m_parent->m_ucIO_MacID[ucBoardNum],uicurrentstatus);	
	//m_parent->RTEX_CMD_SET_OUTPUT(m_parent->m_ucIO_MacID[ucBoardNum],ucbitchg,(uicurrentstatus & (1 << ucbitchg))?TRUE : FALSE);	
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
		if(m_parent->m_uiIOStatus[ucBoardNum] == uicurrentstatus)
		{
			//TRACE("ok\n");
			break;
		}
		//GetMessage(&msg,NULL,0,0);
		//TranslateMessage(&msg);
		//DispatchMessage(&msg);
		QueryPerformanceCounter(&nEndTime);
		dtime=(double)(nEndTime.QuadPart-nBeginTime.QuadPart)/(double)nFreq.QuadPart;
	} while (dtime < (double)(m_parent->m_uiShowPosTimeSlot * 4 * 0.001)); ////delay 2 times m_uiShowPosTimeSlot
	fgboard0btnChg[ucBoardNum] = FALSE;
}

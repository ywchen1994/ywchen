// RTEXFormView.cpp : 實作檔
//

#include<iostream>
/***/
#include "stdafx.h"
#include "RTEXTool.h"
#include "RTEXFormView.h"
#include "AxisSetting.h"
#include "IOBoardSet.h"
#include "IOBoardSetV2.h"
#include "OneAxisSetting.h"
#include "UniversalCmd.h"
#include "MainFrm.h"
#include "DriverRegister.h"
#include "SystemID.h"
#include "OpenJogDialog.h"
#include <list>
#pragma comment(lib,"IPHLPAPI.lib")

// CRTEXFormView

IMPLEMENT_DYNCREATE(CRTEXFormView, CFormView)

CRTEXFormView::CRTEXFormView()
	: CFormView(CRTEXFormView::IDD)
	, m_Recieve_count(0)
	, m_uschrReciveData(NULL)
	, m_GetQSIMSG(false)
	, m_QSIMSGLength(0)
	, m_WaitCmdAck(false)
	, m_CurrentCmd(0)
	, m_intX_Axis_pos(0)
	, m_intY_Axis_pos(0)
	, m_intZ_Axis_pos(0)
	, m_intU_Axis_pos(0)
	, m_uintVmax(100)
	, m_uintCmdtimerCnt(0)
	, m_uiEnableAxis(3)
	, m_intcmbxSpdType(1)
    , m_fVmax((float)0.06)
	, m_uiTa(500)
	, m_boolwaitCmdEvent(false)
	, boolunSetSpdParamter(false)
	, m_boolRTEX_Configurated(false)
	, boolunSetAxisParamter(false)
	, m_intDvrReturnData(0)
	, m_bClientConnected(false)
	, m_TestMode(false)
	, m_connect2RenesasOK(false)
	, m_blWaiitCmdPROGRESS(false)
	, m_ucCMDPROGRESSCase(0)
	, m_uintCmdProgressTimer(0)
	, m_uiSelectedSlaves(0)
	,m_lpShowPosThread(nullptr)
	,m_blShowPos(FALSE)
	, m_uiShowPosTimeSlot(20)
	,m_intSpecialCMDReturnData(0)
	, m_fgInQCMD(false)
	, m_iXpos_um(0)
	, m_iYpos_um(0)
	, m_iZpos_um(0)
	, m_fx(0)
	, m_fy(0)
	, m_fz(0)
	, m_fYaw(0)
	, m_ftheta1(0)
	, m_ftheta2(0)
	, m_ftheta4(0)
	, m_fgScaraRobot(FALSE)
	, m_fScaraRobot_L1(325)
	, m_fScaraRobot_L2(225)
	, m_fScaraRobot_Height(294)
	, m_ucScaraRobot_Arm_dir(1)
	, m_uiInputBoardStatus(0)
	, m_ucInputBoardID(0)
	, m_lpCheckErrThread(nullptr)
	, m_blCheckErr(FALSE)
	, m_uiCheckErrTimeSlot(200)
	, m_intFont_size(100)
	, m_pFont(nullptr)
	, m_iJog_MaxRPM(3000)
	, m_fJog_Speed(50)
	, m_fgHIFCmdBufferFull(false)
	, m_fgQcmdImmediatelyStop(false)
	, m_ucSystemIDreturndata(nullptr)
	, m_chkSum(0)
	, m_Show_Scara_Arm(false)
	, fgStopReceiveThread(FALSE)
	, m_pReceiveThread(nullptr)
	, m_fgGetdebugRS232(false)
	, m_uiPointCnt(0)
	, m_fgStandaloneCode(FALSE)
{
	m_uschrReciveData = new unsigned char[MaxWriteBuff];
	m_lpucCharForSendCmd = new unsigned char[MaxWriteBuff];
	m_ucSystemIDreturndata = new unsigned char[MaxWriteBuff];
	m_uchandleData = new unsigned char[MaxWriteBuff];
	for(int i= 0 ; i< 6 ; i++)
	{
		m_Axis[i] = i + 12;
		m_Axis_Pitch[i] = 5;
		m_Axis_GearRatio[i] = 10000;
		m_MonitorSingal[i] = 0;
		m_Axis_POTLimit[i] =  (float)200;
		m_Axis_NOTLimit[i] = (float)-200;
		m_Axis_Type[i] = 1;  //XYZ 平台 設1
		m_Axis_GearRatio_Mech[i] = 1; //XYZ 平台 設1		
		m_Axis_Encoder_OFS[i] = 0;
		memset(&m_axisStatus[i],0, sizeof(m_axisStatus[i])); 
		m_response_vel[i] = 0;
		m_response_torq[i] = 0;

	}	
	for(UINT i = 0 ; i < (sizeof(m_fgenableIO_borad) / sizeof(m_fgenableIO_borad[0])) ; i++)  // modify 
	{
	  m_fgenableIO_borad[i] = (i >= 2)? FALSE : TRUE;
	  m_fgInput[i] = (BOOL)(i%2);
	  m_ucIO_MacID[i] = (32 + 8 - i * 2) % 32;
	  m_uiIOStatus[i] = 0 ;
	}
	m_HomeJ[0] = 60;
	m_HomeJ[1] = -120;
	m_HomeJ[2] = 15;
	m_HomeJ[3] = 0;
}

CRTEXFormView::~CRTEXFormView()
{
	TRACE("~deconstruter\n");
	if(m_uschrReciveData) 
	{
		delete [] m_uschrReciveData;
		m_uschrReciveData = nullptr;
	}
	if(m_lpucCharForSendCmd)
		delete [] m_lpucCharForSendCmd;
	if(m_pFont)
		delete m_pFont;
	if(m_ucSystemIDreturndata)
		delete [] m_ucSystemIDreturndata;
	if(m_uchandleData)
		delete [] m_uchandleData;
}

void CRTEXFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ComboBox_Cmd, m_ctrlCMDselect);
	DDX_Text(pDX, IDC_EDIT_Vmax, m_uintVmax);
	DDX_Control(pDX, IDC_LIST_Status, m_ctrllistStatus);
	DDX_Control(pDX, IDC_CHECK_TestMode, m_ctrlTestMode);
	DDX_Control(pDX, IDC_IPADDRESS, m_ctrlIPAddr);
	DDX_Check(pDX, IDC_CHECK_Scara, m_fgScaraRobot);
	DDX_Control(pDX, IDC_TAB_LOG, m_ctrlTAB_LOG);
	DDX_Control(pDX, IDC_LIST_Log_History, m_ctrllistLog_History);
	DDX_Control(pDX, IDC_LIST_ServerCommend, m_LIST_ServerCommend);
}

BEGIN_MESSAGE_MAP(CRTEXFormView, CFormView)
	ON_BN_CLICKED(IDC_Button_Connect, &CRTEXFormView::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_Disconnect, &CRTEXFormView::OnBnClickedButtonDisconnect)
	ON_BN_CLICKED(IDC_BUTTON_Set_RTEX_Configuration, &CRTEXFormView::OnBnClickedButtonSetRtexConfiguration)
	ON_BN_CLICKED(IDC_BUTTON_X_Axis, &CRTEXFormView::OnBnClickedButtonXAxis)
	ON_BN_CLICKED(IDC_BUTTON_Y_Axis, &CRTEXFormView::OnBnClickedButtonYAxis)
	ON_CBN_SELCHANGE(IDC_ComboBox_Cmd, &CRTEXFormView::OnCbnSelchangeComboboxCmd)
	ON_BN_CLICKED(IDC_Button_ExecuteCmd, &CRTEXFormView::OnBnClickedButtonExecutecmd)
	ON_BN_CLICKED(IDC_CHECK_ServoOn, &CRTEXFormView::OnBnClickedCheckServoon)
	ON_BN_CLICKED(IDC_BUTTON_ClearMsg, &CRTEXFormView::OnBnClickedButtonClearmsg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_ClearView, &CRTEXFormView::OnBnClickedButtonClearview)
	ON_BN_CLICKED(IDC_BUTTON_SetXvelocity, &CRTEXFormView::OnBnClickedButtonSetxvelocity)
	ON_BN_CLICKED(IDC_BUTTON_SetYvelocity, &CRTEXFormView::OnBnClickedButtonSetyvelocity)
	ON_BN_CLICKED(IDC_BUTTON_Z_Axis, &CRTEXFormView::OnBnClickedButtonZAxis)
	ON_BN_CLICKED(IDC_BUTTON_SetZvelocity, &CRTEXFormView::OnBnClickedButtonSetzvelocity)
	ON_BN_CLICKED(IDC_BUTTON_ParameterSet, &CRTEXFormView::OnBnClickedButtonParameterset)	
	ON_BN_CLICKED(IDC_BUTTON_Universal_Cmd, &CRTEXFormView::OnBnClickedButtonUniversalCmd)
	ON_MESSAGE(WM_SendCmd2StatusBar, &CRTEXFormView::OnSendcmd2statusbar)
	ON_BN_CLICKED(IDC_BUTTON_OpenDriverRegDialog, &CRTEXFormView::OnBnClickedButtonOpendriverregdialog)
	ON_BN_CLICKED(IDC_BUTTON_OpenSpecialCMDDialog, &CRTEXFormView::OnBnClickedButtonOpenspecialcmddialog)
	ON_BN_CLICKED(IDC_CHECK_TestMode, &CRTEXFormView::OnBnClickedCheckTestmode)
	ON_MESSAGE(WM_SendSocketClientclosed, &CRTEXFormView::OnSendsocketclientclosed)
	ON_BN_CLICKED(IDC_CHECK_ShowPostionRepeatly, &CRTEXFormView::OnBnClickedCheckShowpostionrepeatly)
	ON_BN_CLICKED(IDC_Button_ExecuteQCmd, &CRTEXFormView::OnBnClickedButtonExecuteqcmd)
	ON_BN_CLICKED(IDC_BUTTON_Dvr_Reset, &CRTEXFormView::OnBnClickedButtonDvrReset)
	ON_BN_CLICKED(IDC_BUTTON_Scara_home, &CRTEXFormView::OnBnClickedButtonScarahome)
	ON_BN_CLICKED(IDC_BUTTON_U_Axis, &CRTEXFormView::OnBnClickedButtonUAxis)
	ON_BN_CLICKED(IDC_CHECK_Scara, &CRTEXFormView::OnBnClickedCheckScara)
	ON_BN_CLICKED(IDC_BUTTON_IO_Set, &CRTEXFormView::OnBnClickedButtonIoSet)
	ON_BN_CLICKED(IDC_Check_Alarm_Warning, &CRTEXFormView::OnBnClickedCheckAlarmWarning)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_LOG, &CRTEXFormView::OnTcnSelchangeTabLog)
	ON_BN_CLICKED(IDC_BUTTON_IncreaseLogFontSize, &CRTEXFormView::OnBnClickedButtonIncreaselogfontsize)
	ON_BN_CLICKED(IDC_BUTTON_DecreaseLogFontSize, &CRTEXFormView::OnBnClickedButtonDecreaselogfontsize)
	ON_BN_CLICKED(IDC_BUTTON_Warning_Reset, &CRTEXFormView::OnBnClickedButtonWarningReset)
	ON_BN_CLICKED(IDC_BUTTON_Motion_Stop, &CRTEXFormView::OnBnClickedButtonMotionStop)
	ON_BN_CLICKED(IDC_BUTTON_Motion_Run, &CRTEXFormView::OnBnClickedButtonMotionRun)
	ON_BN_CLICKED(IDC_CHECK_DisRedraw, &CRTEXFormView::OnBnClickedCheckDisredraw)
	ON_MESSAGE(WM_SendredrawInthread, &CRTEXFormView::OnSendredrawinthread)
	ON_BN_CLICKED(IDC_CHECK_DisableDebugRS232, &CRTEXFormView::OnBnClickedCheckDisabledebugrs232)
	ON_BN_CLICKED(IDC_CHECK_ShowPosTimeData, &CRTEXFormView::OnBnClickedCheckShowpostimedata)
	ON_BN_CLICKED(IDC_CHECK_FreeDynamicBrk, &CRTEXFormView::OnBnClickedCheckFreedynamicbrk)
	ON_BN_CLICKED(IDC_BUTTON_SystemID, &CRTEXFormView::OnBnClickedButtonSystemid)
	ON_BN_CLICKED(IDC_CHECK_Event, &CRTEXFormView::OnBnClickedCheckEvent)
	ON_BN_CLICKED(IDC_BUTTON_TEST_CODE, &CRTEXFormView::OnBnClickedButtonTestCode)
	ON_BN_CLICKED(IDC_CHECK_Show_Scara, &CRTEXFormView::OnBnClickedCheckShowScara)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_SendviewStop, &CRTEXFormView::OnSendviewstop)
	ON_BN_CLICKED(IDC_BUTTON_DumpDebugMsg, &CRTEXFormView::OnBnClickedButtonDumpdebugmsg)
	ON_BN_CLICKED(IDC_CHECK_DumpDebugMsg, &CRTEXFormView::OnBnClickedCheckDumpdebugmsg)
	ON_BN_CLICKED(IDC_BUTTON_UPATE_ScaraPara, &CRTEXFormView::OnBnClickedButtonUpateScarapara)
	ON_MESSAGE(WM_SendUpdateJoint_Pos, &CRTEXFormView::OnSendUpdateJoint_Pos)
	ON_MESSAGE(WM_Socket_Receive, &CRTEXFormView::OnSocket_Receive)
	ON_BN_CLICKED(IDC_BUTTON_ConnectServer, &CRTEXFormView::OnBnClickedButtonConnectserver)
	ON_BN_CLICKED(IDC_BUTTON_Gripper, &CRTEXFormView::OnBnClickedButtonGripper)
	ON_BN_CLICKED(IDC_LIST_Delete, &CRTEXFormView::OnBnClickedListDelete)
END_MESSAGE_MAP()


// CRTEXFormView 診斷

#ifdef _DEBUG
void CRTEXFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CRTEXFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CRTEXFormView 訊息處理常式


void CRTEXFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
    
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	m_ctrlCMDselect.ResetContent();
	m_ctrllistStatus.ResetContent();
	CString str;
	m_ctrlCMDselect.AddString("CMD_1_AXIS_MOTION");
	m_ctrlCMDselect.AddString("CMD_2_AXES_LINEAR_MOTION");
	m_ctrlCMDselect.AddString("LINEXYZ");
	m_ctrlCMDselect.AddString("Circle Mode");
	m_ctrlCMDselect.AddString("Helix Mode");
	m_ctrlCMDselect.AddString("XYZU_MotionJOG");
	m_ctrlCMDselect.AddString("J1234_MotionJOG");
	m_ctrlCMDselect.AddString("Scara_J4_Rotate");	
	m_ctrlCMDselect.AddString("Wait_IO_Board_Status");
	m_ctrlCMDselect.AddString("SET_OUTPUT_In_Queue");	
	m_ctrlCMDselect.AddString("Delay_Time_In_Queue");  //10
	m_ctrlCMDselect.AddString("Motion Pause");  //11
	m_ctrlCMDselect.AddString("Motion Limit Release"); //12
	m_ctrlCMDselect.AddString("Motion Stop");  //13
	m_ctrlCMDselect.AddString("Motion_Run"); // 14
	m_ctrlCMDselect.AddString("Motion_Status"); // 15
	m_ctrlCMDselect.AddString("Open Jog Mode Dialog"); //16
	m_ctrlCMDselect.SetCurSel(2);


	GetDlgItem(IDC_Button_Connect)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_Disconnect)->EnableWindow(FALSE);
	GetDlgItem((IDC_BUTTON_ParameterSet))->EnableWindow(FALSE);	
	GetDlgItem(IDC_BUTTON_Set_RTEX_Configuration)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_ServoOn)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_FreeDynamicBrk)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_TestMode)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_Dvr_Reset)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_Warning_Reset)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_Scara)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_Scara_home)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_IO_Set)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_ShowPostionRepeatly)->EnableWindow(FALSE);
	GetDlgItem(IDC_Check_Alarm_Warning)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_DisableDebugRS232)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_ShowPosTimeData)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DumpDebugMsg)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_DumpDebugMsg)->EnableWindow(FALSE);	
	pDoc= (CRTEXToolDoc*)GetDocument();
	m_ctrlTestMode.SetCheck(FALSE);
	m_TestMode = FALSE;

	m_ctrlTAB_LOG.InsertItem(0, "Status Msg");
	m_ctrlTAB_LOG.InsertItem(1, "Log history");
	GetDlgItem(IDC_LIST_Status)->ShowWindow(TRUE);
	GetDlgItem(IDC_LIST_Log_History)->ShowWindow(FALSE);
	SetRecieveMsgFontSize();

	//送出要用水平軸的訊息
	::SendMessage(m_ctrllistStatus.m_hWnd,LB_SETHORIZONTALEXTENT,1024,0);
	//1024是最長寬度，可以每次在新增字串以後再重新計算需要多少;
	m_ctrllistStatus.SetScrollRange(SB_HORZ,0,1024);
	//強制顯示水平軸
	m_ctrllistStatus.ShowScrollBar(SB_HORZ);

	//送出要用水平軸的訊息
	::SendMessage(m_ctrllistLog_History.m_hWnd,LB_SETHORIZONTALEXTENT,8192,0);
	//1024是最長寬度，可以每次在新增字串以後再重新計算需要多少;
	m_ctrllistLog_History.SetScrollRange(SB_HORZ,0,8192);
	//強制顯示水平軸
	m_ctrllistLog_History.ShowScrollBar(SB_HORZ);

	UpdateData(FALSE);
	m_threadPara.m_lpPara = this;
	m_threadPara.m_blthreading = FALSE;

	m_clientSocket.SetParent(this);
	m_ctrlIPAddr.SetAddress(192,168,1,10);
	SetDlgItemText(IDC_EDIT_Port,"6188");	
	CButton* pbtn = (CButton*)GetDlgItem(IDC_CHECK_Scara);
	pbtn->SetCheck(1);
	OnBnClickedCheckScara();
	pbtn = (CButton*)GetDlgItem(IDC_CHECK_DisRedraw);
	pbtn->SetCheck(1);
	OnBnClickedCheckDisredraw();
	//ShellExecute(NULL,"open","DebugMsg.exe",NULL,NULL,SW_SHOWNORMAL); 
	//GetDlgItem(IDC_CHECK_Event)->EnableWindow(FALSE);
	CMainFrame* pMainWnd = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	pMainWnd->m_myFormView = this;
	m_pReceiveThread = AfxBeginThread(&CRTEXFormView::HandleMSGThread,this);
	SetDlgItemText(IDC_EDIT_Motion_Tm,"0");	
	char hostname[256];
	gethostname(hostname, sizeof(hostname));
	str.Format("Local Name = %s",hostname);
	ShowStatus(str);
	OnCbnSelchangeComboboxCmd();
	IP_ADAPTER_ADDRESSES AdapterAddress[16];
	DWORD dwbuflen = sizeof(AdapterAddress);
	DWORD dwStatus = GetAdaptersAddresses(AF_INET,NULL,NULL,AdapterAddress,&dwbuflen);
	PIP_ADAPTER_ADDRESSES pAdapterAddress = AdapterAddress;
	PIP_ADAPTER_UNICAST_ADDRESS pUnicastAddr;
	CString substr;
	do 
	{		
		if((pAdapterAddress->IfType == IF_TYPE_ETHERNET_CSMACD)	|| (pAdapterAddress->IfType == IF_TYPE_IEEE80211))
		{
			str.Format("Name = %s (%s) : ",(CString)pAdapterAddress->FriendlyName,(pAdapterAddress->OperStatus == 1? "OK" : "NG"));
			pUnicastAddr = pAdapterAddress->FirstUnicastAddress;
			while(pUnicastAddr)
			{
				PSOCKADDR_IN paddrin = (PSOCKADDR_IN)pUnicastAddr->Address.lpSockaddr;
				substr.Format("IP = %s ",inet_ntoa(paddrin->sin_addr));
				str += substr;			
				pUnicastAddr = pUnicastAddr->Next;
			}
// 			str += " :  MAC Address = ";
// 			for(ULONG i = 0 ; i < pAdapterAddress->PhysicalAddressLength; i++)
// 			{
// 				if(i != 0)  str += "- ";
// 				substr.Format("%02X ",pAdapterAddress->PhysicalAddress[i]);
// 				str += substr;
// 			}
			ShowStatus(str);
		}
		pAdapterAddress = pAdapterAddress->Next;
	} while (pAdapterAddress);
}

void CRTEXFormView::ClientOnReceive()
{
	int iBufSize = 512;	
	// 接收信息，並返回接收到的字節數
	m_QSIMSGLength = m_clientSocket.Receive(m_uschrReciveData, iBufSize);
	// 判斷是否有錯誤發生
	if (m_QSIMSGLength == SOCKET_ERROR)
	{
		TRACE("socket error\r\n");
		return;
	}
	else
	{
		// 利用NULL來截去接收緩存區中多餘的字符
		CSingleLock SingleLock(&Mutex);
		SingleLock.Lock();
		for(UINT i = 0 ; i < m_QSIMSGLength ; i ++)
		{
#ifdef RingBufferUsed
			if(m_rbuffer.BufferIsFull())
			{
				MessageBox("Ring buffer Full");
			}
			else
			{
				m_rbuffer.ADD(m_uschrReciveData[i]);
			}
#else
			m_ReceiveCharList.push(m_uschrReciveData[i]);
#endif
		}
		SingleLock.Unlock();
		//TRACE("receiveBuffer size = %d\r\n",m_ReceiveCharList.size());
		return;  // handle in receivethread
		/////////////////////////////////////////////////////////////
		if(m_uschrReciveData[0] != 'Q')
		{
			ShowStatus("look");
			ShowStatus("look");
			ShowStatus("look");
		}
		if((m_uschrReciveData[0] == 'Q') && (m_uschrReciveData[1] == 'S') /*&& (m_uschrReciveData[2] == 'I')*/)
		{
			/*
			CString str;
			TRACE("on recieve = ");
			for (UINT i = 0 ; i < m_QSIMSGLength ; i++)
			{
				str.Format("0x%02X ",m_uschrReciveData[i]);
				TRACE(str);
			}
			TRACE("\n");
			*/
			if((m_uschrReciveData[5]+6) != m_QSIMSGLength)
			TRACE("combinreceive\r\n");

			HandleQSIMSG(m_uschrReciveData, m_QSIMSGLength);  // 處理QSI MSG 
		}
	}	

}
void CRTEXFormView::ClientOnSocketClose()
{

	TRACE("closed event happened\n");
	OnBnClickedButtonDisconnect();
}
void CRTEXFormView::ClientOnConnect()
{
	m_bClientConnected = TRUE;
	Connect7216OK();	
	ShowStatus("connect ok!!");
}
void CRTEXFormView::ClientOnSend()
{
}


void CRTEXFormView::ShowStatus(const CString& str)
{
	m_ctrllistStatus.AddString(str);
	//m_ctrllistStatus.SetCurSel(m_ctrllistStatus.GetCount()-1);	
	m_ctrllistStatus.PostMessage(LB_SETCURSEL,m_ctrllistStatus.GetCount()-1,0);
}
void CRTEXFormView::SaveSend2Loghistory(const unsigned char* string)
{
	if((m_blShowPos && ((string[4] == CMD_GET_AXIS_POSITION)||(string[4] == CMD_Get_New_AXIS_POSITION))) || 
	   (m_blCheckErr && (string[4] == CMD_Driver_Alarm_Warning))) return;
	CString str, partstr;
	if(string[3] == 'A') str.Format("Ack Event : %02X  Para : ", string[4]);
	else str.Format("CMD : %02X  Para : ", string[4]);
	
	for( int i = 0 ; i < string [5] ; i++)
	{
		partstr.Format("%02X  ",string[6+i]);
		str += partstr;
	}
	m_ctrllistLog_History.AddString(str);
    m_ctrllistLog_History.SetCurSel(m_ctrllistLog_History.GetCount()-1);    
}
void CRTEXFormView::SaveReceive2Loghistory(const unsigned char* string)
{
	if((m_blShowPos && ((string[4] == CMD_GET_AXIS_POSITION)||(string[4] == CMD_Get_New_AXIS_POSITION))) || 
		(m_blCheckErr && (string[4] == CMD_Driver_Alarm_Warning))) return;
	if((string[3]=='I') && ((string[4]==EVENT_Send_NEW_Postion)||(string[4] == EVENT_Error_Occurred))) return;
	CString str, partstr;
	str.Format("  receive : %c  Para : ", string[3]);
	for( int i = 0 ; i < string [5] ; i++)
	{
		partstr.Format("%02X  ",string[6+i]);
		str += partstr;
	}
	m_ctrllistLog_History.AddString(str);
	m_ctrllistLog_History.SetCurSel(m_ctrllistLog_History.GetCount()-1);	
}
void CRTEXFormView::OnCbnSelchangeComboboxCmd()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	UpdateData(TRUE);
	GetDlgItem(IDC_EDIT_Para1)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_Para1)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_Para2)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_Para2)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_Para3)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_Para3)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_Para4)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_Para4)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_Para5)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_Para5)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_Para6)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_Para6)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_Para7)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_Para7)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SetXvelocity)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SetYvelocity)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SetZvelocity)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_Vmax)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_Vmax_Unit)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_Vmax)->ShowWindow(FALSE);
	CButton* btn = (CButton*)GetDlgItem(IDC_CHECK_CW);
	btn->ShowWindow(FALSE);

	SetDlgItemInt(IDC_EDIT_Para1,0);
	SetDlgItemInt(IDC_EDIT_Para2,0);
	SetDlgItemInt(IDC_EDIT_Para3,0);
	SetDlgItemInt(IDC_EDIT_Para4,0);
	SetDlgItemInt(IDC_EDIT_Para5,0);
	SetDlgItemInt(IDC_EDIT_Para6,0);
	SetDlgItemInt(IDC_EDIT_Para7,0);
	SetDlgItemText(IDC_STATIC_Para1,"Para1");
	SetDlgItemText(IDC_STATIC_Para2,"Para2");
	SetDlgItemText(IDC_STATIC_Para3,"Para3");
	SetDlgItemText(IDC_STATIC_Para4,"Para4");
	SetDlgItemText(IDC_STATIC_Para5,"Para5");
	SetDlgItemText(IDC_STATIC_Para6,"Para6");
	SetDlgItemText(IDC_STATIC_Para7,"Para7");
	int CmdSelect(0);
	CmdSelect = m_ctrlCMDselect.GetCurSel();
	if(CmdSelect == 0)
	{
		GetDlgItem(IDC_EDIT_Para1)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para1)->ShowWindow(TRUE);
		SetDlgItemText(IDC_STATIC_Para1,"Cmd1(Pulse)");
		GetDlgItem(IDC_BUTTON_SetXvelocity)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Vmax)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Vmax_Unit)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_Vmax)->ShowWindow(TRUE);

	}
	else if (CmdSelect ==1)
	{
		GetDlgItem(IDC_EDIT_Para1)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para1)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_Para2)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para2)->ShowWindow(TRUE);
		SetDlgItemText(IDC_STATIC_Para1,"Cmd1(Pulse)");
		SetDlgItemText(IDC_STATIC_Para2,"Cmd2(Pulse)");
		GetDlgItem(IDC_BUTTON_SetXvelocity)->ShowWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SetYvelocity)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Vmax)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Vmax_Unit)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_Vmax)->ShowWindow(TRUE);

	}
	else if((CmdSelect == 2) || (CmdSelect == 5))
	{
		GetDlgItem(IDC_EDIT_Para1)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para1)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_Para2)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para2)->ShowWindow(TRUE);
		if(m_uiEnableAxis >=3)
		{
			GetDlgItem(IDC_EDIT_Para3)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_Para3)->ShowWindow(TRUE);
		}
		if(m_uiEnableAxis >= 4)
		{
			GetDlgItem(IDC_EDIT_Para6)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_Para6)->ShowWindow(TRUE);
		}
		SetDlgItemText(IDC_STATIC_Para1,"TarX(mm)");
		SetDlgItemText(IDC_STATIC_Para2,"TarY(mm)");
		SetDlgItemText(IDC_STATIC_Para3,"TarZ(mm)");
		SetDlgItemText(IDC_STATIC_Para6,"TarU(deg)");
	}
	else if(CmdSelect == 3)
	{
		MessageBox("模式選擇\n  0: CircleXY\n  1: CircleYZ\n  2: CircleXZ");
		GetDlgItem(IDC_EDIT_Para1)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para1)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_Para2)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para2)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_Para3)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para3)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_Para4)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para4)->ShowWindow(TRUE);
		if(m_uiEnableAxis >= 4)
		{
  		  GetDlgItem(IDC_EDIT_Para6)->ShowWindow(TRUE);
		  GetDlgItem(IDC_STATIC_Para6)->ShowWindow(TRUE);
		}
		CButton* btn = (CButton*)GetDlgItem(IDC_CHECK_CW);
		btn->ShowWindow(TRUE);
		SetDlgItemText(IDC_STATIC_Para1,"圓心(mm)");
		SetDlgItemText(IDC_STATIC_Para2,"圓心(mm)");
		SetDlgItemText(IDC_STATIC_Para3,"角度(度)");
		SetDlgItemText(IDC_STATIC_Para4,"模式");
		SetDlgItemText(IDC_STATIC_Para6,"TarU(deg)");
		SetDlgItemInt(IDC_EDIT_Para3,360);
	}
	else if(CmdSelect == 4)
	{
		MessageBox("模式選擇\n  0: HelixXY_Z\n  1: HelixYZ_X\n  2: HelixXZ_Y");
		GetDlgItem(IDC_EDIT_Para1)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para1)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_Para2)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para2)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_Para3)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para3)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_Para4)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para4)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_Para5)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para5)->ShowWindow(TRUE);
		if(m_uiEnableAxis >= 4)
		{
		  GetDlgItem(IDC_EDIT_Para6)->ShowWindow(TRUE);
		  GetDlgItem(IDC_STATIC_Para6)->ShowWindow(TRUE);
		}
		CButton* btn = (CButton*)GetDlgItem(IDC_CHECK_CW);
		btn->ShowWindow(TRUE);
		SetDlgItemText(IDC_STATIC_Para1,"圓心(mm)");
		SetDlgItemText(IDC_STATIC_Para2,"圓心(mm)");
		SetDlgItemText(IDC_STATIC_Para3,"圈數");
		SetDlgItemText(IDC_STATIC_Para4,"模式");
		SetDlgItemText(IDC_STATIC_Para5,"進給(mm)");
		SetDlgItemText(IDC_STATIC_Para6,"TarU(deg)");
		SetDlgItemInt(IDC_EDIT_Para3,1);		
	}
	else if(CmdSelect == 6)
	{
		GetDlgItem(IDC_EDIT_Para1)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para1)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_Para2)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para2)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_Para3)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para3)->ShowWindow(TRUE);
		if(m_uiEnableAxis >= 4)
		{
		  GetDlgItem(IDC_EDIT_Para6)->ShowWindow(TRUE);
		  GetDlgItem(IDC_STATIC_Para6)->ShowWindow(TRUE);
		}
		SetDlgItemText(IDC_STATIC_Para1,"TarJX(deg)");
		SetDlgItemText(IDC_STATIC_Para2,"TarJY(deg)");
		SetDlgItemText(IDC_STATIC_Para3,"TarZ(mm)");
		SetDlgItemText(IDC_STATIC_Para6,"TarJU(deg)");
	}
	else if (CmdSelect == 7)
	{
		GetDlgItem(IDC_EDIT_Para1)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para1)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_Para2)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para2)->ShowWindow(TRUE);
		SetDlgItemText(IDC_STATIC_Para1,"圈數");
		SetDlgItemText(IDC_STATIC_Para2,"Rev/min");
	}
	else if ((CmdSelect == 8) || (CmdSelect == 9))
	{
		GetDlgItem(IDC_EDIT_Para1)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para1)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_Para2)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para2)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_Para3)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para3)->ShowWindow(TRUE);
		SetDlgItemText(IDC_STATIC_Para1,"Mac ID");
		SetDlgItemText(IDC_STATIC_Para2,"Bit");
		SetDlgItemText(IDC_STATIC_Para3,"Level");
		if(CmdSelect == 8 )
		{
			GetDlgItem(IDC_EDIT_Para4)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_Para4)->ShowWindow(TRUE);
			SetDlgItemText(IDC_STATIC_Para4,"Timeout");
			GetDlgItem(IDC_EDIT_Para5)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_Para5)->ShowWindow(TRUE);
			SetDlgItemText(IDC_STATIC_Para5,"TO_Mac ID");
			GetDlgItem(IDC_EDIT_Para6)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_Para6)->ShowWindow(TRUE);
			SetDlgItemText(IDC_STATIC_Para6,"TO_Bit");
			GetDlgItem(IDC_EDIT_Para7)->ShowWindow(TRUE);
			GetDlgItem(IDC_STATIC_Para7)->ShowWindow(TRUE);
			SetDlgItemText(IDC_STATIC_Para7,"TO_level");
		}
	}
	else if(CmdSelect ==10)
	{
		GetDlgItem(IDC_EDIT_Para1)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para1)->ShowWindow(TRUE);
		SetDlgItemText(IDC_STATIC_Para1,"DelayTime (s)");
	}
	else if((CmdSelect == 11) || (CmdSelect == 12))
	{
		GetDlgItem(IDC_EDIT_Para1)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Para1)->ShowWindow(TRUE);
		if(CmdSelect ==11)		SetDlgItemText(IDC_STATIC_Para1,"Pause");
		else SetDlgItemText(IDC_STATIC_Para1,"Limit Release");
	}
	else if ((CmdSelect == 13) || (CmdSelect == 14) || (CmdSelect == 15))
	{
		// show nothing
	}
	if(CmdSelect == 5)
	{
		CString str;
		GetDlgItemText(IDC_EDIT_X_Axis,str);
	    float ftarJX = (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_Y_Axis,str);
		float ftarJY = (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_Z_Axis,str);
		float ftarJZ =  (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_Yaw_Axis,str);
		float ftarJU = (float) (float)atof((LPCSTR)str);
		str.Format("%.3f", ftarJX);
		SetDlgItemText(IDC_EDIT_Para1, str);
		str.Format("%.3f", ftarJY);
		SetDlgItemText(IDC_EDIT_Para2, str);
		str.Format("%.3f", ftarJZ);
		SetDlgItemText(IDC_EDIT_Para3, str);
		str.Format("%.3f", ftarJU);
		SetDlgItemText(IDC_EDIT_Para6, str);
	}
	else if(CmdSelect == 6)
	{
		CString str;
		GetDlgItemText(IDC_EDIT_J1_Theta,str);
		float ftarJX = (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_J2_Theta,str);
		float ftarJY = (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_Z_Axis,str);
		float ftarJZ =  (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_J4_Theta,str);
		float ftarJU = (float) (float)atof((LPCSTR)str);
		str.Format("%.3f", ftarJX);
		SetDlgItemText(IDC_EDIT_Para1, str);
		str.Format("%.3f", ftarJY);
		SetDlgItemText(IDC_EDIT_Para2, str);
		str.Format("%.3f", ftarJZ);
		SetDlgItemText(IDC_EDIT_Para3, str);
		str.Format("%.3f", ftarJU);
		SetDlgItemText(IDC_EDIT_Para6, str);
	}
}
afx_msg LRESULT CRTEXFormView::OnSendcmd2statusbar(WPARAM wParam, LPARAM lParam)
{
	unsigned char* string =(unsigned char*) wParam;
	UINT n = (UINT) lParam;
	CStatusBar * pStatus=&((CMainFrame*)AfxGetMainWnd())->m_wndStatusBar2 ;
	CString strSendCmd,strpara;
	//strrecieveCmd.Format("長度 %d    ",m_uschrReciveData[5]);
	strpara.Format("發送Cmd:  0x%02X  參數:  ",string[4]);
	strSendCmd+=strpara;
	if(string[5] == 0 )	strSendCmd += "No Parameter";
	for(int i = 0 ; i < string[5] ;i++ )
	{
		strpara.Format("%02X  ",string[6+i]);
		strSendCmd += strpara;
	}
	pStatus->SetPaneText(0,strSendCmd);

	return 0;
}
afx_msg LRESULT CRTEXFormView::OnSendsocketclientclosed(WPARAM wParam, LPARAM lParam)
{
	TRACE("Socket closed message\n");
	m_clientSocket.Close();
	return 0;
}
void CRTEXFormView::RecieveMsgReport2statusBar(const unsigned char* string)
{
	CString strrecieveCmd,strpara;
	strpara.Format("%c Cmd:  0x%02X  參數:  ",string[3],string[4]);
	strrecieveCmd+=strpara;
	if(string[5] == 0 )	strrecieveCmd += "No Parameter";
	for(int i = 0 ; i < string[5] ;i++ )
	{
		strpara.Format("%02X  ",string[6+i]);
		strrecieveCmd += strpara;
	}
	CStatusBar * pStatus=&((CMainFrame*)AfxGetMainWnd())->m_wndStatusBar1 ;
	pStatus->SetPaneText(0,strrecieveCmd);
}
void CRTEXFormView::SendCmd(unsigned char* string, UINT n)
{
	if((m_fgInQCMD) && (string[4] < IMMEDIATE_COMMAND_START)  && string[3] == 'C')
	{
		
		if(m_fgStandaloneCode)
		{
			unsigned char* puncharS = new unsigned char[n+1];
			puncharS[0] = string[5] +6;
			for(UINT i = 0 ; i < n ; i++) puncharS[i+1] = string[i];
			m_StandaloneFile.Write(puncharS,n + 1);
			delete [] puncharS;
			return;
		}
		unsigned char* punchar = new unsigned char[n];
		for(UINT i = 0 ; i < n ; i++)
		{
			punchar[i] = string[i];
		}
		m_qQmdFIFOlist.push(punchar);
		//TRACE("qcmd size = %d \n",m_qQmdFIFOlist.size());
		return;
	}
	SaveSend2Loghistory(string);
	for(UINT i = 0 ; i <  n ; i++)
	{
		m_lpucCharForSendCmd[i] = string[i];
	}	
  #ifndef _Cmd_NOShow_Status_Bar
	PostMessage(WM_SendCmd2StatusBar,(WPARAM)m_lpucCharForSendCmd,(LPARAM)n);
  #endif

#ifdef CMD_WITH_SN
	if(string[3] != 'A')
	{
		if(m_chkSum == 4) m_chkSum = 1;
		else m_chkSum++;
		string[2] = m_chkSum;
	}
#endif
	m_clientSocket.Send(string,n);
}

void CRTEXFormView::HandleQSIMSG(const unsigned char* RecieveData, const UINT num)
{
	SaveReceive2Loghistory(RecieveData);
  #ifndef _Cmd_NOShow_Status_Bar
	RecieveMsgReport2statusBar(RecieveData);	
  #endif
	if(RecieveData[3] == 'P')
	{
		if (RecieveData[4] == CMD_INIT)
		{
				ShowStatus("Init CMD PROGRESS");
				ClearWaitCMDAcking();

		}
		else if (RecieveData[4] == CMD_GET_RING_STATUS)
		{
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_SERVO_ON)
		{
			ShowStatus("Servon CMD PROGRESS");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_SERVO_OFF)
		{
			ShowStatus("Servoff CMD PROGRESS");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == m_CurrentCmd)
		{
			ShowStatus("Universal CMD PROGRESS");
			ClearWaitCMDAcking();
		}

	}
	else if (RecieveData[3] == 'R')
	{
		if((RecieveData[4] < IMMEDIATE_COMMAND_START) && (m_fgInQCMD))
		{
			m_fgHIFCmdBufferFull = FALSE;
			unsigned char* string = m_qQmdFIFOlist.front();
			delete [] string;
			m_qQmdFIFOlist.pop();
			//TRACE("pop qcmd\n");
		}

		if(RecieveData[4] == CMD_CONFIG_RING)
		{
			ShowStatus("configuration OK");
			ClearWaitCMDAcking();

		}
		else if (RecieveData[4] == CMD_INIT)
		{
			ShowStatus("already Init OK");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_GET_RING_STATUS)
		{
			ShowStatus("Init OK");
			ClearWaitCMDPgress();
			ClearWaitCMDAcking();
            //KillTimer(2);
		}
		else if (RecieveData[4] == CMD_GET_AXIS_STATUS)
		{
			ClearWaitCMDAcking();
			if(m_blWaiitCmdPROGRESS)
			{
				switch(m_ucCMDPROGRESSCase)
				{
				case CMD_SERVO_ON :
					for (UINT i = 0 ; i < m_uiEnableAxis ; i ++)
					{
						if((RecieveData[8+ i * 4] & 0xc0) != 0xc0)
						{
							return;
						}
					}
					ShowStatus("Servo On OK");
					ClearWaitCMDPgress();
					break;
				case CMD_SERVO_OFF:
					for (UINT i = 0 ; i < m_uiEnableAxis ; i ++)
					{
						if((RecieveData[8+ i * 4] & 0x80) != 0x00)
						{
							return;
						}
					}
					ShowStatus("Servo Off OK");
					ClearWaitCMDPgress();
					break;
				default:
					break;
				}
			}
			else
			{
				CString str;
				unsigned short usTemp;
				ShowStatus("DRIVER STATUS =");
				for(UINT i = 0 ; i < (RecieveData[5]) ; i += 4)
				{
					usTemp = (unsigned short)(RecieveData[8 + i] << 8) +(unsigned short)(RecieveData[9 + i]);
     				str.Format("ID:%d ,STATUS: 0x%04X",(RecieveData[6 + i] & 0x1f),usTemp);  //12 :ID
  				    ShowStatus(str);
				}

			}
		}
		else if ((RecieveData[4] == CMD_GET_AXIS_POSITION)||(RecieveData[4] == CMD_Get_New_AXIS_POSITION))  //get position OK
		{
			if(!m_blShowPos)
			ShowStatus("Get Position OK");
			int PostionValue;
			unsigned char ucMACID;
			UINT Timedata(0);
			bool fgGetTime = FALSE;
			for(UINT i = 0 ; i < (RecieveData[5]) ; i += 5)
			{
				PostionValue = (int)(RecieveData[7 + i] << 24) + (int)(RecieveData[8 + i] << 16) + (int)(RecieveData[9 + i] << 8) +(int)(RecieveData[10 + i]);
				if(RecieveData[6 + i] == m_Axis[0])  m_intX_Axis_pos = PostionValue;//SetDlgItemInt(IDC_EDIT_X_Pulse_Axis,PostionValue);
				else if (RecieveData[6 + i] == m_Axis[1]) m_intY_Axis_pos = PostionValue;//SetDlgItemInt(IDC_EDIT_Y_Pulse_Axis,PostionValue);
				else if (RecieveData[6 + i] == m_Axis[2]) m_intZ_Axis_pos = PostionValue;//SetDlgItemInt(IDC_EDIT_Z_Pulse_Axis,PostionValue);
				else if (RecieveData[6 + i] == m_Axis[3]) m_intU_Axis_pos = PostionValue;//SetDlgItemInt(IDC_EDIT_U_Pulse_Axis,PostionValue);
				else if (RecieveData[6 + i] == 0xFF)
				{
					Timedata = (UINT) PostionValue;
					fgGetTime = TRUE;
				}
				else if((RecieveData[6+i] & 0xE0) == 0x80) //default velocity
				{
					ucMACID = (RecieveData[6+i] & 0x1F);
					if(ucMACID == m_Axis[0]) m_response_vel[0] = PostionValue;
					else if(ucMACID == m_Axis[1]) m_response_vel[1] = PostionValue;
					else if(ucMACID == m_Axis[2]) m_response_vel[2] = PostionValue;
					else if(ucMACID == m_Axis[3]) m_response_vel[3] = PostionValue;

				}
				else if((RecieveData[6+i] & 0xE0) == 0xC0)  //default torque
				{
					ucMACID = (RecieveData[6+i] & 0x1F);
					if(ucMACID == m_Axis[0]) m_response_torq[0] = PostionValue;
					else if(ucMACID == m_Axis[1]) m_response_torq[1] = PostionValue;
					else if(ucMACID == m_Axis[2]) m_response_torq[2] = PostionValue;
					else if(ucMACID == m_Axis[3]) m_response_torq[3] = PostionValue;
				}
				else
				{
				  for(UINT k = 0 ; k < (sizeof(m_fgenableIO_borad) / sizeof(m_fgenableIO_borad[0])) ; k++)  // modify 
				  {
					  if((m_fgenableIO_borad[k] == TRUE) && (RecieveData[6 + i] == m_ucIO_MacID[k]))
					  {
						   m_uiIOStatus[k] = (UINT) PostionValue;
						   break;
					  }
				  }
				}
 				//else if (RecieveData[6 + i] == m_ucIO_MacID[0]) m_uiIOStatus[0] = (UINT) PostionValue;
 				//else if (RecieveData[6 + i] == m_ucIO_MacID[1]) m_uiIOStatus[1] = (UINT) PostionValue;
 				//else if (RecieveData[6 + i] == m_ucIO_MacID[2]) m_uiIOStatus[2] = (UINT) PostionValue;
 				//else if (RecieveData[6 + i] == m_ucIO_MacID[3]) m_uiIOStatus[3] = (UINT) PostionValue;
			}
			TransPulse2XYZ();
			//if(m_threadPara.m_case == 6)
			//{
			//  ClearWaitCMDAcking();
			//  return;
			//}
			if(m_blShowPos)  //draw and save point
			{
				//pDoc->DrawPoint(GetDlgItemInt(IDC_EDIT_X_Pulse_Axis),GetDlgItemInt(IDC_EDIT_Y_Pulse_Axis));  // maybe modify
				if((pDoc->m_fgskipRedraw == TRUE) && (m_uiPointCnt++ > 30000)) //30000 point = 5 min if m_uiShowPosTimeSlot = 10;
				{
					m_uiPointCnt = 0;
					pDoc->ClearTxtPoint();
					OnBnClickedButtonClearmsg();
					ShowStatus("5min clear");
				}
				if(m_Show_Scara_Arm)	pDoc->DrawArm(m_iXpos_um, m_iYpos_um, m_iZpos_um, m_fYaw, m_ftheta1,m_ftheta2);
				pDoc->DrawPoint(m_iXpos_um, m_iYpos_um, m_iZpos_um);
				if(fgGetTime)
				  pDoc->AddPoint2txt(m_intX_Axis_pos, m_intY_Axis_pos, m_intZ_Axis_pos, m_iXpos_um, m_iYpos_um, m_iZpos_um,Timedata,m_response_vel,m_response_torq);
				else
				  pDoc->AddPoint2txt(m_intX_Axis_pos, m_intY_Axis_pos, m_intZ_Axis_pos, m_iXpos_um, m_iYpos_um,m_iZpos_um);
			}
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_GET_MONITOR_DATA)
		{
			int monitorValue;
			for(UINT i = 0 ; i < (RecieveData[5]) ; i += 5)
			{
				monitorValue = (int)(RecieveData[7 + i] << 24) + (int)(RecieveData[8 + i] << 16) + (int)(RecieveData[9 + i] << 8) +(int)(RecieveData[10 + i]);
				if(RecieveData[6 + i] == m_Axis[0])  SetDlgItemInt(IDC_EDIT_X_Monitor,monitorValue);
				else if (RecieveData[6 + i] == m_Axis[1]) SetDlgItemInt(IDC_EDIT_Y_Monitor,monitorValue);
				else if (RecieveData[6 + i] == m_Axis[2]) SetDlgItemInt(IDC_EDIT_Z_Monitor,monitorValue);
			}
			pDoc->Addmonitor2Txt(GetDlgItemInt(IDC_EDIT_X_Monitor),GetDlgItemInt(IDC_EDIT_Y_Monitor),GetDlgItemInt(IDC_EDIT_Z_Monitor));
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_SET_SPEED_LIMIT)  //set velocity OK
		{
			ShowStatus("Set Velocity OK");
			ClearWaitCMDAcking();

		}
		else if(RecieveData[4] == CMD_SET_TEST_MODE)
		{
			if(m_TestMode)
			  ShowStatus("TEST MODE  OK");
			else
			  ShowStatus("disable TEST MODE  OK");
			ClearWaitCMDAcking();

		}
		else if (RecieveData[4] == CMD_SET_XYZ_MotionSpeed)
		{
			ShowStatus("Set motionSpd OK");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_GET_XYZ_MotionSpeed)
		{
			ShowStatus("Get motionSpd Parameter OK");
			for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[6 + i];    
			int spdtype = m_float2uchar.inumber;
			for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[10 + i];
			float vmax =  m_float2uchar.fnumber;
			for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[14 + i];
			float ta =  m_float2uchar.fnumber;				
			CString str;
			str.Format("spdtype = %d, vmax = %.5fmm/ms, ta = %.2fms\n",spdtype,vmax,ta);
			m_intcmbxSpdType = spdtype;
			m_fVmax =vmax;
			m_uiTa = (UINT)ta;
			ShowStatus(str);
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_SET_XYZ_AxisParameter)
		{
			ShowStatus("Set Axis Parameter OK");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_GET_XYZ_AxisParameter)
		{
			ShowStatus("Get axis Parameter OK");
			unsigned char axis = RecieveData[6];
			unsigned char Axistype = RecieveData[7];

			for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[8 + i];
			float fpitch = m_float2uchar.fnumber;

			for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[12 + i];
			int igearratio =  m_float2uchar.inumber;

			for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[16 + i];
			float fgearratio_mech = m_float2uchar.fnumber;		

			for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[20 + i];
			float fPOTlimit = m_float2uchar.fnumber;

			for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[24 + i];
			float fNOTlimit = m_float2uchar.fnumber;

			int iEncoderoffset(0);
			if(RecieveData[5] == 26)
			{
				for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[28 + i];
				iEncoderoffset = m_float2uchar.inumber;
			}


			CString str;
			str.Format("    axis = %d, pitch = %.2f, GearRatio = %d\n",axis,fpitch,igearratio);
			ShowStatus(str);
			str.Format("    GearRatio_mech = %.2f, POT = %.2f, NOT = %.2f\n, Encoder offset = %d",fgearratio_mech,fPOTlimit,fNOTlimit,iEncoderoffset);
			ShowStatus(str);
			m_Axis_Type[axis] = Axistype;
			m_Axis_Pitch[axis] = fpitch;
			m_Axis_GearRatio_Mech[axis] = fgearratio_mech;
			m_Axis_GearRatio[axis] = igearratio;
			m_Axis_POTLimit[axis] = fPOTlimit;
			m_Axis_NOTLimit[axis] = fNOTlimit;
			m_Axis_Encoder_OFS[axis] = iEncoderoffset;
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_DRIVER_PARA_RW)  
		{
			ShowStatus("CMD_DRIVER_PARA_RW OK");				
			//set something
			m_intDvrReturnData = (UINT)(RecieveData[6]<<24)+ (UINT)(RecieveData[7]<<16)+ (UINT)(RecieveData[8]<<8)+ (UINT)(RecieveData[9]);				
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_MONITOR)
		{
			ShowStatus("Set Monitor OK");
			ClearWaitCMDAcking();
		}
		else if ((RecieveData[4] == CMD_1_AXIS_MOTION)  ||(RecieveData[4] == CMD_2_AXES_LINEAR_MOTION) || 
			     (RecieveData[4] == CMD_SET_XYZ_MotionLineXYZ) || (RecieveData[4]== CMD_SET_XYZ_MotionCircle) || 
				 (RecieveData[4] == CMD_SET_XYZ_MotionHelix) || (RecieveData[4] == CMD_SET_XYZ_MotionCircle3D) ||
				 (RecieveData[4] == CMD_SET_XYZ_MotionJOG) || (RecieveData[4] == CMD_Scara_J4_Rotate) ||
				 (RecieveData[4] == CMD_SET_J1234_MotionJOG)
				)
		{
			ShowStatus("Host Receives axis motion");
			ClearWaitCMDAcking();
			CButton * pbtn = (CButton*)GetDlgItem(IDC_CHECK_ShowPostionRepeatly);
			if(!pbtn->GetCheck() && !m_fgInQCMD)
			{
				pbtn->SetCheck(1);
				OnBnClickedCheckShowpostionrepeatly();
			}

		}
		else if (RecieveData[4] == CMD_SET_MoveMode)
		{
			ShowStatus("Host Receives SET_MoveMode");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_SET_TARCMD)
		{
			ShowStatus("Host Receives SET_TARCMD");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_SET_Profile_ABS_POSCMD)
		{
			ShowStatus("Host Receives SET_Profile_ABS_POSCMD");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_JOG_Move)
		{
			ShowStatus("Host Receives JOG_Move");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_JOG_REL_Move)
		{
			ShowStatus("Host Receives CMD_JOG_REL_Move");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_Parameter_Writing_S_CMD)
		{
			ShowStatus("Write Driver Register");
			ClearWaitCMDAcking();

		}
		else if (RecieveData[4] == CMD_Parameter_Writing_I_CMD)
		{
			ShowStatus("Write Driver Register");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_Parameter_Reading_S_CMD)
		{
			ShowStatus("Read Driver Register");
			m_intSpecialCMDReturnData = (int)(RecieveData[6] << 24) + (int)(RecieveData[7] << 16) + (int)(RecieveData[8] << 8) +(int)(RecieveData[9]);
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_Parameter_Reading_I_CMD)
		{
			ShowStatus("Read Driver Register");
			m_intSpecialCMDReturnData = (int)(RecieveData[6] << 24) + (int)(RecieveData[7] << 16) + (int)(RecieveData[8] << 8) +(int)(RecieveData[9]);
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_Parameter_EEPROM_Writing_CMD)
		{
			ShowStatus("EEPROM Write Driver Register");
			ClearWaitCMDAcking();

		}
		else if (RecieveData[4] == CMD_SET_PPMode_CMD)
		{
			ShowStatus("SET_PPMode_CMD OK");
			ClearWaitCMDAcking();

		}
		else if (RecieveData[4] == CMD_SET_PPHOME_CMD)
		{
			ShowStatus("SET_PPHOME_CMD OK");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_SetMotionBlend)
		{
			ShowStatus("CMD_SetMotionBlend OK");
			ClearWaitCMDAcking();
		}
		else if(RecieveData[4] == CMD_Driver_Software_Reset)
		{
			ShowStatus("CMD_Driver_Software_Reset OK");
			ClearWaitCMDAcking();

		}
		else if (RecieveData[4] == CMD_SET_Scara_Robot_mech)
		{
			ShowStatus("CMD_SET_Scara_Robot_mech OK");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_GET_Scara_Robot_mech)
		{
			CString str;
			ShowStatus("CMD_GET_Scara_Robot_mech OK");

			for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[6 + i];
			m_fScaraRobot_L1 = m_float2uchar.fnumber;

			for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[10 + i];
			m_fScaraRobot_L2 = m_float2uchar.fnumber;

			for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[14 + i];
			m_fScaraRobot_Height = m_float2uchar.fnumber;

			//m_ucScaraRobot_Arm_dir = ReceveData[18];
			str.Format("  Scara : L1 = %.2f , L2 = %.2f, Height = %.2f",m_fScaraRobot_L1,m_fScaraRobot_L2,m_fScaraRobot_Height);
			ShowStatus(str);
			ClearWaitCMDAcking();
		}
		else if(RecieveData[4] == CMD_Get_Scara_Hand)
		{
			CString str;
			ShowStatus("CMD_Get_Scara_Hand OK");
			m_ucScaraRobot_Arm_dir = RecieveData[6];
			str.Format(" Scara : Hand = %d",m_ucScaraRobot_Arm_dir);
			ShowStatus(str);
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_READ_USER_DATA)
		{
			CString str,substr;
			str="";
			if(m_fgGetdebugRS232 == TRUE)
			{
				substr.Format("get offset = 0x%04X",(unsigned short)(RecieveData[7] << 8) + (unsigned short)RecieveData[8]);
				ShowStatus(substr);
				for(int i = 0 ; i < (RecieveData[5]-4); i++)
				{
				  str += RecieveData[10 + i];
				}
				debugFile.WriteString(str);
				ClearWaitCMDAcking();
				return;
			}
			CStdioFile RDFile;

			for(int i = 0; i <RecieveData[5]; i++ )
			{
				substr.Format("0x%02X ",RecieveData[6+i]);
				str +=substr;
			}

			if(RDFile.Open("READUDATA.txt",CFile::modeReadWrite))
			{  
				RDFile.SeekToEnd();
				RDFile.WriteString("\n");
				RDFile.WriteString(str);
				RDFile.Close();						
			}
			else if(RDFile.Open("READUDATA.txt",CFile::modeReadWrite|CFile::modeCreate))
			{
				TRACE("not exist\r\n");
				RDFile.WriteString(str);
				RDFile.Close();						
			}
			ShowStatus("readUData");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_WRITE_USER_DATA)
		{
			ShowStatus("CMD_WRITE_USER_DATA OK");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_SYNC_USER_DATA)
		{
			ShowStatus("CMD_SYNC_USER_DATA OK");
			ClearWaitCMDAcking();

		}
		else if (RecieveData[4] == CMD_SET_OUTPUT)
		{
			ShowStatus("CMD_SET_OUTPUT OK");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_GET_INPUT)
		{
			m_ucInputBoardID = RecieveData[6];
			m_uiInputBoardStatus = (UINT)(RecieveData[7]<<24)+ (UINT)(RecieveData[8]<<16)+ (UINT)(RecieveData[9]<<8)+ (UINT)(RecieveData[10]);				

			ShowStatus("CMD_GET_INPUT OK");
			ClearWaitCMDAcking();

		}
		else if (RecieveData[4] == CMD_Wait_IO_Board_Status)
		{
			ShowStatus("CMD_Wait_IO_Board_Status OK");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_SET_OUTPUT_In_Queue)
		{
			ShowStatus("CMD_SET_OUTPUT_In_Queue OK");
			ClearWaitCMDAcking();		
		}
		else if(RecieveData[4] == CMD_Delay_Time)
		{
			ShowStatus("CMD_Delay_Time ok");
			ClearWaitCMDAcking();		
		}
		else if(RecieveData[4] == CMD_Motion_Pause)
		{
			ShowStatus("CMD_Motion_Pause ok");
			ClearWaitCMDAcking();		
		}
		else if(RecieveData[4] == CMD_Motion_Stop)
		{
			ShowStatus("CMD_Motion_Stop ok");
			ClearWaitCMDAcking();		
		}
		else if(RecieveData[4] == CMD_Motion_Limit_Release)
		{
			ShowStatus("CMD_Motion_Limit_Release ok");
			ClearWaitCMDAcking();		
		}
		else if (RecieveData[4] == CMD_Driver_Alarm_Warning)
		{
			if(!m_blCheckErr)
			  ShowStatus("CMD_Driver_Alarm_Warning OK");
			UINT axis(0);
			CString str;
			for(UINT i = 0 ; i < RecieveData[5] ; i+= 10)
			{
				for(axis = 0 ; axis < m_uiEnableAxis ; axis++)
				{
					if(m_Axis[axis] == RecieveData[6 + i])
						break;
				}
				if(axis != m_uiEnableAxis)
				{
					m_axisStatus[axis].CMD_Error = RecieveData[7 + i];
					m_axisStatus[axis].CMD_ERROR_Code_L = RecieveData[8 + i];
					m_axisStatus[axis].CMD_ERROR_Code_H = RecieveData[9 + i];
					m_axisStatus[axis].Alarm = RecieveData[10 + i];
					m_axisStatus[axis].Alarm_Main = RecieveData[11 + i];
					m_axisStatus[axis].Alarm_Sub = RecieveData[12 + i];
					m_axisStatus[axis].Warning = RecieveData[13 + i];
					m_axisStatus[axis].Warning_Code_L = RecieveData[14 + i];
					m_axisStatus[axis].Warning_Code_H = RecieveData[15 + i];
					str.Format("MACID = %d",m_Axis[axis]);
					ShowStatus(str);
					str.Format( " Error = %d , Alarm = %d , Warning = %d",m_axisStatus[axis].CMD_Error,  m_axisStatus[axis].Alarm, m_axisStatus[axis].Warning);
					ShowStatus(str);
					str.Format("Err Code = 0x%02X-0x%02X", m_axisStatus[axis].CMD_ERROR_Code_L, m_axisStatus[axis].CMD_ERROR_Code_H);
					ShowStatus(str);
					str.Format("Alarm = %02u-%02u", m_axisStatus[axis].Alarm_Main, m_axisStatus[axis].Alarm_Sub);
					ShowStatus(str);
					str.Format("Warning = 0x%02X-0x%02X", m_axisStatus[axis].Warning_Code_L, m_axisStatus[axis].Warning_Code_H);
					ShowStatus(str);
				}
			}
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_Driver_Alarm_Clear)
		{
			ShowStatus("CMD_Driver_Alarm_Clear OK");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_Motion_Run)
		{
			ShowStatus("CMD_Motion_Run OK");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_Motion_Status)
		{
			ShowStatus("CMD_Motion_Status OK");
			CString str;
			str.Format("Status = 0x%02X, error code = 0x%02X",RecieveData[6], RecieveData[7]);
			ShowStatus(str);
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_Get_Jog_Speed_parameter)
		{
			ShowStatus("CMD_Get_Jog_Speed_parameter ok");
			m_iJog_MaxRPM = (int)(RecieveData[6] << 24) + (int)(RecieveData[7] << 16) + (int)(RecieveData[8] << 8) +(int)(RecieveData[9]);
			for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[10 + i];
			m_fJog_Speed = m_float2uchar.fnumber;
			CString str;
			str.Format("jogMaxRPM = %d RPM , Jog Speed = %.2f %%",m_iJog_MaxRPM, m_fJog_Speed);
			ShowStatus(str);
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_Set_Jog_MaxRPM)
		{
			ShowStatus("CMD_Set_Jog_MaxRPM OK");
			ClearWaitCMDAcking();
		}
		else if(RecieveData[4] == CMD_Set_Jog_Speed)
		{
			ShowStatus("CMD_Set_Jog_Speed OK");
			ClearWaitCMDAcking();
		}
		else if(RecieveData[4] == CMD_Switch_Deubug_RS232)
		{
			ShowStatus("CMD_Switch_Deubug_RS232 OK");
			ClearWaitCMDAcking();
		}
		else if(RecieveData[4] == CMD_ShowPosTime)
		{
			ShowStatus("CMD_ShowPosTime OK");
			ClearWaitCMDAcking();
		}
		else if(RecieveData[4] == CMD_Check_ALL_Queue_Done)
		{
			//ShowStatus("CMD_Check_ALL_Queue_Done OK");
			if(RecieveData[7] == 1)
				ShowStatus("Queue cmd set is not empty");
			else 
				ShowStatus("Queue cmd set is empty");
			ClearWaitCMDAcking();
		}
		else if(RecieveData[4] == CMD_SystemID)
		{
			for(int i = 0 ; i < RecieveData[5] + 6; i++)
			{
			 m_ucSystemIDreturndata[i] = RecieveData[i];
			}
			ShowStatus("getSYSTEM ID Data");
			ClearWaitCMDAcking();
		}
		else if(RecieveData[4] == CMD_Reload_Scara_Parameter)
		{
			ShowStatus("CMD_Reload_Scara_Parameter ok");
			ClearWaitCMDAcking();
		}
		else if(RecieveData[4] == CMD_CLR_MULTI_TERN)
		{
			ShowStatus("CMD_CLR_MULTI_TERN ok");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_Get_7216_Information)
		{
			CString str ,receivestr;
			receivestr="";
			for(int i = 0 ; i < RecieveData[5] ; i++)
			{
				receivestr += (unsigned char)RecieveData[ 6 + i];
			}
			str.Format("7216 information = %s",receivestr);
			ShowStatus(str);
			ClearWaitCMDAcking();
		}
		else if(RecieveData[4] == CMD_SET_Software_Timeout)
		{
			ShowStatus("CMD_SET_Software_Timeout OK");
			ClearWaitCMDAcking();
		}
		else if(RecieveData[4] == CMD_SET_LOGIC_GATE)
		{
			ShowStatus("CMD_SET_LOGIC_GATE OK");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_Set_IOBorad_SendMsg)
		{
			ShowStatus("CMD_Set_IOBorad_SendMsg OK");
			ClearWaitCMDAcking();
		}
		else if(RecieveData[4] == CMD_Set_Positon_Event)
		{
			//ShowStatus("CMD_Set_Positon_Event OK");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == CMD_SetXYLimit)
		{
			bool fgXYLimitEnable = (RecieveData[6] & 0x01)? TRUE : FALSE;
			for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[7 + i];
			float fXYLimitX_Max = m_float2uchar.fnumber;
			for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[11 + i];
			float fXYLimitX_Min = m_float2uchar.fnumber;
			for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[15 + i];
			float fXYLimitY_Max = m_float2uchar.fnumber;
			for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[19 + i];
			float fXYLimitY_Min = m_float2uchar.fnumber;
			
			CString str;
			str.Format("XYLimit = %s : Xmax = %.2f, Xmin= %.2f, Ymax = %.2f, Ymin = %.2f, ",(fgXYLimitEnable)?"Enable":"disable",fXYLimitX_Max,fXYLimitX_Min,fXYLimitY_Max,fXYLimitY_Min);
			ShowStatus(str);
			ClearWaitCMDAcking();
		}
		else if(RecieveData[4] == CMD_ShowAllEnableLine)
		{
		   for(int j = 0 ; j < RecieveData[5] ; j+=17)
		   {
			   int ichannel = RecieveData[6 + j];
			   for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[7 + j + i];
			   float X1 = m_float2uchar.fnumber;
			   for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[11 + j + i];
			   float Y1 = m_float2uchar.fnumber;
			   for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[15 + j + i];
			   float X2 = m_float2uchar.fnumber;
			   for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[19 + j + i];
			   float Y2 = m_float2uchar.fnumber;
			   CString str;
			   str.Format("Line[%d]: X1 = %.3f, Y1 = %.3f, X2 = %.3f, Y2 = %.3f",ichannel,X1,Y1,X2,Y2);
			   ShowStatus(str);
		   }
		   ClearWaitCMDAcking();
		}
		else if (RecieveData[4]  == CMD_SetEmergencyStopChannel)
		{
			CString str;
			if((RecieveData[5] > 0) && (RecieveData[6] == 2))
			{
				for(int i = 0 ; i < (RecieveData[5] -1); i += 4)
				{
					str.Format("Emergency Stop : index = %d , MacID = %d, Channel = %d, level = %s",RecieveData[7 + i],RecieveData[8 + i],RecieveData[9 + i],(RecieveData[10 + i])?"High" : "Low");
					ShowStatus(str);
				}
			}
			ShowStatus("CMD_SetEmergencyStopChannel OK");
			ClearWaitCMDAcking();
		}
		else if(RecieveData[4] == CMD_Access_FlashData)
		{
			ShowStatus("CMD_Access_FlashData OK");
			ClearWaitCMDAcking();
		}
		else if (RecieveData[4] == m_CurrentCmd)
		{			
			ShowStatus("Universal CMD OK");
			ClearWaitCMDAcking();
		}
	}
	else if (RecieveData[3] == 'E')
	{
		if((RecieveData[4] < IMMEDIATE_COMMAND_START) && (m_fgInQCMD))
		{
			if(RecieveData[6] == 0x01)
			{
				ShowStatus("buffer full");
				//TRACE("bufferFULL\n");
				m_fgHIFCmdBufferFull = TRUE;
				ClearWaitCMDAcking();
			}
			else if(RecieveData[6] == 0x0A)
			{
				ShowStatus("7216 status : motion Stop");
				if(m_fgInQCMD)
				{
					ShowStatus("Q CMD immediately stop");
					m_fgQcmdImmediatelyStop  = TRUE;
				}
				ClearWaitCMDAcking();
			}
		}
		else if(RecieveData[4] == CMD_DRIVER_PARA_RW)
		{
			CString str;
			str.Format("Command Err = 0x%0X",RecieveData[6]);
			ShowStatus(str);				
			
			unsigned short wErrorCode=(unsigned short)(RecieveData[7]<<8) + (unsigned short)(RecieveData[8]);
			str.Format("ID: %d , ERORCODE: 0x%x",RecieveData[9],wErrorCode);  //12 :ID
			ShowStatus(str);
			ClearWaitCMDAcking();

		}
		else if(RecieveData[4] == CMD_Get_New_AXIS_POSITION)
		{
			CString str;
			ShowStatus("Error or Warning Happened");
			if(RecieveData[6] == 0x03)  //ERR_HIF_RTEX_NOT_RUNNING
			{
				ShowStatus("ERR_HIF_RTEX_NOT_RUNNING");
			}
			/*
			else if((RecieveData[8] != 0x07))
			{
				str.Format("MotionStatus = 0x%02X, error code = 0x%02X",RecieveData[7], RecieveData[8]);
				ShowStatus(str);
				CButton* btn = (CButton*)GetDlgItem(IDC_CHECK_ShowPostionRepeatly);
				if(btn->GetCheck())
				{
				  btn->SetCheck(0);
				  OnBnClickedCheckShowpostionrepeatly();
				}			
			}
			*/
			else
			{
				str.Format("MotionStatus = 0x%02X, error code = 0x%02X",RecieveData[7], RecieveData[8]);
				ShowStatus(str);
				UINT axis(0);
				if((RecieveData[8]== 0x05) || (RecieveData[8]== 0x0A))
				{
					for(UINT i = 0 ; i < (UINT)(RecieveData[5] - 3) ; i+= 8)
					{
						for(axis = 0 ; axis < m_uiEnableAxis ; axis++)
						{
							if(m_Axis[axis] == RecieveData[9 + i])
								break;
						}
						if(axis != m_uiEnableAxis)
						{
							m_axisStatus[axis].POT = RecieveData[10 + i];
							m_axisStatus[axis].NOT = RecieveData[11 + i];
							m_axisStatus[axis].Speed_Error = RecieveData[12 + i];
							m_axisStatus[axis].RPM = (UINT)(RecieveData[13 + i] << 24)+(UINT)(RecieveData[14 + i] << 16)+(UINT)(RecieveData[15 + i] << 8)+(UINT)(RecieveData[16 + i]);
							str.Format("MACID = %d",m_Axis[axis]);
							ShowStatus(str);
							str.Format("POT = 0x%02X - NOT = 0x%02X", m_axisStatus[axis].POT, m_axisStatus[axis].NOT);
							ShowStatus(str);
							str.Format("Speed Error = %d, RPM = %d", m_axisStatus[axis].Speed_Error, m_axisStatus[axis].RPM);
							ShowStatus(str);
						}
					}
				}
				if(RecieveData[8] == 0x07) // Error_Driver_Alarm
				{
					for(UINT i = 0 ; i < (UINT)(RecieveData[5] - 3) ; i+= 10)
					{
						for(axis = 0 ; axis < m_uiEnableAxis ; axis++)
						{
							if(m_Axis[axis] == RecieveData[9 + i])
								break;
						}
						if(axis != m_uiEnableAxis)
						{
							m_axisStatus[axis].CMD_Error = RecieveData[10 + i];
							m_axisStatus[axis].CMD_ERROR_Code_L = RecieveData[11 + i];
							m_axisStatus[axis].CMD_ERROR_Code_H = RecieveData[12 + i];
							m_axisStatus[axis].Alarm = RecieveData[13 + i];
							m_axisStatus[axis].Alarm_Main = RecieveData[14 + i];
							m_axisStatus[axis].Alarm_Sub = RecieveData[15 + i];
							m_axisStatus[axis].Warning = RecieveData[16 + i];
							m_axisStatus[axis].Warning_Code_L = RecieveData[17 + i];
							m_axisStatus[axis].Warning_Code_H = RecieveData[18 + i];

							str.Format("MACID = %d",m_Axis[axis]);
							ShowStatus(str);
							str.Format( " Error = %d , Alarm = %d , Warning = %d",m_axisStatus[axis].CMD_Error,  m_axisStatus[axis].Alarm, m_axisStatus[axis].Warning);
							ShowStatus(str);
							str.Format("Err Code = 0x%02X-0x%02X", m_axisStatus[axis].CMD_ERROR_Code_L, m_axisStatus[axis].CMD_ERROR_Code_H);
							ShowStatus(str);
							str.Format("Alarm = %02u-%02u", m_axisStatus[axis].Alarm_Main, m_axisStatus[axis].Alarm_Sub);
							ShowStatus(str);
							str.Format("Warning = 0x%02X-0x%02X", m_axisStatus[axis].Warning_Code_L, m_axisStatus[axis].Warning_Code_H);
							ShowStatus(str);
						}
					}
				}
				CButton* btn = (CButton*)GetDlgItem(IDC_CHECK_ShowPostionRepeatly);
				if(btn->GetCheck())
				{
					btn->SetCheck(0);
					OnBnClickedCheckShowpostionrepeatly();
					COleDateTime oleDt1 = COleDateTime::GetCurrentTime();		
					GetDlgItem(IDC_EDIT_Motion_Tm)->SetWindowText(oleDt1.Format("ErrT = %m/%d_%H:%M:%S"));
				}			
			}
			ClearWaitCMDAcking();
		}
		else if(RecieveData[4] == m_CurrentCmd)
		{
			CString str;
			str.Format("cmd = 0x%02X , Error code = 0x%02X",RecieveData[4],RecieveData[6]);
			ShowStatus("some Error happened in this cmd");
			ShowStatus(str);
			ClearWaitCMDAcking();
		}
    }
	else if(RecieveData[3] == 'I')
	{
		if(RecieveData[4] == EVENT_IOBard_Status_change)
		{
			CString str;
			ShowStatus("receive IO Change Status");
			for (int i = 0; i < RecieveData[5]; i += 5)
			{
				UINT status = (UINT)(RecieveData[7+i] << 24)+(UINT)(RecieveData[8+i] << 16)+(UINT)(RecieveData[9+i] << 8)+(UINT)(RecieveData[10+i]);
				str.Format("macid = %d, status = 0x%08X",RecieveData[6+i],status);
				ShowStatus(str);
				//if (RecieveData[6 + i] == m_ucIO_MacID[0]) m_uiIOStatus[0] = status;
				//else if (RecieveData[6 + i] == m_ucIO_MacID[1]) m_uiIOStatus[1] = status;
				//else if (RecieveData[6 + i] == m_ucIO_MacID[2]) m_uiIOStatus[2] = status;
				//else if (RecieveData[6 + i] == m_ucIO_MacID[3]) m_uiIOStatus[3] = status;
				for(UINT k = 0 ; k < (sizeof(m_fgenableIO_borad) / sizeof(m_fgenableIO_borad[0])) ; k++)  // modify 
				{
					if((m_fgenableIO_borad[k] == TRUE) && (RecieveData[6 + i] == m_ucIO_MacID[k]))
					{
						m_uiIOStatus[k] = status;
						break;
					}
				}
			}
			RTEX_EVENT_ACK(EVENT_IOBard_Status_change);
		}
		else if (RecieveData[4] == EVENT_Send_NEW_Postion)
		{
			int PostionValue;
			unsigned char ucMACID;
			UINT Timedata(0);
			bool fgGetTime = FALSE;
			for(UINT i = 0 ; i < (RecieveData[5]) ; i += 5)
			{
				PostionValue = (int)(RecieveData[7 + i] << 24) + (int)(RecieveData[8 + i] << 16) + (int)(RecieveData[9 + i] << 8) +(int)(RecieveData[10 + i]);
				if(RecieveData[6 + i] == m_Axis[0])  m_intX_Axis_pos = PostionValue;//SetDlgItemInt(IDC_EDIT_X_Pulse_Axis,PostionValue);
				else if (RecieveData[6 + i] == m_Axis[1]) m_intY_Axis_pos = PostionValue;// SetDlgItemInt(IDC_EDIT_Y_Pulse_Axis,PostionValue);
				else if (RecieveData[6 + i] == m_Axis[2]) m_intZ_Axis_pos = PostionValue;// SetDlgItemInt(IDC_EDIT_Z_Pulse_Axis,PostionValue);
				else if (RecieveData[6 + i] == m_Axis[3]) m_intU_Axis_pos = PostionValue;// SetDlgItemInt(IDC_EDIT_U_Pulse_Axis,PostionValue);
				else if (RecieveData[6 + i] == 0xFF)
				{
					Timedata = (UINT) PostionValue;
					fgGetTime = TRUE;
				}
				else if (RecieveData[6 + i] == 0xFD)
				{
					CString str;
					if(RecieveData[7+i] & 0x80)
					  str.Format("QQ = %d",(PostionValue & 0x7FFFFFFF));
					else
					  str.Format("Q = %d",PostionValue);
					SetDlgItemText(IDC_EDIT_Motion_Tm,str);
				}
				else if((RecieveData[6+i] & 0xE0) == 0x80) //default velocity
				{
					ucMACID = (RecieveData[6+i] & 0x1F);
					if(ucMACID == m_Axis[0]) m_response_vel[0] = PostionValue;
					else if(ucMACID == m_Axis[1]) m_response_vel[1] = PostionValue;
					else if(ucMACID == m_Axis[2]) m_response_vel[2] = PostionValue;
					else if(ucMACID == m_Axis[3]) m_response_vel[3] = PostionValue;

				}
				else if((RecieveData[6+i] & 0xE0) == 0xC0)  //default torque
				{
					ucMACID = (RecieveData[6+i] & 0x1F);
					if(ucMACID == m_Axis[0]) m_response_torq[0] = PostionValue;
					else if(ucMACID == m_Axis[1]) m_response_torq[1] = PostionValue;
					else if(ucMACID == m_Axis[2]) m_response_torq[2] = PostionValue;
					else if(ucMACID == m_Axis[3]) m_response_torq[3] = PostionValue;
				}
				else
				{
					for(UINT k = 0 ; k < (sizeof(m_fgenableIO_borad) / sizeof(m_fgenableIO_borad[0])) ; k++)  // modify 
					{
						if((m_fgenableIO_borad[k] == TRUE) && (RecieveData[6 + i] == m_ucIO_MacID[k]))
						{
							m_uiIOStatus[k] = (UINT) PostionValue;
							break;
						}
					}
				}
				//else if (RecieveData[6 + i] == m_ucIO_MacID[0]) m_uiIOStatus[0] = (UINT) PostionValue;
				//else if (RecieveData[6 + i] == m_ucIO_MacID[1]) m_uiIOStatus[1] = (UINT) PostionValue;
				//else if (RecieveData[6 + i] == m_ucIO_MacID[2]) m_uiIOStatus[2] = (UINT) PostionValue;
				//else if (RecieveData[6 + i] == m_ucIO_MacID[3]) m_uiIOStatus[3] = (UINT) PostionValue;
			}
			TransPulse2XYZ();
			//if(m_threadPara.m_case == 6)
			//{
			//	return;
			//}
			if((pDoc->m_fgskipRedraw == TRUE) && (m_uiPointCnt++ > 30000)) //30000 point = 5 min if m_uiShowPosTimeSlot = 10;
			{
				m_uiPointCnt = 0;
				pDoc->ClearTxtPoint();
				OnBnClickedButtonClearmsg();
				ShowStatus("5min clear");
			}
			//pDoc->DrawPoint(GetDlgItemInt(IDC_EDIT_X_Pulse_Axis),GetDlgItemInt(IDC_EDIT_Y_Pulse_Axis));  // maybe modify
			pDoc->DrawPoint(m_iXpos_um, m_iYpos_um, m_iZpos_um);
			if(fgGetTime)
				pDoc->AddPoint2txt(m_intX_Axis_pos, m_intY_Axis_pos, m_intZ_Axis_pos, m_iXpos_um, m_iYpos_um, m_iZpos_um,Timedata,m_response_vel,m_response_torq);
			else
				pDoc->AddPoint2txt(m_intX_Axis_pos, m_intY_Axis_pos, m_intZ_Axis_pos, m_iXpos_um, m_iYpos_um,m_iZpos_um);

		}
		else if (RecieveData[4] == EVENT_Error_Occurred)
		{
			CString str;
			str.Format("MotionStatus = 0x%02X, error code = 0x%02X",RecieveData[7], RecieveData[8]);
			ShowStatus(str);
			UINT axis(0);
			if(RecieveData[6] == 0x0A)
			{
				ShowStatus("7216 status : motion Stop");
				if(m_fgInQCMD)
				{
					ShowStatus("Q CMD immediately stop");
					m_fgQcmdImmediatelyStop  = TRUE;
				}
			}			
			if((RecieveData[8]== 0x05) || (RecieveData[8]== 0x0A))
			{
				for(UINT i = 0 ; i < (UINT)(RecieveData[5] - 3) ; i+= 8)
				{
					for(axis = 0 ; axis < m_uiEnableAxis ; axis++)
					{
						if(m_Axis[axis] == RecieveData[9 + i])
							break;
					}
					if(axis != m_uiEnableAxis)
					{
						m_axisStatus[axis].POT = RecieveData[10 + i];
						m_axisStatus[axis].NOT = RecieveData[11 + i];
						m_axisStatus[axis].Speed_Error = RecieveData[12 + i];
						m_axisStatus[axis].RPM = (UINT)(RecieveData[13 + i] << 24)+(UINT)(RecieveData[14 + i] << 16)+(UINT)(RecieveData[15 + i] << 8)+(UINT)(RecieveData[16 + i]);
						str.Format("MACID = %d",m_Axis[axis]);
						ShowStatus(str);
						str.Format("POT = 0x%02X - NOT = 0x%02X", m_axisStatus[axis].POT, m_axisStatus[axis].NOT);
						ShowStatus(str);
						str.Format("Speed Error = %d, RPM = %d", m_axisStatus[axis].Speed_Error, m_axisStatus[axis].RPM);
						ShowStatus(str);
					}
				}
			}
			if(RecieveData[8] == 0x07) // Error_Driver_Alarm
			{
				for(UINT i = 0 ; i < (UINT)(RecieveData[5] - 3) ; i+= 10)
				{
					for(axis = 0 ; axis < m_uiEnableAxis ; axis++)
					{
						if(m_Axis[axis] == RecieveData[9 + i])
							break;
					}
					if(axis != m_uiEnableAxis)
					{
						m_axisStatus[axis].CMD_Error = RecieveData[10 + i];
						m_axisStatus[axis].CMD_ERROR_Code_L = RecieveData[11 + i];
						m_axisStatus[axis].CMD_ERROR_Code_H = RecieveData[12 + i];
						m_axisStatus[axis].Alarm = RecieveData[13 + i];
						m_axisStatus[axis].Alarm_Main = RecieveData[14 + i];
						m_axisStatus[axis].Alarm_Sub = RecieveData[15 + i];
						m_axisStatus[axis].Warning = RecieveData[16 + i];
						m_axisStatus[axis].Warning_Code_L = RecieveData[17 + i];
						m_axisStatus[axis].Warning_Code_H = RecieveData[18 + i];

						str.Format("MACID = %d",m_Axis[axis]);
						ShowStatus(str);
						str.Format( " Error = %d , Alarm = %d , Warning = %d",m_axisStatus[axis].CMD_Error,  m_axisStatus[axis].Alarm, m_axisStatus[axis].Warning);
						ShowStatus(str);
						str.Format("Err Code = 0x%02X-0x%02X", m_axisStatus[axis].CMD_ERROR_Code_L, m_axisStatus[axis].CMD_ERROR_Code_H);
						ShowStatus(str);
						str.Format("Alarm = %02u-%02u", m_axisStatus[axis].Alarm_Main, m_axisStatus[axis].Alarm_Sub);
						ShowStatus(str);
						str.Format("Warning = 0x%02X-0x%02X", m_axisStatus[axis].Warning_Code_L, m_axisStatus[axis].Warning_Code_H);
						ShowStatus(str);
					}
				}
			}
			CButton* btn = (CButton*)GetDlgItem(IDC_CHECK_ShowPostionRepeatly);
			if(btn->GetCheck())
			{
				btn->SetCheck(0);
				OnBnClickedCheckShowpostionrepeatly();
				COleDateTime oleDt1 = COleDateTime::GetCurrentTime();		
				GetDlgItem(IDC_EDIT_Motion_Tm)->SetWindowText(oleDt1.Format("ErrT = %m/%d_%H:%M:%S"));
			}			

		}
		else if(RecieveData[4] == 0x04)  //for Ken Huaung
		{
			ShowStatus("Get Torque data");
			int pos =  (int)(RecieveData[6] << 24) + (int)(RecieveData[7] << 16) + (int)(RecieveData[8] << 8) +(int)(RecieveData[9]);
			int tor =  (int)(RecieveData[10] << 24) + (int)(RecieveData[11] << 16) + (int)(RecieveData[12] << 8) +(int)(RecieveData[13]);
			CString str;
			str.Format("%9d\t%9d\n",pos,tor);
			saveDebugEvent.WriteString(str);
		}
    }
	/*
	else if(RecieveData[3] == 'I')
	{
		if(RecieveData[4] == 0x01)
		{
			ClearWaitCMDAcking();
			if(RecieveData[7] == 0x01)
     			ShowStatus("ERROR_RTEX_NOT_RUNNING");
	  	    else if (RecieveData[7] == 0x0f)
			    ShowStatus("ERROR_UNKNOWN_COMMAND");
		}
		else if (RecieveData[4] == 0x02)// CMD_ERROR
		{
			ClearWaitCMDAcking();
			WORD wErrorCode=WORD(RecieveData[7]<<8) + WORD(RecieveData[8]);
			CString str;
			str.Format("CMD ERROR \r\n  ID: %d \r\n ERORCODE: 0x%x",RecieveData[9],wErrorCode);  //12 :ID
			ShowStatus(str);
			//			CString str;
			//			str.Format("CMD ERROR \r\n  ID: %d \r\n ERORCODE: 0x%x",RecieveData[9],wErrorCode);  //12 :ID
			//			MessageBox(str,"CMD ERRORCODE");			
		}
		else if(RecieveData[4] == 0)  //EVENT_HIF_COMMAND_COMPLETED
		{			
			if(RecieveData[6] == CMD_CONFIG_RING)   // SetRtexConfiguration
			{
				ShowStatus("configuration OK");
				ClearWaitCMDAcking();
			}
			else if (RecieveData[6] == CMD_INIT)  //init
			{
				ShowStatus("Init OK");
				ClearWaitCMDAcking();

			}
			else if (RecieveData[6] == CMD_SERVO_ON)  //servon on ok
			{
				ShowStatus("Servo On OK");
				ClearWaitCMDAcking();

			}
			else if (RecieveData[6] == CMD_SERVO_OFF)  //servon off OK
			{
				ShowStatus("Servo Off OK");
				ClearWaitCMDAcking();

			}
			else if (RecieveData[6] == CMD_GET_AXIS_POSITION)  //get position OK
			{
				ShowStatus("Get Postion OK");
				ClearWaitCMDAcking();
				int PostionValue(0);
				PostionValue = (int)(RecieveData[8] << 24) + (int)(RecieveData[9] << 16) + (int)(RecieveData[10] << 8) +(int)(RecieveData[11]);
				if(RecieveData[7] == m_Axis[0])  SetDlgItemInt(IDC_EDIT_X_Pulse_Axis,PostionValue);
				else if (RecieveData[7] == m_Axis[1])  SetDlgItemInt(IDC_EDIT_Y_Pulse_Axis,PostionValue);
				else if (RecieveData[7] == m_Axis[2])  SetDlgItemInt(IDC_EDIT_Z_Pulse_Axis,PostionValue);
			}
			else if (RecieveData[6] == CMD_SET_SPEED_LIMIT)  //set velocity OK
			{
				ShowStatus("Set Velocity OK");
				ClearWaitCMDAcking();
			}
			else if (RecieveData[6] == CMD_MONITOR)  
			{
				ShowStatus("Set Monitor OK");
				ClearWaitCMDAcking();
			}
			else if ((RecieveData[6] == CMD_DRIVER_PARA_RW))  
			{
				ShowStatus("CMD_DRIVER_PARA_RW OK");				
				//set something
				m_intDvrReturnData = (int)(RecieveData[7]<<24)+ (int)(RecieveData[8]<<16)+ (int)(RecieveData[9]<<8)+ (int)(RecieveData[10]);				
				ClearWaitCMDAcking();
			}
			else if ((RecieveData[6] == CMD_1_AXIS_MOTION)  ||(RecieveData[6] == CMD_2_AXES_LINEAR_MOTION) || (RecieveData[6] == CMD_SET_XYZ_MotionLineXYZ) ||(RecieveData[6]== CMD_SET_XYZ_MotionCircle) || (RecieveData[6] == CMD_SET_XYZ_MotionHelix))
			{
				ShowStatus("axis motion OK");
				ClearWaitCMDAcking();
				int PostionValueX = GetDlgItemInt(IDC_EDIT_X_Pulse_Axis);
				PostionValueX = (int)(RecieveData[8] << 24) + (int)(RecieveData[9] << 16) + (int)(RecieveData[10] << 8) +(int)(RecieveData[11]);
				if(RecieveData[7] == m_Axis[0])  SetDlgItemInt(IDC_EDIT_X_Pulse_Axis,PostionValueX);
				else if (RecieveData[7] == m_Axis[1])  SetDlgItemInt(IDC_EDIT_Y_Pulse_Axis,PostionValueX);
				else if (RecieveData[7] == m_Axis[2])  SetDlgItemInt(IDC_EDIT_Z_Pulse_Axis,PostionValueX);
				if((RecieveData[6] == CMD_2_AXES_LINEAR_MOTION) ||(RecieveData[6] == CMD_SET_XYZ_MotionLineXYZ)||(RecieveData[6] == CMD_SET_XYZ_MotionCircle) || (RecieveData[6] == CMD_SET_XYZ_MotionHelix))
				{
   					int PostionValueY = GetDlgItemInt(IDC_EDIT_Y_Pulse_Axis);
					PostionValueY = (int)(RecieveData[13] << 24) + (int)(RecieveData[14] << 16) + (int)(RecieveData[15] << 8) +(int)(RecieveData[16]);
					if(RecieveData[12] == m_Axis[0])  SetDlgItemInt(IDC_EDIT_X_Pulse_Axis,PostionValueY);
					else if (RecieveData[12] == m_Axis[1])  SetDlgItemInt(IDC_EDIT_Y_Pulse_Axis,PostionValueY);
					else if (RecieveData[12] == m_Axis[2])  SetDlgItemInt(IDC_EDIT_Z_Pulse_Axis,PostionValueY);
				}
				if((RecieveData[6] == CMD_SET_XYZ_MotionHelix) || 
				   ((RecieveData[6] == CMD_SET_XYZ_MotionLineXYZ) && (RecieveData[5] == 16)))
				{
					int PostionValueZ = GetDlgItemInt(IDC_EDIT_Z_Pulse_Axis);
					PostionValueZ= (int)(RecieveData[18] << 24) + (int)(RecieveData[19] << 16) + (int)(RecieveData[20] << 8) +(int)(RecieveData[21]);
					if(RecieveData[17] == m_Axis[0])  SetDlgItemInt(IDC_EDIT_X_Pulse_Axis,PostionValueZ);
					else if (RecieveData[17] == m_Axis[1])  SetDlgItemInt(IDC_EDIT_Y_Pulse_Axis,PostionValueZ);
					else if (RecieveData[17] == m_Axis[2])  SetDlgItemInt(IDC_EDIT_Z_Pulse_Axis,PostionValueZ);

				}
				pDoc->AddPoint2txt(GetDlgItemInt(IDC_EDIT_X_Pulse_Axis),GetDlgItemInt(IDC_EDIT_Y_Pulse_Axis),GetDlgItemInt(IDC_EDIT_Z_Pulse_Axis));
			}
			else if (RecieveData[6] == CMD_SET_XYZ_MotionSpeed)
			{
				ShowStatus("Set motionSpd OK");
				ClearWaitCMDAcking();
			}
			else if (RecieveData[6] == CMD_GET_XYZ_MotionSpeed)
			{
				ShowStatus("Get motionSpd Parameter OK");
				for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[7 + i];    
				int spdtype = m_float2uchar.inumber;
				for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[11 + i];
				float vmax =  m_float2uchar.fnumber;
				for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[15 + i];
				float ta =  m_float2uchar.fnumber;				
				CString str;
				str.Format("spdtype = %d, vmax = %.5fmm/ms, ta = %.2fms\n",spdtype,vmax,ta);
				m_intcmbxSpdType = spdtype;
				m_fVmax =vmax;
				m_uiTa = (UINT)ta;
				ShowStatus(str);
				ClearWaitCMDAcking();
			}
			else if (RecieveData[6] == CMD_SET_XYZ_AxisParameter)
			{
				ShowStatus("Set Axis Parameter OK");
				ClearWaitCMDAcking();
			}
			else if (RecieveData[6] == CMD_GET_XYZ_AxisParameter)
			{
				ShowStatus("Get axis Parameter OK");
				unsigned char axis = RecieveData[7];

				for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[8 + i];
				float fpitch = m_float2uchar.fnumber;

				for(int i = 0; i < 4; i++) m_float2uchar.charnumber[3-i] = RecieveData[12 + i];
				int igearratio =  m_float2uchar.inumber;

				CString str;
				str.Format("axis = %d, pitch = %.2f, GearRatio = %d\n",axis,fpitch,igearratio);
				TRACE(str);
				m_Axis_Pitch[axis] = fpitch;
				m_Axis_GearRatio[axis] = igearratio;
				ShowStatus(str);
				ClearWaitCMDAcking();
			}
			else if (RecieveData[6] == CMD_SET_MoveMode)
			{
				ShowStatus("Set Move Mode OK");
				ClearWaitCMDAcking();
			}
			else if (RecieveData[6] == CMD_SET_TARCMD)
			{
				ShowStatus("Set SET_TARCMD OK");
				ClearWaitCMDAcking();
			}
			else if (RecieveData[6] == CMD_SET_PPMode_CMD)
			{
				ShowStatus("Set SET_PPMode_CMD OK");
				ClearWaitCMDAcking();
			}
			else if (RecieveData[6] == CMD_SET_PPHOME_CMD)
			{
				ShowStatus("Set SET_PPHOME OK");
				ClearWaitCMDAcking();
			}
			else if (RecieveData[6] == CMD_SET_Profile_ABS_POSCMD)
			{
				ShowStatus("Set SET_Profile_ABS_POSCMD OK");
				ClearWaitCMDAcking();
			}
			else if (RecieveData[6] == m_CurrentCmd)
			{
				ShowStatus("Current CMD");
				ClearWaitCMDAcking();

			}
		}
		else if ((RecieveData[4] == EVENT_1_AXIS_POSITION_UPDATE) || (RecieveData[4] == EVENT_2_AXES_POSITION_UPDATE) || (RecieveData[4] == EVENT_3_AXES_POSITION_UPDATE))
		{
			//int PostionValueX = GetDlgItemInt(IDC_EDIT_X_Pulse_Axis);
			//int PostionValueY = GetDlgItemInt(IDC_EDIT_Y_Pulse_Axis);
			int PostionValueX = (int)(RecieveData[7] << 24) + (int)(RecieveData[8] << 16) + (int)(RecieveData[9] << 8) +(int)(RecieveData[10]);
			if(RecieveData[6] == m_Axis[0])  SetDlgItemInt(IDC_EDIT_X_Pulse_Axis,PostionValueX);
			else if (RecieveData[6] == m_Axis[1])  SetDlgItemInt(IDC_EDIT_Y_Pulse_Axis,PostionValueX);
			else if (RecieveData[6] == m_Axis[2])  SetDlgItemInt(IDC_EDIT_Z_Pulse_Axis,PostionValueX);
			if((RecieveData[4] == EVENT_2_AXES_POSITION_UPDATE) || (RecieveData[4] == EVENT_3_AXES_POSITION_UPDATE))  // have 2axis data
			{
			  int PostionValueY = (int)(RecieveData[12] << 24) + (int)(RecieveData[13] << 16) + (int)(RecieveData[14] << 8) +(int)(RecieveData[15]);
			  if(RecieveData[11] == m_Axis[0])  SetDlgItemInt(IDC_EDIT_X_Pulse_Axis,PostionValueY);
			  else if (RecieveData[11] == m_Axis[1])  SetDlgItemInt(IDC_EDIT_Y_Pulse_Axis,PostionValueY);
			  else if (RecieveData[11] == m_Axis[2])  SetDlgItemInt(IDC_EDIT_Z_Pulse_Axis,PostionValueY);
			}
            if(RecieveData[4] == EVENT_3_AXES_POSITION_UPDATE)  //have 3axis data
			{
				int PostionValueZ = (int)(RecieveData[17] << 24) + (int)(RecieveData[18] << 16) + (int)(RecieveData[19] << 8) +(int)(RecieveData[20]);
				if(RecieveData[16] == m_Axis[0])  SetDlgItemInt(IDC_EDIT_X_Pulse_Axis,PostionValueZ);
				else if (RecieveData[16] == m_Axis[1])  SetDlgItemInt(IDC_EDIT_Y_Pulse_Axis,PostionValueZ);
				else if (RecieveData[16] == m_Axis[2])  SetDlgItemInt(IDC_EDIT_Z_Pulse_Axis,PostionValueZ);
			}

			pDoc->DrawPoint(GetDlgItemInt(IDC_EDIT_X_Pulse_Axis),GetDlgItemInt(IDC_EDIT_Y_Pulse_Axis));  // maybe modify
			//pDoc->AddCPointSet(new CPoint(GetDlgItemInt(IDC_EDIT_X_Pulse_Axis),GetDlgItemInt(IDC_EDIT_Y_Pulse_Axis)));  //maybe modify
            pDoc->AddPoint2txt(GetDlgItemInt(IDC_EDIT_X_Pulse_Axis),GetDlgItemInt(IDC_EDIT_Y_Pulse_Axis),GetDlgItemInt(IDC_EDIT_Z_Pulse_Axis));
		}
		else if ((RecieveData[4] == EVENT_1_AXIS_MONITOR_UPDATE) || (RecieveData[4] == EVENT_2_AXIS_MONITOR_UPDATE) || (RecieveData[4] == EVENT_3_AXIS_MONITOR_UPDATE))
		{
			int imonitorX = (int)(RecieveData[7] << 24) + (int)(RecieveData[8] << 16) + (int)(RecieveData[9] << 8) +(int)(RecieveData[10]);
			if(RecieveData[6] == m_Axis[0])  SetDlgItemInt(IDC_EDIT_X_Monitor,imonitorX);
			else if (RecieveData[6] == m_Axis[1])  SetDlgItemInt(IDC_EDIT_Y_Monitor,imonitorX);
			else if (RecieveData[6] == m_Axis[2])  SetDlgItemInt(IDC_EDIT_Z_Monitor,imonitorX);
			if((RecieveData[4] == EVENT_2_AXIS_MONITOR_UPDATE) || (RecieveData[4] == EVENT_3_AXIS_MONITOR_UPDATE))  // have 2axis data
			{
				int imonitorY = (int)(RecieveData[12] << 24) + (int)(RecieveData[13] << 16) + (int)(RecieveData[14] << 8) +(int)(RecieveData[15]);
				if(RecieveData[11] == m_Axis[0])  SetDlgItemInt(IDC_EDIT_X_Monitor,imonitorY);
				else if (RecieveData[11] == m_Axis[1])  SetDlgItemInt(IDC_EDIT_Y_Monitor,imonitorY);
				else if (RecieveData[11] == m_Axis[2])  SetDlgItemInt(IDC_EDIT_Z_Monitor,imonitorY);
			}
			if(RecieveData[4] == EVENT_3_AXIS_MONITOR_UPDATE)  //have 3axis data
			{
				int imonitorZ = (int)(RecieveData[17] << 24) + (int)(RecieveData[18] << 16) + (int)(RecieveData[19] << 8) +(int)(RecieveData[20]);
				if(RecieveData[16] == m_Axis[0])  SetDlgItemInt(IDC_EDIT_X_Monitor,imonitorZ);
				else if (RecieveData[16] == m_Axis[1])  SetDlgItemInt(IDC_EDIT_Y_Monitor,imonitorZ);
				else if (RecieveData[16] == m_Axis[2])  SetDlgItemInt(IDC_EDIT_Z_Monitor,imonitorZ);
			}
			pDoc->Addmonitor2Txt(GetDlgItemInt(IDC_EDIT_X_Monitor),GetDlgItemInt(IDC_EDIT_Y_Monitor),GetDlgItemInt(IDC_EDIT_Z_Monitor));
		}
		
	}*/
}



void CRTEXFormView::SetWaitCMDAcking(bool wait)
{
	m_WaitCmdAck = TRUE;
	m_uintCmdtimerCnt = 0;
	if(wait == TRUE)  m_boolwaitCmdEvent = TRUE;
	else m_boolwaitCmdEvent = FALSE;
	SetTimer(0,1000,NULL);
}
void CRTEXFormView::ClearWaitCMDAcking()
{
	m_WaitCmdAck = FALSE;
	m_boolwaitCmdEvent = FALSE;
	m_uintCmdtimerCnt = 0;
	KillTimer(0);

}
bool CRTEXFormView::WaitCMDAcking(bool reset,UINT time)
{
	//while(m_WaitCmdAck && (m_uintCmdtimerCnt <= time));
	MSG msg;
	while(m_WaitCmdAck && (m_uintCmdtimerCnt <= time))
	{
		if((m_threadPara.m_blthreading != TRUE))
		{
			GetMessage(&msg,NULL,0,0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	if(m_uintCmdtimerCnt > time)
	{
		ShowStatus("timeout");
		//MessageBox("Time Out");
		ClearWaitCMDAcking();
		if(reset) OnBnClickedButtonDisconnect();
        return TRUE;
	}
	return FALSE;
}
void CRTEXFormView::SetWaitCMDProgress(unsigned char cmdcase,UINT timeslot)
{
	m_uintCmdProgressTimer = 0;
	m_blWaiitCmdPROGRESS = TRUE;
    m_ucCMDPROGRESSCase = cmdcase;
	SetTimer(1,timeslot,NULL);
	
	
}
void CRTEXFormView::ClearWaitCMDPgress()
{
	m_blWaiitCmdPROGRESS = FALSE;
	m_uintCmdProgressTimer = 0;
	m_ucCMDPROGRESSCase = 0x00;
	KillTimer(1);
}
bool CRTEXFormView::WaitCMDProgress()
{
	UINT i = 0;
	MSG msg;
    while(m_blWaiitCmdPROGRESS)
	{
		if(i == m_uintCmdProgressTimer)
		{
			if(m_threadPara.m_blthreading != TRUE)
			{
				GetMessage(&msg,NULL,0,0);
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			i = m_uintCmdProgressTimer;
			switch (m_ucCMDPROGRESSCase)
			{
			case CMD_INIT :
				if(i > 5)
				{
					ShowStatus("ring error, can not Establish Ring work");
					ClearWaitCMDPgress();
					//OnBnClickedButtonDisconnect();
					return TRUE;
				}
				else
				{
					SetWaitCMDAcking(1);
					RTEX_GetRTEXRingStatus();
				}
				break;
			case CMD_SERVO_ON: 
			case CMD_SERVO_OFF:
				if(i > 5)
				{
					ShowStatus("can not servo on or servo off");
					ClearWaitCMDPgress();
					SetWaitCMDAcking(1);
					RTEX_CMD_Driver_Alarm_Warning(m_uiSelectedSlaves);
					WaitCMDAcking();
					//OnBnClickedButtonDisconnect();
					return TRUE;
				}
				else
				{
					SetWaitCMDAcking(1);
					RTEX_CMD_GetAxisStatus(m_uiSelectedSlaves);
				}
				break;
			}            
			if(WaitCMDAcking())
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}
void CRTEXFormView::OnTimer(UINT_PTR nIDEvent)/*ya*/
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	CFormView::OnTimer(nIDEvent);
	
	//TRACE("idevent = %d,time++\n",nIDEvent);
	if(nIDEvent == 0)
	{
		m_uintCmdtimerCnt++;
		if(!m_boolwaitCmdEvent)
		{
			if(m_uintCmdtimerCnt > 7)
			{
				//m_ctrllistStatus.AddString(("timeout"));
				ShowStatus("timeout");
				//MessageBox("Time Out");
				ClearWaitCMDAcking();
				OnBnClickedButtonDisconnect();
			}
		}
	}
	else if(nIDEvent ==1)
	{
	  m_uintCmdProgressTimer++;
	}
}


void CRTEXFormView::OnBnClickedButtonConnect()
{
	// TODO: 在此加入控制項告知處理常式程式碼	

	GetDlgItem(IDC_Button_Connect)->EnableWindow(FALSE);
	CString str;
	int iConn;
	m_ctrlIPAddr.GetWindowText(str);
	TRACE("ip = %s\n",str);
	int iport = GetDlgItemInt(IDC_EDIT_Port);
	if((iport < 1000) || (iport > 9999))
	{
		MessageBox("請設置適當PORT！");
		OnBnClickedButtonDisconnect();
		SetDlgItemInt(IDC_EDIT_Port, 6188);
		GetDlgItem(IDC_EDIT_Port)->SetFocus();
		return;

	}
	m_clientSocket.Create();
	//iConn = m_clientSocket.Connect("192.168.134.49", 5050);
	/*
	int nZero=0;
	setsockopt(m_clientSocket,SOL_SOCKET,SO_RCVBUF, (char *)&nZero,sizeof(int)); 
	*/
	bool fgoption = TRUE;
	//very important , must set TCP_NODELAY!!!!!!!
	setsockopt (m_clientSocket,IPPROTO_TCP,TCP_NODELAY,(char*)&fgoption,sizeof(fgoption)); 
	iConn = m_clientSocket.Connect(str, iport);
	TRACE("iconn = %d , geterro = %d\n", iConn, GetLastError());/*ya*/
	AfxBeginThread(&CRTEXFormView::WaitConnectThread,this);
}
void CRTEXFormView::OnBnClickedButtonDisconnect()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	//if(m_connect2RenesasOK)     RTEX_CMD_RESET();
	TRACE("winsock closed\n");
	//m_clientSocket.Close(); 
	PostMessage(WM_SendSocketClientclosed,0,0);
	
	m_bClientConnected = FALSE;
	boolunSetSpdParamter = FALSE;
	boolunSetAxisParamter = FALSE;
	m_boolRTEX_Configurated = FALSE;
	m_connect2RenesasOK = FALSE;
	m_blShowPos = FALSE; 
	m_uiSelectedSlaves = 0;
	GetDlgItem(IDC_Button_Connect)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_Disconnect)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_IP)->EnableWindow(TRUE);
	GetDlgItem(IDC_IPADDRESS)->EnableWindow(TRUE);
	GetDlgItem(IDC_STATIC_Port)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_Port)->EnableWindow(TRUE);


	GetDlgItem(IDC_CHECK_TestMode)->EnableWindow(TRUE);
	GetDlgItem((IDC_BUTTON_ParameterSet))->EnableWindow(FALSE);		
	GetDlgItem(IDC_BUTTON_Set_RTEX_Configuration)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_ServoOn)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_FreeDynamicBrk)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_TestMode)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_Dvr_Reset)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_Warning_Reset)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_IO_Set)->EnableWindow(FALSE);
	SetDlgItemInt(IDC_EDIT_X_Pulse_Axis,0);
	SetDlgItemInt(IDC_EDIT_Y_Pulse_Axis,0);
	SetDlgItemInt(IDC_EDIT_Z_Pulse_Axis,0);
	SetDlgItemInt(IDC_EDIT_U_Pulse_Axis,0);
	SetDlgItemText(IDC_EDIT_X_Axis,"");
	SetDlgItemText(IDC_EDIT_Y_Axis,"");
	SetDlgItemText(IDC_EDIT_Z_Axis,"");
	SetDlgItemText(IDC_EDIT_Yaw_Axis,"");
	SetDlgItemText(IDC_EDIT_J1_Theta,"");
	SetDlgItemText(IDC_EDIT_J2_Theta,"");
	SetDlgItemText(IDC_EDIT_J4_Theta,"");
	GetDlgItem(IDC_BUTTON_DumpDebugMsg)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_DumpDebugMsg)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_Scara)->EnableWindow(FALSE);	
	GetDlgItem(IDC_BUTTON_Scara_home)->EnableWindow(FALSE);
	m_ctrlTestMode.SetCheck(FALSE);
	m_TestMode = FALSE;
	CButton* btn = (CButton*)GetDlgItem(IDC_CHECK_ServoOn);
	btn->SetCheck(0);
	btn = (CButton*)GetDlgItem(IDC_CHECK_FreeDynamicBrk);
	btn->SetCheck(0);
	btn = (CButton*)GetDlgItem(IDC_CHECK_ShowPostionRepeatly);
	btn->SetCheck(0);
	OnBnClickedCheckShowpostionrepeatly();
	GetDlgItem(IDC_CHECK_ShowPostionRepeatly)->EnableWindow(FALSE);
	btn = (CButton*)GetDlgItem(IDC_Check_Alarm_Warning);
	btn->SetCheck(0);
	OnBnClickedCheckAlarmWarning();
	btn = (CButton*)GetDlgItem(IDC_CHECK_DisRedraw);
	btn->SetCheck(1);
	OnBnClickedCheckDisredraw();
	GetDlgItem(IDC_CHECK_DisableDebugRS232)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_ShowPosTimeData)->EnableWindow(FALSE);
	for (UINT i = 0 ; i < 6 ; i++)
	{
	  m_MonitorSingal[i] = 0;
	}
	ClearWaitCMDAcking();
	CComboBox* pcbx = (CComboBox*)GetDlgItem(IDC_COMBO_SelectedDvr);
	pcbx->ResetContent();
}
void CRTEXFormView::OnBnClickedButtonParameterset()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_WaitCmdAck || (!m_connect2RenesasOK)) return;
	UINT i = 0;
	if(m_uiEnableAxis < 1)
	{
		for(i = 0 ; i < (sizeof(m_fgenableIO_borad) / sizeof(m_fgenableIO_borad[0])) ; i++)  // modify 
		{
		  if(m_fgenableIO_borad[i])
		  {
			  break;
		  }
		}
		if(i == (sizeof(m_fgenableIO_borad) /sizeof(m_fgenableIO_borad[0])))
		{
			MessageBox("NO enable slave");
			return;
		}
	}

	UpdateData(TRUE);
	m_threadPara.m_case = 0;	
	m_lpThread = AfxBeginThread(&CRTEXFormView::MythreadFun, (LPVOID)&m_threadPara);
	TRACE("Parameter Set\n");
}
void CRTEXFormView::OnBnClickedButtonSetRtexConfiguration()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_WaitCmdAck || (!m_connect2RenesasOK)) return;
	if(m_uiEnableAxis < 1)
	{
		UINT i = 0;
		for(i = 0 ; i < (sizeof(m_fgenableIO_borad) / sizeof(m_fgenableIO_borad[0])) ; i++)  // modify 
		{
			if(m_fgenableIO_borad[i])
			{
				break;
			}
		}
		if(i == (sizeof(m_fgenableIO_borad) /sizeof(m_fgenableIO_borad[0])))
		{
			MessageBox("NO enable slave");
			return;
		}
	}
	UpdateData(TRUE);
	m_threadPara.m_case = 1;	
	m_lpThread = AfxBeginThread(&CRTEXFormView::MythreadFun, (LPVOID)&m_threadPara);
}
void CRTEXFormView::OnBnClickedButtonOpendriverregdialog()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_WaitCmdAck || (!m_connect2RenesasOK) || (m_uiEnableAxis < 1) || (!m_boolRTEX_Configurated)) return;
	UpdateData(TRUE);
	m_threadPara.m_case = 2;	
	m_lpThread = AfxBeginThread(&CRTEXFormView::MythreadFun, (LPVOID)&m_threadPara);

}
void CRTEXFormView::OnBnClickedButtonOpenspecialcmddialog()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_WaitCmdAck || (!m_connect2RenesasOK) || (m_uiEnableAxis < 1) || (!m_boolRTEX_Configurated)) return;
	UpdateData(TRUE);
	m_threadPara.m_case = 3;	
	m_lpThread = AfxBeginThread(&CRTEXFormView::MythreadFun, (LPVOID)&m_threadPara);

}
void CRTEXFormView::OnBnClickedCheckServoon()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	//modify for socket
	CButton* btn = (CButton*)GetDlgItem(IDC_CHECK_ServoOn);
	if(m_WaitCmdAck || (!m_boolRTEX_Configurated) || (!m_connect2RenesasOK))
	{
		btn->SetCheck(!btn->GetCheck());
		return;
	}
	UpdateData(TRUE);

	TRACE("0x%08X\n",m_uiSelectedSlaves);

	RTEX_CMD_ServoOnOff ((btn->GetCheck()? TRUE : FALSE),m_uiSelectedSlaves);
	SetWaitCMDAcking(1);
	if(WaitCMDAcking()) return;
	if(btn->GetCheck())
		SetWaitCMDProgress(CMD_SERVO_ON,500);
	else
		SetWaitCMDProgress(CMD_SERVO_OFF,500);
	if(WaitCMDProgress())
	{
		btn->SetCheck(btn->GetCheck()? FALSE : TRUE);
	}
}
void CRTEXFormView::OnBnClickedButtonXAxis()
{
	// TODO: 在此加入控制項告知處理常式程式碼	
	if(m_WaitCmdAck || (!m_boolRTEX_Configurated) || (!m_connect2RenesasOK)|| (m_uiEnableAxis < 1)) return;
	UpdateData(TRUE);
	UINT uiselectedXaxis =0;
	uiselectedXaxis += 1 << m_Axis[0];
	RTEX_GetAxisPostion(uiselectedXaxis);
	SetWaitCMDAcking(1);
	if(WaitCMDAcking())
	{	
		MessageBox("RTEX_GetAxisPostion  NG");
		return;
	}

	SetWaitCMDAcking(1);	
	RTEX_CMD_GetAxisStatus(uiselectedXaxis);
	if(WaitCMDAcking())
	{	
		MessageBox("RTEX_GetDriverStatus  NG");
		return;
	}

}
void CRTEXFormView::OnBnClickedButtonYAxis()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_WaitCmdAck || (!m_boolRTEX_Configurated) || (!m_connect2RenesasOK)|| (m_uiEnableAxis < 2)) return;
	UpdateData(TRUE);
	UINT uiselectedYaxis = 0;
	uiselectedYaxis += 1 << m_Axis[1];
	RTEX_GetAxisPostion(uiselectedYaxis);

	SetWaitCMDAcking(1);
	if(WaitCMDAcking())
	{	
		MessageBox("RTEX_GetAxisPostion  NG");
		return;
	}	

	SetWaitCMDAcking(1);	
	RTEX_CMD_GetAxisStatus(uiselectedYaxis);
	if(WaitCMDAcking())
	{	
		MessageBox("RTEX_GetDriverStatus  NG");
		return;
	}	
}
void CRTEXFormView::OnBnClickedButtonZAxis()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_WaitCmdAck || (!m_boolRTEX_Configurated) || (!m_connect2RenesasOK)|| (m_uiEnableAxis < 3)) return;	UpdateData(TRUE);
	UINT uiselectedZaxis = 0;
	uiselectedZaxis += 1 << m_Axis[2];
	RTEX_GetAxisPostion(uiselectedZaxis);

	SetWaitCMDAcking(1);
	if(WaitCMDAcking())
	{	
		MessageBox("RTEX_GetAxisPostion  NG");
		return;
	}	

	SetWaitCMDAcking(1);	
	RTEX_CMD_GetAxisStatus(uiselectedZaxis);
	if(WaitCMDAcking())
	{	
		MessageBox("RTEX_GetDriverStatus  NG");
		return;
	}

}
void CRTEXFormView::OnBnClickedButtonUAxis()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_WaitCmdAck || (!m_boolRTEX_Configurated) || (!m_connect2RenesasOK)|| (m_uiEnableAxis < 4)) return;	UpdateData(TRUE);
	UINT uiselectedUaxis = 0;
	uiselectedUaxis += 1 << m_Axis[3];
	RTEX_GetAxisPostion(uiselectedUaxis);

	SetWaitCMDAcking(1);
	if(WaitCMDAcking())
	{	
		MessageBox("RTEX_GetAxisPostion  NG");
		return;
	}	

	SetWaitCMDAcking(1);	
	RTEX_CMD_GetAxisStatus(uiselectedUaxis);
	if(WaitCMDAcking())
	{	
		MessageBox("RTEX_GetDriverStatus  NG");
		return;
	}
}

void CRTEXFormView::OnBnClickedButtonSetxvelocity()
{
	// TODO: 在此加入控制項告知處理常式程式碼	
	if(m_WaitCmdAck || (!m_boolRTEX_Configurated) || (!m_connect2RenesasOK)|| (m_uiEnableAxis < 1)) return;
	UpdateData(TRUE);
	if((m_uintVmax <= 0) || (m_uintVmax > 10000))
	{
		//MessageBox("請給適當的速度");
		//m_ctrllistStatus.AddString("請給適當的速度");
		ShowStatus("請給適當的速度");
		m_uintVmax = 100;
		UpdateData(FALSE);
		GetDlgItem(IDC_EDIT_Vmax)->SetFocus();
		return;
	}
	unsigned short velocity = (unsigned short) m_uintVmax;
	RTEX_CMD_SET_SPEED_LIMIT(m_Axis[0], velocity);
	SetWaitCMDAcking();
}
void CRTEXFormView::OnBnClickedButtonSetyvelocity()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_WaitCmdAck || (!m_boolRTEX_Configurated) || (!m_connect2RenesasOK)|| (m_uiEnableAxis < 2)) return;
	UpdateData(TRUE);
	if((m_uintVmax <= 0) || (m_uintVmax > 10000))
	{
		//MessageBox("請給適當的速度");
		//m_ctrllistStatus.AddString("請給適當的速度");
		ShowStatus("請給適當的速度");
		m_uintVmax = 100;
		UpdateData(FALSE);
		GetDlgItem(IDC_EDIT_Vmax)->SetFocus();
		return;
	}
	unsigned short velocity = (unsigned short) m_uintVmax;
	RTEX_CMD_SET_SPEED_LIMIT(m_Axis[1], velocity);
	SetWaitCMDAcking();
}
void CRTEXFormView::OnBnClickedButtonSetzvelocity()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_WaitCmdAck || (!m_boolRTEX_Configurated) || (!m_connect2RenesasOK)|| (m_uiEnableAxis < 3)) return;
	UpdateData(TRUE);
	if((m_uintVmax <= 0) || (m_uintVmax > 10000))
	{
		//MessageBox("請給適當的速度");
		//m_ctrllistStatus.AddString("請給適當的速度");
		ShowStatus("請給適當的速度");
		m_uintVmax = 100;
		UpdateData(FALSE);
		GetDlgItem(IDC_EDIT_Vmax)->SetFocus();
		return;
	}
	unsigned short velocity = (unsigned short) m_uintVmax;
	RTEX_CMD_SET_SPEED_LIMIT(m_Axis[2], velocity);
	SetWaitCMDAcking();
}
void CRTEXFormView::OnBnClickedButtonUniversalCmd()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_WaitCmdAck || (!m_connect2RenesasOK)) return;
	UpdateData(TRUE);
	CUniversalCmd Ucmd;
	if(IDOK == Ucmd.DoModal())
	{
		int ilength = Ucmd.m_TransferBuff[5] + 6;
		m_CurrentCmd = Ucmd.m_TransferBuff[4];
		MSG msg;
		while(m_WaitCmdAck)
		{
			GetMessage(&msg,NULL,0,0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if(m_CurrentCmd == CMD_RESET)
			ShowStatus("CMD_Reset");
		else
			SetWaitCMDAcking();
		SendCmd(Ucmd.m_TransferBuff,ilength);
		TRACE("U CMD OK\n");

	}

}
void CRTEXFormView::OnBnClickedButtonClearmsg()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	m_ctrllistStatus.ResetContent();
	m_ctrllistLog_History.ResetContent();
}
void CRTEXFormView::OnBnClickedButtonClearview()
{
	if(m_WaitCmdAck) return;
	m_WaitCmdAck = TRUE;
	// TODO: 在此加入控制項告知處理常式程式碼
	//pDoc->m_view_resolution = 650000;
	//pDoc->m_view_resolutionBK = pDoc->m_view_resolution;
       // pDoc->m_view_Center_pt = CPoint(0,0);
	//pDoc->ClearCPiontSet();	
	pDoc->ClearTxtPoint();
	//pDoc->UpdateAllViews(0);
	this->PostMessage(WM_SendredrawInthread,0,0);
	m_WaitCmdAck = FALSE;
}

void CRTEXFormView::OnBnClickedCheckScara()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	UpdateData(TRUE);
	if(m_fgScaraRobot)
	{
		GetDlgItem(IDC_EDIT_J1_Theta)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_J2_Theta)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_J4_Theta)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Theta)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_Yaw)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_Yaw_Axis)->ShowWindow(TRUE);
		m_uiEnableAxis = 4;
	}
	else
	{
		GetDlgItem(IDC_EDIT_J1_Theta)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_J2_Theta)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_J4_Theta)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_Theta)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_Yaw)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_Yaw_Axis)->ShowWindow(FALSE);
		m_uiEnableAxis = 3;
	}

}
void CRTEXFormView::OnBnClickedCheckTestmode()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_ctrlTestMode.GetCheck())
	{
		m_TestMode = TRUE;
		RTEX_CMD_TestMode(TRUE);
	}
	else
	{
		m_TestMode = FALSE;
		RTEX_CMD_TestMode(FALSE);
	}
}
void CRTEXFormView::OnBnClickedCheckEvent()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CButton *pEvent = (CButton*)GetDlgItem(IDC_CHECK_Event);
	CButton *pbtn = (CButton*)GetDlgItem(IDC_CHECK_ShowPostionRepeatly);
	if(pbtn->GetCheck())
	{
		pEvent->SetCheck(!pEvent->GetCheck());
		pbtn->SetCheck(0);
		OnBnClickedCheckShowpostionrepeatly();
		pEvent->SetCheck(!pEvent->GetCheck());
	}
}
void CRTEXFormView::OnBnClickedCheckShowpostionrepeatly()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CButton *pEvent = (CButton*)GetDlgItem(IDC_CHECK_Event);
	CButton *pbtn = (CButton*)GetDlgItem(IDC_CHECK_ShowPostionRepeatly);
	if(pbtn->GetCheck())
	{
		GetDlgItem(IDC_CHECK_Event)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CHECK_Event)->EnableWindow(TRUE);
	}
	if(pEvent->GetCheck())
	{
		MSG msg;
		if(pbtn->GetCheck())
		{
			while(m_WaitCmdAck)
			{
				GetMessage(&msg,NULL,0,0);
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			SetWaitCMDAcking();
			RTEX_CMD_Set_Positon_Event(1,m_uiShowPosTimeSlot);
		}
		else
		{
			while(m_WaitCmdAck)
			{
				GetMessage(&msg,NULL,0,0);
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			SetWaitCMDAcking();
			RTEX_CMD_Set_Positon_Event(0,m_uiShowPosTimeSlot);
		}
		return;
	}
	if(pbtn->GetCheck())
	{
		m_blShowPos = TRUE;		
		m_lpShowPosThread = AfxBeginThread(&CRTEXFormView::ShowPosThreadFun, (LPVOID)this);
	}
	else
	{
		m_blShowPos = FALSE;
	}
	m_uiPointCnt = 0;
}
void CRTEXFormView::OnBnClickedCheckAlarmWarning()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CButton * pbtn = (CButton*)GetDlgItem(IDC_Check_Alarm_Warning);
	if(pbtn->GetCheck())
	{
		m_blCheckErr = TRUE;
		m_lpCheckErrThread = AfxBeginThread(&CRTEXFormView::CheckErrThreadFun, (LPVOID)this);
	}
	else
	{
		m_blCheckErr = FALSE;
	}

}
void CRTEXFormView::OnBnClickedButtonScarahome()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_WaitCmdAck || (!m_boolRTEX_Configurated) || (!m_connect2RenesasOK)|| (m_uiEnableAxis < 1)) return;
	if((!m_fgScaraRobot))
	{
		MessageBox("Not ScaraRobot mode or in Test mode");
		return;
	}
	RTEX_CMD_Scara_Robot_Home();
	SetWaitCMDAcking();
}
void CRTEXFormView::OnBnClickedButtonIoSet()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	UpdateData(TRUE);
	m_threadPara.m_case = 4;	
	m_lpThread = AfxBeginThread(&CRTEXFormView::MythreadFun, (LPVOID)&m_threadPara);
}
bool CRTEXFormView::QcmdSendCmdFunc()
{
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime;
	LARGE_INTEGER nEndTime;
	double dtime;
	//MSG msg;
	unsigned char* SendString;

		QueryPerformanceFrequency(&nFreq);	
		QueryPerformanceCounter(&nBeginTime);
		Sleep(m_fgHIFCmdBufferFull? 9900: 50);        
		do 
		{
			//GetMessage(&msg,NULL,0,0);
			//TranslateMessage(&msg);
			//DispatchMessage(&msg);
			QueryPerformanceCounter(&nEndTime);
			dtime=(double)(nEndTime.QuadPart-nBeginTime.QuadPart)/(double)nFreq.QuadPart;
		} while (dtime < (m_fgHIFCmdBufferFull? (double)(10): (double)0.10)); //while 直到間隔時間到}

	SendString = m_qQmdFIFOlist.front();
	UINT n = SendString[5] + 6;
	while(m_blWaiitCmdPROGRESS || m_WaitCmdAck)
	{
		//GetMessage(&msg,NULL,0,0);
		//TranslateMessage(&msg);
		//DispatchMessage(&msg);
	}
	SetWaitCMDAcking(1);
	SaveSend2Loghistory(SendString);
#ifndef _Cmd_NOShow_Status_Bar
	for(UINT i = 0 ; i <  n ; i++)
	{
		m_lpucCharForSendCmd[i] = SendString[i];
	}	

	PostMessage(WM_SendCmd2StatusBar,(WPARAM)m_lpucCharForSendCmd,(LPARAM)n);
#endif
	m_CurrentCmd = SendString[4];
#ifdef CMD_WITH_SN
		if(m_chkSum == 4) m_chkSum =1;
		else m_chkSum++;
		SendString[2] = m_chkSum;
#endif
		m_clientSocket.Send(SendString,n);

// 	if(WaitCMDAcking())
// 	{
// 	  //ClearWaitCMDAcking();
// 	  return TRUE;
// 	}            
	while(m_WaitCmdAck && (m_uintCmdtimerCnt <= 3));
	if(m_uintCmdtimerCnt > 3)
	{
		ShowStatus("Q CMD timeout");
		ClearWaitCMDAcking();
//		OnBnClickedButtonDisconnect();
	    return TRUE;
	}            
	return FALSE;
}
void CRTEXFormView::QcmdADDCMDFromTxt()
{
	//m_QMDAddCMDing = TRUE;
	CStdioFile QCmdFile;
	CString str,substr;
	int loop(0),total_loop(0);
	ULONGLONG LoopStartPos(0),postion(0) ;
	int index(0),bkindex(0);
	if(!QCmdFile.Open("QCMD.txt",CFile::modeRead))
	{
		MessageBox("QCMD.txt 不存在");
		return;    
	}
	if(m_fgStandaloneCode)
	{
		if(!m_StandaloneFile.Open("standalone.txt",CFile::modeWrite|CFile::modeCreate))
		{
			ShowStatus("open standalone.txt fail");
		}
		m_StandaloneFile.SeekToBegin();
	}
	GetDlgItem(IDC_EDIT_Motion_Tm)->SetWindowText("");
	GetDlgItem(IDC_EDIT_EXTRA_INFOR)->SetWindowText("");
	QCmdFile.SeekToBegin();	
	postion = QCmdFile.GetPosition();			
	while(QCmdFile.ReadString(str))
	{
		if(str.Left(2) =="//")	continue;

		index = str.Find(' ', 0);
		substr = str.Left(index);
		//TRACE("substr = %s, index = %d got \n",substr ,index);
		//if((substr == "end")||(index == -1)) break;
		if(str == "end") 
		{
			break;
		}
		else if (str == "Loopend")
		{
			CString strloop;
			if(loop-- > 1)
			{
				postion = QCmdFile.Seek((LONGLONG) LoopStartPos,CFile::begin);
				//TRACE("go to postion 0x%08X\n",postion);				
			}
			strloop.Format("loop %d Send OK, Time = ",(total_loop - loop));
			COleDateTime oleDt = COleDateTime::GetCurrentTime();
			//CString strFileName=oleDt.Format("%Y %m %d  %H %M %S %A %B");
			strloop += oleDt.Format("%m/%d_%H:%M:%S");
			ShowStatus(strloop);
			GetDlgItem(IDC_EDIT_EXTRA_INFOR)->SetWindowText(strloop);			
			continue;
		}
		else if ((str == "Scarahome")||(str == "Renesas_loop_end"))
		{
			substr = str;
		}
		else if (index == -1)
		{
			MessageBox("parameter wrong");
			break;
		}

		str = str.Mid(index+1,str.GetLength()-index);
		str.Replace(" ", "");
		//TRACE("str = %s , index = %d \n",str, index);		
		index = -1 ;		
		if (substr == "Loop")
		{
			loop = atoi(str);
			total_loop = loop;
			LoopStartPos = QCmdFile.GetPosition();
			TRACE("loop = %d \n",loop);
			//TRACE("Loop Start postion 0x%016X\n",LoopStartPos);
			continue;
		}
		else if(substr =="Scarahome")
		{
			if((!m_fgScaraRobot)|| (m_TestMode))
			{
				ShowStatus("Not ScaraRobot mode or in Test mode");
				continue;
			}
			else
				RTEX_CMD_Scara_Robot_Home();
		}
		else if (substr =="LOGIC_GATE")
		{
			unsigned char para[9]= {0,0,0,0,0,0,0,0,0};
			for (int i = 0 ; i < 9 ; i++)
			{
				bkindex = index + 1;
				index = str.Find(',',bkindex);					
				if(index < 0) break;
				substr = str.Mid(bkindex, index - bkindex);
				para[i] = (unsigned char )atoi(substr);
				//TRACE("index = %d, para = %d ",index , para[i]);
			}
			Sleep(50);
			while(m_blWaiitCmdPROGRESS || m_WaitCmdAck)
			{
			}
			SetWaitCMDAcking(1);
			RTEX_CMD_SET_LOGIC_GATE(para[0],para[1],para[2],para[3],para[4],para[5],para[6],para[7],para[8]);
			while(m_WaitCmdAck && (m_uintCmdtimerCnt <= 3));
			if(m_uintCmdtimerCnt > 3)
			{
				ShowStatus("Logic Gate timeout");
				ClearWaitCMDAcking();
				m_fgQcmdImmediatelyStop = TRUE;
				break;
			}
			continue;
		}
		else if(substr == "Line")
		{
			//TRACE("Line motion \n");
			float para[4]= {0,0,0,0};

			for (int i = 0 ; i < 4 ; i++)
			{
				bkindex = index + 1;
				index = str.Find(',',bkindex);					
				if(index < 0) break;
				substr = str.Mid(bkindex, index - bkindex);
				para[i] = (float)atof((LPCSTR)substr);
				//TRACE("index = %d, para = %.4f ",index , para[i]);
			}
			if(m_uiEnableAxis < 4) para[3] = 0;
			//TRACE("\n");
			RTEX_CMD_SET_XYZ_MotionLineXYZ(para[0], para[1], para[2], para[3]);
		}
		else if (substr =="WaitIO")
		{
			//TRACE("Wait IO status \n");
			unsigned char para[6]= {0,0,0,0xff, 0, 0};
			for (int i = 0 ; i < 3 ; i++)
			{
				bkindex = index + 1;
				index = str.Find(',',bkindex);					
				if(index < 0) break;
				substr = str.Mid(bkindex, index - bkindex);
				para[i] = (unsigned char )atoi(substr);
				//TRACE("index = %d, para = %d ",index , para[i]);
			}
			//TRACE("\n");
			UINT uitimeout;
			bkindex = index + 1;
			index = str.Find(',', bkindex);
			substr = str.Mid(bkindex, index - bkindex);
			uitimeout =(UINT) atoi(substr);
			//TRACE("timeout =%u\n",uitimeout);
			for (int i = 3 ; i < 6 ; i++)
			{
				bkindex = index + 1;
				index = str.Find(',',bkindex);					
				if(index < 0) break;
				substr = str.Mid(bkindex, index - bkindex);
				para[i] = (unsigned char )atoi(substr);
				//TRACE("index = %d, para = %d ",index , para[i]);
			}
			//TRACE("\n");
			RTEX_CMD_Wait_IO_Board_Status(para[0],para[1],para[2],uitimeout,para[3],para[4],para[5]);
		}
		else if(substr == "SetOutInQ")
		{
			//TRACE("Set Output in Queue \n");
			unsigned char para[3]= {0,0,0};
			for (int i = 0 ; i < 3 ; i++)
			{
				bkindex = index + 1;
				index = str.Find(',',bkindex);					
				if(index < 0) break;
				substr = str.Mid(bkindex, index - bkindex);
				para[i] = (unsigned char )atoi(substr);
				//TRACE("index = %d, para = %d ",index , para[i]);
			}
			//TRACE("\n");
			RTEX_CMD_SET_OUTPUT_In_Queue(para[0],para[1],para[2]);
		}
		else if (substr == "Delaytime")
		{
			int idelaytime(0);
			bkindex = index + 1;
			index = str.Find(',', bkindex);
			substr = str.Mid(bkindex, index - bkindex);
			idelaytime = atoi(substr);
			//TRACE("DelayTime =%d\n",idelaytime);
			RTEX_CMD_Delay_Time(idelaytime);
		}
		else if (substr == "Jogxyzu")
		{
			//TRACE("Jog motion \n");
			float para[4]= {0,0,0,0};

			for (int i = 0 ; i < 4 ; i++)
			{
				bkindex = index + 1;
				index = str.Find(',',bkindex);					
				if(index < 0) break;
				substr = str.Mid(bkindex, index - bkindex);
				para[i] = (float)atof((LPCSTR)substr);
				//TRACE("index = %d, para = %.4f ",index , para[i]);

			}
			if(m_uiEnableAxis < 4) para[3] = 0;
			//TRACE("\n");
			RTEX_CMD_SET_XYZ_MotionJOG(para[0], para[1], para[2], para[3]);
		}
		else if (substr == "Jogj1234")
		{
			//TRACE("Jog1234 motion \n");
			float para[4]= {0,0,0,0};

			for (int i = 0 ; i < 4 ; i++)
			{
				bkindex = index + 1;
				index = str.Find(',',bkindex);					
				if(index < 0) break;
				substr = str.Mid(bkindex, index - bkindex);
				para[i] = (float)atof((LPCSTR)substr);
				//TRACE("index = %d, para = %.4f ",index , para[i]);

			}
			if(m_uiEnableAxis < 4) para[3] = 0;
			//TRACE("\n");
			RTEX_CMD_SET_J1234_MotionJOG(para[0], para[1], para[2], para[3]);
		}
		else if (substr == "Circle")
		{
			CString substr_cmd=substr;
			//TRACE("circle motion \n");
			unsigned char ucmode;
			float para[4]={0,0,0,0};
			unsigned char uccw;

			bkindex = index + 1;
			index = str.Find(',', bkindex);
			substr = str.Mid(bkindex, index - bkindex);
			ucmode = atoi(substr);
			//TRACE("mode = %d ",ucmode);

			for (int i = 0 ; i < 3 ; i++)
			{
				bkindex = index + 1;
				index = str.Find(',', bkindex);
				substr = str.Mid(bkindex, index - bkindex);
				para[i] = (float)atof((LPCSTR)substr);
				//TRACE("para = %.4f ",para[i]);
			}
			bkindex = index + 1;
			index = str.Find(',', bkindex);
			substr = str.Mid(bkindex, index - bkindex);
			uccw = atoi(substr);
			//TRACE("uccw = %d ",uccw);

			bkindex = index + 1;
			index = str.Find(',', bkindex);
			if(index > 0)
			{
				substr = str.Mid(bkindex, index - bkindex);
				para[3] = (float)atof((LPCSTR)substr);
			}
			RTEX_CMD_SET_XYZ_MotionCircle(ucmode, para[0], para[1], para[2], uccw, para[3]);

		}
		else if(substr == "Helix")
		{
			CString substr_cmd=substr;
			//TRACE("circle motion \n");
			unsigned char ucmode;
			float para[5] = {0 ,0 ,0 ,0 ,0};
			unsigned char uccw;


			bkindex = index + 1;
			index = str.Find(',', bkindex);
			substr = str.Mid(bkindex, index - bkindex);
			ucmode = atoi(substr);
			//TRACE("mode = %d ",ucmode);

			for (int i = 0 ; i < 4 ; i++)
			{
				bkindex = index + 1;
				index = str.Find(',', bkindex);
				substr = str.Mid(bkindex, index - bkindex);
				para[i] = (float)atof((LPCSTR)substr);
				//TRACE("para = %.4f ",para[i]);
			}
			bkindex = index + 1;
			index = str.Find(',', bkindex);
			substr = str.Mid(bkindex, index - bkindex);
			uccw = atoi(substr);
			//TRACE("uccw = %d ",uccw);
			bkindex = index + 1;
			index = str.Find(',', bkindex);
			if(index > 0)
			{
				substr = str.Mid(bkindex, index - bkindex);
				para[4] = (float)atof((LPCSTR)substr);
			}
			//TRACE("TarU = %.4f ",para[4]);
			//TRACE("\n");				
			RTEX_CMD_SET_XYZ_MotionHelix(ucmode, para[0], para[1], para[2], para[3], uccw, para[4]);
		}
		else if(substr == "Circle3D")
		{
			float para[7];
			unsigned char uccw;
			//TRACE("Circle3D \n");
			for (int i = 0 ; i < 7 ; i++)
			{
				bkindex = index + 1;
				index = str.Find(',', bkindex);
				substr = str.Mid(bkindex, index - bkindex);
				para[i] = (float)atof((LPCSTR)substr);
				//TRACE("para = %.4f ",para[i]);
			}
			bkindex = index + 1;
			index = str.Find(',', bkindex);
			substr = str.Mid(bkindex, index - bkindex);
			uccw = atoi(substr);
			//TRACE("uccw = %d ",uccw);
			//TRACE("\n");
			RTEX_CMD_SET_XYZ_MotionCircle3D(para[0],para[1],para[2],para[3],para[4],para[5],para[6],uccw);
		}

		else if (substr == "Blend")
		{
			//TRACE("SetmotionBlending \n");
			float para(0);		    
			bkindex = index + 1;
			index = str.Find(',', bkindex);
			substr = str.Mid(bkindex, index - bkindex);
			para = (float)atof((LPCSTR)substr);
			//TRACE("para = %.2f\n",para);
			RTEX_CMD_SetMotionBlend(para);
		}
		else if(substr == "Jogspeed")
		{
			//TRACE("Jog Speed \n");
			float para(0);		    
			bkindex = index + 1;
			index = str.Find(',', bkindex);
			substr = str.Mid(bkindex, index - bkindex);
			para = (float)atof((LPCSTR)substr);
			//TRACE("para = %.2f\n",para);
			if(para > 100) para =100;
			if(para < 1)  para = 1;
			RTEX_CMD_Set_Jog_Speed(para);
		}
		else if (substr == "Setspeedparameter")
		{
			int spdtype;
			float para[2];
			bkindex = index + 1;
			index = str.Find(',', bkindex);
			substr = str.Mid(bkindex, index - bkindex);
			spdtype = atoi(substr);
			//TRACE("spdtype = %d ",spdtype);

			for (int i = 0 ; i < 2 ; i++)
			{
				bkindex = index + 1;
				index = str.Find(',', bkindex);
				substr = str.Mid(bkindex, index - bkindex);
				para[i] = (float)atof((LPCSTR)substr);
				//TRACE("para = %.4f ",para[i]);
			}
			//TRACE("\n");
			RTEX_CMD_SET_XYZ_MotionSpeed(spdtype, para[0], para[1]);
		}
		else if (substr == "resolution")
		{
			int revolution = atoi(str);
			if (revolution < 20000) revolution = 20000;
			//TRACE("revolution = %d \n", revolution);
			pDoc->m_view_resolution = revolution;
			pDoc->m_view_resolutionBK =pDoc->m_view_resolution;
			this->PostMessage(WM_SendredrawInthread,0,0);
			continue;
		}
		else if(substr == "timeslot")
		{
			int timeslot = atoi(str);
			if (timeslot > 1000) timeslot = 1000;
			if (timeslot < 5) timeslot = 5;
			m_uiShowPosTimeSlot = (UINT)timeslot;
			//TRACE("m_uiShowPosTimeSlot = %d \n", m_uiShowPosTimeSlot);
			continue;
		}
		else
		{
			continue;
		}
		if(m_fgStandaloneCode) continue;
		str.Format("Execute Cmd = 0x%02X\n",m_CurrentCmd & 0x7F);
		pDoc->WriteCmd2Txt(str);
		pDoc->WriteMonitormode2txt(m_MonitorSingal[0],m_MonitorSingal[1],m_MonitorSingal[2]);
        
		if(m_fgQcmdImmediatelyStop) break;
		if(QcmdSendCmdFunc())
		{
			//TRACE("timeout\n");
			ShowStatus("QCMD FAIL");
			m_fgQcmdImmediatelyStop = TRUE;
			break;
		}
	}
	if(m_fgStandaloneCode)
	{
		m_StandaloneFile.Close();
		ShowStatus("save standalone file ok");
	}
	TRACE("End Read Txt\n");	
	QCmdFile.Close();	
	if(!m_qQmdFIFOlist.empty())
	{
		TRACE("Qcmd not Empty\n");
	}
	while(!m_qQmdFIFOlist.empty())
	{	
		if(m_fgQcmdImmediatelyStop)
		{
			unsigned char* string = m_qQmdFIFOlist.front();
			delete [] string;
			m_qQmdFIFOlist.pop();
		}
		else if(QcmdSendCmdFunc())
		{
			ShowStatus("QCMD FAIL again");
			break;
		}
	}
	m_fgQcmdImmediatelyStop = FALSE;
	/*CButton* btn = (CButton*)GetDlgItem(IDC_CHECK_ShowPostionRepeatly);
	if(btn->GetCheck())
	{
		btn->SetCheck(0);
		OnBnClickedCheckShowpostionrepeatly();
	}
	*/


}
UINT CRTEXFormView::QCMDThreadFun(LPVOID LPARAM)
{
	CRTEXFormView* view = (CRTEXFormView*)LPARAM;
	CMainFrame* pMainWnd = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	pMainWnd->m_fgRejectClosedByQCmd = TRUE;
	view->m_fgInQCMD = TRUE;
	view->QcmdADDCMDFromTxt();
	view->m_fgInQCMD = FALSE;
	pMainWnd->m_fgRejectClosedByQCmd = FALSE;
	return 0x3333;
}
void CRTEXFormView::OnBnClickedButtonExecuteqcmd()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(!m_fgStandaloneCode)
	{
		if(m_fgInQCMD ||m_WaitCmdAck || (!m_boolRTEX_Configurated) || (!m_connect2RenesasOK)|| (m_uiEnableAxis < 1)) return;
		UpdateData(TRUE);

		m_fgInQCMD = TRUE;
		CButton * pbtn = (CButton*)GetDlgItem(IDC_CHECK_ShowPostionRepeatly);
		if(!pbtn->GetCheck())
		{
			pbtn->SetCheck(1);
			OnBnClickedCheckShowpostionrepeatly();
		}
	}
	/*
	pbtn = (CButton*)GetDlgItem(IDC_CHECK_DisRedraw);
	pbtn->SetCheck(1);
	OnBnClickedCheckDisredraw();
	*/
	//QcmdADDCMDFromTxt();
	AfxBeginThread(&CRTEXFormView::QCMDThreadFun, (LPVOID)this);
	
}
void CRTEXFormView::OnBnClickedButtonDvrReset()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_WaitCmdAck || (!m_boolRTEX_Configurated) || (!m_connect2RenesasOK)|| (m_uiEnableAxis < 1)) return;
	UpdateData(TRUE);
	CButton * btn;
	btn = (CButton*)GetDlgItem(IDC_CHECK_ShowPostionRepeatly);
	btn->SetCheck(0);
	OnBnClickedCheckShowpostionrepeatly();
	btn = (CButton*)GetDlgItem(IDC_Check_Alarm_Warning);
	btn->SetCheck(0);
	OnBnClickedCheckAlarmWarning();

	if(m_ctrlTestMode.GetCheck())
	{
		ShowStatus("can't reset Dvr In test Mode");
			return;
	}

	UINT uiselectedslave= 0;
	for(UINT i = 0 ; i < m_uiEnableAxis; i++)
	{
		uiselectedslave += 1 << m_Axis[i];
	}
	RTEX_CMD_Driver_Software_Reset(uiselectedslave);
	SetWaitCMDAcking(1);
	if(WaitCMDAcking())
	{	
		MessageBox("RTEX_CMD_Driver_Software_Reset  NG");
		return;
	}
	RTEX_CMD_Motion_Run();
	SetWaitCMDAcking(1);
	if(WaitCMDAcking())
	{	
		MessageBox("RTEX_CMD_Motion_Run  NG");
		return;
	}

	btn = (CButton*)GetDlgItem(IDC_CHECK_ServoOn);
	btn->SetCheck(0);
}
void CRTEXFormView::OnBnClickedButtonWarningReset()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_WaitCmdAck || (!m_boolRTEX_Configurated) || (!m_connect2RenesasOK)|| (m_uiEnableAxis < 1)) return;
	UpdateData(TRUE);
	CButton * btn;
	btn = (CButton*)GetDlgItem(IDC_CHECK_ShowPostionRepeatly);
	btn->SetCheck(0);
	OnBnClickedCheckShowpostionrepeatly();
	btn = (CButton*)GetDlgItem(IDC_Check_Alarm_Warning);
	btn->SetCheck(0);
	OnBnClickedCheckAlarmWarning();

	UINT uiselectedslave = 0;
	for(UINT i = 0 ; i < m_uiEnableAxis; i++)
	{
		uiselectedslave += 1 << m_Axis[i];
	}
	RTEX_CMD_Driver_Alarm_Clear(uiselectedslave);
	SetWaitCMDAcking(1);
	if(WaitCMDAcking())
	{	
		MessageBox("RTEX_CMD_Driver_Alarm_Clear  NG");
		return;
	}
	RTEX_CMD_Motion_Run();
	SetWaitCMDAcking(1);
	if(WaitCMDAcking())
	{	
		MessageBox("RTEX_CMD_Motion_Run  NG");
		return;
	}
}

void CRTEXFormView::OnBnClickedButtonExecutecmd()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_WaitCmdAck || (!m_boolRTEX_Configurated) || (!m_connect2RenesasOK)|| (m_uiEnableAxis < 1)) return;
	UpdateData(TRUE);
	//OnBnClickedButtonClearview();
	int CmdSelect(0);
	CmdSelect = m_ctrlCMDselect.GetCurSel();

	//unsigned short vmax = (unsigned short)GetDlgItemInt(IDC_EDIT_Vmax);
	UINT tempvalue(0);
	pDoc->m_view_resolution = pDoc->m_view_resolutionBK;
	pDoc->m_view_Center_pt = CPoint(0,0);
	if(CmdSelect == 16)
	{
		if(!m_fgScaraRobot)
		{
			ShowStatus("not Scara Mode");
			return;
		}
		m_threadPara.m_case = 6;	
		m_lpThread = AfxBeginThread(&CRTEXFormView::MythreadFun, (LPVOID)&m_threadPara);
		return;
	}
	if(CmdSelect == 0)  //check para
	{
		// check X axis view reosultion
		int TargetPos = GetDlgItemInt(IDC_EDIT_Para1);
		tempvalue =UINT(abs(TargetPos) * 1.05);
		tempvalue = (int)(tempvalue * 1000 * m_Axis_Pitch[0] / m_Axis_GearRatio[0]);
		if(pDoc->m_view_resolution < tempvalue)
			pDoc->m_view_resolution =  tempvalue;
		tempvalue = UINT(abs(m_intX_Axis_pos) * 1.05);
		tempvalue = (int)(tempvalue * 1000 * m_Axis_Pitch[0] / m_Axis_GearRatio[0]);
		if(pDoc->m_view_resolution < tempvalue)
			pDoc->m_view_resolution =  tempvalue;

		RTEX_CMD_1_AXIS_MOTION(m_Axis[0],TargetPos);
	}
	else if (CmdSelect == 1)
	{
		if(m_uiEnableAxis < 2)
		{
			MessageBox("只有啟動一軸");
			return;
		}

		// check X axis view reosultion
		int TargetPos1 = GetDlgItemInt(IDC_EDIT_Para1);
		tempvalue =UINT(abs(TargetPos1) * 1.05);
		tempvalue = (int)(tempvalue * 1000 * m_Axis_Pitch[0] / m_Axis_GearRatio[0]);
		if(pDoc->m_view_resolution < tempvalue)
			pDoc->m_view_resolution =  tempvalue;
		tempvalue = UINT(abs(m_intX_Axis_pos) * 1.05);
		tempvalue = (int)(tempvalue * 1000 * m_Axis_Pitch[0] / m_Axis_GearRatio[0]);
		if(pDoc->m_view_resolution < tempvalue)
			pDoc->m_view_resolution =  tempvalue;

		// check Y axis view reosultion
		int TargetPos2 = GetDlgItemInt(IDC_EDIT_Para2);
		tempvalue =UINT(abs(TargetPos2) * 1.05);
		tempvalue = (int)(tempvalue * 1000 * m_Axis_Pitch[1] / m_Axis_GearRatio[1]);
		if(pDoc->m_view_resolution < tempvalue)
			pDoc->m_view_resolution =  tempvalue;
		tempvalue =UINT(abs(m_intY_Axis_pos) * 1.05);
		tempvalue = (int)(tempvalue * 1000 * m_Axis_Pitch[1] / m_Axis_GearRatio[1]);
		if(pDoc->m_view_resolution < tempvalue)
			pDoc->m_view_resolution =  tempvalue;

		RTEX_CMD_2_AXES_LINEAR_MOTION(m_Axis[0],TargetPos1,m_Axis[1],TargetPos2);

	}
	else if((CmdSelect == 2)|| (CmdSelect == 5))
	{
		CString str;
		GetDlgItemText(IDC_EDIT_Para1,str);
		float tarX = (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_Para2,str);
		float tarY = (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_Para3,str);
		float tarZ = (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_Para6,str);
		float tarU = (float)atof((LPCSTR)str);

		if(m_uiEnableAxis < 1)
		{
			MessageBox("啟動軸小於1軸");
			return;
		}
		if(m_uiEnableAxis == 1)
		{
			tarY = 0;
			tarZ = 0;
			SetDlgItemInt(IDC_EDIT_Para2,0);
			SetDlgItemInt(IDC_EDIT_Para3,0);
		}
		else if(m_uiEnableAxis == 2)
		{
			tarZ = 0;
			SetDlgItemInt(IDC_EDIT_Para3,0);
		}
		TRACE("tarx = %.5f,tary = %.5f,tarz = %.5f, taru = %.5f\n" ,tarX,tarY,tarZ,tarU);

		// check X axis view reosultion
		tempvalue = UINT(abs(tarX * 1000 * 1.05));
		if(pDoc->m_view_resolution < tempvalue)
			pDoc->m_view_resolution =  tempvalue;
		tempvalue = UINT(abs(m_iXpos_um * 1.05));
		if(pDoc->m_view_resolution < tempvalue)
			pDoc->m_view_resolution =  tempvalue;

		// check Y axis view reosultion
		tempvalue = UINT(abs(tarY * 1000 * 1.05));
		if(pDoc->m_view_resolution < tempvalue)
			pDoc->m_view_resolution =  tempvalue;
		tempvalue = UINT(abs(m_iYpos_um * 1.05));
		if(pDoc->m_view_resolution < tempvalue)
			pDoc->m_view_resolution =  tempvalue;

		if(CmdSelect == 2)		RTEX_CMD_SET_XYZ_MotionLineXYZ(tarX,tarY,tarZ,tarU);
		else RTEX_CMD_SET_XYZ_MotionJOG(tarX,tarY,tarZ,tarU);
	}
	else if(CmdSelect == 3)
	{
		if(m_uiEnableAxis < 2)
		{
			MessageBox("啟動軸小於2軸");
			return;
		}
		if((GetDlgItemInt(IDC_EDIT_Para4) > (UINT)((m_uiEnableAxis == 2)? 0 : 2)) || (GetDlgItemInt(IDC_EDIT_Para4)< 0))
		{
			MessageBox("設定適當模式");
			SetDlgItemInt(IDC_EDIT_Para4,0);
			GetDlgItem(IDC_EDIT_Para4)->SetFocus();
			return;
		}
		unsigned char ucmode = (unsigned char)GetDlgItemInt(IDC_EDIT_Para4);  // 0, 1, 2
		CString str;
		GetDlgItemText(IDC_EDIT_Para1,str);
		float fcenA = (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_Para2,str);
		float fcenB = (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_Para3,str);
		float fangle = (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_Para6,str);
		float ftarU = (float)atof((LPCSTR)str);

		if((fangle <= 0) || (fangle>360))
		{
			MessageBox("錯誤的角度設定，改設定成360度");
			fangle =360;
		}

		if(ucmode == 0)  //xy circle
		{
			int centerx = (int)(fcenA * 1000) ;
			int centery = (int)(fcenB * 1000) ;
			TRACE("circle cen (%d,%d)\n",centerx,centery);
			double r =pow((double)(centerx - m_iXpos_um),2) + pow((double)(centery - m_iYpos_um),2);
			r = sqrt(r);
			TRACE("circle view r = %.4f\n",r);
			if(pDoc->m_view_resolution < (UINT)abs(centerx + (int)r)) pDoc->m_view_resolution = (UINT)abs(centerx + (int)r);
			if(pDoc->m_view_resolution < (UINT)abs(centerx - (int)r)) pDoc->m_view_resolution = (UINT)abs(centerx - (int)r);
			if(pDoc->m_view_resolution < (UINT)abs(centery + (int)r)) pDoc->m_view_resolution = (UINT)abs(centery + (int)r);
			if(pDoc->m_view_resolution < (UINT)abs(centery - (int)r)) pDoc->m_view_resolution = (UINT)abs(centery - (int)r);

			TRACE("circle view m_view_resolution = %d\n",pDoc->m_view_resolution);
		}
		else if (ucmode == 1) //yz circle ,only care y axis
		{
			int centery = (int)(fcenA * 1000);
			int  centerz = (int)(fcenB * 1000);
			double r =pow((double)(centery - m_iYpos_um),2) + pow((double)(centerz - m_iZpos_um),2);
			r = sqrt(r);

			if(pDoc->m_view_resolution < (UINT)abs(centery + (int)r)) pDoc->m_view_resolution = (UINT)abs(centery + (int)r);
			if(pDoc->m_view_resolution < (UINT)abs(centery - (int)r)) pDoc->m_view_resolution = (UINT)abs(centery - (int)r);

		}
		else if (ucmode == 2) //xz circle ,only care x axis
		{
			int centerx = (int)(fcenA * 1000);
			int  centerz = (int)(fcenB * 1000);
			double r =pow((double)(centerx - m_iXpos_um),2) + pow((double)(centerz - m_iZpos_um),2);
			r = sqrt(r);

			if(pDoc->m_view_resolution < (UINT)abs(centerx + (int)r)) pDoc->m_view_resolution = (UINT)abs(centerx + (int)r);
			if(pDoc->m_view_resolution < (UINT)abs(centerx - (int)r)) pDoc->m_view_resolution = (UINT)abs(centerx - (int)r);
		}

		CButton* btn = (CButton*)GetDlgItem(IDC_CHECK_CW);
		unsigned char uccw = btn->GetCheck();

		RTEX_CMD_SET_XYZ_MotionCircle(ucmode,fcenA,fcenB,fangle,uccw, ftarU);
	}
	else if(CmdSelect == 4)
	{
		if(m_uiEnableAxis < 3)
		{
			MessageBox("啟動軸小於3軸");
			return;
		}
		if((GetDlgItemInt(IDC_EDIT_Para4) > 2) || (GetDlgItemInt(IDC_EDIT_Para4)< 0))
		{
			MessageBox("設定適當模式");
			SetDlgItemInt(IDC_EDIT_Para4,0);
			GetDlgItem(IDC_EDIT_Para4)->SetFocus();
			return;
		}
		unsigned char ucmode = (unsigned char)GetDlgItemInt(IDC_EDIT_Para4);  // 0, 1, 2
		CString str;
		GetDlgItemText(IDC_EDIT_Para1,str);
		float fcenA = (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_Para2,str);
		float fcenB = (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_Para3,str);
		float frotation = (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_Para5,str);
		float fdelta = (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_Para6,str);
		float ftarU = (float)atof((LPCSTR)str);

		if(frotation <= 0) 
		{
			MessageBox("錯誤的團數設定，改設定成1圈");
			frotation =1;
		}

		if(ucmode == 0)  //xy circle
		{
			int centerx = (int)(fcenA * 1000) ;
			int centery = (int)(fcenB * 1000) ;
			TRACE("circle cen (%d,%d)\n",centerx,centery);
			double r =pow((double)(centerx - m_iXpos_um),2) + pow((double)(centery - m_iYpos_um),2);
			r = sqrt(r);
			TRACE("circle view r = %.4f\n",r);
			if(pDoc->m_view_resolution < (UINT)abs(centerx + (int)r)) pDoc->m_view_resolution = (UINT)abs(centerx + (int)r);
			if(pDoc->m_view_resolution < (UINT)abs(centerx - (int)r)) pDoc->m_view_resolution = (UINT)abs(centerx - (int)r);
			if(pDoc->m_view_resolution < (UINT)abs(centery + (int)r)) pDoc->m_view_resolution = (UINT)abs(centery + (int)r);
			if(pDoc->m_view_resolution < (UINT)abs(centery - (int)r)) pDoc->m_view_resolution = (UINT)abs(centery - (int)r);
			TRACE("circle view m_view_resolution = %d\n",pDoc->m_view_resolution);

		}
		else if (ucmode == 1) //yz circle ,only care y axis
		{
			int centery = (int)(fcenA * 1000);
			int  centerz = (int)(fcenB * 1000);
			double r =pow((double)(centery - m_iYpos_um),2) + pow((double)(centerz - m_iZpos_um),2);
			r = sqrt(r);
			if(pDoc->m_view_resolution < (UINT)abs(centery + (int)r)) pDoc->m_view_resolution = (UINT)abs(centery + (int)r);
			if(pDoc->m_view_resolution < (UINT)abs(centery - (int)r)) pDoc->m_view_resolution = (UINT)abs(centery - (int)r);

			UINT tempvalue = (UINT)abs(m_iXpos_um +int(fdelta * 1000));
			if(pDoc->m_view_resolution < tempvalue)
				pDoc->m_view_resolution =  tempvalue;

		}
		else if (ucmode == 2) //xz circle ,only care x axis
		{
			int centerx = (int)(fcenA * 1000);
			int  centerz = (int)(fcenB * 1000);
			double r =pow((double)(centerx - m_iXpos_um),2) + pow((double)(centerz - m_iZpos_um),2);
			r = sqrt(r);

			if(pDoc->m_view_resolution < (UINT)abs(centerx + (int)r)) pDoc->m_view_resolution = (UINT)abs(centerx + (int)r);
			if(pDoc->m_view_resolution < (UINT)abs(centerx - (int)r)) pDoc->m_view_resolution = (UINT)abs(centerx - (int)r);

			UINT tempvalue = (UINT)abs(m_iYpos_um +int(fdelta * 1000));
			if(pDoc->m_view_resolution < tempvalue)
				pDoc->m_view_resolution =  tempvalue;
		}

		CButton* btn = (CButton*)GetDlgItem(IDC_CHECK_CW);
		unsigned char uccw = btn->GetCheck();
		RTEX_CMD_SET_XYZ_MotionHelix(ucmode,fcenA,fcenB,frotation,fdelta,uccw , ftarU);
	}
	else if (CmdSelect == 6)
	{
		if((!m_fgScaraRobot) || (m_uiEnableAxis < 4))
		{
			MessageBox("不是Scara Mode\n或啟動軸小於4軸");
			return;
		}
		CString str;
		GetDlgItemText(IDC_EDIT_Para1,str);
		float tarJX = (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_Para2,str);
		float tarJY = (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_Para3,str);
		float tarJZ = (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_Para6,str);
		float tarJU = (float)atof((LPCSTR)str);

		float tarX = (float)(225 * cos((tarJX+tarJY)*M_PI/180)+325* cos(tarJX*M_PI/180));
		float tarY = (float)(225 * sin((tarJX+tarJY)*M_PI/180)+325* sin(tarJX*M_PI/180));

		// check X axis view reosultion
		tempvalue = UINT(abs(tarX * 1000 * 1.05));
		if(pDoc->m_view_resolution < tempvalue)
			pDoc->m_view_resolution =  tempvalue;
		tempvalue = UINT(abs(m_iXpos_um * 1.05));
		if(pDoc->m_view_resolution < tempvalue)
			pDoc->m_view_resolution =  tempvalue;

		// check Y axis view reosultion
		tempvalue = UINT(abs(tarY * 1000 * 1.05));
		if(pDoc->m_view_resolution < tempvalue)
			pDoc->m_view_resolution =  tempvalue;
		tempvalue = UINT(abs(m_iYpos_um * 1.05));
		if(pDoc->m_view_resolution < tempvalue)
			pDoc->m_view_resolution =  tempvalue;

		RTEX_CMD_SET_J1234_MotionJOG(tarJX, tarJY, tarJZ, tarJU);
	}
	else if(CmdSelect == 7)
	{
		if((!m_fgScaraRobot) || (m_uiEnableAxis < 4))
		{
			MessageBox("不是Scara Mode\n或啟動軸小於4軸");
			return;
		}
		CString str;
		GetDlgItemText(IDC_EDIT_Para1,str);
		float fRev = (float)atof((LPCSTR)str);
		GetDlgItemText(IDC_EDIT_Para2,str);
		float fOmega = (float)atof((LPCSTR)str);
		RTEX_CMD_Scara_J4_Rotate(fRev, fOmega);
	}
	else if ((CmdSelect == 8) || (CmdSelect == 9))
	{
		unsigned char ucmacid, ucbit, uclevel;
		UINT uitimeout, value;
		unsigned char ucTOmacid, ucTObit, ucTOlevel;
		value = GetDlgItemInt(IDC_EDIT_Para1,nullptr,FALSE);
		if(value > 31)
		{ 
			MessageBox("請設定適當的MACID");
			return;
		}
		ucmacid = (unsigned char) value;
		value = GetDlgItemInt(IDC_EDIT_Para2,nullptr,FALSE);
		if(value > 31)
		{ 
			MessageBox("請設定適當的BIT");
			return;
		}
		ucbit = (unsigned char) value;
		value = GetDlgItemInt(IDC_EDIT_Para3,nullptr,FALSE);
		if(value > 1)
		{ 
			MessageBox("請設定適當的Level");
			return;
		}
		uclevel = (unsigned char) value;
		if(CmdSelect == 8)
		{
		  uitimeout = GetDlgItemInt(IDC_EDIT_Para4,nullptr,FALSE);		  
		  value = GetDlgItemInt(IDC_EDIT_Para5,nullptr,FALSE);
		  if((value > 31) && (value != 255))
		  { 
			  MessageBox("請設定適當的Timeout MACID");
			  return;
		  }
		  ucTOmacid = (unsigned char) value;

		  value = GetDlgItemInt(IDC_EDIT_Para6,nullptr,FALSE);
		  if(value > 31)
		  { 
			  MessageBox("請設定適當的Timeout BIT");
			  return;
		  }
		  ucTObit = (unsigned char) value;

		  value = GetDlgItemInt(IDC_EDIT_Para7,nullptr,FALSE);
		  if(value > 1)
		  { 
			  MessageBox("請設定適當的Timeout Level");
			  return;
		  }
		  ucTOlevel = (unsigned char) value;

		  RTEX_CMD_Wait_IO_Board_Status(ucmacid, ucbit, uclevel, uitimeout,ucTOmacid, ucTObit, ucTOlevel);
		}
		else if(CmdSelect == 9)
		{
		  RTEX_CMD_SET_OUTPUT_In_Queue(ucmacid, ucbit, uclevel);
		}
	}
	else if (CmdSelect == 10)
	{
	    int delaytime(0);
		delaytime = GetDlgItemInt(IDC_EDIT_Para1);
		if(delaytime < 0)
		{
			MessageBox("請設定適當的Delay Time");
			return;
		}
		RTEX_CMD_Delay_Time(delaytime);
	}
	else if((CmdSelect ==11) || (CmdSelect ==12))
	{
		unsigned char para(0);
		para = (unsigned char)GetDlgItemInt(IDC_EDIT_Para1, nullptr, FALSE);
		if(para >1)
		{
		    MessageBox("參數請設置  1 或 0");
			return;
		}
		if(CmdSelect == 11) RTEX_CMD_Motion_Pause(para);
		else RTEX_CMD_Motion_Limit_Release(para);
	}
	else if(CmdSelect == 13)
	{
		RTEX_CMD_Motion_Stop();
	}
	else if (CmdSelect == 14)
	{
		RTEX_CMD_Motion_Run();
	}
	else if (CmdSelect == 15)
	{
		RTEX_CMD_Motion_Status();
	}
	CString str;
	pDoc->m_view_resolutionBK = pDoc->m_view_resolution;
	str.Format("Execute Cmd = 0x%02X\n",m_CurrentCmd & 0x7F);
	pDoc->WriteCmd2Txt(str);
	pDoc->WriteMonitormode2txt(m_MonitorSingal[0],m_MonitorSingal[1],m_MonitorSingal[2]);
	pDoc->UpdateAllViews(this);
	SetWaitCMDAcking();
}
void CRTEXFormView::RTEX_EVENT_ACK(unsigned char ucCMD)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'A';
	pCommBuff[4] = ucCMD;
	pCommBuff[5] = 0;
	SendCmd(pCommBuff,6);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_TestMode(bool testmode)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd= CMD_SET_TEST_MODE;
	pCommBuff[5] = 1;
	pCommBuff[6] = (unsigned char)testmode;
	SendCmd(pCommBuff,7);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_Configuration(void)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	//pCommBuff[4] = m_CurrentCmd= (m_ctrlTestMode.GetCheck()? (CMD_CONFIG_RING + 0x80) : CMD_CONFIG_RING);
	pCommBuff[4] = m_CurrentCmd=  CMD_CONFIG_RING;
	pCommBuff[5] = 0x02 * m_uiEnableAxis;
	for(UINT i = 0 ; i < (sizeof(m_fgenableIO_borad) / sizeof(m_fgenableIO_borad[0])) ; i++)  // modify 
	{
		if(m_fgenableIO_borad[i] == TRUE)
		{
			pCommBuff[5] += 0x02;
		}
	}
	for(UINT i = 0 ; i< m_uiEnableAxis; i++)
	{
		pCommBuff[6 + i * 2] = 0xA0 + m_Axis[i];
		pCommBuff[7 + i * 2] = 0x01;
	}
	UINT uiIndex =  6 + m_uiEnableAxis * 2;
	for(UINT i = 0 ; i < (sizeof(m_fgenableIO_borad) / sizeof(m_fgenableIO_borad[0])) ; i++)  // modify 
	{
		if(m_fgenableIO_borad[i] == TRUE)
		{
			pCommBuff[uiIndex] = (m_fgInput[i]? 0xC0 : 0xE0) + m_ucIO_MacID[i];
			pCommBuff[uiIndex + 1] = 0x01;
			uiIndex += 2;
		}
	}
	SendCmd(pCommBuff,6 + pCommBuff[5]);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_Init(void)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[6];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	//pCommBuff[4] = m_CurrentCmd= (m_ctrlTestMode.GetCheck()? (CMD_INIT + 0x80) : CMD_INIT);
	pCommBuff[4] = m_CurrentCmd= CMD_INIT;
	pCommBuff[5] = 0x00;
	SendCmd(pCommBuff,6);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_GetRTEXRingStatus(void)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[6];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd= CMD_GET_RING_STATUS;
	pCommBuff[5] = 0x00;
	SendCmd(pCommBuff,6);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_GetAxisPostion(unsigned int uiSelectedSlaves)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[10];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_GET_AXIS_POSITION;
	pCommBuff[5] = 0x04;

	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));
	SendCmd(pCommBuff,10);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_ServoOnOff(bool servoOn, unsigned int uiSelectedSlaves)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	if(servoOn)
	{
		pCommBuff[4] = m_CurrentCmd = CMD_SERVO_ON;
	}
	else
	{
		pCommBuff[4] = m_CurrentCmd = CMD_SERVO_OFF;
	}
	pCommBuff[5] = 4;

	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));
	SendCmd(pCommBuff,10);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_GetAxisStatus(UINT uiSelectedSlaves)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd= CMD_GET_AXIS_STATUS;
	pCommBuff[5] = 4;
	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));
	SendCmd(pCommBuff,10);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_GetMotionSpdParameter(void)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[6];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_GET_XYZ_MotionSpeed;
	pCommBuff[5] = 0x00;
	SendCmd(pCommBuff,6);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_GetAxisParameter(unsigned char axis)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[7];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_GET_XYZ_AxisParameter;
	pCommBuff[5] = 0x01;
	pCommBuff[6] = axis;
	SendCmd(pCommBuff,7);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_SetAxisParameter(unsigned char axis)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[32];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_SET_XYZ_AxisParameter;
	pCommBuff[5] = 26;
	pCommBuff[6] = axis;
	pCommBuff[7] = m_Axis_Type[axis];

	m_float2uchar.fnumber = m_Axis_Pitch[axis];
	pCommBuff[8] = m_float2uchar.charnumber[3];
	pCommBuff[9] = m_float2uchar.charnumber[2];
	pCommBuff[10] = m_float2uchar.charnumber[1];
	pCommBuff[11] = m_float2uchar.charnumber[0];

	m_float2uchar.inumber = m_Axis_GearRatio[axis];
	pCommBuff[12] = m_float2uchar.charnumber[3];
	pCommBuff[13] = m_float2uchar.charnumber[2];
	pCommBuff[14] = m_float2uchar.charnumber[1];
	pCommBuff[15] = m_float2uchar.charnumber[0];

	m_float2uchar.fnumber = m_Axis_GearRatio_Mech[axis];
	pCommBuff[16] = m_float2uchar.charnumber[3];
	pCommBuff[17] = m_float2uchar.charnumber[2];
	pCommBuff[18] = m_float2uchar.charnumber[1];
	pCommBuff[19] = m_float2uchar.charnumber[0];

	m_float2uchar.fnumber = m_Axis_POTLimit[axis];
	pCommBuff[20] = m_float2uchar.charnumber[3];
	pCommBuff[21] = m_float2uchar.charnumber[2];
	pCommBuff[22] = m_float2uchar.charnumber[1];
	pCommBuff[23] = m_float2uchar.charnumber[0];

	m_float2uchar.fnumber = m_Axis_NOTLimit[axis];
	pCommBuff[24] = m_float2uchar.charnumber[3];
	pCommBuff[25] = m_float2uchar.charnumber[2];
	pCommBuff[26] = m_float2uchar.charnumber[1];
	pCommBuff[27] = m_float2uchar.charnumber[0];
	m_float2uchar.inumber = m_Axis_Encoder_OFS[axis];
	pCommBuff[28] = m_float2uchar.charnumber[3];
	pCommBuff[29] = m_float2uchar.charnumber[2];
	pCommBuff[30] = m_float2uchar.charnumber[1];
	pCommBuff[31] = m_float2uchar.charnumber[0];

	SendCmd(pCommBuff,32);
	delete[] pCommBuff;
	pCommBuff =nullptr;

}
void CRTEXFormView::RTEX_CMD_SET_XYZ_MotionSpeed(int spdtype,float fvmax, float fta)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[18];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_SET_XYZ_MotionSpeed;
	pCommBuff[5] = 12;
	pCommBuff[6] = (unsigned char)((spdtype & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((spdtype & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((spdtype & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((spdtype & 0x000000FF));

	m_float2uchar.fnumber = fvmax;
	pCommBuff[10] = m_float2uchar.charnumber[3];
	pCommBuff[11] = m_float2uchar.charnumber[2];
	pCommBuff[12] = m_float2uchar.charnumber[1];
	pCommBuff[13] = m_float2uchar.charnumber[0];

	m_float2uchar.fnumber = fta;
	pCommBuff[14] = m_float2uchar.charnumber[3];
	pCommBuff[15] = m_float2uchar.charnumber[2];
	pCommBuff[16] = m_float2uchar.charnumber[1];
	pCommBuff[17] = m_float2uchar.charnumber[0];

	SendCmd(pCommBuff,18);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_SET_SPEED_LIMIT(unsigned char macid, unsigned short velocity)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[9];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd =  CMD_SET_SPEED_LIMIT;
	pCommBuff[5] = 0x03;
	pCommBuff[6] = macid;
	pCommBuff[7] = unsigned char((velocity & 0xFF00) >> 8);
	pCommBuff[8] = unsigned char(velocity & 0x00FF);
	SendCmd(pCommBuff,9);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_1_AXIS_MOTION(unsigned char macid, int TargetPos)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];

	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_1_AXIS_MOTION;
	pCommBuff[5] = 0x05;
	pCommBuff[6] = macid;
	pCommBuff[7] = (unsigned char)((TargetPos & 0xFF000000) >> 24);
	pCommBuff[8] = (unsigned char)((TargetPos & 0x00FF0000) >> 16);
	pCommBuff[9] = (unsigned char)((TargetPos & 0x0000FF00) >> 8);
	pCommBuff[10] = (unsigned char)((TargetPos & 0x000000FF));
	SendCmd(pCommBuff,11);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_2_AXES_LINEAR_MOTION(unsigned char macid1, int TargetPos1, unsigned char macid2, int TargetPos2)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_2_AXES_LINEAR_MOTION;
	pCommBuff[5] = 0x0A;
	pCommBuff[6] = macid1;
	pCommBuff[7] = (unsigned char)((TargetPos1 & 0xFF000000) >> 24);
	pCommBuff[8] = (unsigned char)((TargetPos1 & 0x00FF0000) >> 16);
	pCommBuff[9] = (unsigned char)((TargetPos1 & 0x0000FF00) >> 8);
	pCommBuff[10] = (unsigned char)((TargetPos1 & 0x000000FF));
	pCommBuff[11] = macid2;
	pCommBuff[12] = (unsigned char)((TargetPos2 & 0xFF000000) >> 24);
	pCommBuff[13] = (unsigned char)((TargetPos2 & 0x00FF0000) >> 16);
	pCommBuff[14] = (unsigned char)((TargetPos2 & 0x0000FF00) >> 8);
	pCommBuff[15] = (unsigned char)((TargetPos2 & 0x000000FF));
	SendCmd(pCommBuff,16);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_SET_XYZ_MotionLineXYZ(float tarX, float tarY, float tarZ, float tarU)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_SET_XYZ_MotionLineXYZ;
	pCommBuff[5] = 0x10;
	m_float2uchar.fnumber = tarX;
	pCommBuff[6] = m_float2uchar.charnumber[3];
	pCommBuff[7] = m_float2uchar.charnumber[2];
	pCommBuff[8] = m_float2uchar.charnumber[1];
	pCommBuff[9] = m_float2uchar.charnumber[0];
	m_float2uchar.fnumber = tarY;
	pCommBuff[10] = m_float2uchar.charnumber[3];
	pCommBuff[11] = m_float2uchar.charnumber[2];
	pCommBuff[12] = m_float2uchar.charnumber[1];
	pCommBuff[13] = m_float2uchar.charnumber[0];
	m_float2uchar.fnumber = tarZ;
	pCommBuff[14] = m_float2uchar.charnumber[3];
	pCommBuff[15] = m_float2uchar.charnumber[2];
	pCommBuff[16] = m_float2uchar.charnumber[1];
	pCommBuff[17] = m_float2uchar.charnumber[0];
	m_float2uchar.fnumber = tarU;
	pCommBuff[18] = m_float2uchar.charnumber[3];
	pCommBuff[19] = m_float2uchar.charnumber[2];
	pCommBuff[20] = m_float2uchar.charnumber[1];
	pCommBuff[21] = m_float2uchar.charnumber[0];

	SendCmd(pCommBuff,22);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_SET_XYZ_MotionCircle(unsigned char ucmode, float fcenA, float fcenB, float fangle, unsigned char uccw, float ftarU)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_SET_XYZ_MotionCircle;
	pCommBuff[5] = 0x12;
	pCommBuff[6] = ucmode;

	m_float2uchar.fnumber = fcenA;
	pCommBuff[7] = m_float2uchar.charnumber[3];
	pCommBuff[8] = m_float2uchar.charnumber[2];
	pCommBuff[9] = m_float2uchar.charnumber[1];
	pCommBuff[10] = m_float2uchar.charnumber[0];

	m_float2uchar.fnumber = fcenB;
	pCommBuff[11] = m_float2uchar.charnumber[3];
	pCommBuff[12] = m_float2uchar.charnumber[2];
	pCommBuff[13] = m_float2uchar.charnumber[1];
	pCommBuff[14] = m_float2uchar.charnumber[0];

	m_float2uchar.fnumber = fangle;
	pCommBuff[15] = m_float2uchar.charnumber[3];
	pCommBuff[16] = m_float2uchar.charnumber[2];
	pCommBuff[17] = m_float2uchar.charnumber[1];
	pCommBuff[18] = m_float2uchar.charnumber[0];
	pCommBuff[19] = uccw;
	m_float2uchar.fnumber = ftarU;
	pCommBuff[20] = m_float2uchar.charnumber[3];
	pCommBuff[21] = m_float2uchar.charnumber[2];
	pCommBuff[22] = m_float2uchar.charnumber[1];
	pCommBuff[23] = m_float2uchar.charnumber[0];
	SendCmd(pCommBuff,24);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_SET_XYZ_MotionHelix(unsigned char ucmode, float fcenA, float fcenB, float frotation, float fdelta, unsigned char uccw,  float ftarU)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_SET_XYZ_MotionHelix;
	pCommBuff[5] = 0x16;
	pCommBuff[6] = ucmode;
	m_float2uchar.fnumber = fcenA;
	pCommBuff[7] = m_float2uchar.charnumber[3];
	pCommBuff[8] = m_float2uchar.charnumber[2];
	pCommBuff[9] = m_float2uchar.charnumber[1];
	pCommBuff[10] = m_float2uchar.charnumber[0];
	m_float2uchar.fnumber = fcenB;
	pCommBuff[11] = m_float2uchar.charnumber[3];
	pCommBuff[12] = m_float2uchar.charnumber[2];
	pCommBuff[13] = m_float2uchar.charnumber[1];
	pCommBuff[14] = m_float2uchar.charnumber[0];
	m_float2uchar.fnumber = frotation;
	pCommBuff[15] = m_float2uchar.charnumber[3];
	pCommBuff[16] = m_float2uchar.charnumber[2];
	pCommBuff[17] = m_float2uchar.charnumber[1];
	pCommBuff[18] = m_float2uchar.charnumber[0];
	m_float2uchar.fnumber = fdelta;
	pCommBuff[19] = m_float2uchar.charnumber[3];
	pCommBuff[20] = m_float2uchar.charnumber[2];
	pCommBuff[21] = m_float2uchar.charnumber[1];
	pCommBuff[22] = m_float2uchar.charnumber[0];
	pCommBuff[23] = uccw;
	m_float2uchar.fnumber = ftarU;
	pCommBuff[24] = m_float2uchar.charnumber[3];
	pCommBuff[25] = m_float2uchar.charnumber[2];
	pCommBuff[26] = m_float2uchar.charnumber[1];
	pCommBuff[27] = m_float2uchar.charnumber[0];

	SendCmd(pCommBuff,28);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_SetDRIVER_PARA_RW_CMD(int axis,unsigned char Command_Type,unsigned char Parameter_Class,unsigned char Parameter_No,int Setting_Data)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[15];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';	
	pCommBuff[4] = m_CurrentCmd = CMD_DRIVER_PARA_RW;
	pCommBuff[5] = 0x09;
	pCommBuff[6] = axis;
	pCommBuff[7] = Command_Type;//TYPE_CODE L
	pCommBuff[8] = 0x00;//TYPE_CODE H
	pCommBuff[9] = Parameter_Class;  
	pCommBuff[10] = Parameter_No; 
	pCommBuff[11] = (unsigned char)((Setting_Data & 0xFF000000) >> 24);
	pCommBuff[12] = (unsigned char)((Setting_Data & 0x00FF0000) >> 16);
	pCommBuff[13] = (unsigned char)((Setting_Data & 0x0000FF00) >> 8);
	pCommBuff[14] = (unsigned char)((Setting_Data & 0x000000FF));
	SendCmd(pCommBuff,15);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_Monitor_CMD(int axis,unsigned char Channel)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[8];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_MONITOR;
	pCommBuff[5] = 0x02;
	pCommBuff[6] = axis;
	pCommBuff[7] = Channel;
	SendCmd(pCommBuff,8);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_GetMonitorData(unsigned int uiSelectedSlaves)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[10];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_GET_MONITOR_DATA;
	pCommBuff[5] = 0x04;
	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));
	SendCmd(pCommBuff,10);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}

void CRTEXFormView::RTEX_CMD_SET_MoveMode(unsigned int uiSelectedSlaves, unsigned char ucmode)
{
	//modify for socket
	if(ucmode == 0x40) // for Ken Huang
	{
		if(saveDebugEvent.m_hFile == CFile::hFileNull)
		  saveDebugEvent.Open("SaveTorque.txt",CFile::modeCreate|CFile::modeWrite);
	}
	else
	{
		if(saveDebugEvent.m_hFile != CFile::hFileNull)  saveDebugEvent.Close();
	}
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_SET_MoveMode;
	pCommBuff[5] = 5;

	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));
	pCommBuff[10] = ucmode;
	SendCmd(pCommBuff,11);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_SET_TARCMD(unsigned int uiSelectedSlaves, int cmdTarget, int TSPD)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_SET_TARCMD;
	pCommBuff[5] = 12;

	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));

	pCommBuff[10] = (unsigned char)((cmdTarget & 0xFF000000) >> 24);
	pCommBuff[11] = (unsigned char)((cmdTarget & 0x00FF0000) >> 16);
	pCommBuff[12] = (unsigned char)((cmdTarget & 0x0000FF00) >> 8);
	pCommBuff[13] = (unsigned char)((cmdTarget & 0x000000FF));

	pCommBuff[14] = (unsigned char)((TSPD & 0xFF000000) >> 24);
	pCommBuff[15] = (unsigned char)((TSPD & 0x00FF0000) >> 16);
	pCommBuff[16] = (unsigned char)((TSPD & 0x0000FF00) >> 8);
	pCommBuff[17] = (unsigned char)((TSPD & 0x000000FF));
	SendCmd(pCommBuff,18);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_SET_Profile_ABS_POSCMD(unsigned int uiSelectedSlaves)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_SET_Profile_ABS_POSCMD;
	pCommBuff[5] = 4;

	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));
	SendCmd(pCommBuff,10);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_Jog_Mode_Move(unsigned int uiSelectedSlaves, int cmdTarget, int TSPD)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_JOG_Move ;
	pCommBuff[5] = 12;

	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));

	pCommBuff[10] = (unsigned char)((cmdTarget & 0xFF000000) >> 24);
	pCommBuff[11] = (unsigned char)((cmdTarget & 0x00FF0000) >> 16);
	pCommBuff[12] = (unsigned char)((cmdTarget & 0x0000FF00) >> 8);
	pCommBuff[13] = (unsigned char)((cmdTarget & 0x000000FF));

	pCommBuff[14] = (unsigned char)((TSPD & 0xFF000000) >> 24);
	pCommBuff[15] = (unsigned char)((TSPD & 0x00FF0000) >> 16);
	pCommBuff[16] = (unsigned char)((TSPD & 0x0000FF00) >> 8);
	pCommBuff[17] = (unsigned char)((TSPD & 0x000000FF));
	SendCmd(pCommBuff,18);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_JOG_REL_Move(unsigned int uiSelectedSlaves, int cmdTarget, int TSPD)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_JOG_REL_Move ;
	pCommBuff[5] = 12;

	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));

	pCommBuff[10] = (unsigned char)((cmdTarget & 0xFF000000) >> 24);
	pCommBuff[11] = (unsigned char)((cmdTarget & 0x00FF0000) >> 16);
	pCommBuff[12] = (unsigned char)((cmdTarget & 0x0000FF00) >> 8);
	pCommBuff[13] = (unsigned char)((cmdTarget & 0x000000FF));

	pCommBuff[14] = (unsigned char)((TSPD & 0xFF000000) >> 24);
	pCommBuff[15] = (unsigned char)((TSPD & 0x00FF0000) >> 16);
	pCommBuff[16] = (unsigned char)((TSPD & 0x0000FF00) >> 8);
	pCommBuff[17] = (unsigned char)((TSPD & 0x000000FF));
	SendCmd(pCommBuff,18);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_SET_PPMode_CMD(unsigned int uiSelectedSlaves, int AccConst, int DecConst)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd =  CMD_SET_PPMode_CMD;
	pCommBuff[5] = 12;

	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));

	pCommBuff[10] = (unsigned char)((AccConst & 0xFF000000) >> 24);
	pCommBuff[11] = (unsigned char)((AccConst & 0x00FF0000) >> 16);
	pCommBuff[12] = (unsigned char)((AccConst & 0x0000FF00) >> 8);
	pCommBuff[13] = (unsigned char)((AccConst & 0x000000FF));

	pCommBuff[14] = (unsigned char)((DecConst & 0xFF000000) >> 24);
	pCommBuff[15] = (unsigned char)((DecConst & 0x00FF0000) >> 16);
	pCommBuff[16] = (unsigned char)((DecConst & 0x0000FF00) >> 8);
	pCommBuff[17] = (unsigned char)((DecConst & 0x000000FF));
	SendCmd(pCommBuff,18);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_SET_PPHOME_CMD(unsigned int uiSelectedSlaves, unsigned short home_mode, int vel1, int vel2)
{
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd =  CMD_SET_PPHOME_CMD;
	pCommBuff[5] = 14;

	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));

	pCommBuff[10] = (unsigned char)((home_mode & 0xFF00) >> 8);
	pCommBuff[11] = (unsigned char)((home_mode & 0x00FF));

	pCommBuff[12] = (unsigned char)((vel1 & 0xFF000000) >> 24);
	pCommBuff[13] = (unsigned char)((vel1 & 0x00FF0000) >> 16);
	pCommBuff[14] = (unsigned char)((vel1 & 0x0000FF00) >> 8);
	pCommBuff[15] = (unsigned char)((vel1 & 0x000000FF));

	pCommBuff[16] = (unsigned char)((vel2 & 0xFF000000) >> 24);
	pCommBuff[17] = (unsigned char)((vel2 & 0x00FF0000) >> 16);
	pCommBuff[18] = (unsigned char)((vel2 & 0x0000FF00) >> 8);
	pCommBuff[19] = (unsigned char)((vel2 & 0x000000FF));
	SendCmd(pCommBuff,20);
	delete[] pCommBuff;
	pCommBuff =nullptr;


}
void CRTEXFormView::RTEX_CMD_Parameter_Writing_S_CMD(unsigned int uiSelectedSlaves,unsigned char Parameter_Class,unsigned char Parameter_No, unsigned short value)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Parameter_Writing_S_CMD;
	pCommBuff[5] = 8;

	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));

	pCommBuff[10] = Parameter_Class;

	pCommBuff[11] = Parameter_No;

	pCommBuff[12] = (unsigned char)((value & 0xFF00) >> 8);
	pCommBuff[13] = (unsigned char)((value & 0x00FF));

	SendCmd(pCommBuff,14);
	delete[] pCommBuff;
	pCommBuff =nullptr;

}
void CRTEXFormView::RTEX_CMD_Parameter_Writing_I_CMD(unsigned int uiSelectedSlaves,unsigned char Parameter_Class,unsigned char Parameter_No, int value)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Parameter_Writing_I_CMD;
	pCommBuff[5] = 10;

	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));

	pCommBuff[10] = Parameter_Class;

	pCommBuff[11] = Parameter_No;

	pCommBuff[12] = (unsigned char)((value & 0xFF000000) >> 24);
	pCommBuff[13] = (unsigned char)((value & 0x00FF0000) >> 16);
	pCommBuff[14] = (unsigned char)((value & 0x0000FF00) >> 8);
	pCommBuff[15] = (unsigned char)((value & 0x000000FF));
	SendCmd(pCommBuff,16);
	delete[] pCommBuff;
	pCommBuff =nullptr;


}
void CRTEXFormView::RTEX_CMD_Parameter_Reading_S_CMD(unsigned char macid,unsigned char Parameter_Class,unsigned char Parameter_No)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[9];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Parameter_Reading_S_CMD;
	pCommBuff[5] = 0x03;
	pCommBuff[6] = macid;
	pCommBuff[7] = Parameter_Class;
	pCommBuff[8] = Parameter_No;
	SendCmd(pCommBuff,9);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_Parameter_Reading_I_CMD(unsigned char macid,unsigned char Parameter_Class,unsigned char Parameter_No)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[9];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Parameter_Reading_I_CMD;
	pCommBuff[5] = 0x03;
	pCommBuff[6] = macid;
	pCommBuff[7] = Parameter_Class;
	pCommBuff[8] = Parameter_No;
	SendCmd(pCommBuff,9);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_Parameter_EEPROM_Writing_CMD(unsigned int uiSelectedSlaves)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Parameter_EEPROM_Writing_CMD;
	pCommBuff[5] = 4;

	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));
	SendCmd(pCommBuff,10);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_RESET()
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[6];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	//pCommBuff[4] = m_CurrentCmd= (m_ctrlTestMode.GetCheck()? (CMD_INIT + 0x80) : CMD_INIT);
	pCommBuff[4] = m_CurrentCmd= CMD_RESET;
	pCommBuff[5] = 0x00;
	SendCmd(pCommBuff,6);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_CMD_SetMotionBlend(float Percentage)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[10];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';	
	pCommBuff[4] = m_CurrentCmd = CMD_SetMotionBlend;
	pCommBuff[5] = 0x04;
	m_float2uchar.fnumber = Percentage;
	pCommBuff[6] = m_float2uchar.charnumber[3];
	pCommBuff[7] = m_float2uchar.charnumber[2];
	pCommBuff[8] = m_float2uchar.charnumber[1];
	pCommBuff[9] = m_float2uchar.charnumber[0];
	SendCmd(pCommBuff,10);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_CMD_Driver_Software_Reset(unsigned int uiSelectedSlaves)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Driver_Software_Reset;
	pCommBuff[5] = 4;
	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));
	SendCmd(pCommBuff,10);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_Scara_Robot_Home()
{
	if( ((int)GetDlgItemInt(IDC_EDIT_J2_Theta) * (int)m_HomeJ[1]) > 0)
	{
	  RTEX_CMD_SET_J1234_MotionJOG(m_HomeJ[0],m_HomeJ[1],m_HomeJ[2],m_HomeJ[3]);
	}
	else 
	{
	  RTEX_CMD_SET_J1234_MotionJOG(-m_HomeJ[0],-m_HomeJ[1],m_HomeJ[2],m_HomeJ[3]);
	}
}
void CRTEXFormView::RTEX_CMD_SET_XYZ_MotionCircle3D(float P2X, float P2Y, float P2Z ,float P3X, float P3Y, float P3Z,float angle, unsigned char uccw)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_SET_XYZ_MotionCircle3D;
	pCommBuff[5] = 0x1D;
	m_float2uchar.fnumber = P2X;
	pCommBuff[6] = m_float2uchar.charnumber[3];
	pCommBuff[7] = m_float2uchar.charnumber[2];
	pCommBuff[8] = m_float2uchar.charnumber[1];
	pCommBuff[9] = m_float2uchar.charnumber[0];
	m_float2uchar.fnumber = P2Y;
	pCommBuff[10] = m_float2uchar.charnumber[3];
	pCommBuff[11] = m_float2uchar.charnumber[2];
	pCommBuff[12] = m_float2uchar.charnumber[1];
	pCommBuff[13] = m_float2uchar.charnumber[0];
	m_float2uchar.fnumber = P2Z;
	pCommBuff[14] = m_float2uchar.charnumber[3];
	pCommBuff[15] = m_float2uchar.charnumber[2];
	pCommBuff[16] = m_float2uchar.charnumber[1];
	pCommBuff[17] = m_float2uchar.charnumber[0];

	m_float2uchar.fnumber = P3X;
	pCommBuff[18] = m_float2uchar.charnumber[3];
	pCommBuff[19] = m_float2uchar.charnumber[2];
	pCommBuff[20] = m_float2uchar.charnumber[1];
	pCommBuff[21] = m_float2uchar.charnumber[0];
	m_float2uchar.fnumber = P3Y;
	pCommBuff[22] = m_float2uchar.charnumber[3];
	pCommBuff[23] = m_float2uchar.charnumber[2];
	pCommBuff[24] = m_float2uchar.charnumber[1];
	pCommBuff[25] = m_float2uchar.charnumber[0];
	m_float2uchar.fnumber = P3Z;
	pCommBuff[26] = m_float2uchar.charnumber[3];
	pCommBuff[27] = m_float2uchar.charnumber[2];
	pCommBuff[28] = m_float2uchar.charnumber[1];
	pCommBuff[29] = m_float2uchar.charnumber[0];

	m_float2uchar.fnumber = angle;
	pCommBuff[30] = m_float2uchar.charnumber[3];
	pCommBuff[31] = m_float2uchar.charnumber[2];
	pCommBuff[32] = m_float2uchar.charnumber[1];
	pCommBuff[33] = m_float2uchar.charnumber[0];

	pCommBuff[34] = uccw;
	SendCmd(pCommBuff,35);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_SET_XYZ_MotionJOG(float tarX, float tarY, float tarZ, float tarU)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_SET_XYZ_MotionJOG;
	pCommBuff[5] = 0x10;
	m_float2uchar.fnumber = tarX;
	pCommBuff[6] = m_float2uchar.charnumber[3];
	pCommBuff[7] = m_float2uchar.charnumber[2];
	pCommBuff[8] = m_float2uchar.charnumber[1];
	pCommBuff[9] = m_float2uchar.charnumber[0];
	m_float2uchar.fnumber = tarY;
	pCommBuff[10] = m_float2uchar.charnumber[3];
	pCommBuff[11] = m_float2uchar.charnumber[2];
	pCommBuff[12] = m_float2uchar.charnumber[1];
	pCommBuff[13] = m_float2uchar.charnumber[0];
	m_float2uchar.fnumber = tarZ;
	pCommBuff[14] = m_float2uchar.charnumber[3];
	pCommBuff[15] = m_float2uchar.charnumber[2];
	pCommBuff[16] = m_float2uchar.charnumber[1];
	pCommBuff[17] = m_float2uchar.charnumber[0];
	m_float2uchar.fnumber = tarU;
	pCommBuff[18] = m_float2uchar.charnumber[3];
	pCommBuff[19] = m_float2uchar.charnumber[2];
	pCommBuff[20] = m_float2uchar.charnumber[1];
	pCommBuff[21] = m_float2uchar.charnumber[0];

	SendCmd(pCommBuff,22);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_READ_USER_DATA(unsigned char ucpage, unsigned short usoffset,unsigned char uclength)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_READ_USER_DATA;
	pCommBuff[5] = 0x04 ;
	pCommBuff[6] = ucpage;
	pCommBuff[7] = (unsigned char)((usoffset & 0xFF00) >> 8);
	pCommBuff[8] = (unsigned char)((usoffset & 0x00FF));
	pCommBuff[9] = uclength;
	SendCmd(pCommBuff,10);
	delete [] pCommBuff;
	pCommBuff = nullptr;
}
void CRTEXFormView::RTEX_CMD_WRITE_USER_DATA(unsigned char ucpage, unsigned short usoffset,unsigned char uclength, unsigned char* pdata)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_WRITE_USER_DATA;
	pCommBuff[5] = 0x04 + uclength;

	pCommBuff[6] = ucpage;
	pCommBuff[7] = (unsigned char)((usoffset & 0xFF00) >> 8);
	pCommBuff[8] = (unsigned char)((usoffset & 0x00FF));
	pCommBuff[9] = uclength;

	for (int i = 0 ; i < uclength ; i ++)
	{
		pCommBuff[10+i] = pdata[i];
	}
	SendCmd(pCommBuff,(10 + uclength));
	delete [] pCommBuff;
	pCommBuff = nullptr;
}
void CRTEXFormView::RTEX_CMD_SYNC_USER_DATA()
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[6];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_SYNC_USER_DATA;
	pCommBuff[5] = 0x00;
	SendCmd(pCommBuff,6);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_CMD_Scara_J4_Rotate(float fRev, float fOmega)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Scara_J4_Rotate;
	pCommBuff[5] = 0x08;
	m_float2uchar.fnumber = fRev;
	pCommBuff[6] = m_float2uchar.charnumber[3];
	pCommBuff[7] = m_float2uchar.charnumber[2];
	pCommBuff[8] = m_float2uchar.charnumber[1];
	pCommBuff[9] = m_float2uchar.charnumber[0];
	m_float2uchar.fnumber = fOmega;
	pCommBuff[10] = m_float2uchar.charnumber[3];
	pCommBuff[11] = m_float2uchar.charnumber[2];
	pCommBuff[12] = m_float2uchar.charnumber[1];
	pCommBuff[13] = m_float2uchar.charnumber[0];
	SendCmd(pCommBuff,14);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_CMD_SET_J1234_MotionJOG(float ftarJX, float ftarJY, float ftarJZ, float ftarJU)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_SET_J1234_MotionJOG;
	pCommBuff[5] = 0x10;
	m_float2uchar.fnumber = ftarJX;
	pCommBuff[6] = m_float2uchar.charnumber[3];
	pCommBuff[7] = m_float2uchar.charnumber[2];
	pCommBuff[8] = m_float2uchar.charnumber[1];
	pCommBuff[9] = m_float2uchar.charnumber[0];
	m_float2uchar.fnumber = ftarJY;
	pCommBuff[10] = m_float2uchar.charnumber[3];
	pCommBuff[11] = m_float2uchar.charnumber[2];
	pCommBuff[12] = m_float2uchar.charnumber[1];
	pCommBuff[13] = m_float2uchar.charnumber[0];
	m_float2uchar.fnumber = ftarJZ;
	pCommBuff[14] = m_float2uchar.charnumber[3];
	pCommBuff[15] = m_float2uchar.charnumber[2];
	pCommBuff[16] = m_float2uchar.charnumber[1];
	pCommBuff[17] = m_float2uchar.charnumber[0];
	m_float2uchar.fnumber = ftarJU;
	pCommBuff[18] = m_float2uchar.charnumber[3];
	pCommBuff[19] = m_float2uchar.charnumber[2];
	pCommBuff[20] = m_float2uchar.charnumber[1];
	pCommBuff[21] = m_float2uchar.charnumber[0];
	SendCmd(pCommBuff,22);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void  CRTEXFormView::RTEX_CMD_GET_INPUT(unsigned char macid)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_GET_INPUT;
	pCommBuff[5] = 1;
	pCommBuff[6] = macid;
	SendCmd(pCommBuff,7);
	delete[] pCommBuff;
	pCommBuff =nullptr;

}
void  CRTEXFormView::RTEX_CMD_SET_OUTPUT(unsigned char macid, UINT value)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_SET_OUTPUT;
	pCommBuff[5] = 5;
	pCommBuff[6] = macid;
	pCommBuff[7] = (unsigned char)((value & 0xFF000000) >> 24);
	pCommBuff[8] = (unsigned char)((value & 0x00FF0000) >> 16);
	pCommBuff[9] = (unsigned char)((value & 0x0000FF00) >> 8);
	pCommBuff[10] = (unsigned char)((value & 0x000000FF));
	SendCmd(pCommBuff,11);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_SET_OUTPUT(unsigned char macid, unsigned char ucbit, unsigned char uclevel)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_SET_OUTPUT;
	pCommBuff[5] = 3;
	pCommBuff[6] = macid;
	pCommBuff[7] = ucbit;
	pCommBuff[8] = uclevel;
	SendCmd(pCommBuff,9);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_Wait_IO_Board_Status(unsigned char macid, unsigned char ucbit, unsigned char uclevel, UINT uitimeout, unsigned char TOmacid, unsigned char ucTObit, unsigned char ucTOlevel)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Wait_IO_Board_Status;
	pCommBuff[5] = 10;
	pCommBuff[6] = macid;
	pCommBuff[7] = ucbit;
	pCommBuff[8] = uclevel;
	pCommBuff[9] = (unsigned char)((uitimeout & 0xFF000000) >> 24);
	pCommBuff[10] = (unsigned char)((uitimeout & 0x00FF0000) >> 16);
	pCommBuff[11] = (unsigned char)((uitimeout & 0x0000FF00) >> 8);
	pCommBuff[12] = (unsigned char)((uitimeout & 0x000000FF));
	pCommBuff[13] = TOmacid;
	pCommBuff[14] = ucTObit;
	pCommBuff[15] = ucTOlevel;
	SendCmd(pCommBuff,16);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_SET_OUTPUT_In_Queue(unsigned char macid, unsigned char ucbit, unsigned char uclevel)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_SET_OUTPUT_In_Queue;
	pCommBuff[5] = 3;
	pCommBuff[6] = macid;
	pCommBuff[7] = ucbit;
	pCommBuff[8] = uclevel;
	SendCmd(pCommBuff,9);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void  CRTEXFormView::RTEX_CMD_Delay_Time(int idelaytime)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Delay_Time;
	pCommBuff[5] = 4;	
	pCommBuff[6] = (unsigned char)((idelaytime & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((idelaytime & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((idelaytime & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((idelaytime & 0x000000FF));
	SendCmd(pCommBuff,10);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_Motion_Pause(unsigned char ucpause)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[7];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Motion_Pause;
	pCommBuff[5] = 0x01;
	pCommBuff[6] = ucpause;
	SendCmd(pCommBuff,7);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_CMD_Motion_Stop(void)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[6];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Motion_Stop;
	pCommBuff[5] = 0x00;
	SendCmd(pCommBuff,6);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_CMD_Motion_Limit_Release(unsigned char uclimitrelease)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[7];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Motion_Limit_Release;
	pCommBuff[5] = 0x01;
	pCommBuff[6] = uclimitrelease;
	SendCmd(pCommBuff,7);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_CMD_Driver_Alarm_Warning(UINT uiSelectedSlaves)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Driver_Alarm_Warning;
	pCommBuff[5] = 4;
	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));
	SendCmd(pCommBuff,10);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_Driver_Alarm_Clear(UINT uiSelectedSlaves)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Driver_Alarm_Clear;
	pCommBuff[5] = 4;
	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));
	SendCmd(pCommBuff,10);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_Motion_Run()
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[6];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Motion_Run;
	pCommBuff[5] = 0x00;
	SendCmd(pCommBuff,6);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_CMD_Motion_Status(void)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[6];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Motion_Status;
	pCommBuff[5] = 0x00;
	SendCmd(pCommBuff,6);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView:: RTEX_CMD_Get_Jog_Speed_parameter()
{
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[6];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Get_Jog_Speed_parameter;
	pCommBuff[5] = 0x00;
	SendCmd(pCommBuff,6);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_CMD_Set_Jog_MaxRPM(int iMaxRPM)
{
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[10];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Set_Jog_MaxRPM;
	pCommBuff[5] = 0x04;
	m_float2uchar.inumber = iMaxRPM;
	pCommBuff[6] = m_float2uchar.charnumber[3];
	pCommBuff[7] = m_float2uchar.charnumber[2];
	pCommBuff[8] = m_float2uchar.charnumber[1];
	pCommBuff[9] = m_float2uchar.charnumber[0];

	SendCmd(pCommBuff,10);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_CMD_Set_Jog_Speed(float Percentage)
{
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[10];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Set_Jog_Speed;
	pCommBuff[5] = 0x04;
	m_float2uchar.fnumber = Percentage;
	pCommBuff[6] = m_float2uchar.charnumber[3];
	pCommBuff[7] = m_float2uchar.charnumber[2];
	pCommBuff[8] = m_float2uchar.charnumber[1];
	pCommBuff[9] = m_float2uchar.charnumber[0];

	SendCmd(pCommBuff,10);
	delete[] pCommBuff;
	pCommBuff =nullptr;	

}
void CRTEXFormView::RTEX_CMD_Get_New_AXIS_POSITION(UINT uiSelectedSlaves)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Get_New_AXIS_POSITION;
	pCommBuff[5] = 4;
	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));
	SendCmd(pCommBuff,10);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_Get_Scara_Hand()
{
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[6];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Get_Scara_Hand;
	pCommBuff[5] = 0x00;
	SendCmd(pCommBuff,6);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_CMD_SET_Scara_Robot_mech(float fL1, float fL2, float fheight)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_SET_Scara_Robot_mech;
	pCommBuff[5] = 0x0C;
	m_float2uchar.fnumber = fL1;
	pCommBuff[6] = m_float2uchar.charnumber[3];
	pCommBuff[7] = m_float2uchar.charnumber[2];
	pCommBuff[8] = m_float2uchar.charnumber[1];
	pCommBuff[9] = m_float2uchar.charnumber[0];
	m_float2uchar.fnumber = fL2;
	pCommBuff[10] = m_float2uchar.charnumber[3];
	pCommBuff[11] = m_float2uchar.charnumber[2];
	pCommBuff[12] = m_float2uchar.charnumber[1];
	pCommBuff[13] = m_float2uchar.charnumber[0];
	m_float2uchar.fnumber = fheight;
	pCommBuff[14] = m_float2uchar.charnumber[3];
	pCommBuff[15] = m_float2uchar.charnumber[2];
	pCommBuff[16] = m_float2uchar.charnumber[1];
	pCommBuff[17] = m_float2uchar.charnumber[0];
	SendCmd(pCommBuff,18);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_GET_Scara_Robot_mech()
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[6];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_GET_Scara_Robot_mech;
	pCommBuff[5] = 0x00;
	SendCmd(pCommBuff,6);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_CMD_Switch_Deubug_RS232(unsigned char ucdisable)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[7];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Switch_Deubug_RS232;
	pCommBuff[5] = 0x01;
	pCommBuff[6] = ucdisable;
	SendCmd(pCommBuff,7);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_CMD_ShowPosTime(unsigned char ucshowtimedata)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[7];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_ShowPosTime;
	pCommBuff[5] = 0x01;
	pCommBuff[6] = ucshowtimedata;
	SendCmd(pCommBuff,7);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_CMD_Check_ALL_Queue_Done()
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[6];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Check_ALL_Queue_Done;
	pCommBuff[5] = 0x00;
	SendCmd(pCommBuff,6);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_CMD_SystemID(UINT uiSelectedSlaves, unsigned char ucCmdType)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_SystemID;
	pCommBuff[5] = 5;
	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));
	pCommBuff[10] = ucCmdType;
	SendCmd(pCommBuff,11);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_Reload_Scara_Parameter(void)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[6];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Reload_Scara_Parameter;
	pCommBuff[5] = 0x00;
	SendCmd(pCommBuff,6);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_CMD_Update_Scara_Parameter(unsigned char type)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[7];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Reload_Scara_Parameter;
	pCommBuff[5] = 0x01;
	pCommBuff[6] = type;
	SendCmd(pCommBuff,7);
	delete[] pCommBuff;
	pCommBuff =nullptr;	
}
void CRTEXFormView::RTEX_CMD_CLR_MULTI_TERN(UINT uiSelectedSlaves)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_CLR_MULTI_TERN;
	pCommBuff[5] = 4;
	pCommBuff[6] = (unsigned char)((uiSelectedSlaves & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uiSelectedSlaves & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uiSelectedSlaves & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uiSelectedSlaves & 0x000000FF));	
	SendCmd(pCommBuff,10);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_Get_Servo_FW_NAME(void)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Get_7216_Information;
	pCommBuff[5] = 0;	
	SendCmd(pCommBuff,6);
	delete[] pCommBuff;
	pCommBuff =nullptr;

}
void CRTEXFormView::RTEX_CMD_SET_Software_Timeout(UINT uitimeout)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_SET_Software_Timeout;
	pCommBuff[5] = 4;	
	pCommBuff[6] = (unsigned char)((uitimeout & 0xFF000000) >> 24);
	pCommBuff[7] = (unsigned char)((uitimeout & 0x00FF0000) >> 16);
	pCommBuff[8] = (unsigned char)((uitimeout & 0x0000FF00) >> 8);
	pCommBuff[9] = (unsigned char)((uitimeout & 0x000000FF));	
	SendCmd(pCommBuff,10);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_Set_Positon_Event(unsigned char ucenable, UINT uitime)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Set_Positon_Event;
	pCommBuff[5] = 6;	
	pCommBuff[6] = ucenable;	
	pCommBuff[7] = (unsigned char)((uitime & 0xFF000000) >> 24);
	pCommBuff[8] = (unsigned char)((uitime & 0x00FF0000) >> 16);
	pCommBuff[9] = (unsigned char)((uitime & 0x0000FF00) >> 8);
	pCommBuff[10] = (unsigned char)((uitime & 0x000000FF));
	pCommBuff[11] = 1;	
	SendCmd(pCommBuff,12);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_Set_IOBorad_SendMsg(unsigned char ucenable)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_Set_IOBorad_SendMsg;
	pCommBuff[5] = 1;	
	pCommBuff[6] = ucenable;	
	SendCmd(pCommBuff,7);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_SET_LOGIC_GATE(unsigned char ucLogicPort,unsigned char ucEnable,unsigned char ucMode,unsigned char ucMAcIDA,unsigned char ucPortA,unsigned char ucMAcIDB,unsigned char ucPortB,unsigned char ucMAcIDC,unsigned char ucPortC)
{
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[512];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = m_CurrentCmd = CMD_SET_LOGIC_GATE;
	pCommBuff[5] = 9;	
	pCommBuff[6] = ucLogicPort;
	pCommBuff[7] = ucEnable;
	pCommBuff[8] = ucMode;
	pCommBuff[9] = ucMAcIDA;
	pCommBuff[10] = ucPortA;
	pCommBuff[11] = ucMAcIDB;
	pCommBuff[12] = ucPortB;
	pCommBuff[13] = ucMAcIDC;
	pCommBuff[14] = ucPortC;
	SendCmd(pCommBuff,15);
	delete[] pCommBuff;
	pCommBuff =nullptr;
}

UINT CRTEXFormView::MythreadFun(LPVOID LParam)
{
	CMythreadParam* para = (CMythreadParam*)LParam;	
	CRTEXFormView* lpview = (CRTEXFormView*)(para->m_lpPara);
	para-> m_blthreading = TRUE;
	switch (para->m_case)
	{
	case 0 :
		lpview->Thread_Parameterset();
		break;
	case 1 :
		lpview->Thread_SetRtexConfiguration();
		break;
	case 2 :
		lpview->Thread_Opendriverregdialog();
		break;
	case 3 :
		lpview->Thread_OpenSpCMDdialog();
		break;
	case 4 :
		lpview->Thread_openIO_BoradDialog();
		break;
	case 5 :
		lpview->Thread_OpenSystemIDdialog();
		break;
	case 6 :
		lpview->Thread_OpenJogDialog();
		break;
	case 7 :
		lpview->Thread_Zero();
		break;
	case 8:
		lpview->ServerCommend();
	default:
		break;
	}
	para-> m_blthreading = FALSE;
	para->m_case = 0xFF;
	return 0;

}
void CRTEXFormView::Thread_Parameterset(void)
{
	CButton * pbtn = (CButton*)GetDlgItem(IDC_CHECK_ShowPostionRepeatly);
	if(pbtn->GetCheck())
	{
		pbtn->SetCheck(0);
		OnBnClickedCheckShowpostionrepeatly();
	}
	SetWaitCMDAcking(1);
	RTEX_CMD_Motion_Run();
	if(WaitCMDAcking())
	{
		MessageBox("can't set motion run");
		return;
	}

	RTEX_GetMotionSpdParameter();
	SetWaitCMDAcking(1);
	if(WaitCMDAcking())
	{
		MessageBox("can't get speed parameter from Renesas");
		return;
	}
	RTEX_CMD_Get_Jog_Speed_parameter();
	SetWaitCMDAcking(1);
	if(WaitCMDAcking())
	{
		MessageBox("can't get Jog speed parameter from Renesas");
		return;
	}
	if(m_fgScaraRobot)
	{
		RTEX_CMD_GET_Scara_Robot_mech();
		SetWaitCMDAcking(1);
		if(WaitCMDAcking())
		{
			MessageBox("can't GET_Scara_Robot_mech from Renesas");
			return;
		}
	}

	if(m_boolRTEX_Configurated)
	{
		for(UINT i = 0; i < m_uiEnableAxis ; i++)
		{
			RTEX_GetAxisParameter(i);
			SetWaitCMDAcking(1);
			if(WaitCMDAcking())
			{
				MessageBox("can't get Axis parameter from Renesas");
				return;
			}

		}
	}
	
	CAxisSetting axissetting(this);
	for(int i = 0 ; i < 6 ; i++)
	{
		axissetting.m_MacID_axis[i] = m_Axis[i];
		axissetting.m_Axis_Pitch[i] = m_Axis_Pitch[i];
		axissetting.m_Axis_GearRatio[i] = m_Axis_GearRatio[i];
		axissetting.m_Axis_POTLimit[i] = m_Axis_POTLimit[i];
		axissetting.m_Axis_NOTLimit[i] = m_Axis_NOTLimit[i];
		axissetting.m_Axis_GearRatio_Mech[i] = m_Axis_GearRatio_Mech[i];
		axissetting.m_Axis_Type[i] = m_Axis_Type[i];
		axissetting.m_Axis_EncoderOffset[i] = m_Axis_Encoder_OFS[i];
	}
	axissetting.m_HomeJ1 = m_HomeJ[0];
	axissetting.m_HomeJ2 = m_HomeJ[1];
	axissetting.m_HomeJ3 = m_HomeJ[2];
	axissetting.m_HomeJ4 = m_HomeJ[3];
	axissetting.m_boolRTEX_Configurated = m_boolRTEX_Configurated;
	axissetting.m_uiEnableAxis = m_uiEnableAxis;
	axissetting.m_intcmbxSpdType = m_intcmbxSpdType;
	axissetting.m_fVmax = m_fVmax;
	axissetting.m_uiTa = m_uiTa;
	axissetting.m_iJog_MaxRPM = m_iJog_MaxRPM;
	axissetting.m_fJog_Speed = m_fJog_Speed;

	axissetting.m_uiShowPosTimeSlot = m_uiShowPosTimeSlot;
	axissetting.m_uiCheckErrTimeSlot = m_uiCheckErrTimeSlot;
	if(m_fgScaraRobot)
	{
	  axissetting.m_fScaraRobot_L1= m_fScaraRobot_L1;
	  axissetting.m_fScaraRobot_L2= m_fScaraRobot_L2;
	  axissetting.m_fScaraRobot_Height= m_fScaraRobot_Height;
	  axissetting.m_iScaraRobot_Arm_dir = (int)m_ucScaraRobot_Arm_dir;
	}
	if(IDOK == axissetting.DoModal())
	{
		m_uiEnableAxis = axissetting.m_uiEnableAxis;	  
		for(UINT i = 0 ; i< m_uiEnableAxis; i++)
		{
			m_Axis[i] = axissetting.m_MacID_axis[i];
			m_Axis_Pitch[i] = axissetting.m_page[i].m_Pitch;
			m_Axis_GearRatio[i] = axissetting.m_page[i].m_GearRatio;
			m_Axis_POTLimit[i] = axissetting.m_page[i].m_POTLimit;
			m_Axis_NOTLimit[i] = axissetting.m_page[i].m_NOTLimit;
			m_Axis_GearRatio_Mech[i] = axissetting.m_page[i].m_GearRatio_Mech;
			m_Axis_Encoder_OFS[i] = axissetting.m_Axis_EncoderOffset[i];
			TRACE("m_axis_pitch[%d] = %.2f, m_Axis_GearRatio[%d] = %d\n",i,m_Axis_Pitch[i],i,m_Axis_GearRatio[i]);
		}
		m_intcmbxSpdType = axissetting.m_intcmbxSpdType;
		m_fVmax = axissetting.m_fVmax;
		m_uiTa = axissetting.m_uiTa;
		m_uiShowPosTimeSlot = axissetting.m_uiShowPosTimeSlot;
		if(m_uiShowPosTimeSlot < 10) m_uiShowPosTimeSlot = 10;
		m_uiCheckErrTimeSlot = axissetting.m_uiCheckErrTimeSlot;
		if(m_uiCheckErrTimeSlot < 100) m_uiCheckErrTimeSlot = 100;
	}
	TRACE("enable axis = %d\n",m_uiEnableAxis);
	TRACE("spdtype = %d, vmax = %.2f, ta = %d\n",m_intcmbxSpdType,m_fVmax,m_uiTa);
	
}
void CRTEXFormView::Thread_SetRtexConfiguration(void)
{
	SetWaitCMDAcking(1);
	RTEX_Configuration();	
	if(WaitCMDAcking()) return;
	m_boolRTEX_Configurated = TRUE;
	
	//Set Bits high for MAC ID
	m_uiSelectedSlaves = 0;
	for(UINT i = 0 ; i < m_uiEnableAxis; i++)
	{
		m_uiSelectedSlaves += (1 << m_Axis[i]);
	}
	SetWaitCMDAcking(1);
	RTEX_Init();
	if(WaitCMDAcking()) return;
	SetWaitCMDProgress(CMD_INIT,500);
	if(WaitCMDProgress()) return;

	if(m_fgScaraRobot && (m_uiEnableAxis > 0))
	{
  	  SetWaitCMDAcking(1);
  	  RTEX_CMD_GET_Scara_Robot_mech();
	  if(WaitCMDAcking())
	  {
		  MessageBox("can't GET_Scara_Robot_mech from Renesas");
		  return;
	  }
	  SetWaitCMDAcking(1);
	  RTEX_CMD_Reload_Scara_Parameter();
	  if(WaitCMDAcking())
	  {
		  MessageBox("can't _Reload_Scara_Parameter from Renesas");
		  return;
	  }
	}
	SetWaitCMDAcking(1);
	RTEX_CMD_Motion_Run();
	if(WaitCMDAcking())
	{
		MessageBox("can't set motion run");
		return;
	}
	for(UINT i = 0; i < m_uiEnableAxis ; i++)
	{
		SetWaitCMDAcking(1);
		RTEX_GetAxisParameter(i);
		if(WaitCMDAcking())
		{
			MessageBox("can't get Axis parameter from Renesas");
			return;
		}
	}
	for(UINT i = 0 ; i < (sizeof(m_fgenableIO_borad) / sizeof(m_fgenableIO_borad[0])) ; i++)  // modify 
	{
		if((m_fgenableIO_borad[i] == TRUE) && (m_fgInput[i] == FALSE))
		{
			SetWaitCMDAcking(1);
			RTEX_CMD_SET_OUTPUT_In_Queue(m_ucIO_MacID[i],31,1);  //Motor Power 
			if(WaitCMDAcking())
			{
				MessageBox("can't Set first output board channel 31 high");
				return;
			}
			SetWaitCMDAcking(1);
			RTEX_CMD_SET_OUTPUT_In_Queue(m_ucIO_MacID[i],30,1);  // robot light
			if(WaitCMDAcking())
			{
				MessageBox("can't Set first output board channel 30 high");
				return;
			}
			SetWaitCMDAcking(1);
			RTEX_CMD_SET_OUTPUT_In_Queue(m_ucIO_MacID[i],0,1);  //Break release
			if(WaitCMDAcking())
			{
				MessageBox("can't Set first output board channel 30 high");
				return;
			}

			break;
		}
	
	}
	SetWaitCMDAcking(1);
	//RTEX_GetAxisPostion(m_uiSelectedSlaves);
	RTEX_CMD_Get_New_AXIS_POSITION(m_uiSelectedSlaves);
	if(WaitCMDAcking())
	{ 
		MessageBox("can't get current position");
		return;
	}
	GetDlgItem(IDC_BUTTON_Set_RTEX_Configuration)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_TestMode)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_ServoOn)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_Scara)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_ShowPostionRepeatly)->EnableWindow(TRUE);
	GetDlgItem(IDC_Check_Alarm_Warning)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_FreeDynamicBrk)->EnableWindow(TRUE);
	CComboBox* pcbx = (CComboBox*)GetDlgItem(IDC_COMBO_SelectedDvr);
	pcbx->AddString("All driver");
	for (UINT i = 0 ; i < m_uiEnableAxis ;i++)
	{
		CString str;
		str.Format("MacID : %d",m_Axis[i]);
		pcbx->AddString(str);
	}
	pcbx->SetCurSel(0);
}
void CRTEXFormView::Setaxisparameter(void)
{
	CString str,strADD;	
	str.Format("設置Axis 參數\n");
	for(UINT i = 0 ; i < m_uiEnableAxis ; i++)
	{
		strADD.Format("axis[%d] : (Pitch = %.2f mm, GearRatio = %d cmd/rev,\n",i,m_Axis_Pitch[i],m_Axis_GearRatio[i]);
		str += strADD;
		strADD.Format("                 GearRatio_mech = %.2f, POT = %.2f mm, NOT = %.2f mm,\n",m_Axis_GearRatio_Mech[i], m_Axis_POTLimit[i], m_Axis_NOTLimit[i]);
		str += strADD;
		strADD.Format("                 encoder_ofst = %d)\n", m_Axis_Encoder_OFS[i]);
        str += strADD;
	}
	MessageBox(str,"Axis Parameter");

	for(UINT i = 0 ; i < m_uiEnableAxis ; i++)
	{
		SetWaitCMDAcking(1);
		RTEX_SetAxisParameter(i);
		if(WaitCMDAcking()) return;
	}
}
void CRTEXFormView::Thread_Opendriverregdialog(void)
{	
    CDriverRegister  pDvrReg(this);
	pDvrReg.DoModal();

}
void CRTEXFormView::Thread_OpenSpCMDdialog(void)
{
	//remove this function
}
void CRTEXFormView::Thread_openIO_BoradDialog(void)
{
	int IODeviceNum = 0;
	if(m_boolRTEX_Configurated)
	{
		CButton * pbtn = (CButton*)GetDlgItem(IDC_CHECK_ShowPostionRepeatly);
		if(!pbtn->GetCheck())
		{
			pbtn->SetCheck(1);
			OnBnClickedCheckShowpostionrepeatly();
		}

		UINT uiselectedslave = 0;
		for(UINT i = 0 ; i < (sizeof(m_fgenableIO_borad) / sizeof(m_fgenableIO_borad[0])) ; i++)  // modify 
		{
			if(m_fgenableIO_borad[i])
			{
				IODeviceNum++;
				uiselectedslave += 1 << m_ucIO_MacID[i];
			}
		}
		if(uiselectedslave)
		{
			TRACE("selectedslave = %08X\n",uiselectedslave);
			for(UINT i = 0 ; i < (sizeof(m_fgenableIO_borad) / sizeof(m_fgenableIO_borad[0])) ; i++)  // modify 
			{
				TRACE("io_status[%d] = %08X\n",i , m_uiIOStatus[i]);
			}
			TRACE("IO Device num = %d\n",IODeviceNum);
		}
	}
	bool fgUseboardV2 = FALSE;
	for(UINT i = 4 ; i < (sizeof(m_fgenableIO_borad) / sizeof(m_fgenableIO_borad[0])) ; i++)  // modify 
	{
		if(m_fgenableIO_borad[i] == TRUE)  fgUseboardV2 = TRUE;
	}
	if((!m_boolRTEX_Configurated) || (fgUseboardV2 == TRUE))
	{
		CIOBoardSetV2 ioboradsetV2(this);
		ioboradsetV2.DoModal();
		return;
	}
	CIOBoardSet  ioboradset(this);
	if(IDOK == ioboradset.DoModal())
	{
		if(!m_boolRTEX_Configurated)
		{
			for(UINT i = 0 ; i < (sizeof(ioboradset.m_fgenableIO_borad) / sizeof(ioboradset.m_fgenableIO_borad[0])) ; i++)  // modify 
			{
			  m_fgenableIO_borad[i] =  ioboradset.m_fgenableIO_borad[i];
			  m_fgInput[i] = ioboradset.m_fgInput[i];
			  TRACE("I/O Borad %d : enable = % d, input = %d, macID = %d\n",i , m_fgenableIO_borad[i], m_fgInput[i], m_ucIO_MacID[i]);
			}
		}
	}

}
void CRTEXFormView::Thread_OpenSystemIDdialog(void)
{
	CSystemID systemID(this);
	systemID.DoModal();
}
void CRTEXFormView::Thread_OpenJogDialog(void)
{
	TRACE("open jog Dialog\n");
	if(m_boolRTEX_Configurated)
	{
		CButton * pbtn = (CButton*)GetDlgItem(IDC_CHECK_ShowPostionRepeatly);
		if(!pbtn->GetCheck())
		{
			pbtn->SetCheck(1);
			OnBnClickedCheckShowpostionrepeatly();
		}
	}
	COpenJogDialog jogDialog(this);
	jogDialog.DoModal();
}
void CRTEXFormView::Setspeedparameter()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_WaitCmdAck || (!m_connect2RenesasOK)||(boolunSetSpdParamter == TRUE)) return;	
	CString str;
	str.Format("設置motion參數\n(Vmax = %.5f mm/ms, Ta = %d ms, SpeedType = %d)",m_fVmax,m_uiTa,m_intcmbxSpdType);
	MessageBox(str,"Motion Parameter");
	RTEX_CMD_SET_XYZ_MotionSpeed(m_intcmbxSpdType,m_fVmax, (float)m_uiTa);
	SetWaitCMDAcking(1);
	if(WaitCMDAcking())
	{	
		MessageBox("SET Speed Parameter NG");
		return;
	}	
}
void CRTEXFormView::DriverRegDialog_GetAxisDriverPara(int axis,unsigned char para_class, unsigned char para_reg)
{
	SetWaitCMDAcking(1);
	RTEX_SetDRIVER_PARA_RW_CMD(axis, Type_Code_x6_Parameter_Reading, para_class, para_reg, 0);
	if(WaitCMDAcking())
	{	
		MessageBox("RTEX_SetDRIVER_NON_CYCLINC_MODE mode 0 NG");
		return;
	}	
}
void CRTEXFormView::DriverRegDialog_SetAxisDriverPara(int axis,unsigned char para_class, unsigned char para_reg, int value)
{
	SetWaitCMDAcking(1);
	RTEX_SetDRIVER_PARA_RW_CMD(axis,Type_Code_x6_Parameter_Writing,para_class, para_reg,value);
	if(WaitCMDAcking())
	{	
		MessageBox("RTEX_SetDRIVER_NON_CYCLINC_CMD  NG");
		return;
	}	
}
void CRTEXFormView::DriverRegDialog_EEPROMWriteDriverPara(int axis)
{
	SetWaitCMDAcking(1);
	RTEX_SetDRIVER_PARA_RW_CMD(axis,Type_Code_x6_Parameter_EEPROM_W,0,0,0);
	if(WaitCMDAcking())
	{	
		MessageBox("RTEX_SetDRIVER_NON_CYCLINC_CMD  NG");
		return;
	}	

}
void CRTEXFormView::DriverRegDialog_Monitor_CMD(int axis,unsigned char Channel)
{
	SetWaitCMDAcking(1);
	RTEX_Monitor_CMD(axis, Channel);
	if(WaitCMDAcking())
	{	
		MessageBox("RTEX_Monitor_CMD  NG");
		return;
	}	
}

UINT CRTEXFormView::ShowPosThreadFun(LPVOID LParam)
{
	CRTEXFormView* view = (CRTEXFormView*) LParam;
	CMainFrame* pMainWnd = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	pMainWnd->m_fgRejectClosedByShowPos = TRUE;
	view->ShowPosEvent();
	pMainWnd->m_fgRejectClosedByShowPos = FALSE;
	return 0x2222;
}
void CRTEXFormView::ShowPosEvent(void)
{
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime;
	LARGE_INTEGER nEndTime;
	double dtime;
	int j = 0;
	UINT uiselectedsalve = 0 , uiselectedsalveALL = 0;
	bool blMonitor = FALSE;
	//int Xpos(0), Ypos(0), Zpos(0), Upos(0);
	int SamePos(0);
	TRACE("m_uiEnableAxis =  %d \n",m_uiEnableAxis);
	for(UINT i = 0 ; i < m_uiEnableAxis ; i++)
	{
		TRACE("m_MonitorSingal[%d] = %d\n",i, m_MonitorSingal[i]);
		if(m_MonitorSingal[i] != 0)
		{
			TRACE("moni %d\n",i);
			blMonitor = TRUE;
			uiselectedsalve += (1 << m_Axis[i]);
		}
	}
	uiselectedsalveALL = m_uiSelectedSlaves;	
	for(UINT i = 0 ; i < (sizeof(m_fgenableIO_borad) / sizeof(m_fgenableIO_borad[0])) ; i++)  // modify 
	{
		if(m_fgenableIO_borad[i] == TRUE)
		{
			uiselectedsalveALL += (1<< m_ucIO_MacID[i]);
		}
	}
	QueryPerformanceFrequency(&nFreq);
	while(m_blShowPos)
	{
		j++;
		QueryPerformanceCounter(&nBeginTime);
		Sleep(m_uiShowPosTimeSlot/2);
		if((!m_blWaiitCmdPROGRESS) && (!m_WaitCmdAck) && (m_boolRTEX_Configurated) && (m_connect2RenesasOK))
		{
			SetWaitCMDAcking(1);
			if(((j % 2) != 0) ||(blMonitor == FALSE))
			{
			  //RTEX_GetAxisPostion(uiselectedsalveALL);
			  RTEX_CMD_Get_New_AXIS_POSITION(uiselectedsalveALL);
			}
			else
			  RTEX_GetMonitorData(uiselectedsalve);
			while(m_WaitCmdAck && (m_uintCmdtimerCnt <= 3));
			if(m_uintCmdtimerCnt > 3)
			{
				ShowStatus(" ask position timeout");
				ClearWaitCMDAcking();
				//OnBnClickedButtonDisconnect();
				//return;
			}            
		}		

		do 
		{
		  QueryPerformanceCounter(&nEndTime);
		  dtime=(double)(nEndTime.QuadPart-nBeginTime.QuadPart)/(double)nFreq.QuadPart;
		} while (dtime < (double)(m_uiShowPosTimeSlot * 0.001)); //while 直到間隔時間到
	}
}
UINT CRTEXFormView::CheckErrThreadFun(LPVOID LParam)
{
	CRTEXFormView* view = (CRTEXFormView*) LParam;
	view->CheckErrEvent();
	return 0;
}
void CRTEXFormView::CheckErrEvent()
{
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime;
	LARGE_INTEGER nEndTime;
	double dtime;

	QueryPerformanceFrequency(&nFreq);
	while(m_blCheckErr)
	{
		QueryPerformanceCounter(&nBeginTime);
		if((!m_blWaiitCmdPROGRESS) && (!m_WaitCmdAck) && (m_boolRTEX_Configurated) && (m_connect2RenesasOK))
		{
			RTEX_CMD_Driver_Alarm_Warning(m_uiSelectedSlaves);
			SetWaitCMDAcking(1);
			while(m_WaitCmdAck && (m_uintCmdtimerCnt <= 2));
			if(m_uintCmdtimerCnt > 2)
			{
				ShowStatus("timeout");
				ClearWaitCMDAcking();
				OnBnClickedButtonDisconnect();
				return;
			}
			do 
			{
				QueryPerformanceCounter(&nEndTime);
				dtime=(double)(nEndTime.QuadPart-nBeginTime.QuadPart)/(double)nFreq.QuadPart;
			} while (dtime < (double)(m_uiCheckErrTimeSlot * 0.001)); //while 直到間隔時間到
		}

	}
}


void CRTEXFormView::TransPulse2XYZ(void)
{
	CString str;
	float fXpos, fYpos, fZpos, fUdeg , fYaw;

	if(m_fgScaraRobot)
	{
		
		float theta1 =360 * (float)m_intX_Axis_pos / m_Axis_GearRatio_Mech[0] / m_Axis_GearRatio[0];
		float theta2 =360 * (float)m_intY_Axis_pos / m_Axis_GearRatio_Mech[1] / m_Axis_GearRatio[1];
		fXpos = (float)(m_fScaraRobot_L2 * cos((theta1+theta2)*M_PI/180)+ m_fScaraRobot_L1* cos(theta1*M_PI/180));
		fYpos = (float)(m_fScaraRobot_L2 * sin((theta1+theta2)*M_PI/180)+ m_fScaraRobot_L1* sin(theta1*M_PI/180));
		fZpos = m_Axis_Pitch[2] * (float)m_intZ_Axis_pos / (float)m_Axis_GearRatio[2] / m_Axis_GearRatio_Mech[2];
		fUdeg = (float)360 * (float)m_intU_Axis_pos / (float)m_Axis_GearRatio[3] / m_Axis_GearRatio_Mech[3] ;	    
		fZpos = fZpos - fUdeg * m_Axis_Pitch[2] / (float)360;
		fYaw = theta1 + theta2 + fUdeg;

		m_fYaw = fYaw;
		m_ftheta1 = theta1;		m_ftheta2 = theta2;		m_ftheta4 = fUdeg;
	}
	else
	{
		fXpos = m_Axis_Pitch[0] * (float)m_intX_Axis_pos / (float)m_Axis_GearRatio[0];
		fYpos = m_Axis_Pitch[1] * (float)m_intY_Axis_pos /  (float)m_Axis_GearRatio[1];
		fZpos = m_Axis_Pitch[2] * (float)m_intZ_Axis_pos / (float)m_Axis_GearRatio[2];
	}
	m_fx = fXpos;		m_fy = fYpos;		m_fz = fZpos;
	m_iXpos_um =(int)(0.5 + (fXpos * 1000));
	m_iYpos_um =(int)(0.5 + (fYpos * 1000));
	m_iZpos_um =(int)(0.5 + (fZpos * 1000));

	PostMessage(WM_SendUpdateJoint_Pos,0,0);
}
afx_msg LRESULT CRTEXFormView::OnSendUpdateJoint_Pos(WPARAM wParam, LPARAM lParam)
{
	CString str;
	
	SetDlgItemInt(IDC_EDIT_X_Pulse_Axis,m_intX_Axis_pos);
	SetDlgItemInt(IDC_EDIT_Y_Pulse_Axis,m_intY_Axis_pos);
	SetDlgItemInt(IDC_EDIT_Z_Pulse_Axis,m_intZ_Axis_pos);
	SetDlgItemInt(IDC_EDIT_U_Pulse_Axis,m_intU_Axis_pos);
	str.Format("%.3f",m_ftheta1);
	SetDlgItemText(IDC_EDIT_J1_Theta,str);
	str.Format("%.3f",m_ftheta2);
	SetDlgItemText(IDC_EDIT_J2_Theta,str);
	str.Format("%.3f",m_ftheta4);
	SetDlgItemText(IDC_EDIT_J4_Theta,str);
	str.Format("%.3f",m_fYaw);
	SetDlgItemText(IDC_EDIT_Yaw_Axis,str);
	str.Format("%.3f",m_fx);
	GetDlgItem(IDC_EDIT_X_Axis)->SetWindowText(str);
	str.Format("%.3f",m_fy);
	GetDlgItem(IDC_EDIT_Y_Axis)->SetWindowText(str);
	str.Format("%.3f",m_fz);
	GetDlgItem(IDC_EDIT_Z_Axis)->SetWindowText(str);
	return 0;
}

void CRTEXFormView::OnTcnSelchangeTabLog(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此加入控制項告知處理常式程式碼
	*pResult = 0;
	switch (m_ctrlTAB_LOG.GetCurSel())
	{
	case 0 :
        GetDlgItem(IDC_LIST_Status)->ShowWindow(TRUE);
		GetDlgItem(IDC_LIST_Log_History)->ShowWindow(FALSE);
		break;
	case 1 :
		GetDlgItem(IDC_LIST_Status)->ShowWindow(FALSE);
		GetDlgItem(IDC_LIST_Log_History)->ShowWindow(TRUE);
		break;
	}
}
void CRTEXFormView::SetRecieveMsgFontSize(void)
{
	if(m_pFont)
	  delete m_pFont;
	m_pFont = new CFont;
	m_pFont->CreatePointFont(m_intFont_size, "Arial");
	m_ctrllistStatus.SetFont(m_pFont, 1);	
	m_ctrllistLog_History.SetFont(m_pFont, 1);	
}

void CRTEXFormView::OnBnClickedButtonIncreaselogfontsize()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_intFont_size <= 290) m_intFont_size += 10;
	SetRecieveMsgFontSize();
}


void CRTEXFormView::OnBnClickedButtonDecreaselogfontsize()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_intFont_size >=60)	m_intFont_size -= 10;
	SetRecieveMsgFontSize();
}


void CRTEXFormView::OnBnClickedButtonMotionStop()
{
	// TODO: 在此加入控制項告知處理常式程式碼
    if(m_WaitCmdAck || (!m_boolRTEX_Configurated) || (!m_connect2RenesasOK)|| (m_uiEnableAxis < 1)) return;	
	SetWaitCMDAcking();
	if(m_fgInQCMD)
	{
		ShowStatus("Q CMD immediately stop");
		m_fgQcmdImmediatelyStop  = TRUE;
	}	
	RTEX_CMD_Motion_Stop();	
	m_fgHIFCmdBufferFull = FALSE;
}


void CRTEXFormView::OnBnClickedButtonMotionRun()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_WaitCmdAck || (!m_boolRTEX_Configurated) || (!m_connect2RenesasOK)|| (m_uiEnableAxis < 1)) return;
	SetWaitCMDAcking();
	RTEX_CMD_Motion_Run();
	m_fgQcmdImmediatelyStop  = FALSE;
	m_fgHIFCmdBufferFull = FALSE;

}


void CRTEXFormView::OnBnClickedCheckDisredraw()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CButton* pbtn = (CButton*)GetDlgItem(IDC_CHECK_DisRedraw);
	if(pbtn->GetCheck())
	{
		pDoc->m_fgskipRedraw = TRUE;
	}
	else
	{
		pDoc->m_fgskipRedraw = FALSE;
	}
}

afx_msg LRESULT CRTEXFormView::OnSendredrawinthread(WPARAM wParam, LPARAM lParam)
{
	TRACE("send Redraw in Thread\n");
	pDoc->UpdateAllViews(0);	
	return 0;
}


void CRTEXFormView::Connect7216OK(void)
{
	m_connect2RenesasOK = TRUE;
	GetDlgItem(IDC_BUTTON_Disconnect)->EnableWindow(TRUE);	
	GetDlgItem(IDC_STATIC_IP)->EnableWindow(FALSE);
	GetDlgItem(IDC_IPADDRESS)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_Port)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_Port)->EnableWindow(FALSE);

	GetDlgItem((IDC_BUTTON_ParameterSet))->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_Set_RTEX_Configuration)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_TestMode)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_Scara)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_Dvr_Reset)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_Warning_Reset)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_Scara_home)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_IO_Set)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_DisableDebugRS232)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_ShowPosTimeData)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DumpDebugMsg)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_DumpDebugMsg)->EnableWindow(TRUE);
}
UINT CRTEXFormView::WaitConnectThread(LPVOID LPARAM)
{
	CRTEXFormView* view = (CRTEXFormView*)LPARAM;
	DWORD dwStart=GetTickCount();
	DWORD dwEnd =dwStart;
	do 
	{
		if(view->m_bClientConnected)
			break;
		dwEnd =GetTickCount() - dwStart;
	} while (dwEnd < 3000);
	if(!view->m_bClientConnected)
	{
		//MessageBox("連線失敗!!");
		view->ShowStatus("connect Fail");
		view->OnBnClickedButtonDisconnect();
	}
	return 0x5566;
}


void CRTEXFormView::OnBnClickedCheckDisabledebugrs232()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CButton* pbtn = (CButton*)GetDlgItem(IDC_CHECK_DisableDebugRS232);
	if(m_WaitCmdAck || (!m_connect2RenesasOK))
	{
        pbtn->SetCheck(!pbtn->GetCheck());
		return;
	}
	SetWaitCMDAcking();
	if(pbtn->GetCheck()) RTEX_CMD_Switch_Deubug_RS232(1);
	else RTEX_CMD_Switch_Deubug_RS232(0);
}


void CRTEXFormView::OnBnClickedCheckShowpostimedata()
{
	// TODO: 在此加入控制項告知處理常式程式碼
    CButton* pbtn = (CButton*)GetDlgItem(IDC_CHECK_ShowPosTimeData);
	if(m_WaitCmdAck || (!m_connect2RenesasOK))
	{
		pbtn->SetCheck(!pbtn->GetCheck());
		return;
	}
	SetWaitCMDAcking();	
	if(pbtn->GetCheck()) RTEX_CMD_ShowPosTime(1);
	else RTEX_CMD_ShowPosTime(0);
}


void CRTEXFormView::OnBnClickedCheckFreedynamicbrk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CButton* pbtn =(CButton*)GetDlgItem(IDC_CHECK_FreeDynamicBrk);
	if(m_WaitCmdAck || (!m_connect2RenesasOK) || m_fgInQCMD)
	{
		pbtn->SetCheck(!pbtn->GetCheck());
		return;
	}
	if(pbtn->GetCheck())
	{
		pbtn = (CButton*)GetDlgItem(IDC_CHECK_ServoOn);
		pbtn->SetCheck(0);
		OnBnClickedCheckServoon();
		SetWaitCMDAcking();
		RTEX_CMD_Parameter_Writing_I_CMD(m_uiSelectedSlaves, 5, 6, 7);
	}
	else
	{
		SetWaitCMDAcking(1);
		RTEX_CMD_Parameter_Writing_I_CMD(m_uiSelectedSlaves, 5, 6, 0);
		if(WaitCMDAcking())
		{
			ShowStatus("disable free dynamic brk error");
			return;
		}
		pbtn = (CButton*)GetDlgItem(IDC_CHECK_ServoOn);
		pbtn->SetCheck(1);
		OnBnClickedCheckServoon();
	}
}


void CRTEXFormView::OnBnClickedButtonSystemid()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_WaitCmdAck || (!m_connect2RenesasOK) || (m_uiEnableAxis < 1) || (!m_boolRTEX_Configurated)) return;
	UpdateData(TRUE);
	m_threadPara.m_case = 5;	
	m_lpThread = AfxBeginThread(&CRTEXFormView::MythreadFun, (LPVOID)&m_threadPara);
}
void CRTEXFormView::OnBnClickedCheckShowScara()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CButton* pbtn = (CButton*)GetDlgItem(IDC_CHECK_Show_Scara);
	if(m_fgScaraRobot && pbtn->GetCheck())
	{
		m_Show_Scara_Arm = true;
	}
	else
	{
		m_Show_Scara_Arm = false;
	}
}

void CRTEXFormView::OnBnClickedButtonTestCode()
{	
	for(int i = 0 ; i < 8; i++)
	{
			SetWaitCMDAcking(1);
		RTEX_CMD_READ_USER_DATA(2,i *128 ,128);
		if(WaitCMDAcking())
		{
			MessageBox("CMD Fail");
			return;
		}
	}
	for(int i = 0 ; i < 16; i++)
	{
		SetWaitCMDAcking(1);
		RTEX_CMD_READ_USER_DATA(2,0x600 + (i *128) ,128);
		if(WaitCMDAcking())
		{
			MessageBox("CMD Fail");
			return;
		}

	}	
}
UINT CRTEXFormView::HandleMSGThread(LPVOID LPARAM)
{
	CRTEXFormView* view = (CRTEXFormView*)LPARAM;
	view->ProccessMsg();
	return 0x1111;
}
void CRTEXFormView::ProccessMsg()
{
	CSingleLock SingleLock(&Mutex);
	for(;;)
	{
		Sleep(1);
		if(fgStopReceiveThread)
		{
			fgStopReceiveThread = false;
			TRACE("exit1 receiveThread\n");
			return;
		}
		if(!m_bClientConnected)
		{
			continue;
		}
		//if(m_ReceiveCharList.empty()) continue;
#ifdef RingBufferUsed
		if(m_rbuffer.uiBufferDataSize() >= 6)
#else
		if(m_ReceiveCharList.size() >= 6)
#endif
		{
			//TRACE("test listsize = %d\n",m_ReceiveCharList.size()); 
			//TRACE("first ch = %c\n",m_ReceiveCharList.front());
			SingleLock.Lock();
   			for(int i = 0 ; i < 6; i++)
			{
#ifdef RingBufferUsed
				m_uchandleData[i] = m_rbuffer.Extract();
#else
				//if(m_ReceiveCharList.empty()) TRACE("empty\n");
				m_uchandleData[i] = m_ReceiveCharList.front();
				//TRACE("0x%02X ",m_uchandleData[i]);
				m_ReceiveCharList.pop();
#endif
			}
			SingleLock.Unlock();
			//TRACE("\n");
			//TRACE("test1 listsize = %d length = %d\n",m_ReceiveCharList.size(),m_uchandleData[5]);			
#ifdef RingBufferUsed
			while(m_rbuffer.uiBufferDataSize() < m_uchandleData[5])
#else
			while(m_ReceiveCharList.size() < m_uchandleData[5])
#endif
			{
				if(fgStopReceiveThread)
				{
					fgStopReceiveThread = false;
					TRACE("exit2 receiveThread\n");
					return;
				}
			}

			//TRACE("listsize = %d ,cmd size =%d\n",m_ReceiveCharList.size(),m_uchandleData[5]);
			SingleLock.Lock();
			for (int i = 0; i < m_uchandleData[5]; i++)
			{
#ifdef RingBufferUsed
				m_uchandleData[6+i] = m_rbuffer.Extract();
#else
				//if(m_ReceiveCharList.empty()) TRACE("empty\n");
				m_uchandleData[6+i] = m_ReceiveCharList.front();
				m_ReceiveCharList.pop();
#endif
			}
			SingleLock.Unlock();
			if((m_uchandleData[0] == 'Q') && (m_uchandleData[1] == 'S') /*&& (m_uchandleData[2] == 'I')*/)
			{
			  //TRACE("length = %d\n",m_uchandleData[5]);
			  HandleQSIMSG(m_uchandleData, m_uchandleData[5]+6);  // 處理QSI MSG 
			  //TRACE("over\n");
			}
		}
	}
}


void CRTEXFormView::OnBnClickedButtonDumpdebugmsg()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	GetDlgItem(IDC_BUTTON_DumpDebugMsg)->EnableWindow(FALSE);
	if(debugFile.Open("Debugmsg.txt",CFile::modeWrite))
	{
		TRACE("open old file ok\n");
		debugFile.SeekToEnd();
	}
	else
	{
		TRACE("file is not exist\n");
		if(debugFile.Open("Debugmsg.txt",CFile::modeCreate|CFile::modeWrite))
		{
			TRACE("open new file ok\n");
		}
		else
		{
			TRACE("can't open file\n");
			debugFile.Close();
			m_fgGetdebugRS232 = FALSE;
			GetDlgItem(IDC_BUTTON_DumpDebugMsg)->EnableWindow(TRUE);
			return;
		}
	}
	m_fgGetdebugRS232 = TRUE;
	for(int i = 0 ; i < 64; i++)
	{
		SetWaitCMDAcking(1);
		RTEX_CMD_READ_USER_DATA(3,i *128 ,128);
		if(WaitCMDAcking())
		{
			MessageBox("CMD Fail");
            GetDlgItem(IDC_BUTTON_DumpDebugMsg)->EnableWindow(TRUE);
			return;
		}

	}
	debugFile.Close();
	ShowStatus("get DebugMsg From 7216");
	m_fgGetdebugRS232 = FALSE;
    GetDlgItem(IDC_BUTTON_DumpDebugMsg)->EnableWindow(TRUE);
}
void CRTEXFormView::OnBnClickedCheckDumpdebugmsg()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	SetWaitCMDAcking(1);
	CButton* pbtn = (CButton*)GetDlgItem(IDC_CHECK_DumpDebugMsg);
	pbtn->EnableWindow(FALSE);
	unsigned char data[1]= {2};
	
	if(pbtn->GetCheck())	data[0] = 3;
	else		data[0] = 2;

	RTEX_CMD_WRITE_USER_DATA(1,2,1,data);
	if(WaitCMDAcking())
	{
		MessageBox("CMD Fail");
		GetDlgItem(IDC_BUTTON_DumpDebugMsg)->EnableWindow(TRUE);
        pbtn->EnableWindow(TRUE);
		return;
	}

	SetWaitCMDAcking(1);
	RTEX_CMD_SYNC_USER_DATA();
	if(WaitCMDAcking())
	{
		MessageBox("CMD Fail");
		GetDlgItem(IDC_BUTTON_DumpDebugMsg)->EnableWindow(TRUE);
		pbtn->EnableWindow(TRUE);
		return;
	}
	pbtn->EnableWindow(TRUE);
}
afx_msg LRESULT CRTEXFormView::OnSocket_Receive(WPARAM wParam, LPARAM lParam) //test WM
{
	ClientOnReceive();
	return 0;
}
afx_msg LRESULT CRTEXFormView::OnSendviewstop(WPARAM wParam, LPARAM lParam)
{

    CButton *pbtn = (CButton*)GetDlgItem(IDC_CHECK_ShowPostionRepeatly);
	if(pbtn->GetCheck())
	{
		ShowStatus("Send Closed windows disable Ask Positon");
        pbtn->SetCheck(0);
		OnBnClickedCheckShowpostionrepeatly();
	}
	if(m_fgInQCMD)
	{
		ShowStatus("Send Closed windows disable QCMD");
		m_fgQcmdImmediatelyStop  = TRUE;
	}		
	//OnBnClickedButtonMotionStop();
	return 0;
}
void CRTEXFormView::OnDestroy()
{
	TRACE("destroy\n");
	fgStopReceiveThread = TRUE;
	//while(fgStopReceiveThread);
	if(::WaitForSingleObject(m_pReceiveThread->m_hThread,1000) == WAIT_TIMEOUT)
	{
		TRACE("timeout wait receive thread over\n");
	}
	TRACE("check StopReceive ok\n");
	while(!m_qQmdFIFOlist.empty())
	{
		unsigned char* string = m_qQmdFIFOlist.front();
		delete [] string;
		m_qQmdFIFOlist.pop();
	}
	while(!m_ReceiveCharList.empty())
	{
		m_ReceiveCharList.pop();
	}

	CFormView::OnDestroy();

	// TODO: 在此加入您的訊息處理常式程式碼
}
void CRTEXFormView::OnBnClickedButtonUpateScarapara()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(m_WaitCmdAck || (!m_boolRTEX_Configurated) || (!m_connect2RenesasOK)|| (m_uiEnableAxis < 1)) return;
	UpdateData(TRUE);
	//OnBnClickedButtonClearview();	
	if(m_ctrlCMDselect.GetCurSel() != 15)
	{
		MessageBox("NG CMD");
		return;
	}

	m_threadPara.m_case = 7;	
	m_lpThread = AfxBeginThread(&CRTEXFormView::MythreadFun, (LPVOID)&m_threadPara);
}
void CRTEXFormView::Thread_Zero(void)
{	
	CComboBox* pcmbx = (CComboBox*)GetDlgItem(IDC_COMBO_SelectedDvr);
	int index = pcmbx->GetCurSel();
	CButton* pbtn = (CButton*)GetDlgItem(IDC_CHECK_UpdateOffset);
	bool fgUpdate = (pbtn->GetCheck())? TRUE : FALSE;
	//if(m_TestMode) fgUpdate = FALSE;

	UINT uiSelectedSlave = 0;
	if(index == 0) uiSelectedSlave = m_uiSelectedSlaves;
	else		uiSelectedSlave = (1 << m_Axis[index-1]);

	SetWaitCMDAcking(1);
	RTEX_CMD_CLR_MULTI_TERN(uiSelectedSlave);
	if(WaitCMDAcking())
	{
		MessageBox("can't clear multi Tern");
		return;
	}
	Sleep(1000);

	SetWaitCMDAcking(1);
	RTEX_CMD_CLR_MULTI_TERN(uiSelectedSlave);
	if(WaitCMDAcking())
	{
		MessageBox("can't clear multi Tern");
		return;
	}
	Sleep(1000);

	SetWaitCMDAcking(1);
	RTEX_CMD_Driver_Software_Reset(m_uiSelectedSlaves);
	if(WaitCMDAcking())
	{
		MessageBox("can't Reset Driver");
		return;
	}
	Sleep(1500);

	SetWaitCMDAcking(1);
	RTEX_CMD_Motion_Run();
	if(WaitCMDAcking())
	{
		MessageBox("can't set motion run");
		return;
	}
	////////////////////////////////////////////////////////////////////////
	if(fgUpdate)
	{
		Sleep(1000);
		SetWaitCMDAcking(1);
		if(index == 0)		RTEX_CMD_Update_Scara_Parameter();
		else 		RTEX_CMD_Update_Scara_Parameter(index-1);
		if(WaitCMDAcking())
		{
			MessageBox("can't Update Scara Parameter");
			return;
		}
	}
	Sleep(500);
	SetWaitCMDAcking(1);
	RTEX_GetAxisPostion(m_uiSelectedSlaves);
	if(WaitCMDAcking())
	{
		MessageBox("can't RTEX_GetAxisPostion");
		return;
	}
	for(UINT i = 0; i < m_uiEnableAxis ; i++)
	{
		SetWaitCMDAcking(1);
		RTEX_GetAxisParameter(i);
		if(WaitCMDAcking())
		{
			MessageBox("can't get Axis parameter from Renesas");
			return;
		}
	}

	for(UINT i = 0; i < m_uiEnableAxis ; i++)
	{
		if(index != 0 && (i!= (index-1)))
		{
			continue;
		}
		if(abs(m_Axis_Encoder_OFS[i]) > 10000)
		{
			MessageBox("Encoder offset value > 10000");
			return;
		}
	}
	//OnBnClickedButtonDisconnect();
	//MessageBox("Scara need to power off and turn on once\nand RTEXTool Reconncet to scara");
}
void CRTEXFormView::RTEX_CMD_WriteFlashData(unsigned short usOffset,int data)
{
	// TODO: 在此加入控制項告知處理常式程式碼
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[1024];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = CMD_Access_FlashData;
	pCommBuff[5] = 8;
	pCommBuff[6] = 1;
	pCommBuff[7] = (unsigned char)((usOffset & 0xFF00) >> 8);
	pCommBuff[8] = (unsigned char)(usOffset & 0x00FF);
	pCommBuff[9] = 4;
	pCommBuff[10] = (unsigned char)((data & 0xFF000000) >> 24);
	pCommBuff[11] = (unsigned char)((data & 0x00FF0000) >> 16);
	pCommBuff[12] = (unsigned char)((data & 0x0000FF00) >> 8);
	pCommBuff[13] = (unsigned char)((data & 0x000000FF) >> 0);
	SendCmd(pCommBuff,14);
	delete [] pCommBuff;
	pCommBuff =nullptr;
}
void CRTEXFormView::RTEX_CMD_SyncFlashData()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	//modify for socket
	unsigned char* pCommBuff;
	pCommBuff = new unsigned char[1024];
	pCommBuff[0] = 'Q';
	pCommBuff[1] = 'S';
	pCommBuff[2] = 'I';
	pCommBuff[3] = 'C';
	pCommBuff[4] = CMD_Access_FlashData;
	pCommBuff[5] = 1;
	pCommBuff[6] = 2;
	SendCmd(pCommBuff,7);
	delete [] pCommBuff;
	pCommBuff =nullptr;
}

void CRTEXFormView::OnBnClickedButtonConnectserver()
{
		m_threadPara.m_case = 8;
		m_lpThread = AfxBeginThread(&CRTEXFormView::MythreadFun, (LPVOID)&m_threadPara);
	
}
void CRTEXFormView::ServerCommend()
{
	CSocket client, serverSocket;
	if (!AfxSocketInit())MessageBox("Failed to Initialize Sockets");
	if (!serverSocket.Socket())MessageBox("Create Faild");
	BOOL bOptVal = TRUE;
	int bOptLen = sizeof(BOOL);
	serverSocket.SetSockOpt(SO_REUSEADDR, (void *)&bOptVal, bOptLen, SOL_SOCKET);


	int nServerPort = 8888;
	if (!serverSocket.Bind(nServerPort))MessageBox("Bind Faild");
	if (!serverSocket.Listen(10))MessageBox("Listen Faild");
	while (1){
	if (serverSocket.Accept(client))
	{
		//ServerResponse("server on.");
		char szRecValue[20] = { 0 };
		CString str;
		client.Receive(szRecValue, 16);

			if (szRecValue[2] == 'r'&& szRecValue[3] == '2')
			{
				int X_1 = szRecValue[4];
				int X_2 = szRecValue[5];
				int X_3 = szRecValue[6];
				float X_receive = X_1 * 100 + X_2 + X_3*0.01;


				int Y_1 = szRecValue[7];
				int Y_2 = szRecValue[8];
				int Y_3 = szRecValue[9];
				float Y_receive = Y_1 * 100 + Y_2 + Y_3*0.01;

				int Z_1 = szRecValue[10];
				int Z_2 = szRecValue[11];
				int Z_3 = szRecValue[12];
				float Z_receive = Z_1 * 100 + Z_2 + Z_3*0.01;

				int theta_1 = szRecValue[13];
				int theta_2 = szRecValue[14];
				int theta_3 = szRecValue[15];
				float theta_receive = theta_1 * 100 + theta_2 + theta_3*0.01;

				RTEX_CMD_SET_XYZ_MotionJOG(X_receive, Y_receive, Z_receive, theta_receive);
				str.Format("X:"_T("%.2f ")"Y:"_T("%.2f ")"Z:"_T("%.2f ")"theta:"_T("%.2f "), X_receive, Y_receive, Z_receive, theta_receive);
				ServerResponse(str);
				ServerResponse("Move Done!");
				
				
			}
			else if (szRecValue[2] == 'r'&& szRecValue[3] == '3'){
				unsigned char ucbitchg = 1200 - IDC_CHECK_BIT0;
				UINT  uicurrentstatus = m_uiIOStatus[0];
				uicurrentstatus ^= (UINT)(1 << ucbitchg);//將1移至ucbitchg的bit
				RTEX_CMD_SET_OUTPUT(m_ucIO_MacID[0], uicurrentstatus);	//在此結束後夾爪會改變	
				Gripper_state ^= true;
				if (Gripper_state)ServerResponse("夾");
			}
			
				client.Close();
		}
	else
	{
		AfxMessageBox("Creat Faild(Connect)");
	}
	}

}

void CRTEXFormView::OnBnClickedButtonGripper()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	unsigned char ucbitchg = 1200 - IDC_CHECK_BIT0;
	UINT  uicurrentstatus = m_uiIOStatus[0];
	uicurrentstatus ^= (UINT)(1 << ucbitchg);//將1移至ucbitchg的bit
	RTEX_CMD_SET_OUTPUT(m_ucIO_MacID[0], uicurrentstatus);	//在此結束後夾爪會改變
	Gripper_state ^= true;
	if (Gripper_state)ServerResponse("夾");
}
void CRTEXFormView::ServerResponse(CString str)
{ 
	m_LIST_ServerCommend.AddString(str);

}

void CRTEXFormView::OnBnClickedListDelete()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	m_LIST_ServerCommend.ResetContent();
}

#pragma once
//#define RingBufferUsed
#ifdef RingBufferUsed
#include "RingBuffer.h"
#endif
#include "Const.h"
#include "afxwin.h"
#include "RTEXToolDoc.h"
#include "SocketClient.h"
#include "afxcmn.h"
#include <queue>
// CRTEXFormView 表單檢視
#define  CMD_WITH_SN
#define M_PI 3.1415926535
#define MaxIOBoardNum 8
#define IMMEDIATE_COMMAND_START     0xA0
#define CMD_CONFIG_RING				0xA0
#define CMD_INIT					0xA1
#define CMD_GET_RING_STATUS			0xA2
#define CMD_GET_AXIS_STATUS			0xA3
#define CMD_GET_AXIS_POSITION		0xA4
#define CMD_SERVO_ON				0xA5
#define CMD_SERVO_OFF				0xA6
#define CMD_SET_SPEED_LIMIT			0xA7
#define CMD_SET_TEST_MODE			0xA8

#define CMD_GET_XYZ_MotionSpeed     0xAA
#define CMD_SET_XYZ_AxisParameter   0xAB
#define CMD_GET_XYZ_AxisParameter   0xAC
#define CMD_DRIVER_PARA_RW          0xAD   //20121228-ting-1
#define CMD_MONITOR                 0xAE   //20121228-ting-1  
#define CMD_GET_MONITOR_DATA        0xAF
#define CMD_Parameter_Writing_S_CMD             0xB0  //[20121227-Leonard]
#define CMD_Parameter_Writing_I_CMD             0xB1  //[20121227-Leonard]
#define CMD_Parameter_Reading_S_CMD             0xB2  //[20121227-Leonard]
#define CMD_Parameter_Reading_I_CMD             0xB3  //[20121227-Leonard]
#define CMD_Parameter_EEPROM_Writing_CMD        0xB4  //[20121227-Leonard]
#define CMD_SET_PPMode_CMD                      0xB5  //[20121225-Leonard-1]
#define CMD_SET_PPHOME_CMD                      0xB6  //[20121225-Leonard-1]
#define CMD_CLR_MULTI_TERN                      0xB7  //[20130118-ting]
#define CMD_Driver_Software_Reset               0xB8  //[20130118-ting]
#define CMD_GET_Scara_Robot_mech                0xB9
#define CMD_SystemID                            0xBA  //[20130118-ting]   // hmi not ready
#define CMD_Motion_Pause       		        0xBB  //[20130304-Leonard-1]
#define CMD_Motion_Stop        		        0xBC  //[20130304-Leonard-1]
#define CMD_Motion_Limit_Release                0xBD  //[20130304-Leonard-1]
#define CMD_Driver_Alarm_Warning		0xBE  //[20130304-Leonard-1]
#define CMD_Driver_Alarm_Clear			0xBF  //[20130304-Leonard-1]
#define CMD_Motion_Run							0xC0  //[20130313-Leonard-1]
#define CMD_Motion_Status						0xC1  //[20130313-Leonard-1]
#define CMD_Get_Jog_Speed_parameter           0xC2  // 20130319-pp
#define CMD_Get_New_AXIS_POSITION               0xC3
#define CMD_Get_Scara_Hand				0xC4  //[20130326-Leonard-1]
#define CMD_Check_ALL_Queue_Done                0XC5 //20130402-pp-2
#define CMD_Reload_Scara_Parameter		0xC6	//[20130408-Leooard-1]
#define CMD_GET_INPUT				0xE0
#define CMD_SET_OUTPUT				0xE1
#define CMD_SET_LOGIC_GATE			0xE2 //[20130522-Leonard]
#define CMD_LOAD_USER_DATA			0xF0
#define CMD_READ_USER_DATA			0xF1
#define CMD_WRITE_USER_DATA			0xF2
#define CMD_SYNC_USER_DATA			0xF3
#define CMD_Switch_Deubug_RS232		0XF4  //20130402-pp
#define CMD_ShowPosTime             0xF5
#define CMD_ShowAllEnableLine                   0xF6
#define CMD_SetLineLimitImmdediatly          0xF7
#define CMD_SetXYLimit                           0xF8
#define CMD_SetEmergencyStopChannel             0xF9
#define CMD_Access_FlashData				  0xFA
#define CMD_Set_Positon_Event        0xFB
#define CMD_Set_IOBorad_SendMsg                 0xFC
#define CMD_Get_7216_Information      0xFD
#define CMD_SET_Software_Timeout           0xFE 
#define CMD_RESET			        0xFF

/*==========================================*/
#define CMD_1_AXIS_MOTION			   0x20
#define CMD_2_AXES_LINEAR_MOTION       0x30
#define CMD_2_AXES_ARC_MOTION		   0x31
#define CMD_SET_XYZ_MotionLineXYZ      0x32
#define CMD_SET_XYZ_MotionCircle       0x33
#define CMD_SET_XYZ_MotionHelix        0x34
#define	CMD_SET_XYZ_MotionCircle3D     0x35  //[20130129-ting]
#define CMD_SET_XYZ_MotionJOG          0x36  //[20130131-ting] 
#define CMD_SET_J1234_MotionJOG        0x37   //[20130206-ting]

#define CMD_SET_Scara_Robot_mech         0x51  //[20130116-Leonard-1]
#define CMD_Scara_J4_Rotate              0x53  //[20130204-Leonard-1]
#define CMD_Delay_Time			        0x54  //[20130304-Leonard-1]
#define CMD_Wait_IO_Board_Status        0x55  //20130227-pp
#define CMD_SET_OUTPUT_In_Queue         0x56  //20130304-pp
#define CMD_SET_XYZ_MotionSpeed          0x60
#define CMD_SET_MoveMode                 0x61  //[20121210-Leonard-1]
#define CMD_SET_TARCMD                   0x62  //[20121225-Leonard-1]
#define CMD_SET_Profile_ABS_POSCMD       0x63  //[20121225-Leonard-1]
#define CMD_JOG_Move                     0x64  //[20121227-Leonard]
#define CMD_JOG_REL_Move		 0x65  //[20130118-Leonard-1]
#define CMD_SetMotionBlend               0x66
#define CMD_Set_Jog_MaxRPM               0x67  // 20130319-pp
#define CMD_Set_Jog_Speed                0x68  // 20130319-pp
#define CMD_SetLineLimit                0x6E

	
#define EVENT_Send_Nothing                    0x00
#define EVENT_IOBard_Status_change	        0x01
#define EVENT_Send_NEW_Postion                0x02
#define EVENT_Error_Occurred                0x03


/*
#define EVENT_HIF_COMMAND_COMPLETED		0x00
#define EVENT_HIF_COMMAND_ERROR			0x01
#define ERROR_RTEX_NOT_RUNNING		    0x01
#define ERROR_UNKNOWN_COMMAND		    0x0f

#define EVENT_RESET						0xa0
#define EVENT_1_AXIS_POSITION_UPDATE	0xa1
#define EVENT_2_AXES_POSITION_UPDATE	0xa2
#define EVENT_3_AXES_POSITION_UPDATE	0xa3
#define EVENT_4_AXES_POSITION_UPDATE	0xa4
#define EVENT_1_AXIS_MONITOR_UPDATE     0xa5
#define EVENT_2_AXIS_MONITOR_UPDATE   	0xa6
#define EVENT_3_AXIS_MONITOR_UPDATE 	0xa7
*/

#define Type_Code_x6_Parameter_Reading  0x10   
#define Type_Code_x6_Parameter_Writing  0x11
#define Type_Code_x6_Parameter_EEPROM_W  0x81


#define WM_SendCmd2StatusBar        WM_USER + 101
#define  WM_SendSocketClientclosed   WM_USER + 102
#define WM_SendredrawInthread        WM_USER + 103
#define WM_SendviewStop             WM_USER + 104
#define WM_SendJogDialogUpdatePos     WM_USER + 105
#define WM_SendUpdateJoint_Pos        WM_USER + 106
#define WM_Socket_Receive         WM_USER + 107
class CMythreadParam
{
public:
	LPVOID m_lpPara;
	UINT   m_case;
	BOOL m_blthreading;
};

/////////////////////////////////////////////////////////////////////////////////////////
#define IO_function_length	10    //[20130426-ting]
#define IO_function_max		5

struct st_IO_function_Group
{
	unsigned char function_number;
	unsigned char charnumber[IO_function_max][IO_function_length];
};
/////////////////////////////////////////////////////////////////////////////////////////

struct AxisStatus
{
	unsigned char CMD_Error;
	unsigned char CMD_ERROR_Code_L;  //[20130326-Leonard-2]
	unsigned char CMD_ERROR_Code_H;  //[20130326-Leonard-2]
	unsigned char Alarm;
	unsigned char Alarm_Main;
	unsigned char Alarm_Sub;
	unsigned char Warning;
	unsigned char Warning_Code_L;
	unsigned char Warning_Code_H;
	unsigned char POT;  //[20130313-Leonard-1]
	unsigned char NOT;  //[20130313-Leonard-1]
	unsigned char Speed_Error;	//[20130422-Leonard-1]
	unsigned int RPM;	//[20130422-Leonard-1]
};

class CRTEXFormView : public CFormView
{
	DECLARE_DYNCREATE(CRTEXFormView)

protected:
	CRTEXFormView();           // 動態建立所使用的保護建構函式
	virtual ~CRTEXFormView();

public:
	enum { IDD = IDD_RTEXFORMVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	
	DECLARE_MESSAGE_MAP()
	
	CRTEXToolDoc* pDoc;		
	
	//=======與控制項關聯的變數=============
	int m_intX_Axis_pos;
	int m_intY_Axis_pos;
	int m_intZ_Axis_pos;
	int m_intU_Axis_pos;
	CComboBox m_ctrlCMDselect;
	UINT m_uintVmax;
	CListBox m_ctrllistStatus;
	CButton m_ctrlTestMode;
	CIPAddressCtrl m_ctrlIPAddr;

	//自定windows 訊息
	afx_msg LRESULT OnSendsocketclientclosed(WPARAM wParam, LPARAM lParam); //closed Socket 用事件驅動
	afx_msg LRESULT OnSendcmd2statusbar(WPARAM wParam, LPARAM lParam);  // 發事件 ，把cmd與參數傳到status bar
	
	
	//取代 WriiteToPort  增加把cmd寫到status bar
	void SendCmd(unsigned char* string, UINT n);
	unsigned char* m_lpucCharForSendCmd;

	//將收到的qsi訊息字串送到status bar上
	void RecieveMsgReport2statusBar(const unsigned char* string);

	//報字串在listbox上
	void ShowStatus(const CString& str);

	void SaveSend2Loghistory(const unsigned char* string);
	void SaveReceive2Loghistory(const unsigned char* string);


	//socket的類別
	CSocketClient m_clientSocket;	
	bool m_bClientConnected;	
	
	//已連線的flag
	bool m_connect2RenesasOK;

	//無法設定參數的flag
	bool boolunSetSpdParamter;
	bool boolunSetAxisParamter;
public:
	//已config and init 的flag
	bool m_boolRTEX_Configurated;
protected:	
	//測試模式，不接driver
	bool m_TestMode;


	//記錄最後一筆cmd的變數
	unsigned char m_CurrentCmd;


    //接受字串所需要變數!!
	UINT m_Recieve_count;  //only used by rs232 method
    bool m_GetQSIMSG;  //only used by rs232 method
	unsigned char*m_uschrReciveData;	
	UINT m_QSIMSGLength;

	// 不管是用socket還是rs232，都是這個function管控收過來的cmd
	void HandleQSIMSG(const unsigned char* RecieveData, const UINT num);		


public:
	virtual void OnInitialUpdate();

	//int,unsigned char, float 轉換專用
	union {
		int inumber;
		float fnumber;
		unsigned char charnumber[4];
	}m_float2uchar;

	//聲明如下四個函數跟SocketClient類的事件關聯。
	//必須是public，因為 socketclient裡會呼叫母類別的這四個function
	void ClientOnSend();
	void ClientOnReceive();
	void ClientOnSocketClose();
	void ClientOnConnect();

	//=======等待命令回應相關變數與函式=============
	bool m_blWaiitCmdPROGRESS;
	unsigned char m_ucCMDPROGRESSCase;
	UINT m_uintCmdProgressTimer;
	UINT m_uintCmdtimerCnt;
	bool m_boolwaitCmdEvent;
	bool m_WaitCmdAck;
	void SetWaitCMDProgress(unsigned char cmdcase,UINT timeslot);
	void ClearWaitCMDPgress();
	bool WaitCMDProgress();
	void SetWaitCMDAcking(bool wait = FALSE);
	void ClearWaitCMDAcking();
	bool WaitCMDAcking(bool reset = TRUE , UINT time = 7);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	//=======Button click=============
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnBnClickedButtonParameterset();
	afx_msg void OnBnClickedButtonSetRtexConfiguration();	
	afx_msg void OnBnClickedButtonOpendriverregdialog();
	afx_msg void OnBnClickedButtonOpenspecialcmddialog();
	afx_msg void OnBnClickedCheckServoon();
	afx_msg void OnBnClickedButtonXAxis();
	afx_msg void OnBnClickedButtonYAxis();
	afx_msg void OnBnClickedButtonZAxis();
	afx_msg void OnBnClickedButtonUAxis();
	afx_msg void OnBnClickedButtonSetxvelocity();
	afx_msg void OnBnClickedButtonSetyvelocity();
	afx_msg void OnBnClickedButtonSetzvelocity();	
	afx_msg void OnBnClickedButtonUniversalCmd();
	afx_msg void OnBnClickedButtonClearmsg();
	afx_msg void OnBnClickedButtonClearview();
	afx_msg void OnBnClickedCheckScara();
	afx_msg void OnBnClickedCheckTestmode();
	afx_msg void OnBnClickedButtonScarahome();
	afx_msg void OnBnClickedCheckShowpostionrepeatly();
	afx_msg void OnBnClickedCheckAlarmWarning();
	afx_msg void OnBnClickedButtonIoSet();
	afx_msg void OnBnClickedButtonExecuteqcmd();
	afx_msg void OnBnClickedButtonExecutecmd();
	afx_msg void OnBnClickedButtonDvrReset();
	afx_msg void OnBnClickedButtonIncreaselogfontsize();
	afx_msg void OnBnClickedButtonDecreaselogfontsize();

	// select change event
	afx_msg void OnCbnSelchangeComboboxCmd();
    
	// routine 發位置與monitor cmd
	CWinThread* m_lpShowPosThread;
	BOOL m_blShowPos;
	static UINT ShowPosThreadFun(LPVOID LParam);
	void ShowPosEvent(void);
	UINT m_uiShowPosTimeSlot;

	//routine 發check error, alarm, Warning
	CWinThread* m_lpCheckErrThread;
	BOOL m_blCheckErr;
	static UINT CheckErrThreadFun(LPVOID LParam);
	void CheckErrEvent(void);
	UINT m_uiCheckErrTimeSlot;


	
	// 發達QSI 格式的封包functon
	void RTEX_EVENT_ACK(unsigned char ucCMD);
	void RTEX_CMD_TestMode(bool testmode);
	void RTEX_Configuration(void);
	void RTEX_Init(void);
	void RTEX_GetRTEXRingStatus(void);
	void RTEX_GetAxisPostion(unsigned int uiSelectedSlaves);
	void RTEX_CMD_ServoOnOff(bool servoOn, unsigned int uiSelectedSlaves);
	void RTEX_CMD_GetAxisStatus(UINT uiSelectedSlaves);
	void RTEX_GetMotionSpdParameter(void);
	void RTEX_GetAxisParameter(unsigned char axis);
	void RTEX_SetAxisParameter(unsigned char axis);	
	void RTEX_CMD_SET_XYZ_MotionSpeed(int spdtype,float fvmax, float fta);
	void RTEX_CMD_SET_SPEED_LIMIT(unsigned char macid, unsigned short velocity);
	void RTEX_CMD_1_AXIS_MOTION(unsigned char macid, int TargetPos);
	void RTEX_CMD_2_AXES_LINEAR_MOTION(unsigned char macid1, int TargetPos1, unsigned char macid2, int TargetPos2);
	void RTEX_CMD_SET_XYZ_MotionLineXYZ(float tarX, float tarY, float tarZ, float tarU);
	void RTEX_CMD_SET_XYZ_MotionCircle(unsigned char ucmode, float fcenA, float fcenB, float fangle, unsigned char uccw, float ftarU);
	void RTEX_CMD_SET_XYZ_MotionHelix(unsigned char ucmode, float fcenA, float fcenB, float frotation, float fdelta, unsigned char uccw, float ftarU);
	void RTEX_SetDRIVER_PARA_RW_CMD(int axis,unsigned char Command_Type,unsigned char Parameter_Class,unsigned char Parameter_No,int Setting_Data);	
	void RTEX_Monitor_CMD(int axis,unsigned char Channel);
	void RTEX_GetMonitorData(unsigned int uiSelectedSlaves);
	
	void RTEX_CMD_SET_MoveMode(unsigned int uiSelectedSlaves, unsigned char ucmode);
	void RTEX_CMD_SET_TARCMD(unsigned int uiSelectedSlaves, int cmdTarget, int TSPD);
	void RTEX_CMD_SET_Profile_ABS_POSCMD(unsigned int uiSelectedSlaves);	
	void RTEX_CMD_Jog_Mode_Move(unsigned int uiSelectedSlaves, int cmdTarget, int TSPD);
	void RTEX_CMD_JOG_REL_Move(unsigned int uiSelectedSlaves, int cmdTarget, int TSPD);
	void RTEX_CMD_SET_PPMode_CMD(unsigned int uiSelectedSlaves, int AccConst, int DecConst);
	void RTEX_CMD_SET_PPHOME_CMD(unsigned int uiSelectedSlaves, unsigned short home_mode, int vel1, int vel2);
	
	void RTEX_CMD_Parameter_Writing_S_CMD(unsigned int uiSelectedSlaves,unsigned char Parameter_Class,unsigned char Parameter_No, unsigned short value);
	void RTEX_CMD_Parameter_Writing_I_CMD(unsigned int uiSelectedSlaves,unsigned char Parameter_Class,unsigned char Parameter_No, int value);
	void RTEX_CMD_Parameter_Reading_S_CMD(unsigned char macid,unsigned char Parameter_Class,unsigned char Parameter_No);
	void RTEX_CMD_Parameter_Reading_I_CMD(unsigned char macid,unsigned char Parameter_Class,unsigned char Parameter_No);
	void RTEX_CMD_Parameter_EEPROM_Writing_CMD(unsigned int uiSelectedSlaves);
	void RTEX_CMD_RESET(void);
	void RTEX_CMD_SetMotionBlend(float Percentage);
	void RTEX_CMD_Driver_Software_Reset(unsigned int uiSelectedSlaves);
	void RTEX_CMD_Scara_Robot_Home(void);
	void RTEX_CMD_SET_XYZ_MotionCircle3D(float P2X, float P2Y, float P2Z ,float P3X, float P3Y, float P3Z,float angle, unsigned char uccw);
	void RTEX_CMD_SET_XYZ_MotionJOG(float tarX, float tarY, float tarZ, float tarU);
	void RTEX_CMD_READ_USER_DATA(unsigned char ucpage, unsigned short usoffset,unsigned char uclength);
	void RTEX_CMD_WRITE_USER_DATA(unsigned char ucpage, unsigned short usoffset,unsigned char uclength, unsigned char* pdata);
	void RTEX_CMD_SYNC_USER_DATA(void);
	void RTEX_CMD_Scara_J4_Rotate(float fRev, float fOmega);
	void RTEX_CMD_SET_J1234_MotionJOG(float ftarJX, float ftarJY, float ftarJZ, float ftarJU);
	void RTEX_CMD_GET_INPUT(unsigned char macid);
	void RTEX_CMD_SET_OUTPUT(unsigned char macid, UINT value);
	void RTEX_CMD_SET_OUTPUT(unsigned char macid, unsigned char ucbit, unsigned char uclevel); //overload function
	void RTEX_CMD_Wait_IO_Board_Status(unsigned char macid, unsigned char ucbit, unsigned char uclevel, UINT uitimeout, unsigned char TOmacid, unsigned char ucTObit, unsigned char ucTOlevel);
	void RTEX_CMD_SET_OUTPUT_In_Queue(unsigned char macid, unsigned char ucbit, unsigned char uclevel);
	void RTEX_CMD_Delay_Time(int idelaytime);
	void RTEX_CMD_Motion_Pause(unsigned char ucpause);
	void RTEX_CMD_Motion_Stop(void);
	void RTEX_CMD_Motion_Limit_Release(unsigned char uclimitrelease);
	void RTEX_CMD_Driver_Alarm_Warning(UINT uiSelectedSlaves);
	void RTEX_CMD_Driver_Alarm_Clear(UINT uiSelectedSlaves);
	void RTEX_CMD_Motion_Run(void);
	void RTEX_CMD_Motion_Status(void);
	void RTEX_CMD_Get_Jog_Speed_parameter(void);
	void RTEX_CMD_Set_Jog_MaxRPM(int iMaxRPM);
	void RTEX_CMD_Set_Jog_Speed(float Percentage);
	void RTEX_CMD_Get_New_AXIS_POSITION(UINT uiSelectedSlaves);
	void RTEX_CMD_Get_Scara_Hand();
	void RTEX_CMD_SET_Scara_Robot_mech(float fL1, float fL2, float fheight);
	void RTEX_CMD_GET_Scara_Robot_mech(void);
	void RTEX_CMD_Switch_Deubug_RS232(unsigned char ucdisable);
	void RTEX_CMD_ShowPosTime(unsigned char ucshowtimedata);
	void RTEX_CMD_Check_ALL_Queue_Done(void);
	void RTEX_CMD_SystemID(UINT uiSelectedSlaves, unsigned char ucCmdType);
	void RTEX_CMD_Reload_Scara_Parameter(void);
	void RTEX_CMD_Update_Scara_Parameter(unsigned char type = 0xAA);
	void RTEX_CMD_Set_Group(unsigned char ucmode);
	void RTEX_CMD_CLR_MULTI_TERN(UINT uiSelectedSlaves);
	void RTEX_CMD_Get_Servo_FW_NAME(void);
	void RTEX_CMD_SET_Software_Timeout(UINT uitimeout);
	void RTEX_CMD_Set_Positon_Event(unsigned char ucenable, UINT uitime);
	void RTEX_CMD_Set_IOBorad_SendMsg(unsigned char ucenable);
	void RTEX_CMD_SET_LOGIC_GATE(unsigned char ucLogicPort,unsigned char ucEnable,unsigned char ucMode,unsigned char ucMAcIDA,unsigned char ucPortA,unsigned char ucMAcIDB,unsigned char ucPortB,unsigned char ucMAcIDC,unsigned char ucPortC);



public:

	//有關Renesas 的參數
	
	int m_Axis[6];                    //MAC ID
	float m_Axis_Pitch[6];            // pitch
	int m_Axis_GearRatio[6];          //齒輪比
	unsigned char m_Axis_Type[6];      // #define R_2_R   0      #define R_2_L 1	
	float m_Axis_GearRatio_Mech[6];
	float m_Axis_POTLimit[6];            //軟體正極限
	float m_Axis_NOTLimit[6];            //軟體負極限
	int m_Axis_Encoder_OFS[6];
	unsigned char m_MonitorSingal[6]; //monitor的種類
    
    UINT m_uiEnableAxis;              //啟動軸數	
	int m_intcmbxSpdType;             //T or S curve
	float m_fVmax;                    // 物件速度
	unsigned int m_uiTa;              //加減速時間
	UINT m_uiSelectedSlaves;	      // 已啟動的MAC ID
	int m_iXpos_um;
	int m_iYpos_um;
	int m_iZpos_um;
	float m_fx,m_fy,m_fz,m_fYaw;
	float m_ftheta1, m_ftheta2, m_ftheta4;

	//CAxisSetting 用到的母類別function
	void Setspeedparameter(void);
	void Setaxisparameter(void);

	//CDriverRegister 用到母類別的四個function 一定要是public
    void DriverRegDialog_GetAxisDriverPara(int axis,unsigned char para_class, unsigned char para_reg);
	void DriverRegDialog_SetAxisDriverPara(int axis,unsigned char para_class, unsigned char para_reg, int value);
	void DriverRegDialog_EEPROMWriteDriverPara(int axis);
	void DriverRegDialog_Monitor_CMD(int axis,unsigned char Channel);
	int m_intDvrReturnData;

	//SpecialCMD
	int m_intSpecialCMDReturnData;
        //QCMD flag
	bool m_fgInQCMD;
private:
	//開thread所用的
	CMythreadParam m_threadPara;
	CWinThread*  m_lpThread;	
	static UINT MythreadFun(LPVOID LParam);
	void Thread_Parameterset(void);
	void Thread_SetRtexConfiguration(void);	
	void Thread_Opendriverregdialog(void);
	void Thread_OpenSpCMDdialog(void);
	void Thread_openIO_BoradDialog(void);
	void Thread_OpenSystemIDdialog(void);
	void Thread_OpenJogDialog(void);
	void Thread_Zero(void);
public:
	void TransPulse2XYZ(void);
	BOOL m_fgScaraRobot;
	float m_fScaraRobot_L1;
	float m_fScaraRobot_L2;
	float m_fScaraRobot_Height;
	unsigned char m_ucScaraRobot_Arm_dir;

	BOOL m_fgenableIO_borad[MaxIOBoardNum];	
	BOOL m_fgInput[MaxIOBoardNum];
	unsigned char m_ucIO_MacID[MaxIOBoardNum];
	UINT m_uiIOStatus[MaxIOBoardNum];
	UINT m_uiInputBoardStatus;
	unsigned char m_ucInputBoardID;	
	AxisStatus m_axisStatus[6];	
	st_IO_function_Group IO_function;  // ting add
	CTabCtrl m_ctrlTAB_LOG;
	afx_msg void OnTcnSelchangeTabLog(NMHDR *pNMHDR, LRESULT *pResult);
	CListBox m_ctrllistLog_History;
	CFont* m_pFont;
	int m_intFont_size;
	void SetRecieveMsgFontSize(void);
	afx_msg void OnBnClickedButtonWarningReset();
	afx_msg void OnBnClickedButtonMotionStop();
	afx_msg void OnBnClickedButtonMotionRun();
	int m_iJog_MaxRPM;
	float m_fJog_Speed;
	bool m_fgHIFCmdBufferFull;
	std::queue<unsigned char*> m_qQmdFIFOlist;
	void QcmdADDCMDFromTxt();
	bool QcmdSendCmdFunc();
	bool m_fgQcmdImmediatelyStop;
	static UINT QCMDThreadFun(LPVOID LPARAM);
	afx_msg void OnBnClickedCheckDisredraw();
	static UINT WaitConnectThread(LPVOID LPARAM);
	void Connect7216OK(void);
protected:
	afx_msg LRESULT OnSendredrawinthread(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedCheckDisabledebugrs232();
	afx_msg void OnBnClickedCheckShowpostimedata();
	afx_msg void OnBnClickedCheckFreedynamicbrk();
	float m_HomeJ[4];
	afx_msg void OnBnClickedButtonSystemid();
	unsigned char* m_ucSystemIDreturndata;
	unsigned char m_chkSum;
	afx_msg void OnBnClickedCheckEvent();
	afx_msg void OnBnClickedButtonTestCode();
	afx_msg void OnBnClickedCheckShowScara();
	bool m_Show_Scara_Arm;
#ifdef RingBufferUsed
	CRingBuffer m_rbuffer;
#else
	std::queue<unsigned char> m_ReceiveCharList;
#endif
	CWinThread* m_pReceiveThread;
	static UINT HandleMSGThread(LPVOID LPARAM);
	void ProccessMsg(void);
	unsigned char* m_uchandleData;	
	volatile bool fgStopReceiveThread;
	CMutex Mutex;
	afx_msg void OnDestroy();
protected:	
	afx_msg LRESULT OnSendviewstop(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSendUpdateJoint_Pos(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSocket_Receive(WPARAM wParam, LPARAM lParam); //test WM
public:
	bool m_fgGetdebugRS232;
	CStdioFile debugFile;
	afx_msg void OnBnClickedButtonDumpdebugmsg();
	afx_msg void OnBnClickedCheckDumpdebugmsg();
	UINT m_uiPointCnt;
	int m_response_vel[6];
	int m_response_torq[6];

	CFile m_StandaloneFile;
	afx_msg void OnBnClickedCheckQcmdtofile();
	bool m_fgStandaloneCode;
	CStdioFile saveDebugEvent;  //for Ken Huang
	afx_msg void OnBnClickedButtonUpateScarapara();
	void RTEX_CMD_WriteFlashData(unsigned short usOffset,int data);
	void RTEX_CMD_SyncFlashData();
	afx_msg void OnBnClickedButtonConnectserver();
	/*******/
	void ServerCommend(void);

	afx_msg void OnBnClickedButtonGripper();
	CListBox m_LIST_ServerCommend;
	bool Gripper_state=false;
	void ServerResponse(CString str);
	afx_msg void OnBnClickedListDelete();
	
};


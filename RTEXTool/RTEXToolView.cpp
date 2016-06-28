
// RTEXToolView.cpp : CRTEXToolView 類別的實作
//

#include "stdafx.h"
// SHARED_HANDLERS 可以定義在實作預覽、縮圖和搜尋篩選條件處理常式的
// ATL 專案中，並允許與該專案共用文件程式碼。
#ifndef SHARED_HANDLERS
#include "RTEXTool.h"
#endif

#include "RTEXToolDoc.h"
#include "RTEXToolView.h"
#include <list>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRTEXToolView

IMPLEMENT_DYNCREATE(CRTEXToolView, CView)

BEGIN_MESSAGE_MAP(CRTEXToolView, CView)
           ON_UPDATE_COMMAND_UI(ID_INDICATOR_Pos, &CRTEXToolView::OnUpdateIndicatorPos)
           ON_WM_RBUTTONDOWN()
           ON_WM_RBUTTONUP()
           ON_WM_MOUSEMOVE()
           ON_WM_MOUSEWHEEL()
           ON_WM_LBUTTONDBLCLK()
           ON_WM_LBUTTONDOWN()
           ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CRTEXToolView 建構/解構

CRTEXToolView::CRTEXToolView()
	: m_View_moving(false)
	, m_RBtnDWN_Pt(0,0)
	, m_theta1(0)
	, m_theta2(0)
	, m_theta3(0)
	, xJ1_backup(1)
	, yJ1_backup(1)
	, zJ1_backup(1)
	, xJ2_backup(1)
	, yJ2_backup(1)
	, zJ2_backup(1)
	, xJ3_backup(1)
	, yJ3_backup(1)
	, zJ3_backup(1)
	, x1_backup(1)
	, y1_backup(1)
	, z1_backup(1)
	, m_view_angle_change(false)
{
	// TODO: 在此加入建構程式碼
	sin_a = (float)sin(m_theta1 *M_PI /180);
	sin_b = (float)sin(m_theta2 *M_PI /180);
	sin_c = (float)sin(m_theta3 *M_PI /180);
	cos_a = (float)cos(m_theta1 *M_PI /180);
	cos_b = (float)cos(m_theta2 *M_PI /180);
	cos_c = (float)cos(m_theta3 *M_PI /180);
}

CRTEXToolView::~CRTEXToolView()
{
}

BOOL CRTEXToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此經由修改 CREATESTRUCT cs 
	// 達到修改視窗類別或樣式的目的

	return CView::PreCreateWindow(cs);
}

// CRTEXToolView 描繪

void CRTEXToolView::OnDraw(CDC* pDC)
{
	//CRTEXToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此加入原生資料的描繪程式碼
	UINT sizenum = pDoc->m_view_resolution * 5 / uiscale;
	CPen axisPen;
	if(!axisPen.CreatePen(PS_DOT,10,RGB(255,0,0)))
	{
		// Pen creation failed. Abort the program
		AfxMessageBox(_T("Pen creation failed drawing a line"), MB_OK);
		AfxAbort();
	}
	CPen* pOldPen = pDC->SelectObject(&axisPen);  // Select the pen
	CRect cr;
	CSize size;
	GetClientRect(&cr);
	pDC->DPtoLP(&cr);
	
	int showaxis = pDoc->m_view_resolution;
	CPoint ptAxis;
	
	pDC->MoveTo(0,0);
	ptAxis = Cpt3DptTo2Dpt(showaxis,0,0);
	pDC->LineTo(ptAxis.x,ptAxis.y);	
	pDC->Ellipse(ptAxis.x-sizenum,ptAxis.y+sizenum,ptAxis.x+sizenum,ptAxis.y-sizenum);
	pDC->TextOut(ptAxis.x,ptAxis.y,"X");
	pDC->MoveTo(0,0);
	ptAxis = Cpt3DptTo2Dpt(0,showaxis,0);
	pDC->LineTo(ptAxis.x,ptAxis.y);	
	pDC->Ellipse(ptAxis.x-sizenum,ptAxis.y+sizenum,ptAxis.x+sizenum,ptAxis.y-sizenum);
	pDC->TextOut(ptAxis.x,ptAxis.y,"Y");
	pDC->MoveTo(0,0);
	ptAxis = Cpt3DptTo2Dpt(0,0,showaxis);	
	pDC->LineTo(ptAxis.x,ptAxis.y);
	pDC->Ellipse(ptAxis.x-sizenum,ptAxis.y+sizenum,ptAxis.x+sizenum,ptAxis.y-sizenum);
	pDC->TextOut(ptAxis.x,ptAxis.y,"Z");
	/*
	pDC->MoveTo(cr.TopLeft().x,0);
	pDC->LineTo(cr.BottomRight().x,0);
	pDC->MoveTo(0,cr.BottomRight().y);
	pDC->LineTo(0,cr.TopLeft().y);	
	pDC->TextOut(cr.TopLeft().x,cr.TopLeft().y,"X vs Y");
	*/
	pDC->SelectObject(pOldPen);

	CPen axisPen1;
	if(!axisPen1.CreatePen(PS_DOT,10,RGB(0,0,255)))
	{
		// Pen creation failed. Abort the program
		AfxMessageBox(_T("Pen creation failed drawing a line"), MB_OK);
		AfxAbort();
	}
	pOldPen = pDC->SelectObject(&axisPen1);  // Select the pen

	pDC->MoveTo(0,0);
	ptAxis = Cpt3DptTo2Dpt(-showaxis,0,0);
	pDC->LineTo(ptAxis.x,ptAxis.y);
	pDC->MoveTo(0,0);
	ptAxis = Cpt3DptTo2Dpt(0,-showaxis,0);
	pDC->LineTo(ptAxis.x,ptAxis.y);
	pDC->MoveTo(0,0);
	ptAxis = Cpt3DptTo2Dpt(0,0,-showaxis);	
	pDC->LineTo(ptAxis.x,ptAxis.y);

	pDC->TextOut(cr.TopLeft().x,cr.TopLeft().y,"X vs Y vs Z");

	pDC->SelectObject(pOldPen);
	
	/*
	CPoint* currentPt;
	for(auto iter = pDoc->begin() ; iter != pDoc->end() ; ++iter)
	{
		currentPt = *iter;
		pDC->Ellipse(currentPt->x-sizenum,currentPt->y+sizenum,currentPt->x+sizenum,currentPt->y-sizenum);

	}
	*/
	if(!pDoc->m_fgskipRedraw)
	{
		sizenum = pDoc->m_view_resolution  / uiscale;
		CString str;
		int xx,yy,zz;
		pDoc->savefile.SeekToBegin();    
		while(pDoc->savefile.ReadString(str))
		{
			if(str.Left(4) =="XYZ:")
			{
				xx = atoi((LPCSTR)str.Mid(39,9));
				yy = atoi((LPCSTR)str.Mid(49,9));
				zz = atoi((LPCSTR)str.Mid(59,9));
				CPoint pt;
				pt = Cpt3DptTo2Dpt(xx,yy,zz);
				pDC->Ellipse(pt.x-sizenum,pt.y+sizenum,pt.x+sizenum,pt.y-sizenum);
				//pDC->Ellipse(xx-sizenum,yy+sizenum,xx+sizenum,yy-sizenum);
			}
		}
		pDoc->savefile.SeekToEnd();
	}
}


// CRTEXToolView 診斷

#ifdef _DEBUG
void CRTEXToolView::AssertValid() const
{
	CView::AssertValid();
}

void CRTEXToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRTEXToolDoc* CRTEXToolView::GetDocument() const // 內嵌非偵錯版本
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRTEXToolDoc)));
	return (CRTEXToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CRTEXToolView 訊息處理常式


void CRTEXToolView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別

	CView::OnPrepareDC(pDC, pInfo);
	CRect cr;
	GetClientRect(&cr);
	pDC->SetMapMode(MM_ANISOTROPIC);
	int showWin = pDoc->m_view_resolution * 2;
	pDC->SetWindowExt(showWin,-showWin);	
	if(cr.Width() < cr.Height())
		pDC->SetViewportExt(cr.Width(),cr.Width());
	else
		pDC->SetViewportExt(cr.Height(),cr.Height());

		pDC->SetViewportOrg(cr.Width()/2,cr.Height()/2);      // Set viewport extent		
	CPoint cPt = pDoc->m_view_Center_pt;
	pDC->SetWindowOrg(cPt.x,cPt.y);

}


void CRTEXToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	pDoc = GetDocument();
	pDoc->setViewpointer(this);
}


void CRTEXToolView::OnUpdateIndicatorPos(CCmdUI *pCmdUI)
{
	// TODO: 在此加入您的命令更新 UI 處理常式程式碼
	CPoint curPos;
	CClientDC aDC(this);
	OnPrepareDC(&aDC);              // Set up the device context	2
	GetCursorPos(&curPos);     // Get cursor position in screen coords
	ScreenToClient(&curPos);   // Convert to client coords
	aDC.DPtoLP(&curPos);       // Convert to logical
	CString scaleStr;
	scaleStr.Format(_T("X-Y : (%d, %d)"), curPos.x,curPos.y);
	pCmdUI->SetText(scaleStr); 
	pCmdUI->Enable(); 

}


void CRTEXToolView::DrawPoint(int x, int y,int z)
{
	//if(pDoc->m_fgskipRedraw) return;
	UINT sizenum = pDoc->m_view_resolution / uiscale;
	CClientDC pDc(this);
	OnPrepareDC(&pDc);
	CPoint pt;
	pt = Cpt3DptTo2Dpt(x,y,z);
	pDc.Ellipse(pt.x-sizenum,pt.y+sizenum,pt.x+sizenum,pt.y-sizenum);
	//pDc.Ellipse(x-sizenum,y+sizenum,x+sizenum,y-sizenum);
}
void CRTEXToolView::DrawArm(int x1, int y1,int z1,float m_fYaw,float theta1,float theta2)
{
	UINT sizenum = pDoc->m_view_resolution / uiscale;
	CClientDC pDC(this);
	OnPrepareDC(&pDC);
	CPoint pt,ptAxis;
	float m_fScaraRobot_L1 = 325;
	float m_fScaraRobot_L2 = 225;
	float m_fScaraRobot_Height_Asix1 = (290+74);

	float fXpos=0,fYpos=0,fZpos=0;
	int xJ1, yJ1,zJ1,xJ2, yJ2,zJ2,xJ3, yJ3,zJ3;
	
	///////////////////////////////////////////////////////////////
	CPen axisPen2;

	if(!axisPen2.CreatePen(PS_SOLID ,500,RGB(255,255,255)))
	{
		// Pen creation failed. Abort the program
		AfxMessageBox(_T("Pen creation failed drawing a line"), MB_OK);
		AfxAbort();
	}
	CPen* pOldPen = pDC.SelectObject(&axisPen2);  // Select the pen

	///////////////////////////////////////////////////////////////
	pDC.MoveTo(0,0);
	ptAxis = Cpt3DptTo2Dpt(xJ1_backup,yJ1_backup,zJ1_backup);	
	pDC.LineTo(ptAxis.x,ptAxis.y);
	pDC.Ellipse(ptAxis.x-sizenum,ptAxis.y+sizenum,ptAxis.x+sizenum,ptAxis.y-sizenum);

	ptAxis = Cpt3DptTo2Dpt(xJ2_backup,yJ2_backup,zJ2_backup);	
	pDC.LineTo(ptAxis.x,ptAxis.y);
	pDC.Ellipse(ptAxis.x-sizenum,ptAxis.y+sizenum,ptAxis.x+sizenum,ptAxis.y-sizenum);

	ptAxis = Cpt3DptTo2Dpt(xJ3_backup,yJ3_backup,zJ3_backup);	
	pDC.LineTo(ptAxis.x,ptAxis.y);
	pDC.Ellipse(ptAxis.x-sizenum,ptAxis.y+sizenum,ptAxis.x+sizenum,ptAxis.y-sizenum);

	ptAxis = Cpt3DptTo2Dpt(x1_backup,y1_backup,z1_backup);	
	pDC.LineTo(ptAxis.x,ptAxis.y);
//	pDC.Ellipse(ptAxis.x-sizenum,ptAxis.y+sizenum,ptAxis.x+sizenum/5,ptAxis.y-sizenum/5);

	pDC.SelectObject(pOldPen);

	////////////////draw xyz back //////////////////////////////////////	
	CPen axisPen;
	if(!axisPen.CreatePen(PS_DOT,10,RGB(255,0,0)))
	{
		// Pen creation failed. Abort the program
		AfxMessageBox(_T("Pen creation failed drawing a line"), MB_OK);
		AfxAbort();
	}
	pOldPen = pDC.SelectObject(&axisPen);  // Select the pen

	int showaxis = pDoc->m_view_resolution;
	//	CPoint ptAxis;

	pDC.MoveTo(0,0);
	ptAxis = Cpt3DptTo2Dpt(showaxis,0,0);
	pDC.LineTo(ptAxis.x,ptAxis.y);	
	pDC.Ellipse(ptAxis.x-sizenum,ptAxis.y+sizenum,ptAxis.x+sizenum,ptAxis.y-sizenum);
	pDC.TextOut(ptAxis.x,ptAxis.y,"X");
	pDC.MoveTo(0,0);
	ptAxis = Cpt3DptTo2Dpt(0,showaxis,0);
	pDC.LineTo(ptAxis.x,ptAxis.y);	
	pDC.Ellipse(ptAxis.x-sizenum,ptAxis.y+sizenum,ptAxis.x+sizenum,ptAxis.y-sizenum);
	pDC.TextOut(ptAxis.x,ptAxis.y,"Y");
	pDC.MoveTo(0,0);
	ptAxis = Cpt3DptTo2Dpt(0,0,showaxis);	
	pDC.LineTo(ptAxis.x,ptAxis.y);
	pDC.Ellipse(ptAxis.x-sizenum,ptAxis.y+sizenum,ptAxis.x+sizenum,ptAxis.y-sizenum);
	pDC.TextOut(ptAxis.x,ptAxis.y,"Z");

	pDC.SelectObject(pOldPen);

	CPen axisPen1;
	if(!axisPen1.CreatePen(PS_DOT,10,RGB(0,0,255)))
	{
		// Pen creation failed. Abort the program
		AfxMessageBox(_T("Pen creation failed drawing a line"), MB_OK);
		AfxAbort();
	}
	pOldPen = pDC.SelectObject(&axisPen1);  // Select the pen

	pDC.MoveTo(0,0);
	ptAxis = Cpt3DptTo2Dpt(-showaxis,0,0);
	pDC.LineTo(ptAxis.x,ptAxis.y);
	pDC.MoveTo(0,0);
	ptAxis = Cpt3DptTo2Dpt(0,-showaxis,0);
	pDC.LineTo(ptAxis.x,ptAxis.y);
	pDC.MoveTo(0,0);
	ptAxis = Cpt3DptTo2Dpt(0,0,-showaxis);	
	pDC.LineTo(ptAxis.x,ptAxis.y);

	//	pDC.TextOut(cr.TopLeft().x,cr.TopLeft().y,"X vs Y vs Z");

	pDC.SelectObject(pOldPen);

	////////////////draw xyz back end//////////////////////////////////////

	///////////////////////////////////////////////////////////////
	CPen axisPen3;

	if(!axisPen3.CreatePen(PS_SOLID ,500,RGB(50,100,150)))
	{
		// Pen creation failed. Abort the program
		AfxMessageBox(_T("Pen creation failed drawing a line"), MB_OK);
		AfxAbort();
	}
	 CPen* pOldPen3 = pDC.SelectObject(&axisPen3);  // Select the pen

	///////////////////////////////////////////////////////////////


	xJ1 = 0 ;
	yJ1 = 0 ;
	zJ1 = 0 +(int)(0.5 + (m_fScaraRobot_Height_Asix1 * 1000));

	fXpos = (float)(m_fScaraRobot_L1 * cos((theta1)*M_PI/180));
	fYpos = (float)(m_fScaraRobot_L1 * sin((theta1)*M_PI/180));

	xJ2 = xJ1+(int)(0.5 + (fXpos * 1000));
	yJ2 = yJ1+(int)(0.5 + (fYpos * 1000));
	zJ2 = zJ1;

	fXpos = (float)(m_fScaraRobot_L2 * cos((theta1+theta2)*M_PI/180));
	fYpos = (float)(m_fScaraRobot_L2 * sin((theta1+theta2)*M_PI/180));

	xJ3 = xJ2+(int)(0.5 + (fXpos * 1000));
	yJ3 = yJ2+(int)(0.5 + (fYpos * 1000));
	zJ3 = zJ2;

	pDC.MoveTo(0,0);
	ptAxis = Cpt3DptTo2Dpt(xJ1,yJ1,zJ1);	
	pDC.LineTo(ptAxis.x,ptAxis.y);
	pDC.Ellipse(ptAxis.x-sizenum,ptAxis.y+sizenum,ptAxis.x+sizenum,ptAxis.y-sizenum);

	ptAxis = Cpt3DptTo2Dpt(xJ2,yJ2,zJ2);	
	pDC.LineTo(ptAxis.x,ptAxis.y);
	pDC.Ellipse(ptAxis.x-sizenum,ptAxis.y+sizenum,ptAxis.x+sizenum,ptAxis.y-sizenum);

	ptAxis = Cpt3DptTo2Dpt(xJ3,yJ3,zJ3);	
	pDC.LineTo(ptAxis.x,ptAxis.y);
	pDC.Ellipse(ptAxis.x-sizenum,ptAxis.y+sizenum,ptAxis.x+sizenum,ptAxis.y-sizenum);

	ptAxis = Cpt3DptTo2Dpt(x1,y1,z1);	
	pDC.LineTo(ptAxis.x,ptAxis.y);
//	pDC.Ellipse(ptAxis.x-sizenum,ptAxis.y+sizenum,ptAxis.x+sizenum,ptAxis.y-sizenum);

	pDC.SelectObject(pOldPen3);

	xJ1_backup = xJ1;
	yJ1_backup = yJ1;
	zJ1_backup = zJ1;

	xJ2_backup = xJ2;
	yJ2_backup = yJ2;
	zJ2_backup = zJ2;

	xJ3_backup = xJ3;
	yJ3_backup = yJ3;
	zJ3_backup = zJ3;

	x1_backup = x1;
	y1_backup = y1;
	z1_backup = z1;



}

void CRTEXToolView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	//CView::OnRButtonDown(nFlags, point);
	if(m_view_angle_change)
		return;
	if(!m_View_moving)
	{
		CClientDC aDC(this);                 // DC is for this view
		OnPrepareDC(&aDC);                   // Get origin adjusted
		aDC.DPtoLP(&point);                  // convert point to logical coordinates

		m_RBtnDWN_Pt = point;
		m_View_moving = TRUE;
		SetCapture();                   
	}
}


void CRTEXToolView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	//CView::OnRButtonUp(nFlags, point);
	if(m_View_moving)
	{
		if(this == GetCapture())
			ReleaseCapture();        // Stop capturing mouse messages
		m_View_moving = FALSE;
	}
}


void CRTEXToolView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	//CView::OnMouseMove(nFlags, point);
	if((m_View_moving==TRUE) && (nFlags == MK_RBUTTON))
	{
		CClientDC aDC(this);                 // DC is for this view
		OnPrepareDC(&aDC);                   // Get origin adjusted
		aDC.DPtoLP(&point);                  // convert point to logical coordinates
		CPoint CentPt = pDoc->m_view_Center_pt;		
		CentPt -=  (point - m_RBtnDWN_Pt);
		pDoc->m_view_Center_pt= CentPt;
		Invalidate();
		UpdateWindow();
	}
	if((m_view_angle_change==TRUE) && (nFlags == MK_LBUTTON))
	{
		CClientDC aDC(this);                 // DC is for this view
		OnPrepareDC(&aDC);                   // Get origin adjusted
		aDC.DPtoLP(&point);                  // convert point to logical coordinates
		
		CPoint deltaPt = (point - m_RBtnDWN_Pt);
		m_RBtnDWN_Pt = point;
		//m_theta1 += deltaPt.y /100;
		//m_theta2 += deltaPt.x /100;
		m_theta1 += 360 * (float)deltaPt.y / (float)pDoc->m_view_resolution;
		m_theta2 += 360 * (float)deltaPt.x / (float)pDoc->m_view_resolution;

		sin_a = (float)sin(m_theta1 *M_PI /180);
		sin_b = (float)sin(m_theta2 *M_PI /180);
		cos_a = (float)cos(m_theta1 *M_PI /180);
		cos_b = (float)cos(m_theta2 *M_PI /180);
		Invalidate();
		UpdateWindow();
	}

}
BOOL CRTEXToolView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if((m_View_moving==TRUE) && (nFlags == MK_RBUTTON))
	{
		float tmpResolution =	(float)pDoc->m_view_resolution;
		//if(zDelta > 0) tmpResolution += 5000;
		//if(zDelta < 0) tmpResolution -= 5000;

		if(zDelta > 0) tmpResolution *= (float)1.10;
		if(zDelta < 0) tmpResolution *= (float)0.90;

		if(tmpResolution < 5000) tmpResolution = 5000;
		pDoc->m_view_resolution = (int)tmpResolution;
		Invalidate();
		UpdateWindow();
	}
	if((m_view_angle_change==TRUE) && (nFlags == MK_LBUTTON))
	{
		if(zDelta > 0) m_theta3 += 5;
		if(zDelta < 0) m_theta3 -= 5;
		sin_c = (float)sin(m_theta3 *M_PI /180);
		cos_c = (float)cos(m_theta3 *M_PI /180);
		Invalidate();
		UpdateWindow();
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CRTEXToolView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	//CView::OnLButtonDblClk(nFlags, point);
	pDoc->m_view_resolution =pDoc->m_view_resolutionBK;
	pDoc->m_view_Center_pt = CPoint(0,0);
	m_theta1 = 0;
	m_theta2 = 0 ;
	m_theta3 = 0;
	sin_a = (float)sin(m_theta1 *M_PI /180);
	cos_a = (float)cos(m_theta1 *M_PI /180);
	sin_b = (float)sin(m_theta2 *M_PI /180);
	cos_b = (float)cos(m_theta2 *M_PI /180);
	sin_c = (float)sin(m_theta3 *M_PI /180);
	cos_c = (float)cos(m_theta3 *M_PI /180);
	Invalidate();
	UpdateWindow();

}


CPoint CRTEXToolView::Cpt3DptTo2Dpt(int x, int y, int z)
{
	/*
	float sin_a,cos_a;
	float sin_b,cos_b;
	float sin_c,cos_c;
	sin_a = (float)sin(m_theta1 *M_PI /180);
	sin_b = (float)sin(m_theta2 *M_PI /180);
	sin_c = (float)sin(m_theta3 *M_PI /180);
	cos_a = (float)cos(m_theta1 *M_PI /180);
	cos_b = (float)cos(m_theta2 *M_PI /180);
	cos_c = (float)cos(m_theta3 *M_PI /180);
	*/
	//float XX = cos_b * x + 0 * y - sin_b * z; 
	//float YY = sin_a * sin_b * x + cos_a *y + sin_a * cos_b * z;
	float XX = cos_b * cos_c * x + cos_b * sin_c * y -sin_b * z;
	float YY = ((sin_a * sin_b * cos_c) - (cos_a *sin_c )) * x + ((sin_a * sin_b * sin_c) + (cos_a * cos_c)) * y + (sin_a * cos_b) * z;
	return CPoint((int)XX,(int)YY);
}


void CRTEXToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if(m_View_moving)
		return;
	if(!m_view_angle_change)
	{
		CClientDC aDC(this);                 // DC is for this view
		OnPrepareDC(&aDC);                   // Get origin adjusted
		aDC.DPtoLP(&point);                  // convert point to logical coordinates

		m_RBtnDWN_Pt = point;
		m_view_angle_change = TRUE;
		SetCapture();                   
	}
	//CView::OnLButtonDown(nFlags, point);
}


void CRTEXToolView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if(m_view_angle_change)
	{
		if(this == GetCapture())
			ReleaseCapture();        // Stop capturing mouse messages
		m_view_angle_change = FALSE;
	}


	//CView::OnLButtonUp(nFlags, point);
}

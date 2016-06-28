
// RTEXToolView.h : CRTEXToolView ���O������
//

#pragma once
//#include "RTEXToolDoc.h"
#define M_PI 3.1415926535
const UINT uiscale = 200;
class CRTEXToolView : public CView
{
protected: // �ȱq�ǦC�ƫإ�
	CRTEXToolView();
	DECLARE_DYNCREATE(CRTEXToolView)

// �ݩ�
public:
	CRTEXToolDoc* GetDocument() const;

// �@�~
public:

// �мg
public:
	virtual void OnDraw(CDC* pDC);  // �мg�H�yø���˵�
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	CRTEXToolDoc* pDoc;
protected:
	bool m_View_moving;
	CPoint m_RBtnDWN_Pt;

// �{���X��@
public:
	virtual ~CRTEXToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ͪ��T�������禡
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual void OnInitialUpdate();
	afx_msg void OnUpdateIndicatorPos(CCmdUI *pCmdUI);
	void DrawPoint(int x, int y, int z = 0);
	void DrawArm(int x1, int y1,int z1,float m_fYaw,float m_theta1,float m_theta2);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	float m_theta1;
	float m_theta2;
	float m_theta3;

	int xJ1_backup ;
	int yJ1_backup ;
	int zJ1_backup ;

	int xJ2_backup ;
	int yJ2_backup ;
	int zJ2_backup ;

	int xJ3_backup ;
	int yJ3_backup ;
	int zJ3_backup ;

	int x1_backup ;
	int y1_backup ;
	int z1_backup ;

	float sin_a,cos_a;
	float sin_b,cos_b;
	float sin_c,cos_c;
	CPoint Cpt3DptTo2Dpt(int x, int y, int z);
	bool m_view_angle_change;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // RTEXToolView.cpp ������������
inline CRTEXToolDoc* CRTEXToolView::GetDocument() const
   { return reinterpret_cast<CRTEXToolDoc*>(m_pDocument); }
#endif


#pragma once
#include "afxext.h"
class CMySplitterWnd :
	public CSplitterWnd
{
	//DECLARE_DYNAMIC(CMySplitterWnd)
public:
	CMySplitterWnd(void);
	~CMySplitterWnd(void);
protected:
	int m_cx;
	int m_cy;
public:
	void SetFixsplitter(void);
	void Unsetfixsplitter(void);
	DECLARE_MESSAGE_MAP()
	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


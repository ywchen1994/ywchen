#include "StdAfx.h"
#include "MySplitterWnd.h"

//IMPLEMENT_DYNAMIC(CMySplitterWnd, CSplitterWnd)
CMySplitterWnd::CMySplitterWnd(void)
	: m_cx(m_cxSplitterGap)
	, m_cy(m_cySplitterGap)
{
}


CMySplitterWnd::~CMySplitterWnd(void)
{
}


void CMySplitterWnd::SetFixsplitter(void)
{
	m_cx = m_cxSplitterGap;
	m_cy = m_cySplitterGap;
	m_cxSplitterGap = 10;
	m_cySplitterGap = 10;
}


void CMySplitterWnd::Unsetfixsplitter(void)
{
	m_cxSplitterGap = m_cx;
	m_cySplitterGap = m_cy;
}
BEGIN_MESSAGE_MAP(CMySplitterWnd, CSplitterWnd)
	
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()




void CMySplitterWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��

	//CSplitterWnd::OnLButtonDown(nFlags, point);
}


BOOL CMySplitterWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��

	return FALSE;
}


void CMySplitterWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��

	CWnd::OnMouseMove(nFlags, point);
}


// RTEXToolDoc.cpp : CRTEXToolDoc ���O����@
//

#include "stdafx.h"
// SHARED_HANDLERS �i�H�w�q�b��@�w���B�Y�ϩM�j�M�z�����B�z�`����
// ATL �M�פ��A�ä��\�P�ӱM�צ@�Τ��{���X�C
#ifndef SHARED_HANDLERS
#include "RTEXTool.h"
#endif

#include "RTEXToolDoc.h"
#include "RTEXToolView.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CRTEXToolDoc

IMPLEMENT_DYNCREATE(CRTEXToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CRTEXToolDoc, CDocument)
END_MESSAGE_MAP()


// CRTEXToolDoc �غc/�Ѻc

CRTEXToolDoc::CRTEXToolDoc()
	: m_view_Center_pt(0,0)
	, m_view_resolution(500000)
	, m_view_resolutionBK(500000)
	, m_pView(nullptr)
	, m_fgskipRedraw(false)
{
	// TODO: �b���[�J�@���غc�{���X	
	savefile.Open("point.txt",CFile::modeReadWrite|CFile::modeCreate); 
	saveMonitor.Open("monitorXYZ.txt",CFile::modeReadWrite|CFile::modeCreate); 
	savefile.SeekToEnd();
	//savefile.WriteString("\n");
	savefile.WriteString("ToolStart\n");
	
}

CRTEXToolDoc::~CRTEXToolDoc()
{
	for(auto iter = m_pPtList.begin() ; iter != m_pPtList.end() ; ++iter)
		delete *iter;
	m_pPtList.clear(); // Finally delete all pointers
	savefile.WriteString("ToolEnd\n");
	savefile.Close();
	saveMonitor.Close();
}

BOOL CRTEXToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �b���[�J���s��l�Ƶ{���X
	// (SDI ���|���Φ����)

	return TRUE;
}




// CRTEXToolDoc �ǦC��

void CRTEXToolDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �b���[�J�x�s�{���X
	}
	else
	{
		// TODO: �b���[�J���J�{���X
	}
}

#ifdef SHARED_HANDLERS

// �Y�Ϫ��䴩
void CRTEXToolDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// �ק�o�q�{���X�Hø�s��󪺸��
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// �j�M�B�z�`�����䴩
void CRTEXToolDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// �]�w�Ӧۤ���ƪ��j�M���e�C
	// ���e�������ӥH ";" �j�}

	// �d��:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CRTEXToolDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CRTEXToolDoc �E�_

#ifdef _DEBUG
void CRTEXToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRTEXToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CRTEXToolDoc �R�O


void CRTEXToolDoc::setViewpointer(CWnd* pview)
{
	m_pView = pview;
}


void CRTEXToolDoc::DrawPoint(int xx, int yy, int zz)
{
	((CRTEXToolView*)m_pView)->DrawPoint(xx,yy,zz);
}

// void CRTEXToolDoc::DrawArm(int xx, int yy, int zz)
// {
// 	((CRTEXToolView*)m_pView)->DrawArm(xx,yy,zz);
// }
void CRTEXToolDoc::DrawArm(int xx, int yy, int zz,float m_fYaw,float m_theta1,float m_theta2)
{
	((CRTEXToolView*)m_pView)->DrawArm(xx,yy,zz,m_fYaw,m_theta1,m_theta2);
}


void CRTEXToolDoc::AddCPointSet(CPoint* pt)
{
	m_pPtList.push_back(pt);
}


void CRTEXToolDoc::ClearCPiontSet(void)
{
	for(auto iter = m_pPtList.begin() ; iter != m_pPtList.end() ; ++iter)
		delete *iter;
	m_pPtList.clear(); // Finally delete all pointers
	
}


std::list<CPoint*>::const_iterator CRTEXToolDoc::begin(void)
{
	return m_pPtList.begin();
}


std::list<CPoint*>::const_iterator CRTEXToolDoc::end(void)
{
	return m_pPtList.end();
}

void CRTEXToolDoc::AddPoint2txt(int x_pulse, int y_pulse, int z_pulse, int x_um, int y_um, int z_um)
{
	CString str;
	str.Format("XYZ: %09d %09d %09d um: %09d %09d %09d\n",x_pulse,y_pulse,z_pulse, x_um, y_um, z_um);
	savefile.WriteString(str);

}

void CRTEXToolDoc::AddPoint2txt(int x_pulse, int y_pulse, int z_pulse, int x_um, int y_um, int z_um, UINT timedata, int* pvelocity, int* ptorq)
{
	CString str;
	str.Format("XYZ: %09d %09d %09d um: %09d %09d %09d T: %010u vel: %09d %09d %09d tor: %09d %09d %09d\n",x_pulse,y_pulse,z_pulse, x_um, y_um , z_um,timedata,pvelocity[0],pvelocity[1],pvelocity[2],ptorq[0],ptorq[1],ptorq[2]);
	savefile.WriteString(str);

	//str.Format("XYZ: %09d %09d %09d um: %09d %09d %09d %010u\n",x_pulse,y_pulse,z_pulse, x_um, y_um , z_um,timedata);
	//savefile.WriteString(str);
	//str.Format("vel: %09d %09d %09d\n",pvelocity[0],pvelocity[1],pvelocity[2]);
	//savefile.WriteString(str);
	//str.Format("tor: %09d %09d %09d\n",ptorq[0],ptorq[1],ptorq[2]);
	//savefile.WriteString(str);
}


void CRTEXToolDoc::ClearTxtPoint(void)
{
// 		savefile.Close();
// 		if(!savefile.Open("point.txt",CFile::modeReadWrite|CFile::modeCreate))  // ���}�@���ɮ�!!
// 		{
// 			TRACE("open fail\n");
// 		}
		//savefile.WriteString("clear1\n");
		savefile.SetLength(0);
		savefile.SeekToEnd();
		savefile.WriteString("ToolStart\n");		
}


void CRTEXToolDoc::WriteCmd2Txt(CString str)
{
	savefile.SeekToEnd();
	savefile.WriteString(str);

	saveMonitor.SeekToEnd();
	saveMonitor.WriteString(str);
}


void CRTEXToolDoc::WriteMonitormode2txt(unsigned char Xmode, unsigned char Ymode, unsigned char Zmode)
{
	CString str;
	str.Format("mode X = %d, mode Y = %d, mode Z = %d\n",Xmode,Ymode,Zmode);
	saveMonitor.SeekToEnd();
	saveMonitor.WriteString(str);

}


void CRTEXToolDoc::Addmonitor2Txt(int Xmonitor, int Ymonitor, int Zmonitor)
{
	CString str;
	str.Format("Monitor: %09d %09d %09d\n",Xmonitor,Ymonitor,Zmonitor);
	saveMonitor.WriteString(str);
}

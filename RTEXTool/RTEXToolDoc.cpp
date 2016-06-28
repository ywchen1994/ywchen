
// RTEXToolDoc.cpp : CRTEXToolDoc 類別的實作
//

#include "stdafx.h"
// SHARED_HANDLERS 可以定義在實作預覽、縮圖和搜尋篩選條件處理常式的
// ATL 專案中，並允許與該專案共用文件程式碼。
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


// CRTEXToolDoc 建構/解構

CRTEXToolDoc::CRTEXToolDoc()
	: m_view_Center_pt(0,0)
	, m_view_resolution(500000)
	, m_view_resolutionBK(500000)
	, m_pView(nullptr)
	, m_fgskipRedraw(false)
{
	// TODO: 在此加入一次建構程式碼	
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

	// TODO: 在此加入重新初始化程式碼
	// (SDI 文件會重用此文件)

	return TRUE;
}




// CRTEXToolDoc 序列化

void CRTEXToolDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此加入儲存程式碼
	}
	else
	{
		// TODO: 在此加入載入程式碼
	}
}

#ifdef SHARED_HANDLERS

// 縮圖的支援
void CRTEXToolDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改這段程式碼以繪製文件的資料
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

// 搜尋處理常式的支援
void CRTEXToolDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 設定來自文件資料的搜尋內容。
	// 內容部分應該以 ";" 隔開

	// 範例:  strSearchContent = _T("point;rectangle;circle;ole object;");
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

// CRTEXToolDoc 診斷

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


// CRTEXToolDoc 命令


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
// 		if(!savefile.Open("point.txt",CFile::modeReadWrite|CFile::modeCreate))  // 重開一次檔案!!
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

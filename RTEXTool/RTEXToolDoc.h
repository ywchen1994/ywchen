
// RTEXToolDoc.h : CRTEXToolDoc ���O������
//


#pragma once
#include "atltypes.h"
#include <list>


class CRTEXToolDoc : public CDocument
{
protected: // �ȱq�ǦC�ƫإ�
	CRTEXToolDoc();
	DECLARE_DYNCREATE(CRTEXToolDoc)

// �ݩ�
public:

// �@�~
public:

// �мg
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// �{���X��@
public:
	virtual ~CRTEXToolDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ͪ��T�������禡
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ���j�M�B�z�`���]�w�j�M���e�� Helper �禡
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:	
	CPoint m_view_Center_pt;
	UINT m_view_resolution;
	UINT m_view_resolutionBK;
	void setViewpointer(CWnd* pview);
	CWnd* m_pView;	
	void DrawPoint(int xx, int yy,int zz);
	void DrawArm(int xx, int yy,int zz,float m_fYaw,float m_theta1,float m_theta2);

protected:
	std::list<CPoint*> m_pPtList;	
public:
	void AddCPointSet(CPoint* pt);
	void ClearCPiontSet(void);
	std::list<CPoint*>::const_iterator begin(void);
	std::list<CPoint*>::const_iterator end(void);
	CStdioFile   savefile ,saveMonitor;
	void AddPoint2txt(int x_pulse, int y_pulse, int z_pulse, int x_um , int y_um, int z_um);
	void AddPoint2txt(int x_pulse, int y_pulse, int z_pulse, int x_um, int y_um, int z_um, UINT timedata, int* pvelocity, int* ptorq);
	void ClearTxtPoint(void);
	void WriteCmd2Txt(CString str);
	void WriteMonitormode2txt(unsigned char Xmode, unsigned char Ymode, unsigned char Zmode);
	void Addmonitor2Txt(int Xmonitor, int Ymonitor, int Zmonitor);
	bool m_fgskipRedraw;
};

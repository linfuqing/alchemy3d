// ChessDoc.cpp : CChessDoc ���ʵ��
//

#include "stdafx.h"
#include "Chess.h"

#include "ChessDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChessDoc

IMPLEMENT_DYNCREATE(CChessDoc, CDocument)

BEGIN_MESSAGE_MAP(CChessDoc, CDocument)
END_MESSAGE_MAP()


// CChessDoc ����/����

CChessDoc::CChessDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CChessDoc::~CChessDoc()
{
}

BOOL CChessDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CChessDoc ���л�

void CChessDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CChessDoc ���

#ifdef _DEBUG
void CChessDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CChessDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CChessDoc ����

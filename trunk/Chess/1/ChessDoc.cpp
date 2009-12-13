// ChessDoc.cpp : CChessDoc 类的实现
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


// CChessDoc 构造/析构

CChessDoc::CChessDoc()
{
	// TODO: 在此添加一次性构造代码

}

CChessDoc::~CChessDoc()
{
}

BOOL CChessDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CChessDoc 序列化

void CChessDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CChessDoc 诊断

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


// CChessDoc 命令

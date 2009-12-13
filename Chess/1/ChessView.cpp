// ChessView.cpp : CChessView 类的实现
//

#include "stdafx.h"
#include "Chess.h"

#include "ChessDoc.h"
#include "ChessView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChessView

IMPLEMENT_DYNCREATE(CChessView, CView)

BEGIN_MESSAGE_MAP(CChessView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CChessView 构造/析构

CChessView::CChessView()
{
	// TODO: 在此处添加构造代码

}

CChessView::~CChessView()
{
}

BOOL CChessView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CChessView 绘制

void CChessView::OnDraw(CDC* /*pDC*/)
{
	CChessDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CChessView 打印

BOOL CChessView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CChessView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CChessView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CChessView 诊断

#ifdef _DEBUG
void CChessView::AssertValid() const
{
	CView::AssertValid();
}

void CChessView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CChessDoc* CChessView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CChessDoc)));
	return (CChessDoc*)m_pDocument;
}
#endif //_DEBUG


// CChessView 消息处理程序

// ChessDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Chess.h"
#include "ChessDlg.h"

#include "NewGame.h"

#include "ChessMoveGenerator.h"
#include "ChessEveluation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CChessDlg �Ի���


#define BOARDWIDTH   15
#define BOARDHEIGHT  14

#define GRILLEWIDTH  38
#define GRILLEHEIGHT 38

#define OFFSETWIDTH  14
#define OFFSETHEIGHT 15

#define PLAYER_SIDE RED_SIDE

const CHESSMANTYPE INIT_CHESSBOARD[CHESS_HEIGHT][CHESS_WIDTH] = 
{
	{BLACK_CAR , BLACK_HORSE, BLACK_ELEPHANT, BLACK_BISHOP, BLACK_KING, BLACK_BISHOP, BLACK_ELEPHANT, BLACK_HORSE, BLACK_CAR },
	{NO_CHESS  , NO_CHESS   , NO_CHESS      , NO_CHESS    , NO_CHESS  , NO_CHESS    , NO_CHESS      , NO_CHESS   , NO_CHESS  },
	{NO_CHESS  , BLACK_CANON, NO_CHESS      , NO_CHESS    , NO_CHESS  , NO_CHESS    , NO_CHESS      , BLACK_CANON, NO_CHESS  },
	{BLACK_PAWN, NO_CHESS   , BLACK_PAWN    , NO_CHESS    , BLACK_PAWN, NO_CHESS    , BLACK_PAWN    , NO_CHESS   , BLACK_PAWN},
	{NO_CHESS  , NO_CHESS   , NO_CHESS      , NO_CHESS    , NO_CHESS  , NO_CHESS    , NO_CHESS      , NO_CHESS   , NO_CHESS  },

	{NO_CHESS  , NO_CHESS   , NO_CHESS      , NO_CHESS    , NO_CHESS  , NO_CHESS    , NO_CHESS      , NO_CHESS   , NO_CHESS  },
	{RED_PAWN  , NO_CHESS   , RED_PAWN      , NO_CHESS    , RED_PAWN  , NO_CHESS    , RED_PAWN      , NO_CHESS   , RED_PAWN  },
	{NO_CHESS  , RED_CANON  , NO_CHESS      , NO_CHESS    , NO_CHESS  , NO_CHESS    , NO_CHESS      , RED_CANON  , NO_CHESS  },
	{NO_CHESS  , NO_CHESS   , NO_CHESS      , NO_CHESS    , NO_CHESS  , NO_CHESS    , NO_CHESS      , NO_CHESS   , NO_CHESS  },
	{RED_CAR   , RED_HORSE  , RED_ELEPHANT  , RED_BISHOP  , RED_KING  , RED_BISHOP  , RED_ELEPHANT  , RED_HORSE  , RED_CAR   }
};


CChessDlg::CChessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChessDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NODECOUNT, m_OutputInfo);
}

BEGIN_MESSAGE_MAP(CChessDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_NEWGAME, OnNewGame)
//	ON_WM_ERASEBKGND()
//ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CChessDlg ��Ϣ�������

BOOL CChessDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	m_Chessman.Create( IDB_CHESSMAN, 36, 14, RGB(0, 255, 0) );

	BITMAP Bitmap;
	m_BoardBitmap.LoadBitmap(IDB_CHESSBOARD);
	m_BoardBitmap.GetBitmap(&Bitmap);

	m_nBoardWidth  = Bitmap.bmWidth;
	m_nBoardHeight = Bitmap.bmHeight;

	m_BoardBitmap.DeleteObject();

	m_ChessBoard.SetBoard(INIT_CHESSBOARD);

	//CMoveGenerator<CHESSMANTYPE, CHESS_WIDTH, CHESS_HEIGHT>* pMoveGenerator = new CMoveGenerator<CHESSMANTYPE, CHESS_WIDTH, CHESS_HEIGHT>;
	//CEveluation<CHESSMANTYPE, CHESS_WIDTH, CHESS_HEIGHT>*    pEveluation    = new CChessEveluation;

	m_pSearchEngine = new CNegamaxEngine CHESS;
	m_pSearchEngine->SetSearchDepth(3);
	m_pSearchEngine->SetMoveGenerator(new CChessMoveGenerator);
	m_pSearchEngine->SetEveluator(new CChessEveluation);

	m_MoveChess.Type = NO_CHESS;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CChessDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�
void CChessDlg::OnPaint()
{
	/*if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}*/

	CPaintDC dc(this);
	CDC MemDC;
	POINT pt;
	
	MemDC.CreateCompatibleDC(&dc);
	m_BoardBitmap.LoadBitmap(IDB_CHESSBOARD);

	CBitmap* pOldBitmap = MemDC.SelectObject(&m_BoardBitmap);
	for(INT i = 0; i < CHESS_WIDTH; i++)
	{
		for(INT j = 0; j < CHESS_HEIGHT; j++)
		{
			if(m_ChessBoard.gPosition[j][i] == NO_CHESS)
				continue;

			pt.x = i * GRILLEWIDTH  + OFFSETWIDTH;
			pt.y = j * GRILLEHEIGHT + OFFSETHEIGHT;

			m_Chessman.Draw(&MemDC, CHESS_TYPE_SUBSCRIPT[m_ChessBoard.gPosition[j][i]], pt, ILD_TRANSPARENT);
		}
	}

	if(m_MoveChess.Type != NO_CHESS)
		m_Chessman.Draw(&MemDC, CHESS_TYPE_SUBSCRIPT[m_MoveChess.Type], m_MoveChess.To, ILD_TRANSPARENT);

	pt.x = 0;
	pt.y = 0;

	dc.BitBlt(0, 0, m_nBoardWidth, m_nBoardHeight, &MemDC, 0, 0, SRCCOPY);
	MemDC.SelectObject(pOldBitmap);
	MemDC.DeleteDC();

	m_BoardBitmap.DeleteObject();
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CChessDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CChessDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_MoveChess.Type != NO_CHESS)
	{
		if(point.x < 0)
			point.x = 0;
		if(point.y < 0)
			point.y = 0;
		if(point.x > m_nBoardWidth)
			point.x = m_nBoardWidth;
		if(point.y > m_nBoardHeight)
			point.y = m_nBoardHeight;

		point.x -= GRILLEWIDTH  / 2;
		point.y -= GRILLEHEIGHT / 2;

		m_MoveChess.To = point;

		InvalidateRect(NULL, FALSE);
		UpdateWindow();
		//Invalidate();
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CChessDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	INT x = (point.x - OFFSETWIDTH ) / GRILLEWIDTH,
		y = (point.y - OFFSETHEIGHT) / GRILLEHEIGHT;

	m_BackupChessBoard.SetBoard(&m_ChessBoard);

	if( point.x > 0 && point.y > 0 && point.x < m_nBoardWidth && point.y < m_nBoardHeight && IsSide(PLAYER_SIDE, m_ChessBoard.gPosition[y][x]) )
	{
		m_MoveChess.From.x = x;
		m_MoveChess.From.y = y;
		m_MoveChess.Type   = m_ChessBoard.gPosition[y][x];
		
		m_ChessBoard.gPosition[y][x] = NO_CHESS;

		point.x -= GRILLEWIDTH  / 2;
		point.y -= GRILLEHEIGHT / 2;

		m_MoveChess.To = point;

		InvalidateRect(NULL, FALSE);
		UpdateWindow();
		//Invalidate();

		SetCapture();
	}

	CDialog::OnLButtonDown(nFlags, point);
}

extern INT g_nCount;

void CChessDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	INT x = (point.x - OFFSETWIDTH ) / GRILLEWIDTH,
		y = (point.y - OFFSETHEIGHT) / GRILLEHEIGHT;

	BOOL bTurnSide = FALSE;
	if( m_MoveChess.Type != NO_CHESS && CChessRules::TestMove(m_BackupChessBoard, m_MoveChess.From.x, m_MoveChess.From.y, x, y, TRUE) )
	{
		bTurnSide = TRUE;
		m_ChessBoard.gPosition[y][x] = m_MoveChess.Type;
	}
	else
		m_ChessBoard.SetBoard(&m_BackupChessBoard);

	m_MoveChess.Type = NO_CHESS;

	InvalidateRect(NULL, FALSE);
	UpdateWindow();
	//Invalidate();

	ReleaseCapture();

	if(bTurnSide)
	{
		INT nTimeCount = GetTickCount();
		m_pSearchEngine->SearchNextMove(m_ChessBoard);
		CString sNodeCount;
		sNodeCount.Format(L"%d ms. %d Nodes were eveluated.", GetTickCount() -  nTimeCount, g_nCount);
		m_OutputInfo.SetWindowText(sNodeCount);
		
		InvalidateRect(NULL, FALSE);
		UpdateWindow();
		//Invalidate();
	}

	g_nCount = 0;

	CDialog::OnLButtonUp(nFlags, point);
}

void CChessDlg::OnNewGame()
{
	CNewGame NewGameDlg;

	if(NewGameDlg.DoModal() == IDOK)
	{
		if(m_pSearchEngine)
			delete m_pSearchEngine;

		switch( NewGameDlg.GetSelectedEngine() )
		{
		case 1:
			m_pSearchEngine = new CAlphaBetaEngine CHESS;
			break;
		default:
			m_pSearchEngine = new CNegamaxEngine CHESS;
			break;
		}

		m_pSearchEngine->SetSearchDepth( NewGameDlg.GetSelectedPly() );
		m_pSearchEngine->SetMoveGenerator(new CChessMoveGenerator);
		m_pSearchEngine->SetEveluator(new CChessEveluation);
	}
	else
		return;

	m_ChessBoard.SetBoard(INIT_CHESSBOARD);
	m_MoveChess.Type = NO_CHESS;

	InvalidateRect(NULL, FALSE);
	UpdateWindow();
}

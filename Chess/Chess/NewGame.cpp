// NewGame.cpp : implementation file
//

#include "stdafx.h"
#include "Chess.h"
#include "NewGame.h"


// CNewGame dialog

IMPLEMENT_DYNAMIC(CNewGame, CDialog)

CNewGame::CNewGame(CWnd* pParent /*=NULL*/)
	: CDialog(CNewGame::IDD, pParent)
{

}

CNewGame::~CNewGame()
{
}

void CNewGame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTENGINE, m_SearchEngineList);
	DDX_Control(pDX, IDC_PLY, m_SetPly);
}


BEGIN_MESSAGE_MAP(CNewGame, CDialog)
END_MESSAGE_MAP()

void CNewGame::OnOK()
{
	m_nSelectedEngine = m_SearchEngineList.GetCurSel();
	m_nSelectedPly    = m_SetPly.GetPos();
	CDialog::OnOK();
}

BOOL CNewGame::OnInitDialog()
{
	BOOL bOnInitDialog = CDialog::OnInitDialog();

	m_SearchEngineList.AddString(L"Negamax Search Engine");
	m_SearchEngineList.AddString(L"AlphaBeta Search Engine");

	m_SearchEngineList.SetCurSel(0);

	m_SetPly.SetRange(1, 15);
	m_SetPly.SetPos(3);

	return bOnInitDialog;
}

// CNewGame message handlers

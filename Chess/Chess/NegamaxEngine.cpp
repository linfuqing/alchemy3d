#include "StdAfx.h"
#include "NegamaxEngine.h"

CNegamaxEngine::CNegamaxEngine(void)
{
}

CNegamaxEngine::~CNegamaxEngine(void)
{
}

void CNegamaxEngine::SearchAGoodMove(BASECHESSBOARD& ChessBoard, const CHESSSIDE Side)
{
	m_nMaxDepth = m_nSearchDepth;

	m_ChessBoard.SetChessBoard(&ChessBoard);

	__Negamax(m_nMaxDepth, Side);

	MakeMove(m_pBestMove);

	ChessBoard.SetChessBoard(&m_ChessBoard);
}

///超级无敌博大负极大值算法
INT CNegamaxEngine::__Negamax(const INT nDepth, const CHESSSIDE Side)
{
	BOOL      bIsCurrentSide;
	CHESSSIDE CurrentSide = (bIsCurrentSide = (m_nMaxDepth - nDepth) % 2) ? CHESSSIDE(!Side) : Side;
	if(nDepth <= 0)
		return m_pEveluation->Eveluate(m_ChessBoard, CurrentSide);

	INT nScore = m_pEveluation->IsGameOver(m_ChessBoard, nDepth, Side, bIsCurrentSide);
	if(nScore)
		return nScore;

	CHESSMANTYPE Type;

	INT nCurrent = -MAX_DEPTH_VALUE,
		nCount   = m_pMoveGenerator->CreatePossibleMove(m_ChessBoard, nDepth, CurrentSide);

	for(INT i = 0; i < nCount; i++)
	{
		Type   = MakeMove(&m_pMoveGenerator->m_gMoveList[nDepth][i]);
		nScore = -__Negamax(nDepth - 1, Side);
		UnMakeMove(&m_pMoveGenerator->m_gMoveList[nDepth][i], Type);

		if(nScore > nCurrent)
		{
			nCurrent = nScore;
			if(nDepth == m_nMaxDepth)
				m_pBestMove = &m_pMoveGenerator->m_gMoveList[nDepth][i];
		}
	}

	return nCurrent;
}
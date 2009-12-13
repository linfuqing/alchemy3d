#include "StdAfx.h"
#include "NegamaxEngine.h"

CNegamaxEngine::CNegamaxEngine(void)
{
}

CNegamaxEngine::~CNegamaxEngine(void)
{
}

void CNegamaxEngine::SearchAGoodMove(BASECHESSBOARD& ChessBoard)
{
	m_nMaxDepth = m_nSearchDepth;

	m_ChessBoard = ChessBoard;

	Negamax(m_nMaxDepth);

	MakeMove(m_pBestMove);

	ChessBoard = m_ChessBoard;
}

INT CNegamaxEngine::Negamax(INT nDepth)
{
	if(nDepth < 0)
		return m_pEveluation->Eveluate(m_ChessBoard, (m_nMaxDepth - nDepth) % 2 ? BLACK_SIDE : RED_SIDE);

	INT nScore = IsGameOver(m_ChessBoard, nDepth);
	if(nScore)
		return nScore;

	CHESSMANTYPE Type;

	INT nCurrent = -MAX_DEPTH_VALUE,
		nCount   = m_pMoveGenerator->CreatePossibleMove(m_ChessBoard, nDepth, (m_nMaxDepth - nDepth) % 2 ? BLACK_SIDE : RED_SIDE);

	for(INT i = 0; i < nCount; i++)
	{
		Type = MakeMove(&m_pMoveGenerator->m_gMoveList[nDepth][i]);
		nScore = -Negamax(nDepth - 1);
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
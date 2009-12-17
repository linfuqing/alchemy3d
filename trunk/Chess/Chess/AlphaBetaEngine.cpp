#include "StdAfx.h"
#include "AlphaBetaEngine.h"

CAlphaBetaEngine::CAlphaBetaEngine(void)
{
}

CAlphaBetaEngine::~CAlphaBetaEngine(void)
{
}

void CAlphaBetaEngine::SearchAGoodMove(BASECHESSBOARD& ChessBoard, const SEARCHSIDE Side)
{
	m_nMaxDepth = m_nSearchDepth;

	m_ChessBoard.SetChessBoard(&ChessBoard);

	__AlphaBeta(m_nMaxDepth, -MAX_DEPTH_VALUE, MAX_DEPTH_VALUE, Side);

	MakeMove(m_pBestMove);

	ChessBoard.SetChessBoard(&m_ChessBoard);
}

INT CAlphaBetaEngine::__AlphaBeta(INT nDepth, INT nAlpha, INT nBeta, const SEARCHSIDE Side)
{
		BOOL      bIsCurrentSide;
	SEARCHSIDE CurrentSide = (bIsCurrentSide = (m_nMaxDepth - nDepth) % 2) ? SEARCHSIDE(!Side) : Side;
	if(nDepth <= 0)
		return m_pEveluation->Eveluate(m_ChessBoard, CurrentSide);

	INT nScore = m_pEveluation->IsGameOver(m_ChessBoard, nDepth, Side, bIsCurrentSide);
	if(nScore)
		return nScore;

	CHESSMANTYPE Type;

	INT nCount   = m_pMoveGenerator->CreatePossibleMove(m_ChessBoard, nDepth, CurrentSide);
	for(INT i = 0; i < nCount; i++)
	{
		Type   = MakeMove(&m_pMoveGenerator->m_gMoveList[nDepth][i]);
		nScore = -__AlphaBeta(nDepth - 1, -nBeta,  -nAlpha, Side);
		UnMakeMove(&m_pMoveGenerator->m_gMoveList[nDepth][i], Type);

		if(nScore > nAlpha)
		{
			nAlpha = nScore;
			if(nDepth == m_nMaxDepth)
				m_pBestMove = &m_pMoveGenerator->m_gMoveList[nDepth][i];

			if(nAlpha >= nBeta)
				break;
		}
	}

	return nAlpha;
}
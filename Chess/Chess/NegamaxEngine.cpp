#include "StdAfx.h"
#include "NegamaxEngine.h"

/*template<typename TYPE, UINT WIDTH, UINT HEIGHT>
CNegamaxEngine<TYPE, WIDTH, HEIGHT>::CNegamaxEngine(void)
{
}

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
CNegamaxEngine<TYPE, WIDTH, HEIGHT>::~CNegamaxEngine(void)
{
}*/

/*template<typename TYPE, UINT WIDTH, UINT HEIGHT>
void CNegamaxEngine<TYPE, WIDTH, HEIGHT>::SearchAGoodMove(BASECHESSBOARD& ChessBoard, const SEARCHSIDE Side)
{
	m_nMaxDepth = m_nSearchDepth;

	m_ChessBoard.SetChessBoard(&ChessBoard);

	__Negamax(m_nMaxDepth, Side);

	MakeMove(m_pBestMove);

	ChessBoard.SetChessBoard(&m_ChessBoard);
}

///超级无敌博大负极大值算法
template<typename TYPE, UINT WIDTH, UINT HEIGHT>
INT CNegamaxEngine<TYPE, WIDTH, HEIGHT>::__Negamax(const INT nDepth, const SEARCHSIDE Side)
{
	BOOL      bIsCurrentSide;
	SEARCHSIDE CurrentSide = (bIsCurrentSide = (m_nMaxDepth - nDepth) % 2) ? SEARCHSIDE(!Side) : Side;
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
}*/
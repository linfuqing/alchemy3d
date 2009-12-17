#pragma once
#include "searchengine.h"

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
class CAlphaBetaEngine :
	public CSearchEngine<TYPE, WIDTH, HEIGHT>
{
public:
	CAlphaBetaEngine(void);
	~CAlphaBetaEngine(void);

	virtual void SearchNextMove(BOARD<TYPE, WIDTH, HEIGHT>& Board, const SEARCHSIDE Side = NEGATIVE_SIDE);
private:
	INT __AlphaBeta(INT nDepth, INT nAlpha, INT nBeta);
};

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
CAlphaBetaEngine<TYPE, WIDTH, HEIGHT>::CAlphaBetaEngine(void)
{
}

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
CAlphaBetaEngine<TYPE, WIDTH, HEIGHT>::~CAlphaBetaEngine(void)
{
}

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
void CAlphaBetaEngine<TYPE, WIDTH, HEIGHT>::SearchNextMove(BOARD<TYPE, WIDTH, HEIGHT>& Board, const SEARCHSIDE Side)
{
	m_nMaxDepth = m_nSearchDepth;

	m_Board.SetBoard(&Board);

	m_Side = Side;

	__AlphaBeta(m_nMaxDepth, -m_pEveluation->m_nMAX_VALUE, m_pEveluation->m_nMAX_VALUE);

	MakeMove(m_pBestMove);

	Board.SetBoard(&m_Board);
}

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
INT CAlphaBetaEngine<TYPE, WIDTH, HEIGHT>::__AlphaBeta(INT nDepth, INT nAlpha, INT nBeta)
{
	BOOL      bIsCurrentSide;
	SEARCHSIDE CurrentSide = (bIsCurrentSide = (m_nMaxDepth - nDepth) % 2) ? SEARCHSIDE(!m_Side) : m_Side;
	if(nDepth <= 0)
		return m_pEveluation->Eveluate(m_Board, CurrentSide);

	INT nScore = m_pEveluation->IsGameOver(m_Board, nDepth, m_Side, bIsCurrentSide);
	if(nScore)
		return nScore;

	TYPE Type;

	m_pMoveGenerator->m_nMoveCount = 0;
	m_pMoveGenerator->m_nPly       = nDepth;
	m_pMoveGenerator->CreatePossibleMove(m_Board, CurrentSide);

	INT nCurrent = -m_pEveluation->m_nMAX_VALUE, nCount = m_pMoveGenerator->m_nMoveCount;
	for(INT i = 0; i < nCount; i++)
	{
		Type   = MakeMove(&m_pMoveGenerator->m_gMoveList[nDepth][i]);
		nScore = -__AlphaBeta(nDepth - 1, -nBeta,  -nAlpha);
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
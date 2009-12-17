#pragma once
#include "searchengine.h"

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
class CNegamaxEngine :
	public CSearchEngine<TYPE, WIDTH, HEIGHT>
{
public:
	CNegamaxEngine(void);
	~CNegamaxEngine(void);

	virtual void SearchNextMove(BOARD<TYPE, WIDTH, HEIGHT>& Board, const SEARCHSIDE Side = NEGATIVE_SIDE);
private:
	INT __Negamax(const INT nDepth);
};

///-----------------------------------------------------------------------------------------------------------
template<typename TYPE, UINT WIDTH, UINT HEIGHT>
CNegamaxEngine<TYPE, WIDTH, HEIGHT>::CNegamaxEngine(void)
{
}

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
CNegamaxEngine<TYPE, WIDTH, HEIGHT>::~CNegamaxEngine(void)
{
}

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
void CNegamaxEngine<TYPE, WIDTH, HEIGHT>::SearchNextMove(BOARD<TYPE, WIDTH, HEIGHT>& Board, const SEARCHSIDE Side)
{
	m_nMaxDepth = m_nSearchDepth;

	m_Board.SetBoard(&Board);

	m_Side = Side;

	__Negamax(m_nMaxDepth);

	MakeMove(m_pBestMove);

	Board.SetBoard(&m_Board);
}

///超级无敌博大负极大值算法
template<typename TYPE, UINT WIDTH, UINT HEIGHT>
INT CNegamaxEngine<TYPE, WIDTH, HEIGHT>::__Negamax(const INT nDepth)
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
		nScore = -__Negamax(nDepth - 1);
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
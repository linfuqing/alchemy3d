#pragma once

#include "MoveGenerator.h"
#include "Eveluation.h"

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
class CSearchEngine
{
public:
	CSearchEngine(void);
	virtual ~CSearchEngine(void);
	virtual void SearchNextMove(BOARD<TYPE, WIDTH, HEIGHT>& Board, const SEARCHSIDE Side = NEGATIVE_SIDE) = 0;
public:
	inline void SetSearchDepth(const INT nDepth);
	inline void SetEveluator(CEveluation<TYPE, WIDTH, HEIGHT>* const pEveluation);
	inline void SetMoveGenerator(CMoveGenerator<TYPE, WIDTH, HEIGHT>* const pMoveGenerator);
protected:
	inline TYPE MakeMove(LPSEARCHMOVE const pMove);
	inline void UnMakeMove(LPSEARCHMOVE const pMove, const TYPE Type);
protected:
	BOARD<TYPE, WIDTH, HEIGHT> m_Board;

	LPSEARCHMOVE m_pBestMove;

	CEveluation<TYPE, WIDTH, HEIGHT>* m_pEveluation;
	CMoveGenerator<TYPE, WIDTH, HEIGHT>* m_pMoveGenerator;

	INT m_nSearchDepth;
	INT m_nMaxDepth;

	SEARCHSIDE m_Side;
};

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
inline void CSearchEngine<TYPE, WIDTH, HEIGHT>::SetSearchDepth(const INT nDepth)
{
	m_nSearchDepth = nDepth;
}

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
inline void CSearchEngine<TYPE, WIDTH, HEIGHT>::SetEveluator(CEveluation<TYPE, WIDTH, HEIGHT>* const pEveluation)
{
	m_pEveluation = pEveluation;
}

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
inline void CSearchEngine<TYPE, WIDTH, HEIGHT>::SetMoveGenerator(CMoveGenerator<TYPE, WIDTH, HEIGHT>* const pMoveGenerator)
{
	m_pMoveGenerator = pMoveGenerator;
}

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
inline TYPE CSearchEngine<TYPE, WIDTH, HEIGHT>::MakeMove(LPSEARCHMOVE const pMove)
{
	TYPE Type = m_Board.gPosition[pMove->To.y][pMove->To.x];

	m_Board.gPosition[pMove->To.y][pMove->To.x]     = m_Board.gPosition[pMove->From.y][pMove->From.x];
	m_Board.gPosition[pMove->From.y][pMove->From.x] = NO_CHESS;

	return Type;
}

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
inline void CSearchEngine<TYPE, WIDTH, HEIGHT>::UnMakeMove(LPSEARCHMOVE const pMove, const TYPE Type)
{
	m_Board.gPosition[pMove->From.y][pMove->From.x] = m_Board.gPosition[pMove->To.y][pMove->To.x];
	m_Board.gPosition[pMove->To.y  ][pMove->To.x  ] = Type;
}

///--------------------------------------------------------------------------------------------
template<typename TYPE, UINT WIDTH, UINT HEIGHT>
CSearchEngine<TYPE, WIDTH, HEIGHT>::CSearchEngine(void)
{
}

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
CSearchEngine<TYPE, WIDTH, HEIGHT>::~CSearchEngine(void)
{
	delete m_pEveluation;
	delete m_pMoveGenerator;
}
#pragma once

#include "MoveGenerator.h"
#include "Eveluation.h"

class CSearchEngine
{
public:
	CSearchEngine(void);
	virtual ~CSearchEngine(void);
	virtual void SearchAGoodMove(BASECHESSBOARD& ChessBoard, const CHESSSIDE Side = BLACK_SIDE) = 0;
public:
	inline void SetSearchDepth(const INT nDepth);
	inline void SetEveluator(CEveluation* const pEveluation);
	inline void SetMoveGenerator(CMoveGenerator* const pMoveGenerator);
protected:
	inline CHESSMANTYPE MakeMove(LPCHESSMOVE const pMove);
	inline void UnMakeMove(LPCHESSMOVE const pMove, const CHESSMANTYPE Type);
protected:
	BASECHESSBOARD m_ChessBoard;

	LPCHESSMOVE m_pBestMove;

	CEveluation* m_pEveluation;
	CMoveGenerator* m_pMoveGenerator;

	INT m_nSearchDepth;
	INT m_nMaxDepth;
};

inline void CSearchEngine::SetSearchDepth(const INT nDepth)
{
	m_nSearchDepth = nDepth;
}

inline void CSearchEngine::SetEveluator(CEveluation* const pEveluation)
{
	m_pEveluation = pEveluation;
}

inline void CSearchEngine::SetMoveGenerator(CMoveGenerator* const pMoveGenerator)
{
	m_pMoveGenerator = pMoveGenerator;
}


inline CHESSMANTYPE CSearchEngine::MakeMove(LPCHESSMOVE const pMove)
{
	CHESSMANTYPE Type = m_ChessBoard.gChess[pMove->To.y][pMove->To.x];

	m_ChessBoard.gChess[pMove->To.y][pMove->To.x]     = m_ChessBoard.gChess[pMove->From.y][pMove->From.x];
	m_ChessBoard.gChess[pMove->From.y][pMove->From.x] = NO_CHESS;

	return Type;
}

inline void CSearchEngine::UnMakeMove(LPCHESSMOVE const pMove, const CHESSMANTYPE Type)
{
	m_ChessBoard.gChess[pMove->From.y][pMove->From.x] = m_ChessBoard.gChess[pMove->To.y][pMove->To.x];
	m_ChessBoard.gChess[pMove->To.y  ][pMove->To.x  ] = Type;
}
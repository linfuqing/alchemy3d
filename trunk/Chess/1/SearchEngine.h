#pragma once

#include "MoveGenerator.h"
#include "Eveluation.h"

#define MAX_DEPTH_VALUE 20000
#define MAX_DEPTH       10
#define MAX_VALUE       (MAX_DEPTH_VALUE - MAX_DEPTH)

class CSearchEngine
{
public:
	CSearchEngine(void);
	virtual ~CSearchEngine(void);
	virtual void SearchAGoodMove(BASECHESSBOARD& ChessBoard) = 0;
public:
	inline void SetSearchDepth(INT nDepth);
	inline void SetEveluator(CEveluation* pEveluation);
	inline void SetMoveGenerator(CMoveGenerator* pMoveGenerator);
protected:
	CHESSMANTYPE MakeMove(LPCHESSMOVE pMove);
	void UnMakeMove(LPCHESSMOVE pMove, CHESSMANTYPE Type);

	INT IsGameOver(BASECHESSBOARD& ChessBoard, INT nDepth);
protected:
	BASECHESSBOARD m_ChessBoard;

	LPCHESSMOVE m_pBestMove;

	CEveluation* m_pEveluation;
	CMoveGenerator* m_pMoveGenerator;

	INT m_nSearchDepth;
	INT m_nMaxDepth;
};

inline void CSearchEngine::SetSearchDepth(INT nDepth)
{
	m_nSearchDepth = nDepth;
}

inline void CSearchEngine::SetEveluator(CEveluation *pEveluation)
{
	m_pEveluation = pEveluation;
}

inline void CSearchEngine::SetMoveGenerator(CMoveGenerator *pMoveGenerator)
{
	m_pMoveGenerator = pMoveGenerator;
}

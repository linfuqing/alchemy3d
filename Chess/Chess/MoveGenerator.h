#pragma once

#include "ChessRules.h"

#define MAX_PLY   8
#define MAX_MOVE  80

class CMoveGenerator :
	public CChessRules
{
	friend class CNegamaxEngine;
	friend class CAlphaBetaEngine;
public:
	CMoveGenerator(void);
	~CMoveGenerator(void);

	//static BOOL IsValueMove(BASECHESSBOARD& ChessBoard, INT nFromX, INT nFromY, INT nToX, INT nToY);

	INT CreatePossibleMove(BASECHESSBOARD& ChessBoard, INT nPly, CHESSSIDE Side);
protected:
	CHESSMOVE m_gMoveList[MAX_PLY][MAX_MOVE];

	//INT AddMove(INT nFromX, INT nFromY, INT nToX, INT nToY, INT nPly);
	inline virtual VOID AddMove(const INT nFromX, const INT nFromY, const INT nToX, const INT nToY);


	//void GenerateMove(BASECHESSBOARD& ChessBoard, INT x, INT y, INT nPly);
protected:
	INT m_nMoveCount;
private:
	INT m_nPly;
};

inline VOID CMoveGenerator::AddMove(const INT nFromX, const INT nFromY, const INT nToX, const INT nToY)
{
	//LPCHESSMOVE pMove = &m_gMoveList[nPly][m_nMoveCount];
	LPCHESSMOVE pMove = &m_gMoveList[m_nPly][m_nMoveCount];

	pMove->From.x = nFromX;
	pMove->From.y = nFromY;
	pMove->  To.x =   nToX;
	pMove->  To.y =   nToY;

	m_nMoveCount++;
}
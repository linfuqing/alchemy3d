#pragma once

#include "ChessRules.h"

#define MAX_RELATEPOSITION 20

#define MAX_DEPTH_VALUE 20000
#define MAX_DEPTH       10
#define MAX_VALUE       (MAX_DEPTH_VALUE - MAX_DEPTH)

#define FAIL_VALUE 18888

#define BASEVALUE_PAWN     100
#define BASEVALUE_BISHOP   250
#define BASEVALUE_ELEPHANT 250
#define BASEVALUE_CAR      500
#define BASEVALUE_HORSE    350
#define BASEVALUE_CANON    350
#define BASEVALUE_KING     1000

#define FLEXIBILITY_PAWN     15
#define FLEXIBILITY_BISHOP   1
#define FLEXIBILITY_ELEPHANT 1
#define FLEXIBILITY_CAR      6
#define FLEXIBILITY_HORSE    12
#define FLEXIBILITY_CANON    6
#define FLEXIBILITY_KING     0

class CEveluation :
	public CChessRules
{
public:
	CEveluation(void);
	virtual ~CEveluation(void);

	virtual INT Eveluate(BASECHESSBOARD& ChessBoard, const CHESSSIDE Side);
	
	INT IsGameOver(BASECHESSBOARD& ChessBoard, const INT nDepth, const CHESSSIDE Side, const BOOL bIsCurrentSide);
protected:
	//INT GetRelatePiece(BASECHESSBOARD& ChessBoard, INT x, INT y);
	//BOOL CanTouch(BASECHESSBOARD& ChessBoard, INT nFromX, INT nFromY, INT nToX, INT nToY);
	//VOID AddPoint(INT x, INT y);
	inline virtual VOID AddMove(const INT nFromX, const INT nFromY, const INT nToX, const INT nToY);
protected:
	INT m_BaseValue[14];
	INT m_FlexValue[14];

	CHESSBOARD<SHORT> m_Attract;
	CHESSBOARD<BYTE>  m_Guard;
	CHESSBOARD<BYTE> m_Flexibility;
	CHESSBOARD<INT> m_ChessValue;

	INT m_nPositionCount;
	CHESSMANPOSITION gRelatePosition[MAX_RELATEPOSITION];
};

inline VOID CEveluation::AddMove(const INT nFromX, const INT nFromY, const INT nToX, const INT nToY)
{
	gRelatePosition[m_nPositionCount].x = nToX;
	gRelatePosition[m_nPositionCount].y = nToY;

	m_nPositionCount++;
}
#pragma once

#define MAX_RELATEPOSITION 20
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

class CEveluation
{
public:
	CEveluation(void);
	virtual ~CEveluation(void);

	virtual INT Eveluate(BASECHESSBOARD& ChessBoard, CHESSSIDE Side);
protected:
	INT GetRelatePiece(BASECHESSBOARD& ChessBoard, INT x, INT y);
	BOOL CanTouch(BASECHESSBOARD& ChessBoard, INT nFromX, INT nFromY, INT nToX, INT nToY);
	VOID AddPoint(INT x, INT y);
protected:
	INT m_BaseValue[7];
	INT m_FlexValue[7];

	CHESSBOARD<SHORT> m_Attract;
	CHESSBOARD<BYTE>  m_Guard;
	CHESSBOARD<BYTE> m_Flexibility;
	CHESSBOARD<INT> m_ChessValue;

	INT m_nPositionCount;
	CHESSMANPOSITION gRelatePosition[MAX_RELATEPOSITION];
};

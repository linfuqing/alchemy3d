#pragma once

/*#include "ChessRules.h"

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
#define FLEXIBILITY_KING     0*/

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
class CEveluation// :
	//public CChessRules
{
public:
	CEveluation(INT nMaxValue);
	~CEveluation(void);

	virtual INT Eveluate(BOARD<TYPE, WIDTH, HEIGHT>& Board, const SEARCHSIDE Side) = 0;
	
	virtual INT IsGameOver(BOARD<TYPE, WIDTH, HEIGHT>& Board, const INT nDepth, const SEARCHSIDE Side, const BOOL bIsCurrentSide) = 0;

	const INT m_nMAX_VALUE;
	//INT GetRelatePiece(BASECHESSBOARD& Board, INT x, INT y);
	//BOOL CanTouch(BASECHESSBOARD& Board, INT nFromX, INT nFromY, INT nToX, INT nToY);
	//VOID AddPoint(INT x, INT y);
	//inline virtual VOID AddMove(const INT nFromX, const INT nFromY, const INT nToX, const INT nToY);
/*protected:
	INT m_BaseValue[14];
	INT m_FlexValue[14];

	CHESSBOARD<SHORT> m_Attract;
	CHESSBOARD<BYTE>  m_Guard;
	CHESSBOARD<BYTE> m_Flexibility;
	CHESSBOARD<INT> m_ChessValue;*/

	//INT m_nPositionCount;
	//CHESSMANPOSITION gRelatePosition[MAX_RELATEPOSITION];
};

/*template<typename TYPE, UINT WIDTH, UINT HEIGHT>
inline VOID CEveluation<TYPE, WIDTH, HEIGHT>::AddMove(const INT nFromX, const INT nFromY, const INT nToX, const INT nToY)
{
	gRelatePosition[m_nPositionCount].x = nToX;
	gRelatePosition[m_nPositionCount].y = nToY;

	m_nPositionCount++;
}*/

/*const INT RED_PAWN_ADDITIONAL_VALUE[CHESS_HEIGHT][CHESS_WIDTH] = 
{
	{ 0,  0,   0,   0,   0,   0,   0,  0,  0},
	{90, 90, 110, 120, 120, 120, 110, 90, 90},
	{90, 90, 110, 120, 120, 120, 110, 90, 90},
	{70, 90, 110, 110, 110, 110, 110, 90, 70},
	{70, 70,  70,  70,  70,  70,  70, 70, 70},
	{ 0,  0,   0,   0,   0,   0,   0,  0,  0},
	{ 0,  0,   0,   0,   0,   0,   0,  0,  0},
	{ 0,  0,   0,   0,   0,   0,   0,  0,  0},
	{ 0,  0,   0,   0,   0,   0,   0,  0,  0},
	{ 0,  0,   0,   0,   0,   0,   0,  0,  0}
};

const INT BLACK_PAWN_ADDITIONAL_VALUE[CHESS_HEIGHT][CHESS_WIDTH] = 
{
	{ 0,  0,   0,   0,   0,   0,   0,  0,  0},
	{ 0,  0,   0,   0,   0,   0,   0,  0,  0},
	{ 0,  0,   0,   0,   0,   0,   0,  0,  0},
	{ 0,  0,   0,   0,   0,   0,   0,  0,  0},
	{ 0,  0,   0,   0,   0,   0,   0,  0,  0},
	{70, 70,  70,  70,  70,  70,  70, 70, 70},
	{70, 90, 110, 110, 110, 110, 110, 90, 70},
	{90, 90, 110, 120, 120, 120, 110, 90, 90},
	{90, 90, 110, 120, 120, 120, 110, 90, 90},
	{ 0,  0,   0,   0,   0,   0,   0,  0,  0}
};*/

//Class:
template<typename TYPE, UINT WIDTH, UINT HEIGHT>
CEveluation<TYPE, WIDTH, HEIGHT>::CEveluation(INT nMaxValue):m_nMAX_VALUE(nMaxValue)
{
	/*m_BaseValue[CHESS_TYPE_SUBSCRIPT[RED_KING      ]] = BASEVALUE_KING;
	m_BaseValue[CHESS_TYPE_SUBSCRIPT[RED_CAR       ]] = BASEVALUE_CAR;
	m_BaseValue[CHESS_TYPE_SUBSCRIPT[RED_HORSE     ]] = BASEVALUE_HORSE;
	m_BaseValue[CHESS_TYPE_SUBSCRIPT[RED_CANON     ]] = BASEVALUE_CANON;
	m_BaseValue[CHESS_TYPE_SUBSCRIPT[RED_BISHOP    ]] = BASEVALUE_BISHOP;
	m_BaseValue[CHESS_TYPE_SUBSCRIPT[RED_ELEPHANT  ]] = BASEVALUE_ELEPHANT;
	m_BaseValue[CHESS_TYPE_SUBSCRIPT[RED_PAWN      ]] = BASEVALUE_PAWN;

	m_BaseValue[CHESS_TYPE_SUBSCRIPT[BLACK_KING    ]] = BASEVALUE_KING;
	m_BaseValue[CHESS_TYPE_SUBSCRIPT[BLACK_CAR     ]] = BASEVALUE_CAR;
	m_BaseValue[CHESS_TYPE_SUBSCRIPT[BLACK_HORSE   ]] = BASEVALUE_HORSE;
	m_BaseValue[CHESS_TYPE_SUBSCRIPT[BLACK_CANON   ]] = BASEVALUE_CANON;
	m_BaseValue[CHESS_TYPE_SUBSCRIPT[BLACK_BISHOP  ]] = BASEVALUE_BISHOP;
	m_BaseValue[CHESS_TYPE_SUBSCRIPT[BLACK_ELEPHANT]] = BASEVALUE_ELEPHANT;
	m_BaseValue[CHESS_TYPE_SUBSCRIPT[BLACK_PAWN    ]] = BASEVALUE_PAWN;

	m_FlexValue[CHESS_TYPE_SUBSCRIPT[RED_KING      ]] = FLEXIBILITY_KING;
	m_FlexValue[CHESS_TYPE_SUBSCRIPT[RED_CAR       ]] = FLEXIBILITY_CAR;
	m_FlexValue[CHESS_TYPE_SUBSCRIPT[RED_HORSE     ]] = FLEXIBILITY_HORSE;
	m_FlexValue[CHESS_TYPE_SUBSCRIPT[RED_CANON     ]] = FLEXIBILITY_CANON;
	m_FlexValue[CHESS_TYPE_SUBSCRIPT[RED_BISHOP    ]] = FLEXIBILITY_BISHOP;
	m_FlexValue[CHESS_TYPE_SUBSCRIPT[RED_ELEPHANT  ]] = FLEXIBILITY_ELEPHANT;
	m_FlexValue[CHESS_TYPE_SUBSCRIPT[RED_PAWN      ]] = FLEXIBILITY_PAWN;

	m_FlexValue[CHESS_TYPE_SUBSCRIPT[BLACK_KING    ]] = FLEXIBILITY_KING;
	m_FlexValue[CHESS_TYPE_SUBSCRIPT[BLACK_CAR     ]] = FLEXIBILITY_CAR;
	m_FlexValue[CHESS_TYPE_SUBSCRIPT[BLACK_HORSE   ]] = FLEXIBILITY_HORSE;
	m_FlexValue[CHESS_TYPE_SUBSCRIPT[BLACK_CANON   ]] = FLEXIBILITY_CANON;
	m_FlexValue[CHESS_TYPE_SUBSCRIPT[BLACK_BISHOP  ]] = FLEXIBILITY_BISHOP;
	m_FlexValue[CHESS_TYPE_SUBSCRIPT[BLACK_ELEPHANT]] = FLEXIBILITY_ELEPHANT;
	m_FlexValue[CHESS_TYPE_SUBSCRIPT[BLACK_PAWN    ]] = FLEXIBILITY_PAWN;*/
}

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
CEveluation<TYPE, WIDTH, HEIGHT>::~CEveluation(void)
{
}

/*INT g_nCount = 0;

///Fuck This:
template<typename TYPE, UINT WIDTH, UINT HEIGHT>
INT CEveluation<TYPE, WIDTH, HEIGHT>::Eveluate(BOARD<TYPE, WIDTH, HEIGHT>& Board, SEARCHSIDE Side)
{
	g_nCount++;

	memset( &m_Attract,     0, sizeof(CHESSBOARD<SHORT>) );
	memset( &m_Guard,       0, sizeof(CHESSBOARD<BYTE>)  );
	memset( &m_Flexibility, 0, sizeof(CHESSBOARD<BYTE>)  );
	memset( &m_ChessValue,  0, sizeof(CHESSBOARD<INT>)   );

	INT i, j, k;

	CHESSMANTYPE ChessType, TargetType;

	for(i = 0; i < CHESS_WIDTH; i++)
	{
		for(j = 0; j < CHESS_HEIGHT; j++)
		{
			if(Board.gPosition[j][i] != NO_CHESS)
			{
				ChessType = Board.gPosition[j][i];
				//GetRelatePiece(Board, i, j);
				m_nPositionCount = 0;
				CChessRules::GetMoves(Board, i, j, FALSE);
				for(k = 0; k < m_nPositionCount; k++)
				{
					TargetType = Board.gPosition[gRelatePosition[k].y][gRelatePosition[k].x];
					if(TargetType == NO_CHESS)
						m_Flexibility.gPosition[j][i]++;
					else
					{
						if( ChessType >> SIDE_SHIFT == TargetType >> SIDE_SHIFT )
							m_Guard.gPosition[gRelatePosition[k].y][gRelatePosition[k].x]++;
						else
						{
							m_Attract.gPosition[gRelatePosition[k].y][gRelatePosition[k].x]++;
							m_Flexibility.gPosition[j][i]                                  ++;

							if( (TargetType == RED_KING && Side == BLACK_SIDE) || (TargetType == BLACK_KING && Side == RED_SIDE) )
								return FAIL_VALUE;
							else
							{
								///Fuck This:
								m_Attract.gPosition[gRelatePosition[k].y][gRelatePosition[k].x] += 
								(30 + (m_BaseValue[CHESS_TYPE_SUBSCRIPT[TargetType]] - m_BaseValue[CHESS_TYPE_SUBSCRIPT[ChessType]]) / 10) / 10;
							}
						}
					}
				}
			}
		}
	}

	INT          nHalfValue;

	SEARCHSIDE    ChessSide;
	CHESSMANTYPE King;

	SHORT        nSubscript;
	INT          nRedValue = 0, nBlackValue = 0;
	for(i = 0; i < CHESS_WIDTH; i++)
	{
		for(j = 0; j < CHESS_HEIGHT; j++)
		{
			if(Board.gPosition[j][i] != NO_CHESS)
			{
				ChessType = Board.gPosition[j][i];

				nSubscript = CHESS_TYPE_SUBSCRIPT[ChessType];

				///PartI
				m_ChessValue.gPosition[j][i] ++;
				m_ChessValue.gPosition[j][i] += m_FlexValue[nSubscript] * m_Flexibility.gPosition[j][i];

				if(ChessType == RED_PAWN)
					m_ChessValue.gPosition[j][i] += RED_PAWN_ADDITIONAL_VALUE[j][i];
				if(ChessType == BLACK_PAWN)
					m_ChessValue.gPosition[j][i] += BLACK_PAWN_ADDITIONAL_VALUE[j][i];

				///PartII
				nHalfValue = m_BaseValue[nSubscript];

				m_ChessValue.gPosition[j][i] += nHalfValue;

				///Fuck This:
				nHalfValue /= 16;

				ChessSide = SEARCHSIDE(ChessType >> SIDE_SHIFT);

				if(m_Attract.gPosition[j][i])
				{
					King = CHESSMANTYPE( KING + (ChessSide << SIDE_SHIFT) );

					if(Side == ChessSide)
					{
						///Fuck This:
						m_ChessValue.gPosition[j][i] += ChessType == King ? -20 : -(nHalfValue * 2);
						if(m_Guard.gPosition[j][i])
							m_ChessValue.gPosition[j][i] += nHalfValue;
					}
					else
					{
						if(ChessType == King)
							return FAIL_VALUE;

						///Fuck This:
						m_ChessValue.gPosition[j][i] -= m_Guard.gPosition[j][i] ? nHalfValue : nHalfValue * 10;
					}

					m_ChessValue.gPosition[j][i] -= m_Attract.gPosition[j][i];
				}
				else if(m_Guard.gPosition[j][i])
					//And Fuck This:
					m_ChessValue.gPosition[j][i] += 5;

				///PartIII:
				if(ChessSide == RED_SIDE)
					nRedValue   += m_ChessValue.gPosition[j][i];
				else
					nBlackValue += m_ChessValue.gPosition[j][i];
			}
		}
	}

	return Side == RED_SIDE ? (nRedValue - nBlackValue) : (nBlackValue - nRedValue);
}

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
INT CEveluation<TYPE, WIDTH, HEIGHT>::IsGameOver(BOARD<TYPE, WIDTH, HEIGHT> &Board, const INT nDepth, const SEARCHSIDE Side, const BOOL bIsCurrentSide)
{
	INT i, j;
	BOOL RedLive = FALSE, BlackLive = FALSE;

	for(i = 3; i < 6; i++)
	{
		for(j = 7; j < 10; j++)
		{
			if(Board.gPosition[j][i] == RED_KING)
				RedLive   = TRUE;
			if(Board.gPosition[j][i] == BLACK_KING)
				BlackLive = TRUE;
		}

		for(j = 0; j < 3 ; j++)
		{
			if(Board.gPosition[j][i] == RED_KING)
				RedLive   = TRUE;
			if(Board.gPosition[j][i] == BLACK_KING)
				BlackLive = TRUE;
		}
	}

	//i = (m_nMaxDepth - nDepth + 1) % 2;

	if(Side == RED_SIDE)
	{
		if(!RedLive)
			return bIsCurrentSide ? ( MAX_VALUE + nDepth) : (-MAX_VALUE - nDepth);
		if(!BlackLive)
			return bIsCurrentSide ? (-MAX_VALUE - nDepth) : ( MAX_VALUE + nDepth);
	}
	else if(Side == BLACK_SIDE)
	{
		if(!RedLive)
			return bIsCurrentSide ? (-MAX_VALUE - nDepth) : ( MAX_VALUE + nDepth);
		if(!BlackLive)
			return bIsCurrentSide ? ( MAX_VALUE + nDepth) : (-MAX_VALUE - nDepth);
	}

	return 0;
}*/
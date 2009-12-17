#include "StdAfx.h"
#include "ChessEveluation.h"

const INT RED_PAWN_ADDITIONAL_VALUE[CHESS_HEIGHT][CHESS_WIDTH] = 
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
};

//Class:
CChessEveluation::CChessEveluation(void):CEveluation(MAX_DEPTH_VALUE)
{
	m_BaseValue[CHESS_TYPE_SUBSCRIPT[RED_KING      ]] = BASEVALUE_KING;
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
	m_FlexValue[CHESS_TYPE_SUBSCRIPT[BLACK_PAWN    ]] = FLEXIBILITY_PAWN;
}

CChessEveluation::~CChessEveluation(void)
{
}

INT g_nCount = 0;

///Fuck This:
INT CChessEveluation::Eveluate(BOARD CHESS& ChessBoard, CHESSSIDE Side)
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
			if(ChessBoard.gPosition[j][i] != NO_CHESS)
			{
				ChessType = ChessBoard.gPosition[j][i];
				//GetRelatePiece(ChessBoard, i, j);
				m_nPositionCount = 0;
				CChessRules::GetMoves(ChessBoard, i, j, FALSE);
				for(k = 0; k < m_nPositionCount; k++)
				{
					TargetType = ChessBoard.gPosition[gRelatePosition[k].y][gRelatePosition[k].x];
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

	CHESSSIDE    ChessSide;
	CHESSMANTYPE King;

	SHORT        nSubscript;
	INT          nRedValue = 0, nBlackValue = 0;
	for(i = 0; i < CHESS_WIDTH; i++)
	{
		for(j = 0; j < CHESS_HEIGHT; j++)
		{
			if(ChessBoard.gPosition[j][i] != NO_CHESS)
			{
				ChessType = ChessBoard.gPosition[j][i];

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

INT CChessEveluation::IsGameOver(BOARD CHESS &ChessBoard, const INT nDepth, const CHESSSIDE Side, const BOOL bIsCurrentSide)
{
	INT i, j;
	BOOL RedLive = FALSE, BlackLive = FALSE;

	for(i = 3; i < 6; i++)
	{
		for(j = 7; j < 10; j++)
		{
			if(ChessBoard.gPosition[j][i] == RED_KING)
				RedLive   = TRUE;
			if(ChessBoard.gPosition[j][i] == BLACK_KING)
				BlackLive = TRUE;
		}

		for(j = 0; j < 3 ; j++)
		{
			if(ChessBoard.gPosition[j][i] == RED_KING)
				RedLive   = TRUE;
			if(ChessBoard.gPosition[j][i] == BLACK_KING)
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
}
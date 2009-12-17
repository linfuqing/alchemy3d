#include "StdAfx.h"
#include "Eveluation.h"

///***********
/*
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
CEveluation::CEveluation(void)
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

CEveluation::~CEveluation(void)
{
}

INT g_nCount = 0;

///Fuck This:
INT CEveluation::Eveluate(BASECHESSBOARD& ChessBoard, SEARCHSIDE Side)
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

	SEARCHSIDE    ChessSide;
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

	/*INT nHalfValue;
	for(i = 0; i < CHESS_WIDTH; i++)
	{
		for(j = 0; j < CHESS_HEIGHT; j++)
		{
			if(ChessBoard.gPosition[j][i] != NO_CHESS)
			{
				ChessType = ChessBoard.gPosition[j][i];

				nHalfValue = m_BaseValue[ToNoSide(ChessType) - 1];

				m_ChessValue.gPosition[j][i] += nHalfValue;

				///Fuck This:
				nHalfValue /= 16;

				if(m_Attract.gPosition[j][i])
				{
					SEARCHSIDE    ChessSide = SEARCHSIDE(            ChessType >> SIDE_SHIFT  );
					CHESSMANTYPE King      = CHESSMANTYPE( KING + (ChessSide << SIDE_SHIFT) );

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

					m_ChessValue.gPosition[j][i] += m_Attract.gPosition[j][i];
				}
				else if(m_Guard.gPosition[j][i])
					//And Fuck This:
					m_ChessValue.gPosition[j][i] += 5;
			}
		}
	}
}*/

/*INT CEveluation::IsGameOver(BASECHESSBOARD &ChessBoard, const INT nDepth, const SEARCHSIDE Side, const BOOL bIsCurrentSide)
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
}*/

/*
///可合并
INT CEveluation::GetRelatePiece(BASECHESSBOARD& ChessBoard, INT x, INT y)
{
	m_nPositionCount = 0;
	
	INT i, j;
	switch(ChessBoard.gPosition[y][x])
	{
	case RED_KING:
	case BLACK_KING:
		///可改进
		for(i = 3; i < 6; i++)
		{
			for(j = 0; j < 3; j++)
				if( CanTouch(ChessBoard, x, y, i, j) )
					AddPoint(i, j);
			for(j = 7; j < 10; j++)
				if( CanTouch(ChessBoard, x, y, i, j) )
					AddPoint(i, j);
		}
		break;
	case RED_CAR:
	case BLACK_CAR:
		for(i = x + 1, j = y; i < CHESS_WIDTH   ; i++)
		{
			if(NO_CHESS == ChessBoard.gPosition[j][i])
				AddPoint(i, j);
			else
			{
				AddPoint(i, j);
				break;
			}
		}

		for(i = x - 1, j = y; i >= 0            ; i--)
		{
			if(NO_CHESS == ChessBoard.gPosition[j][i])
				AddPoint(i, j);
			else
			{
				AddPoint(i, j);
				break;
			}
		}

		for(i = x   , j = y + 1; j < CHESS_HEIGHT; j++)
		{
			if(NO_CHESS == ChessBoard.gPosition[j][i])
				AddPoint(i, j);
			else
			{
				AddPoint(i, j);
				break;
			}
		}

		for(i = x    , j = y - 1; j >= 0        ; j--)
		{
			if(NO_CHESS == ChessBoard.gPosition[j][i])
				AddPoint(i, j);
			else
			{
				AddPoint(i, j);
				break;
			}
		}
		break;
	case RED_HORSE:
	case BLACK_HORSE:
		i = x + 1;
		j = y + 2;
		///CanTouch needn`t
		if( i < CHESS_WIDTH && j < CHESS_HEIGHT && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);

		i = x + 1;
		j = y - 2;
		if( i < CHESS_WIDTH && j >= 0           && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);

		i = x - 1;
		j = y + 2;
		if( i >= 0          && j < CHESS_HEIGHT && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);

		i = x - 1;
		j = y - 2;
		if( i >= 0          && j >= 0           && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);

		i = x + 2;
		j = y + 1;
		if( i < CHESS_WIDTH && j < CHESS_HEIGHT && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);

		i = x + 2;
		j = y - 1;
		if( i < CHESS_WIDTH && j >= 0           && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);

		i = x - 2;
		j = y + 1;
		if( i >= 0          && j < CHESS_HEIGHT && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);

		i = x - 2;
		j = y - 1;
		if( i >= 0          && j >= 0           && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);
		break;
	case RED_CANON:
	case BLACK_CANON:
		BOOL bNoChess;

		bNoChess = TRUE;
		for(i = x + 1, j = y; i < CHESS_WIDTH; i++)
		{
			if(NO_CHESS == ChessBoard.gPosition[j][i])
			{
				if(bNoChess)
					AddPoint(i, j);
			}
			else
			{
				if(bNoChess)
					bNoChess = FALSE;
				else
				{
					AddPoint(i, j);
					break;
				}
			}
		}

		bNoChess = TRUE;
		for(i = x - 1, j = y; i >= 0         ; i--)
		{
			if(NO_CHESS == ChessBoard.gPosition[j][i])
			{
				if(bNoChess)
					AddPoint(i, j);
			}
			else
			{
				if(bNoChess)
					bNoChess = FALSE;
				else
				{
					AddPoint(i, j);
					break;
				}
			}
		}

		bNoChess = TRUE;
		for(i = x   , j = y + 1; j < CHESS_HEIGHT; j++)
		{
			if(NO_CHESS == ChessBoard.gPosition[j][i])
			{
				if(bNoChess)
					AddPoint(i, j);
			}
			else
			{
				if(bNoChess)
					bNoChess = FALSE;
				else
				{
					AddPoint(i, j);
					break;
				}
			}
		}

		bNoChess = TRUE;
		for(i = x    , j = y - 1; j >= 0        ; j--)
		{
			if(NO_CHESS == ChessBoard.gPosition[j][i])
			{
				if(bNoChess)
					AddPoint(i, j);
			}
			else
			{
				if(bNoChess)
					bNoChess = FALSE;
				else
				{
					AddPoint(i, j);
					break;
				}
			}
		}
		break;
	case RED_ELEPHANT:
	case BLACK_ELEPHANT:
		i = x + 2;
		j = y + 2;
		if( i < CHESS_WIDTH && j < CHESS_HEIGHT && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);

		i = x + 2;
		j = y - 2;
		if( i < CHESS_WIDTH && j >= 0           && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);

		i = x - 2;
		j = y + 2;
		if( i >= 0          && j < CHESS_HEIGHT && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);

		i = x - 2;
		j = y - 2;
		if( i >= 0          && j >= 0           && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);
		break;

	case RED_BISHOP:
		for(i = 3; i < 6; i++)
		{
			for(j = 7; j < 10; j++)
				if( CanTouch(ChessBoard, x, y, i, j) )
					AddPoint(i, j);
		}
		break;
	case BLACK_BISHOP:
		for(i = 3; i < 6; i++)
		{
			for(j = 0; j < 3; j++)
				if( CanTouch(ChessBoard, x, y, i, j) )
					AddPoint(i, j);
		}
		break;
	case RED_PAWN:
		i = x;
		j = y - 1;
		if(j > 0              )
			AddPoint(i, j);

		if(y < 5)
		{
			j = y;

			i = x - 1;
			if(i > 0          )
				AddPoint(i, j);
			i = x + 1;
			if(i < CHESS_WIDTH)
				AddPoint(i, j);
		}
		break;
	case BLACK_PAWN:
		i = x;
		j = y + 1;
		if(j < CHESS_HEIGHT   )
			AddPoint(i, j);

		if(y > 4)
		{
			j = y;

			i = x - 1;
			if(i > 0          )
				AddPoint(i, j);
			i = x + 1;
			if(i < CHESS_WIDTH)
				AddPoint(i, j);
		}
		break;

	default:
		break;
	}
	/*INT i, j;
	switch(ChessBoard.gPosition[y][x])
	{
	case RED_KING:
	case BLACK_KING:
		for(i = 3; i < 6; i++)
		{
			for(j = 0; j < 3; j++)
				if( CanTouch(ChessBoard, x, y, i, j) )
					AddPoint(i, j);

			for(j = 7; j < 10; j++)
				if( CanTouch(ChessBoard, x, y, i, j) )
					AddPoint(i, j);
		}
		break;
	case RED_HORSE:
	case BLACK_HORSE:
		i = x + 2;
		j = y + 1;
		if( i < CHESS_WIDTH && j < CHESS_HEIGHT && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);

		i = x + 2;
		j = y - 1;
		if( i < CHESS_WIDTH && j >= 0           && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);

		i = x - 2;
		j = y + 1;
		if( i >= 0          && j < CHESS_HEIGHT && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);

		i = x - 2;
		j = y - 1;
		if( i > 0           && j > 0            && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);


		i = x + 1;
		j = y + 2;
		if( i < CHESS_WIDTH && j < CHESS_HEIGHT && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);

		i = x + 1;
		j = y - 2;
		if( i < CHESS_WIDTH && j >= 0           && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);

		i = x - 1;
		j = y + 2;
		if( i >= 0          && j < CHESS_HEIGHT && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);

		i = x - 1;
		j = y - 2;
		if( i > 0           && j > 0            && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);
		break;
	case RED_CAR:
	case BLACK_CAR:
		for(i = x + 1, j = y    ; i < CHESS_WIDTH; i++)
		{
			if(NO_CHESS == ChessBoard.gPosition[j][i])
				AddPoint(i, j);
			else
			{
				AddPoint(i, j);
				break;
			}
		}

		for(i = x - 1, j = y    ; i < CHESS_WIDTH; i--)
		{
			if(NO_CHESS == ChessBoard.gPosition[j][i])
				AddPoint(i, j);
			else
			{
				AddPoint(i, j);
				break;
			}
		}

		for(i = x    , j = y + 1; j < CHESS_HEIGHT; j++)
		{
			if(NO_CHESS == ChessBoard.gPosition[j][i])
				AddPoint(i, j);
			else
			{
				AddPoint(i, j);
				break;
			}
		}

		for(i = x    , j = y - 1; j < CHESS_HEIGHT; j++)
		{
			if(NO_CHESS == ChessBoard.gPosition[j][i])
				AddPoint(i, j);
			else
			{
				AddPoint(i, j);
				break;
			}
		}
		break;
	case RED_CANON:
	case BLACK_CANON:
		for(i = x + 1, j = y    ; i < CHESS_WIDTH; i++)
		{
			if(NO_CHESS == ChessBoard.gPosition[j][i])
				AddPoint(i, j);
			else
			{
				AddPoint(i, j);
				break;
			}
		}
		break;
	case RED_BISHOP:
		break;
	case RED_ELEPHANT:
		break;
	case RED_PAWN:
		break;




	case BLACK_BISHOP:
		break;
	case BLACK_ELEPHANT:
		break;
	case BLACK_PAWN:
		break;

	default:
		break;
	}

	return m_nPositionCount;
}

BOOL CEveluation::CanTouch(BASECHESSBOARD& ChessBoard, INT nFromX, INT nFromY, INT nToX, INT nToY)
{
	if(nFromX == nToX && nFromY == nToY)
	return FALSE;

	INT  i, j, nIncrease;

	CHESSMANTYPE From = ChessBoard.gPosition[nFromY][nFromX], To = ChessBoard.gPosition[nToY][nToX];

	switch(From)
	{
	case RED_KING:
		if(To == BLACK_KING)
		{
			if(nFromX != nToX)
				return FALSE;

			for(j = nFromY; j > nToY; j--)
				if(ChessBoard.gPosition[j][nFromX] != NO_CHESS)
					return FALSE;
		}
		else
		{
			if(nToY < 7 || nToX > 5 || nToX < 3)
				return FALSE;

			if(abs( (nToX - nFromX) ) + abs( (nToY - nFromY) ) > 1)
				return FALSE;
		}
		break;
	case BLACK_KING:
		if(To == RED_KING)
		{
			if(nFromX != nToX)
				return FALSE;

			for(j = nFromY; j < nToY; j++)
				if(ChessBoard.gPosition[j][nFromX] != NO_CHESS)
					return FALSE;
		}
		else
		{
			if(nToY > 2 || nToX > 5 || nToX < 3)
				return FALSE;

			if(abs( (nToX - nFromX) ) + abs( (nToY - nFromY) ) > 1)
				return FALSE;
		}
		break;
	case RED_BISHOP:
		if(nToY < 7 || nToX > 5 || nToX < 3)
			return FALSE;
		if(abs( (nToX - nFromX) ) != 1 || abs( (nToY - nFromY) ) != 1)
			return FALSE;
		break;
	case BLACK_BISHOP:
		if(nToY > 2 || nToX > 5 || nToX < 3)
			return FALSE;
		if(abs( (nToX - nFromX) ) != 1 || abs( (nToY - nFromY) ) != 1)
			return FALSE;
		break;
	case RED_ELEPHANT:
		if(nToY < 5)
			return FALSE;
		if(abs( (nToX - nFromX) ) != 2 || abs( (nToY - nFromY) ) != 2)
			return FALSE;
		if(ChessBoard.gPosition[(nFromY + nToY) / 2][(nFromX + nToX) / 2] != NO_CHESS)
			return FALSE;
		break;
	case BLACK_ELEPHANT:
		if(nToY > 4)
			return FALSE;
		if(abs( (nToX - nFromX) ) != 2 || abs( (nToY - nFromY) ) != 2)
			return FALSE;
		if(ChessBoard.gPosition[(nFromY + nToY) / 2][(nFromX + nToX) / 2] != NO_CHESS)
			return FALSE;
		break;
	case RED_PAWN:
		if(nToY > nFromY)
			return FALSE;
		if(nFromY > 4 && nFromY == nToY)
			return FALSE;
		if(nFromY - nToY + abs( (nToX - nFromX) ) > 1)
			return FALSE;
		break;
	case BLACK_PAWN:
		if(nToY < nFromY)
			return FALSE;
		if(nFromY < 5 && nFromY == nToY)
			return FALSE;
		if(nToY - nFromY + abs( (nToX - nFromX) ) > 1)
			return FALSE;
		break;

	case RED_CAR:
	case BLACK_CAR:
		if(nFromX != nToX && nFromY != nToY)
			return FALSE;
		if(nFromX == nToX)
		{
			nIncrease = nFromY < nToY ? 1 : -1;
			for(j = nFromY + nIncrease; j != nToY; j+=nIncrease)
				if(ChessBoard.gPosition[j][nFromX] != NO_CHESS)
					return FALSE;
		}
		else
		{
			nIncrease = nFromX < nToX ? 1 : -1;
			for(i = nFromX + nIncrease; i != nToX; i+=nIncrease)
				if(ChessBoard.gPosition[nFromY][i] != NO_CHESS)
					return FALSE;
		}
		break;
	case RED_HORSE:
	case BLACK_HORSE:
		if(!( (abs( (nToX - nFromX) ) == 1&& abs( (nToY - nFromY) ) == 2) || (abs( (nToX - nFromX) ) == 2&& abs( (nToY - nFromY) ) == 1) ) )
			return FALSE;

		if(nToX - nFromX == 2)
		{
			i = nFromX + 1;
			j = nFromY;
		}
		else if(nFromX - nToX == 2)
		{
			i = nFromX - 1;
			j = nFromY;
		}
		else if(nToY - nFromY == 2)
		{
			i = nFromX;
			j = nFromY + 1;
		}
		else if(nFromY - nToY == 2)
		{
			i = nFromX;
			j = nFromY - 1;
		}

		if(ChessBoard.gPosition[j][i] != NO_CHESS)
			return FALSE;
		break;
	case RED_CANON:
	case BLACK_CANON:
		if(nFromX != nToX && nFromY != nToY)
			return FALSE;

		if(To == NO_CHESS)
		{	
			if(nFromX == nToX)
			{
				nIncrease = nFromY < nToY ? 1 : -1;
				for(j = nFromY + nIncrease; j != nToY; j+=nIncrease)
					if(ChessBoard.gPosition[j][nFromX] != NO_CHESS)
						return FALSE;
			}
			else
			{
				nIncrease = nFromX < nToX ? 1 : -1;
				for(i = nFromX + nIncrease; i != nToX; i+=nIncrease)
					if(ChessBoard.gPosition[nFromY][i] != NO_CHESS)
						return FALSE;
			}
		}
		else
		{
			BOOL bNoChess = TRUE;

			if(nFromX == nToX)
			{
				nIncrease = nFromY < nToY ? 1 : -1;
				for(j = nFromY + nIncrease; j != nToY; j += nIncrease)
					if(ChessBoard.gPosition[j][nFromX] != NO_CHESS)
						if(!bNoChess)
							return FALSE;
						else
							bNoChess = FALSE;
			}
			else
			{
				nIncrease = nFromX < nToX ? 1 : -1;
				for(i = nFromX + nIncrease; i != nToX; i += nIncrease)
					if(ChessBoard.gPosition[nFromY][i] != NO_CHESS)
						if(!bNoChess)
							return FALSE;
						else
							bNoChess = FALSE;
			}

			if(bNoChess)
				return FALSE;
		}
		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
}*/

//VOID CEveluation::AddPoint(INT x, INT y)
/*INT CEveluation::AddMove(INT nFromX, INT nFromY, INT nToX, INT nToY)
{
	gRelatePosition[m_nPositionCount].x = nToX;
	gRelatePosition[m_nPositionCount].y = nToY;

	return ++m_nPositionCount;
}*/
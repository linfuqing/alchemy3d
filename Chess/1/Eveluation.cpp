#include "StdAfx.h"
#include "Eveluation.h"

///***********
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

inline INT GetPawnAdditionalValue(INT x, INT y, BASECHESSBOARD& ChessBoard)
{
	return ToNoSide(ChessBoard[y][x]) == PAWN ? (ChessBoard[y][x] >> SIDE_SHIFT ? BLACK_PAWN_ADDITIONAL_VALUE[y][x] : RED_PAWN_ADDITIONAL_VALUE[y][x]) : 0;
}

//Class:
CEveluation::CEveluation(void)
{
	m_BaseValue[KING     - 1] = BASEVALUE_KING;
	m_BaseValue[CAR      - 1] = BASEVALUE_CAR;
	m_BaseValue[HORSE    - 1] = BASEVALUE_HORSE;
	m_BaseValue[CANON    - 1] = BASEVALUE_CANON;
	m_BaseValue[BISHOP   - 1] = BASEVALUE_BISHOP;
	m_BaseValue[ELEPHANT - 1] = BASEVALUE_ELEPHANT;
	m_BaseValue[PAWN     - 1] = BASEVALUE_PAWN;

	m_FlexValue[KING     - 1] = FLEXIBILITY_KING;
	m_FlexValue[CAR      - 1] = FLEXIBILITY_CAR;
	m_FlexValue[HORSE    - 1] = FLEXIBILITY_HORSE;
	m_FlexValue[CANON    - 1] = FLEXIBILITY_CANON;
	m_FlexValue[BISHOP   - 1] = FLEXIBILITY_BISHOP;
	m_FlexValue[ELEPHANT - 1] = FLEXIBILITY_ELEPHANT;
	m_FlexValue[PAWN     - 1] = FLEXIBILITY_PAWN;
}

CEveluation::~CEveluation(void)
{
}

INT g_nCount = 0;

///Fuck This:
INT CEveluation::Eveluate(BASECHESSBOARD& ChessBoard, CHESSSIDE Side)
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
			if(ChessBoard[j][i] != NO_CHESS)
			{
				ChessType = ChessBoard[j][i];
				GetRelatePiece(ChessBoard, i, j);
				for(k = 0; k < m_nPositionCount; k++)
				{
					TargetType = ChessBoard[gRelatePosition[k].y][gRelatePosition[k].x];
					if(TargetType == NO_CHESS)
						m_Flexibility[j][i]++;
					else
					{
						if( IsSameSide(ChessType, TargetType) )
							m_Guard[gRelatePosition[k].y][gRelatePosition[k].x]++;
						else
						{
							m_Attract[gRelatePosition[k].y][gRelatePosition[k].x]++;
							m_Flexibility[j][i]                                  ++;

							if( (TargetType == RED_KING && Side == BLACK_SIDE) || (TargetType == BLACK_KING && Side == RED_SIDE) )
								return FAIL_VALUE;
							else
								///Fuck This:
								m_Attract[gRelatePosition[k].y][gRelatePosition[k].x] += 
								(30 + (m_BaseValue[ToNoSide(TargetType) - 1] - m_BaseValue[ToNoSide(ChessType) - 1]) / 10) / 10;
						}
					}
				}
			}
		}
	}

	INT          nHalfValue;

	CHESSSIDE    ChessSide;
	CHESSMANTYPE King;

	INT          nRedValue = 0, nBlackValue = 0;
	for(i = 0; i < CHESS_WIDTH; i++)
	{
		for(j = 0; j < CHESS_HEIGHT; j++)
		{
			if(ChessBoard[j][i] != NO_CHESS)
			{
				ChessType = ChessBoard[j][i];

				///PartI
				m_ChessValue[j][i] ++;
				m_ChessValue[j][i] += m_FlexValue[ToNoSide(ChessType) - 1] * m_Flexibility[j][i];
				m_ChessValue[j][i] += GetPawnAdditionalValue(i, j, ChessBoard);

				///PartII
				nHalfValue = m_BaseValue[ToNoSide(ChessType) - 1];

				m_ChessValue[j][i] += nHalfValue;

				///Fuck This:
				nHalfValue /= 16;

				ChessSide = CHESSSIDE(            ChessType >> SIDE_SHIFT  );

				if(m_Attract[j][i])
				{
					King = CHESSMANTYPE( KING + (ChessSide << SIDE_SHIFT) );

					if(Side == ChessSide)
					{
						///Fuck This:
						m_ChessValue[j][i] += ChessType == King ? -20 : -(nHalfValue * 2);
						if(m_Guard[j][i])
							m_ChessValue[j][i] += nHalfValue;
					}
					else
					{
						if(ChessType == King)
							return FAIL_VALUE;

						///Fuck This:
						m_ChessValue[j][i] -= m_Guard[j][i] ? nHalfValue : nHalfValue * 10;
					}

					m_ChessValue[j][i] += m_Attract[j][i];
				}
				else if(m_Guard[j][i])
					//And Fuck This:
					m_ChessValue[j][i] += 5;

				///PartIII:
				if(ChessSide == RED_SIDE)
					nRedValue   += m_ChessValue[j][i];
				else
					nBlackValue += m_ChessValue[j][i];
			}
		}
	}

	return Side == RED_SIDE ? (nRedValue - nBlackValue) : (nBlackValue - nRedValue);

	/*INT nHalfValue;
	for(i = 0; i < CHESS_WIDTH; i++)
	{
		for(j = 0; j < CHESS_HEIGHT; j++)
		{
			if(ChessBoard[j][i] != NO_CHESS)
			{
				ChessType = ChessBoard[j][i];

				nHalfValue = m_BaseValue[ToNoSide(ChessType) - 1];

				m_ChessValue[j][i] += nHalfValue;

				///Fuck This:
				nHalfValue /= 16;

				if(m_Attract[j][i])
				{
					CHESSSIDE    ChessSide = CHESSSIDE(            ChessType >> SIDE_SHIFT  );
					CHESSMANTYPE King      = CHESSMANTYPE( KING + (ChessSide << SIDE_SHIFT) );

					if(Side == ChessSide)
					{
						///Fuck This:
						m_ChessValue[j][i] += ChessType == King ? -20 : -(nHalfValue * 2);
						if(m_Guard[j][i])
							m_ChessValue[j][i] += nHalfValue;
					}
					else
					{
						if(ChessType == King)
							return FAIL_VALUE;

						///Fuck This:
						m_ChessValue[j][i] -= m_Guard[j][i] ? nHalfValue : nHalfValue * 10;
					}

					m_ChessValue[j][i] += m_Attract[j][i];
				}
				else if(m_Guard[j][i])
					//And Fuck This:
					m_ChessValue[j][i] += 5;
			}
		}
	}*/
}

///可合并
INT CEveluation::GetRelatePiece(BASECHESSBOARD& ChessBoard, INT x, INT y)
{
	m_nPositionCount = 0;
	
	INT i, j;
	switch(ChessBoard[y][x])
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
		for(i = x + 1, j = y; i < CHESS_WIDTH; i++)
		{
			if(NO_CHESS == ChessBoard[j][i])
				AddPoint(i, j);
			else
			{
				if( !IsSameSide(ChessBoard[y][x],ChessBoard[j][i]) )
					AddPoint(i, j);
				break;
			}
		}

		for(i = x - 1, j = y; i >= 0         ; i--)
		{
			if(NO_CHESS == ChessBoard[j][i])
				AddPoint(i, j);
			else
			{
				if( !IsSameSide(ChessBoard[y][x],ChessBoard[j][i]) )
					AddPoint(i, j);
				break;
			}
		}

		for(i = x   , j = y + 1; j < CHESS_WIDTH; j++)
		{
			if(NO_CHESS == ChessBoard[j][i])
				AddPoint(i, j);
			else
			{
				if( !IsSameSide(ChessBoard[y][x],ChessBoard[j][i]) )
					AddPoint(i, j);
				break;
			}
		}

		for(i = x    , j = y - 1; i >= 0        ; j--)
		{
			if(NO_CHESS == ChessBoard[j][i])
				AddPoint(i, j);
			else
			{
				if( !IsSameSide(ChessBoard[y][x],ChessBoard[j][i]) )
					AddPoint(i, j);
				break;
			}
		}
		break;
	case RED_HORSE:
	case BLACK_HORSE:
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
		if( i >= 0          && j >= 0           && CanTouch(ChessBoard, x, y, i, j) )
			AddPoint(i, j);

		i = x + 1;
		j = y + 2;
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

		bNoChess = true;
		for(i = x + 1, j = y; i < CHESS_WIDTH; i++)
		{
			if(NO_CHESS == ChessBoard[j][i])
			{
				if(!bNoChess)
					AddPoint(i, j);
			}
			else
			{
				if(bNoChess)
					bNoChess = false;
				else
				{
					if( !IsSameSide(ChessBoard[y][x],ChessBoard[j][i]) )
						AddPoint(i, j);
					break;
				}
			}
		}

		bNoChess = true;
		for(i = x - 1, j = y; i >= 0         ; i--)
		{
			if(NO_CHESS == ChessBoard[j][i])
			{
				if(!bNoChess)
					AddPoint(i, j);
			}
			else
			{
				if(bNoChess)
					bNoChess = false;
				else
				{
					if( !IsSameSide(ChessBoard[y][x],ChessBoard[j][i]) )
						AddPoint(i, j);
					break;
				}
			}
		}

		bNoChess = true;
		for(i = x   , j = y + 1; j < CHESS_WIDTH; j++)
		{
			if(NO_CHESS == ChessBoard[j][i])
			{
				if(!bNoChess)
					AddPoint(i, j);
			}
			else
			{
				if(bNoChess)
					bNoChess = false;
				else
				{
					if( !IsSameSide(ChessBoard[y][x],ChessBoard[j][i]) )
						AddPoint(i, j);
					break;
				}
			}
		}

		bNoChess = true;
		for(i = x    , j = y - 1; i >= 0        ; j--)
		{
			if(NO_CHESS == ChessBoard[j][i])
			{
				if(!bNoChess)
					AddPoint(i, j);
			}
			else
			{
				if(bNoChess)
					bNoChess = false;
				else
				{
					if( !IsSameSide(ChessBoard[y][x],ChessBoard[j][i]) )
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
		if( j > 0 && IsSameSide(ChessBoard[y][x], ChessBoard[j][i]) )
			AddPoint(i, j);

		if(y < 5)
		{
			j = y;

			i = x - 1;
			if( i > 0           && IsSameSide(ChessBoard[y][x], ChessBoard[j][i]) )
				AddPoint(i, j);
			i = x + 1;
			if( i < CHESS_WIDTH && IsSameSide(ChessBoard[y][x], ChessBoard[j][i]) )
				AddPoint(i, j);
		}
		break;
	case BLACK_PAWN:
		i = x;
		j = y + 1;
		if( j < CHESS_HEIGHT && IsSameSide(ChessBoard[y][x], ChessBoard[j][i]) )
			AddPoint(i, j);

		if(y > 4)
		{
			j = y;

			i = x - 1;
			if( i > 0           && IsSameSide(ChessBoard[y][x], ChessBoard[j][i]) )
				AddPoint(i, j);
			i = x + 1;
			if( i < CHESS_WIDTH && IsSameSide(ChessBoard[y][x], ChessBoard[j][i]) )
				AddPoint(i, j);
		}
		break;

	default:
		break;
	}
	/*INT i, j;
	switch(ChessBoard[y][x])
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
			if(NO_CHESS == ChessBoard[j][i])
				AddPoint(i, j);
			else
			{
				AddPoint(i, j);
				break;
			}
		}

		for(i = x - 1, j = y    ; i < CHESS_WIDTH; i--)
		{
			if(NO_CHESS == ChessBoard[j][i])
				AddPoint(i, j);
			else
			{
				AddPoint(i, j);
				break;
			}
		}

		for(i = x    , j = y + 1; j < CHESS_HEIGHT; j++)
		{
			if(NO_CHESS == ChessBoard[j][i])
				AddPoint(i, j);
			else
			{
				AddPoint(i, j);
				break;
			}
		}

		for(i = x    , j = y - 1; j < CHESS_HEIGHT; j++)
		{
			if(NO_CHESS == ChessBoard[j][i])
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
			if(NO_CHESS == ChessBoard[j][i])
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
	}*/

	return m_nPositionCount;
}

BOOL CEveluation::CanTouch(BASECHESSBOARD& ChessBoard, INT nFromX, INT nFromY, INT nToX, INT nToY)
{
	if(nFromX == nToX && nFromY == nToY)
	return FALSE;

	INT  i, j, nIncrease;

	CHESSMANTYPE From = ChessBoard[nFromY][nFromX], To = ChessBoard[nToY][nToX];

	switch(From)
	{
	case RED_KING:
		if(To == BLACK_KING)
		{
			if(nFromX != nToX)
				return FALSE;

			for(j = nFromY; j < nToY; j--)
				if(ChessBoard[j][nFromX] != NO_CHESS)
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
				if(ChessBoard[j][nFromX] != NO_CHESS)
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
		if(ChessBoard[(nFromY + nToY) / 2][(nFromX + nToX) / 2] != NO_CHESS)
			return FALSE;
		break;
	case BLACK_ELEPHANT:
		if(nToY > 4)
			return FALSE;
		if(abs( (nToX - nFromX) ) != 2 || abs( (nToY - nFromY) ) != 2)
			return FALSE;
		if(ChessBoard[(nFromY + nToY) / 2][(nFromX + nToX) / 2] != NO_CHESS)
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
			for(j = nFromY + 1; j < nToY; j+=nIncrease)
				if(ChessBoard[j][nFromX] != NO_CHESS)
					return FALSE;
		}
		else
		{
			nIncrease = nFromX < nToX ? 1 : -1;
			for(i = nFromX + 1; i < nToX; i+=nIncrease)
				if(ChessBoard[nFromY][i] != NO_CHESS)
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

		if(ChessBoard[j][i] != NO_CHESS)
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
				for(j = nFromY + 1; j < nToY; j+=nIncrease)
					if(ChessBoard[j][nFromX] != NO_CHESS)
						return FALSE;
			}
			else
			{
				nIncrease = nFromX < nToX ? 1 : -1;
				for(i = nFromX + 1; i < nToX; i+=nIncrease)
					if(ChessBoard[nFromY][i] != NO_CHESS)
						return FALSE;
			}
		}
		else
		{
			BOOL bNoChess = TRUE;

			if(nFromX == nToX)
			{
				nIncrease = nFromY < nToY ? 1 : -1;
				for(j = nFromY; j < nToY; j += nIncrease)
					if(ChessBoard[j][nFromX] != NO_CHESS)
						if(!bNoChess)
							return FALSE;
						else
							bNoChess = FALSE;
			}
			else
			{
				nIncrease = nFromX < nToX ? 1 : -1;
				for(i = nFromX; i < nToX; i += nIncrease)
					if(ChessBoard[nFromY][i] != NO_CHESS)
						if(!bNoChess)
							return FALSE;
						else
							bNoChess = FALSE;
			}
		}
		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
}

VOID CEveluation::AddPoint(INT x, INT y)
{
	gRelatePosition[m_nPositionCount].x = x;
	gRelatePosition[m_nPositionCount].y = y;

	m_nPositionCount ++;
}
#include "StdAfx.h"
#include "ChessRules.h"

CChessRules::CChessRules(void)
{
}

CChessRules::~CChessRules(void)
{
}

VOID CChessRules::GetMoves(BOARD CHESS& ChessBoard, const INT x, const INT y, const BOOL bAttack)
{
	INT i, j;
	CHESSMANTYPE From = ChessBoard.gPosition[y][x];

	switch(From)
	{
	case RED_KING:
	case BLACK_KING:
		///¿É¸Ä½ø
		for(i = 3; i < 6; i++)
		{
			for(j = 0; j < 3; j++)
				if( TestMove(ChessBoard, x, y, i, j, bAttack) )
					AddMove(x, y, i, j);
			for(j = 7; j < 10; j++)
				if( TestMove(ChessBoard, x, y, i, j, bAttack) )
					AddMove(x, y, i, j);
		}
		break;
	case RED_CAR:
	case BLACK_CAR:
		for(i = x + 1, j = y; i < CHESS_WIDTH   ; i++)
		{
			if(NO_CHESS == ChessBoard.gPosition[j][i])
				AddMove(x, y, i, j);
			else
			{
				if( !( bAttack && IsSameSide(From,ChessBoard.gPosition[j][i]) ) )
					AddMove(x, y, i, j);
				break;
			}
		}

		for(i = x - 1, j = y; i >= 0            ; i--)
		{
			if(NO_CHESS == ChessBoard.gPosition[j][i])
				AddMove(x, y, i, j);
			else
			{
				if( !( bAttack && IsSameSide(From,ChessBoard.gPosition[j][i]) ) )
					AddMove(x, y, i, j);
				break;
			}
		}

		for(i = x   , j = y + 1; j < CHESS_HEIGHT; j++)
		{
			if(NO_CHESS == ChessBoard.gPosition[j][i])
				AddMove(x, y, i, j);
			else
			{
				if( !( bAttack && IsSameSide(From,ChessBoard.gPosition[j][i]) ) )
					AddMove(x, y, i, j);
				break;
			}
		}

		for(i = x    , j = y - 1; j >= 0        ; j--)
		{
			if(NO_CHESS == ChessBoard.gPosition[j][i])
				AddMove(x, y, i, j);
			else
			{
				if( !( bAttack && IsSameSide(From,ChessBoard.gPosition[j][i]) ) )
					AddMove(x, y, i, j);
				break;
			}
		}
		break;
	case RED_HORSE:
	case BLACK_HORSE:
		i = x + 1;
		j = y + 2;
		///IsValueMove needn`t
		if( i < CHESS_WIDTH && j < CHESS_HEIGHT && TestMove(ChessBoard, x, y, i, j, bAttack) )
			AddMove(x, y, i, j);

		i = x + 1;
		j = y - 2;
		if( i < CHESS_WIDTH && j >= 0           && TestMove(ChessBoard, x, y, i, j, bAttack) )
			AddMove(x, y, i, j);

		i = x - 1;
		j = y + 2;
		if( i >= 0          && j < CHESS_HEIGHT && TestMove(ChessBoard, x, y, i, j, bAttack) )
			AddMove(x, y, i, j);

		i = x - 1;
		j = y - 2;
		if( i >= 0          && j >= 0           && TestMove(ChessBoard, x, y, i, j, bAttack) )
			AddMove(x, y, i, j);

		i = x + 2;
		j = y + 1;
		if( i < CHESS_WIDTH && j < CHESS_HEIGHT && TestMove(ChessBoard, x, y, i, j, bAttack) )
			AddMove(x, y, i, j);

		i = x + 2;
		j = y - 1;
		if( i < CHESS_WIDTH && j >= 0           && TestMove(ChessBoard, x, y, i, j, bAttack) )
			AddMove(x, y, i, j);

		i = x - 2;
		j = y + 1;
		if( i >= 0          && j < CHESS_HEIGHT && TestMove(ChessBoard, x, y, i, j, bAttack) )
			AddMove(x, y, i, j);

		i = x - 2;
		j = y - 1;
		if( i >= 0          && j >= 0           && TestMove(ChessBoard, x, y, i, j, bAttack) )
			AddMove(x, y, i, j);
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
					AddMove(x, y, i, j);
			}
			else
			{
				if(bNoChess)
					bNoChess = FALSE;
				else
				{
					if( !( bAttack && IsSameSide(From,ChessBoard.gPosition[j][i]) ) )
						AddMove(x, y, i, j);
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
					AddMove(x, y, i, j);
			}
			else
			{
				if(bNoChess)
					bNoChess = FALSE;
				else
				{
					if( !( bAttack && IsSameSide(From,ChessBoard.gPosition[j][i]) ) )
						AddMove(x, y, i, j);
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
					AddMove(x, y, i, j);
			}
			else
			{
				if(bNoChess)
					bNoChess = FALSE;
				else
				{
					if( !( bAttack && IsSameSide(From,ChessBoard.gPosition[j][i]) ) )
						AddMove(x, y, i, j);
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
					AddMove(x, y, i, j);
			}
			else
			{
				if(bNoChess)
					bNoChess = FALSE;
				else
				{
					if( !( bAttack && IsSameSide(From,ChessBoard.gPosition[j][i]) ) )
						AddMove(x, y, i, j);
					break;
				}
			}
		}
		break;
	case RED_ELEPHANT:
	case BLACK_ELEPHANT:
		i = x + 2;
		j = y + 2;
		if( i < CHESS_WIDTH && j < CHESS_HEIGHT && TestMove(ChessBoard, x, y, i, j, bAttack) )
			AddMove(x, y, i, j);

		i = x + 2;
		j = y - 2;
		if( i < CHESS_WIDTH && j >= 0           && TestMove(ChessBoard, x, y, i, j, bAttack) )
			AddMove(x, y, i, j);

		i = x - 2;
		j = y + 2;
		if( i >= 0          && j < CHESS_HEIGHT && TestMove(ChessBoard, x, y, i, j, bAttack) )
			AddMove(x, y, i, j);

		i = x - 2;
		j = y - 2;
		if( i >= 0          && j >= 0           && TestMove(ChessBoard, x, y, i, j, bAttack) )
			AddMove(x, y, i, j);
		break;

	case RED_BISHOP:
		for(i = 3; i < 6; i++)
		{
			for(j = 7; j < 10; j++)
				if( TestMove(ChessBoard, x, y, i, j, bAttack) )
					AddMove(x, y, i, j);
		}
		break;
	case BLACK_BISHOP:
		for(i = 3; i < 6; i++)
		{
			for(j = 0; j < 3; j++)
				if( TestMove(ChessBoard, x, y, i, j, bAttack) )
					AddMove(x, y, i, j);
		}
		break;
	case RED_PAWN:
		i = x;
		j = y - 1;
		if( j > 0               && !IsSameSide(From, ChessBoard.gPosition[j][i]) )
			AddMove(x, y, i, j);

		if(y < 5)
		{
			j = y;

			i = x - 1;
			if( i > 0           && !IsSameSide(From, ChessBoard.gPosition[j][i]) )
				AddMove(x, y, i, j);
			i = x + 1;
			if( i < CHESS_WIDTH && !IsSameSide(From, ChessBoard.gPosition[j][i]) )
				AddMove(x, y, i, j);
		}
		break;
	case BLACK_PAWN:
		i = x;
		j = y + 1;
		if( j < CHESS_HEIGHT    && !IsSameSide(From, ChessBoard.gPosition[j][i]) )
			AddMove(x, y, i, j);

		if(y > 4)
		{
			j = y;

			i = x - 1;
			if( i > 0           && !IsSameSide(From, ChessBoard.gPosition[j][i]) )
				AddMove(x, y, i, j);
			i = x + 1;
			if( i < CHESS_WIDTH && !IsSameSide(From, ChessBoard.gPosition[j][i]) )
				AddMove(x, y, i, j);
		}
		break;

	default:
		break;
	}
}

BOOL CChessRules::TestMove(BOARD CHESS &ChessBoard, const INT nFromX, const INT nFromY, const INT nToX, const INT nToY, const BOOL bAttack)
{
	if(nFromX == nToX && nFromY == nToY)
		return FALSE;

	CHESSMANTYPE From = ChessBoard.gPosition[nFromY][nFromX], To = ChessBoard.gPosition[nToY][nToX];

	if( bAttack && IsSameSide(From, To) )
		return FALSE;

	INT  i, j, nIncrease;

	switch(From)
	{
	case RED_KING:
		if(To == BLACK_KING)
		{
			if(nFromX != nToX)
				return FALSE;

			for(j = nFromY - 1; j > nToY; j--)
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

			for(j = nFromY + 1; j < nToY; j++)
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
}
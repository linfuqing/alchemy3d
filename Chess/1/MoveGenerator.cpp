#include "StdAfx.h"
#include "MoveGenerator.h"

CMoveGenerator::CMoveGenerator(void)
{
}

CMoveGenerator::~CMoveGenerator(void)
{
}

BOOL CMoveGenerator::IsValueMove(BASECHESSBOARD& ChessBoard, INT nFromX, INT nFromY, INT nToX, INT nToY)
{
	if(nFromX == nToX && nFromY == nToY)
	return FALSE;

	INT  i, j, nIncrease;

	CHESSMANTYPE From = ChessBoard.gChess[nFromY][nFromX], To = ChessBoard.gChess[nToY][nToX];

	if( IsSameSide(From, To) )
		return FALSE;

	switch(From)
	{
	case RED_KING:
		if(To == BLACK_KING)
		{
			if(nFromX != nToX)
				return FALSE;

			for(j = nFromY; j < nToY; j--)
				if(ChessBoard.gChess[j][nFromX] != NO_CHESS)
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
				if(ChessBoard.gChess[j][nFromX] != NO_CHESS)
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
		if(ChessBoard.gChess[(nFromY + nToY) / 2][(nFromX + nToX) / 2] != NO_CHESS)
			return FALSE;
		break;
	case BLACK_ELEPHANT:
		if(nToY > 4)
			return FALSE;
		if(abs( (nToX - nFromX) ) != 2 || abs( (nToY - nFromY) ) != 2)
			return FALSE;
		if(ChessBoard.gChess[(nFromY + nToY) / 2][(nFromX + nToX) / 2] != NO_CHESS)
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
				if(ChessBoard.gChess[j][nFromX] != NO_CHESS)
					return FALSE;
		}
		else
		{
			nIncrease = nFromX < nToX ? 1 : -1;
			for(i = nFromX + 1; i < nToX; i+=nIncrease)
				if(ChessBoard.gChess[nFromY][i] != NO_CHESS)
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

		if(ChessBoard.gChess[j][i] != NO_CHESS)
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
					if(ChessBoard.gChess[j][nFromX] != NO_CHESS)
						return FALSE;
			}
			else
			{
				nIncrease = nFromX < nToX ? 1 : -1;
				for(i = nFromX + 1; i < nToX; i+=nIncrease)
					if(ChessBoard.gChess[nFromY][i] != NO_CHESS)
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
					if(ChessBoard.gChess[j][nFromX] != NO_CHESS)
						if(!bNoChess)
							return FALSE;
						else
							bNoChess = FALSE;
			}
			else
			{
				nIncrease = nFromX < nToX ? 1 : -1;
				for(i = nFromX; i < nToX; i += nIncrease)
					if(ChessBoard.gChess[nFromY][i] != NO_CHESS)
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

INT CMoveGenerator::CreatePossibleMove(BASECHESSBOARD& ChessBoard, INT nPly, CHESSSIDE Side)
{
	m_nMoveCount = 0;

	for(INT i = 0; i < CHESS_WIDTH; i ++)
	{
		for(INT j = 0; j < CHESS_HEIGHT; j++)
		{
			CHESSMANTYPE Type = ChessBoard.gChess[j][i];

			if(Type != NO_CHESS)
			{
				if( ( Side == RED_SIDE && IsBlackSide(Type) ) || ( Side == BLACK_SIDE && IsRedSide(Type) ) )
					continue;

				GenerateMove(ChessBoard, i, j, nPly);
			}
		}
	}

	return m_nMoveCount;
}

INT CMoveGenerator::AddMove(INT nFromX, INT nFromY, INT nToX, INT nToY, INT nPly)
//INT CMoveGenerator::AddMove(INT nFromX, INT nFromY, INT nToX, INT nToY)
{
	LPCHESSMOVE pMove = &m_gMoveList[nPly][m_nMoveCount];
	//LPCHESSMOVE pMove = &m_gMoveList[m_nPly][m_nMoveCount];

	pMove->From.x = nFromX;
	pMove->From.y = nFromY;
	pMove->  To.x =   nToX;
	pMove->  To.y =   nToY;

	return ++m_nMoveCount;
}

void CMoveGenerator::GenerateMove(BASECHESSBOARD& ChessBoard, INT x, INT y, INT nPly)
{
	/*m_nPly = nPly;
	GetAllMove(ChessBoard, x, y, IsValueMove, AddMove);*/

	INT i, j;

	switch(ChessBoard.gChess[y][x])
	{
	case RED_KING:
	case BLACK_KING:
		///¿É¸Ä½ø
		for(i = 3; i < 6; i++)
		{
			for(j = 0; j < 3; j++)
				if( IsValueMove(ChessBoard, x, y, i, j) )
					AddMove(x, y, i, j, nPly);
			for(j = 7; j < 10; j++)
				if( IsValueMove(ChessBoard, x, y, i, j) )
					AddMove(x, y, i, j, nPly);
		}
		break;
	case RED_CAR:
	case BLACK_CAR:
		for(i = x + 1, j = y; i < CHESS_WIDTH; i++)
		{
			if(NO_CHESS == ChessBoard.gChess[j][i])
				AddMove(x, y, i, j, nPly);
			else
			{
				if( !IsSameSide(ChessBoard.gChess[y][x],ChessBoard.gChess[j][i]) )
					AddMove(x, y, i, j, nPly);
				break;
			}
		}

		for(i = x - 1, j = y; i >= 0         ; i--)
		{
			if(NO_CHESS == ChessBoard.gChess[j][i])
				AddMove(x, y, i, j, nPly);
			else
			{
				if( !IsSameSide(ChessBoard.gChess[y][x],ChessBoard.gChess[j][i]) )
					AddMove(x, y, i, j, nPly);
				break;
			}
		}

		for(i = x   , j = y + 1; j < CHESS_WIDTH; j++)
		{
			if(NO_CHESS == ChessBoard.gChess[j][i])
				AddMove(x, y, i, j, nPly);
			else
			{
				if( !IsSameSide(ChessBoard.gChess[y][x],ChessBoard.gChess[j][i]) )
					AddMove(x, y, i, j, nPly);
				break;
			}
		}

		for(i = x    , j = y - 1; i >= 0        ; j--)
		{
			if(NO_CHESS == ChessBoard.gChess[j][i])
				AddMove(x, y, i, j, nPly);
			else
			{
				if( !IsSameSide(ChessBoard.gChess[y][x],ChessBoard.gChess[j][i]) )
					AddMove(x, y, i, j, nPly);
				break;
			}
		}
		break;
	case RED_HORSE:
	case BLACK_HORSE:
		i = x + 1;
		j = y + 2;
		if( i < CHESS_WIDTH && j < CHESS_HEIGHT && IsValueMove(ChessBoard, x, y, i, j) )
			AddMove(x, y, i, j, nPly);

		i = x + 1;
		j = y - 2;
		if( i < CHESS_WIDTH && j >= 0           && IsValueMove(ChessBoard, x, y, i, j) )
			AddMove(x, y, i, j, nPly);

		i = x - 1;
		j = y + 2;
		if( i >= 0          && j < CHESS_HEIGHT && IsValueMove(ChessBoard, x, y, i, j) )
			AddMove(x, y, i, j, nPly);

		i = x - 1;
		j = y - 2;
		if( i >= 0          && j >= 0           && IsValueMove(ChessBoard, x, y, i, j) )
			AddMove(x, y, i, j, nPly);

		i = x + 1;
		j = y + 2;
		if( i < CHESS_WIDTH && j < CHESS_HEIGHT && IsValueMove(ChessBoard, x, y, i, j) )
			AddMove(x, y, i, j, nPly);

		i = x + 2;
		j = y - 1;
		if( i < CHESS_WIDTH && j >= 0           && IsValueMove(ChessBoard, x, y, i, j) )
			AddMove(x, y, i, j, nPly);

		i = x - 2;
		j = y + 1;
		if( i >= 0          && j < CHESS_HEIGHT && IsValueMove(ChessBoard, x, y, i, j) )
			AddMove(x, y, i, j, nPly);

		i = x - 2;
		j = y - 1;
		if( i >= 0          && j >= 0           && IsValueMove(ChessBoard, x, y, i, j) )
			AddMove(x, y, i, j, nPly);
		break;
	case RED_CANON:
	case BLACK_CANON:
		BOOL bNoChess;

		bNoChess = true;
		for(i = x + 1, j = y; i < CHESS_WIDTH; i++)
		{
			if(NO_CHESS == ChessBoard.gChess[j][i])
			{
				if(!bNoChess)
					AddMove(x, y, i, j, nPly);
			}
			else
			{
				if(bNoChess)
					bNoChess = false;
				else
				{
					if( !IsSameSide(ChessBoard.gChess[y][x],ChessBoard.gChess[j][i]) )
						AddMove(x, y, i, j, nPly);
					break;
				}
			}
		}

		bNoChess = true;
		for(i = x - 1, j = y; i >= 0         ; i--)
		{
			if(NO_CHESS == ChessBoard.gChess[j][i])
			{
				if(!bNoChess)
					AddMove(x, y, i, j, nPly);
			}
			else
			{
				if(bNoChess)
					bNoChess = false;
				else
				{
					if( !IsSameSide(ChessBoard.gChess[y][x],ChessBoard.gChess[j][i]) )
						AddMove(x, y, i, j, nPly);
					break;
				}
			}
		}

		bNoChess = true;
		for(i = x   , j = y + 1; j < CHESS_WIDTH; j++)
		{
			if(NO_CHESS == ChessBoard.gChess[j][i])
			{
				if(!bNoChess)
					AddMove(x, y, i, j, nPly);
			}
			else
			{
				if(bNoChess)
					bNoChess = false;
				else
				{
					if( !IsSameSide(ChessBoard.gChess[y][x],ChessBoard.gChess[j][i]) )
						AddMove(x, y, i, j, nPly);
					break;
				}
			}
		}

		bNoChess = true;
		for(i = x    , j = y - 1; i >= 0        ; j--)
		{
			if(NO_CHESS == ChessBoard.gChess[j][i])
			{
				if(!bNoChess)
					AddMove(x, y, i, j, nPly);
			}
			else
			{
				if(bNoChess)
					bNoChess = false;
				else
				{
					if( !IsSameSide(ChessBoard.gChess[y][x],ChessBoard.gChess[j][i]) )
						AddMove(x, y, i, j, nPly);
					break;
				}
			}
		}
		break;
	case RED_ELEPHANT:
	case BLACK_ELEPHANT:
		i = x + 2;
		j = y + 2;
		if( i < CHESS_WIDTH && j < CHESS_HEIGHT && IsValueMove(ChessBoard, x, y, i, j) )
			AddMove(x, y, i, j, nPly);

		i = x + 2;
		j = y - 2;
		if( i < CHESS_WIDTH && j >= 0           && IsValueMove(ChessBoard, x, y, i, j) )
			AddMove(x, y, i, j, nPly);

		i = x - 2;
		j = y + 2;
		if( i >= 0          && j < CHESS_HEIGHT && IsValueMove(ChessBoard, x, y, i, j) )
			AddMove(x, y, i, j, nPly);

		i = x - 2;
		j = y - 2;
		if( i >= 0          && j >= 0           && IsValueMove(ChessBoard, x, y, i, j) )
			AddMove(x, y, i, j, nPly);
		break;

	case RED_BISHOP:
		for(i = 3; i < 6; i++)
		{
			for(j = 7; j < 10; j++)
				if( IsValueMove(ChessBoard, x, y, i, j) )
					AddMove(x, y, i, j, nPly);
		}
		break;
	case BLACK_BISHOP:
		for(i = 3; i < 6; i++)
		{
			for(j = 0; j < 3; j++)
				if( IsValueMove(ChessBoard, x, y, i, j) )
					AddMove(x, y, i, j, nPly);
		}
		break;
	case RED_PAWN:
		i = x;
		j = y - 1;
		if( j > 0 && IsSameSide(ChessBoard.gChess[y][x], ChessBoard.gChess[j][i]) )
			AddMove(x, y, i, j, nPly);

		if(y < 5)
		{
			j = y;

			i = x - 1;
			if( i > 0           && IsSameSide(ChessBoard.gChess[y][x], ChessBoard.gChess[j][i]) )
				AddMove(x, y, i, j, nPly);
			i = x + 1;
			if( i < CHESS_WIDTH && IsSameSide(ChessBoard.gChess[y][x], ChessBoard.gChess[j][i]) )
				AddMove(x, y, i, j, nPly);
		}
		break;
	case BLACK_PAWN:
		i = x;
		j = y + 1;
		if( j < CHESS_HEIGHT && IsSameSide(ChessBoard.gChess[y][x], ChessBoard.gChess[j][i]) )
			AddMove(x, y, i, j, nPly);

		if(y > 4)
		{
			j = y;

			i = x - 1;
			if( i > 0           && IsSameSide(ChessBoard.gChess[y][x], ChessBoard.gChess[j][i]) )
				AddMove(x, y, i, j, nPly);
			i = x + 1;
			if( i < CHESS_WIDTH && IsSameSide(ChessBoard.gChess[y][x], ChessBoard.gChess[j][i]) )
				AddMove(x, y, i, j, nPly);
		}
		break;

	default:
		break;
	}
}
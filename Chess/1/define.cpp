#include "stdafx.h"

void GetAllMove(
				BASECHESSBOARD& ChessBoard, 
				INT x, INT y, 
				BOOL fnbTextMove(BASECHESSBOARD& ChessBoard, INT nFromX, INT nFormY, INT nToX, INT nToY), 
				void fnAddMove(INT nFromX, INT nFormY, INT nToX, INT nToY) )
{
	INT i, j;

	switch(ChessBoard[y][x])
	{
	case RED_KING:
	case BLACK_KING:
		///¿É¸Ä½ø
		for(i = 3; i < 6; i++)
		{
			for(j = 0; j < 3; j++)
				if( fnbTextMove(ChessBoard, x, y, i, j) )
					fnAddMove(x, y, i, j);
			for(j = 7; j < 10; j++)
				if( fnbTextMove(ChessBoard, x, y, i, j) )
					fnAddMove(x, y, i, j);
		}
		break;
	case RED_CAR:
	case BLACK_CAR:
		for(i = x + 1, j = y; i < CHESS_WIDTH; i++)
		{
			if(NO_CHESS == ChessBoard.gChess[j][i])
				fnAddMove(x, y, i, j);
			else
			{
				if( !IsSameSide(ChessBoard[y][x],ChessBoard.gChess[j][i]) )
					fnAddMove(x, y, i, j);
				break;
			}
		}

		for(i = x - 1, j = y; i >= 0         ; i--)
		{
			if(NO_CHESS == ChessBoard.gChess[j][i])
				fnAddMove(x, y, i, j);
			else
			{
				if( !IsSameSide(ChessBoard[y][x],ChessBoard.gChess[j][i]) )
					fnAddMove(x, y, i, j);
				break;
			}
		}

		for(i = x   , j = y + 1; j < CHESS_WIDTH; j++)
		{
			if(NO_CHESS == ChessBoard.gChess[j][i])
				fnAddMove(x, y, i, j);
			else
			{
				if( !IsSameSide(ChessBoard[y][x],ChessBoard.gChess[j][i]) )
					fnAddMove(x, y, i, j);
				break;
			}
		}

		for(i = x    , j = y - 1; i >= 0        ; j--)
		{
			if(NO_CHESS == ChessBoard.gChess[j][i])
				fnAddMove(x, y, i, j);
			else
			{
				if( !IsSameSide(ChessBoard[y][x],ChessBoard.gChess[j][i]) )
					fnAddMove(x, y, i, j);
				break;
			}
		}
		break;
	case RED_HORSE:
	case BLACK_HORSE:
		i = x + 1;
		j = y + 2;
		if( i < CHESS_WIDTH && j < CHESS_HEIGHT && fnbTextMove(ChessBoard, x, y, i, j) )
			fnAddMove(x, y, i, j);

		i = x + 1;
		j = y - 2;
		if( i < CHESS_WIDTH && j >= 0           && fnbTextMove(ChessBoard, x, y, i, j) )
			fnAddMove(x, y, i, j);

		i = x - 1;
		j = y + 2;
		if( i >= 0          && j < CHESS_HEIGHT && fnbTextMove(ChessBoard, x, y, i, j) )
			fnAddMove(x, y, i, j);

		i = x - 1;
		j = y - 2;
		if( i >= 0          && j >= 0           && fnbTextMove(ChessBoard, x, y, i, j) )
			fnAddMove(x, y, i, j);

		i = x + 1;
		j = y + 2;
		if( i < CHESS_WIDTH && j < CHESS_HEIGHT && fnbTextMove(ChessBoard, x, y, i, j) )
			fnAddMove(x, y, i, j);

		i = x + 2;
		j = y - 1;
		if( i < CHESS_WIDTH && j >= 0           && fnbTextMove(ChessBoard, x, y, i, j) )
			fnAddMove(x, y, i, j);

		i = x - 2;
		j = y + 1;
		if( i >= 0          && j < CHESS_HEIGHT && fnbTextMove(ChessBoard, x, y, i, j) )
			fnAddMove(x, y, i, j);

		i = x - 2;
		j = y - 1;
		if( i >= 0          && j >= 0           && fnbTextMove(ChessBoard, x, y, i, j) )
			fnAddMove(x, y, i, j);
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
					fnAddMove(x, y, i, j);
			}
			else
			{
				if(bNoChess)
					bNoChess = false;
				else
				{
					if( !IsSameSide(ChessBoard[y][x],ChessBoard.gChess[j][i]) )
						fnAddMove(x, y, i, j);
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
					fnAddMove(x, y, i, j);
			}
			else
			{
				if(bNoChess)
					bNoChess = false;
				else
				{
					if( !IsSameSide(ChessBoard[y][x],ChessBoard.gChess[j][i]) )
						fnAddMove(x, y, i, j);
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
					fnAddMove(x, y, i, j);
			}
			else
			{
				if(bNoChess)
					bNoChess = false;
				else
				{
					if( !IsSameSide(ChessBoard[y][x],ChessBoard.gChess[j][i]) )
						fnAddMove(x, y, i, j);
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
					fnAddMove(x, y, i, j);
			}
			else
			{
				if(bNoChess)
					bNoChess = false;
				else
				{
					if( !IsSameSide(ChessBoard[y][x],ChessBoard.gChess[j][i]) )
						fnAddMove(x, y, i, j);
					break;
				}
			}
		}
		break;
	case RED_ELEPHANT:
	case BLACK_ELEPHANT:
		i = x + 2;
		j = y + 2;
		if( i < CHESS_WIDTH && j < CHESS_HEIGHT && fnbTextMove(ChessBoard, x, y, i, j) )
			fnAddMove(x, y, i, j);

		i = x + 2;
		j = y - 2;
		if( i < CHESS_WIDTH && j >= 0           && fnbTextMove(ChessBoard, x, y, i, j) )
			fnAddMove(x, y, i, j);

		i = x - 2;
		j = y + 2;
		if( i >= 0          && j < CHESS_HEIGHT && fnbTextMove(ChessBoard, x, y, i, j) )
			fnAddMove(x, y, i, j);

		i = x - 2;
		j = y - 2;
		if( i >= 0          && j >= 0           && fnbTextMove(ChessBoard, x, y, i, j) )
			fnAddMove(x, y, i, j);
		break;

	case RED_BISHOP:
		for(i = 3; i < 6; i++)
		{
			for(j = 7; j < 10; j++)
				if( fnbTextMove(ChessBoard, x, y, i, j) )
					fnAddMove(x, y, i, j);
		}
		break;
	case BLACK_BISHOP:
		for(i = 3; i < 6; i++)
		{
			for(j = 0; j < 3; j++)
				if( fnbTextMove(ChessBoard, x, y, i, j) )
					fnAddMove(x, y, i, j);
		}
		break;
	case RED_PAWN:
		i = x;
		j = y - 1;
		if( j > 0 && IsSameSide(ChessBoard[y][x], ChessBoard.gChess[j][i]) )
			fnAddMove(x, y, i, j);

		if(y < 5)
		{
			j = y;

			i = x - 1;
			if( i > 0           && IsSameSide(ChessBoard[y][x], ChessBoard.gChess[j][i]) )
				fnAddMove(x, y, i, j);
			i = x + 1;
			if( i < CHESS_WIDTH && IsSameSide(ChessBoard[y][x], ChessBoard.gChess[j][i]) )
				fnAddMove(x, y, i, j);
		}
		break;
	case BLACK_PAWN:
		i = x;
		j = y + 1;
		if( j < CHESS_HEIGHT && IsSameSide(ChessBoard[y][x], ChessBoard.gChess[j][i]) )
			fnAddMove(x, y, i, j);

		if(y > 4)
		{
			j = y;

			i = x - 1;
			if( i > 0           && IsSameSide(ChessBoard[y][x], ChessBoard.gChess[j][i]) )
				fnAddMove(x, y, i, j);
			i = x + 1;
			if( i < CHESS_WIDTH && IsSameSide(ChessBoard[y][x], ChessBoard.gChess[j][i]) )
				fnAddMove(x, y, i, j);
		}
		break;

	default:
		break;
	}
}
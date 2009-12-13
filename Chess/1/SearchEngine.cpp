#include "StdAfx.h"
#include "SearchEngine.h"

CSearchEngine::CSearchEngine(void)
{
}

CSearchEngine::~CSearchEngine(void)
{
	delete m_pEveluation;
	delete m_pMoveGenerator;
}

CHESSMANTYPE CSearchEngine::MakeMove(LPCHESSMOVE pMove)
{
	CHESSMANTYPE Type = m_ChessBoard.gChess[pMove->To.y][pMove->To.x];

	m_ChessBoard.gChess[pMove->To.y][pMove->To.x]     = m_ChessBoard.gChess[pMove->From.y][pMove->From.x];
	m_ChessBoard.gChess[pMove->From.y][pMove->From.x] = NO_CHESS;

	return Type;
}

void CSearchEngine::UnMakeMove(LPCHESSMOVE pMove, CHESSMANTYPE Type)
{
	m_ChessBoard.gChess[pMove->From.y][pMove->From.x] = m_ChessBoard.gChess[pMove->To.y][pMove->To.x];
	m_ChessBoard.gChess[pMove->To.y  ][pMove->To.x  ] = Type;
}

//////////////////////////////////////G_CHESSBOARD
INT CSearchEngine::IsGameOver(BASECHESSBOARD &ChessBoard, INT nDepth)
{
	INT i, j;
	BOOL RedLive = FALSE, BlackLive = FALSE;

	for(i = 3; i < 6; i++)
	{
		for(j = 7; j < 10; j++)
		{
			if(ChessBoard[j][i] == RED_KING)
				RedLive   = TRUE;
			if(ChessBoard[j][i] == BLACK_KING)
				BlackLive = TRUE;
		}

		for(j = 0; j < 3 ; j++)
		{
			if(ChessBoard[j][i] == RED_KING)
				RedLive   = TRUE;
			if(ChessBoard[j][i] == BLACK_KING)
				BlackLive = TRUE;
		}
	}

	i = (m_nMaxDepth - nDepth + 1) % 2;

	if(!RedLive)
		return i ? ( MAX_VALUE + nDepth) : (-MAX_VALUE - nDepth);
	if(!BlackLive)
		return i ? (-MAX_VALUE - nDepth) : ( MAX_VALUE + nDepth);

	return 0;
}
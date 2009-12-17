#include "StdAfx.h"
#include "ChessMoveGenerator.h"

CChessMoveGenerator::CChessMoveGenerator(void)
{
}

CChessMoveGenerator::~CChessMoveGenerator(void)
{
}

VOID CChessMoveGenerator::CreatePossibleMove(BOARD CHESS& ChessBoard, CHESSSIDE Side)
{
//	m_nMoveCount = 0;
//	m_nPly       = nPly;
////////////////////////////////
	for(INT i = 0; i < CHESS_WIDTH; i ++)
		for(INT j = 0; j < CHESS_HEIGHT; j++)
			if( IsSide(Side, ChessBoard.gPosition[j][i]) )
				//GenerateMove(ChessBoard, i, j, nPly);
				CChessRules::GetMoves(ChessBoard, i, j, TRUE);
}
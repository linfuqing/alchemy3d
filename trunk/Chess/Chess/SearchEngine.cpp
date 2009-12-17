#include "StdAfx.h"
#include "SearchEngine.h"

/*template<typename TYPE, UINT WIDTH, UINT HEIGHT>
CSearchEngine<TYPE, WIDTH, HEIGHT>::CSearchEngine(void)
{
}

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
CSearchEngine<TYPE, WIDTH, HEIGHT>::~CSearchEngine(void)
{
	delete m_pEveluation;
	delete m_pMoveGenerator;
}*/

/*INT CSearchEngine::IsGameOver(BASECHESSBOARD &ChessBoard, const INT nDepth, const SEARCHSIDE Side)
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

	i = (m_nMaxDepth - nDepth + 1) % 2;

	if(Side == RED_SIDE)
	{
		if(!RedLive)
			return i ? (-MAX_VALUE - nDepth) : ( MAX_VALUE + nDepth);
		if(!BlackLive)
			return i ? ( MAX_VALUE + nDepth) : (-MAX_VALUE - nDepth);
	}
	else if(Side == BLACK_SIDE)
	{
		if(!RedLive)
			return i ? ( MAX_VALUE + nDepth) : (-MAX_VALUE - nDepth);
		if(!BlackLive)
			return i ? (-MAX_VALUE - nDepth) : ( MAX_VALUE + nDepth);
	}

	return 0;
}*/
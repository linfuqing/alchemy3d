#pragma once

//#include "ChessRules.h"

#define MAX_PLY   8
#define MAX_MOVE  80

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
class CNegamaxEngine;
template<typename TYPE, UINT WIDTH, UINT HEIGHT>
class CAlphaBetaEngine;

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
class CMoveGenerator// :
//	public CChessRules
{
	friend class CNegamaxEngine<TYPE, WIDTH, HEIGHT>;
	friend class CAlphaBetaEngine<TYPE, WIDTH, HEIGHT>;
	//friend class CAlphaBetaEngine;
public:
	CMoveGenerator(void);
	~CMoveGenerator(void);

	//static BOOL IsValueMove(BASECHESSBOARD& Board, INT nFromX, INT nFromY, INT nToX, INT nToY);

	virtual VOID CreatePossibleMove(BOARD<TYPE, WIDTH, HEIGHT>& Board, SEARCHSIDE Side) = 0;
protected:
	SEARCHMOVE m_gMoveList[MAX_PLY][MAX_MOVE];

	//INT AddMove(INT nFromX, INT nFromY, INT nToX, INT nToY, INT nPly);
	inline virtual VOID AddMove(const INT nFromX, const INT nFromY, const INT nToX, const INT nToY);


	//void GenerateMove(BASECHESSBOARD& Board, INT x, INT y, INT nPly);
protected:
	INT m_nMoveCount;
	INT m_nPly;
};

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
inline VOID CMoveGenerator<TYPE, WIDTH, HEIGHT>::AddMove(const INT nFromX, const INT nFromY, const INT nToX, const INT nToY)
{
	//LPSEARCHMOVE pMove = &m_gMoveList[nPly][m_nMoveCount];
	LPSEARCHMOVE pMove = &m_gMoveList[m_nPly][m_nMoveCount];

	pMove->From.x = nFromX;
	pMove->From.y = nFromY;
	pMove->  To.x =   nToX;
	pMove->  To.y =   nToY;

	m_nMoveCount++;
}

///-------------------------------------------------------------------------------------------------------------------------
template<typename TYPE, UINT WIDTH, UINT HEIGHT>
CMoveGenerator<TYPE, WIDTH, HEIGHT>::CMoveGenerator(void)
{
}

template<typename TYPE, UINT WIDTH, UINT HEIGHT>
CMoveGenerator<TYPE, WIDTH, HEIGHT>::~CMoveGenerator(void)
{
}

/*template<typename TYPE, UINT WIDTH, UINT HEIGHT>
INT CMoveGenerator<TYPE, WIDTH, HEIGHT>::CreatePossibleMove(BOARD<TYPE, WIDTH, HEIGHT>& Board, INT nPly, SEARCHSIDE Side)
{
	m_nMoveCount = 0;
	m_nPly       = nPly;

	for(INT i = 0; i < CHESS_WIDTH; i ++)
		for(INT j = 0; j < CHESS_HEIGHT; j++)
			if( IsSide(Side, Board.gPosition[j][i]) )
				//GenerateMove(Board, i, j, nPly);
				CChessRules::GetMoves(Board, i, j, TRUE);

	return m_nMoveCount;
}*/
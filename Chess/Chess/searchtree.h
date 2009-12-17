#pragma once
  
template<typename TYPE, UINT WIDTH, UINT HEIGHT>
struct BOARD
{
	TYPE gPosition[HEIGHT][WIDTH];
	inline void SetBoard(const TYPE gPosition[HEIGHT][WIDTH])
	{
		memcpy( this->gPosition, gPosition, sizeof(TYPE[HEIGHT][WIDTH]) );
	}

	inline void SetBoard(BOARD<TYPE, WIDTH, HEIGHT>* const pChessBoard)
	{
		SetBoard(pChessBoard->gPosition);
	}
};

typedef enum
{
	POSITIVE_SIDE = TRUE,
	NEGATIVE_SIDE = FALSE
}SEARCHSIDE;

typedef struct
{
	INT x;
	INT y;
}SEARCHPOSITION, * LPSEARCHPOSITION;

typedef struct
{
	//CHESSMANTYPE     Type;
	SEARCHPOSITION From;
	SEARCHPOSITION To;
	//INT              nScore;
}SEARCHMOVE, * LPSEARCHMOVE;

#include "NegamaxEngine.h"
#include "AlphaBetaEngine.h"
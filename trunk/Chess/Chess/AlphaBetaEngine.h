#pragma once
#include "searchengine.h"

class CAlphaBetaEngine :
	public CSearchEngine
{
public:
	CAlphaBetaEngine(void);
	~CAlphaBetaEngine(void);

	virtual void SearchAGoodMove(BASECHESSBOARD& ChessBoard, const CHESSSIDE Side = BLACK_SIDE);
private:
	INT __AlphaBeta(INT nDepth, INT nAlpha, INT nBeta,const CHESSSIDE Side);
};

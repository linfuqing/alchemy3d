#pragma once
#include "searchengine.h"

class CNegamaxEngine :
	public CSearchEngine
{
public:
	CNegamaxEngine(void);
	~CNegamaxEngine(void);

	virtual void SearchAGoodMove(BASECHESSBOARD& ChessBoard, const CHESSSIDE Side = BLACK_SIDE);
private:
	INT __Negamax(const INT nDepth, const CHESSSIDE Side);
};

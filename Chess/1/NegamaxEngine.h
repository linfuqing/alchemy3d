#pragma once
#include "searchengine.h"

class CNegamaxEngine :
	public CSearchEngine
{
public:
	CNegamaxEngine(void);
	~CNegamaxEngine(void);

	virtual void SearchAGoodMove(BASECHESSBOARD& ChessBoard);
protected:
	INT Negamax(INT nDepth);
};

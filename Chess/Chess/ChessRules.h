#pragma once

class CChessRules
{
public:
	CChessRules(void);
	~CChessRules(void);

	static BOOL TestMove(BOARD CHESS& ChessBoard, const INT nFromX, const INT nFromY, const INT nToX, const INT nToY, const BOOL bAttack);
protected:
	VOID GetMoves(BOARD CHESS& ChessBoard, const INT x, const INT y, const BOOL bAttack);

	inline virtual VOID AddMove(const INT nFromX, const INT nFromY, const INT nToX, const INT nToY) = 0;
};

#pragma once
//#include "movegenerator.h"
#include "ChessRules.h"
class CChessMoveGenerator :
	public CMoveGenerator CHESS, public CChessRules
{
public:
	CChessMoveGenerator(void);
	~CChessMoveGenerator(void);

	virtual VOID CreatePossibleMove(BOARD CHESS& ChessBoard, CHESSSIDE Side);
protected:
	inline virtual VOID AddMove(const INT nFromX, const INT nFromY, const INT nToX, const INT nToY);
};

inline VOID CChessMoveGenerator::AddMove(const INT nFromX, const INT nFromY, const INT nToX, const INT nToY)
{
	//CMoveGenerator<CHESSMANTYPE, CHESS_WIDTH, CHESS_HEIGHT>::AddMove(nFromX, nFromY, nToX, nToY);
	LPCHESSMOVE pMove = &m_gMoveList[m_nPly][m_nMoveCount];

	pMove->From.x = nFromX;
	pMove->From.y = nFromY;
	pMove->  To.x =   nToX;
	pMove->  To.y =   nToY;

	m_nMoveCount++;
}
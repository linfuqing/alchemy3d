#pragma once

#define MAX_PLY   8
#define MAX_MOVE  80

class CMoveGenerator
{
public:
	CMoveGenerator(void);
	~CMoveGenerator(void);

	static BOOL IsValueMove(BASECHESSBOARD& ChessBoard, INT nFromX, INT nFromY, INT nToX, INT nToY);

	INT CreatePossibleMove(BASECHESSBOARD& ChessBoard, INT nPly, CHESSSIDE Side);

	CHESSMOVE m_gMoveList[MAX_PLY][MAX_MOVE];
protected:
	INT AddMove(INT nFromX, INT nFromY, INT nToX, INT nToY, INT nPly);
	//INT AddMove(INT nFromX, INT nFromY, INT nToX, INT nToY);


	void GenerateMove(BASECHESSBOARD& ChessBoard, INT x, INT y, INT nPly);
protected:
	INT m_nMoveCount;
/*private:
	INT m_nPly;*/
};
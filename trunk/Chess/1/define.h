#pragma once
  
#define CHESS_WIDTH  9
#define CHESS_HEIGHT 10

template<typename T>
struct CHESSBOARD
{
	T gChess[CHESS_HEIGHT][CHESS_WIDTH];

	inline T* operator[](INT n)
	{
		return gChess[n];
	}
};

//typedef CHESSBOARD * LPCHESSBOARD;
typedef enum
{
	RED_SIDE   = 0,
	BLACK_SIDE = 1
}CHESSSIDE;

#define SIDE_SHIFT 3

#define KING     1
#define CAR      2
#define HORSE    3
#define CANON    4
#define BISHOP   5
#define ELEPHANT 6
#define PAWN     7

typedef enum
{
	NO_CHESS       = 0, //��

	///����ʮ���Ʊ�ʾ˧����Ϊ1-7,ת��Ϊ������Ϊ001-111, ��߶�һλ�������,����˧��001,��˧��0001, �ڽ���1001.

	RED_KING       = KING     + (  RED_SIDE << SIDE_SHIFT), //��˧
	RED_CAR        = CAR      + (  RED_SIDE << SIDE_SHIFT), //�쳵
	RED_HORSE      = HORSE    + (  RED_SIDE << SIDE_SHIFT), //����
	RED_CANON      = CANON    + (  RED_SIDE << SIDE_SHIFT), //����
	RED_BISHOP     = BISHOP   + (  RED_SIDE << SIDE_SHIFT), //��ʿ
	RED_ELEPHANT   = ELEPHANT + (  RED_SIDE << SIDE_SHIFT), //����
	RED_PAWN       = PAWN     + (  RED_SIDE << SIDE_SHIFT), //���

	BLACK_KING     = KING     + (BLACK_SIDE << SIDE_SHIFT), //�ڽ�
	BLACK_CAR      = CAR      + (BLACK_SIDE << SIDE_SHIFT), //�ڳ�
	BLACK_HORSE    = HORSE    + (BLACK_SIDE << SIDE_SHIFT), //����
	BLACK_CANON    = CANON    + (BLACK_SIDE << SIDE_SHIFT), //����
	BLACK_BISHOP   = BISHOP   + (BLACK_SIDE << SIDE_SHIFT), //��ʿ
	BLACK_ELEPHANT = ELEPHANT + (BLACK_SIDE << SIDE_SHIFT), //����
	BLACK_PAWN     = PAWN     + (BLACK_SIDE << SIDE_SHIFT)  //�ڱ�
}CHESSMANTYPE;

typedef CHESSBOARD<CHESSMANTYPE> BASECHESSBOARD;

inline BOOL IsRedSide(CHESSMANTYPE Type)
{
	return Type && Type >> SIDE_SHIFT;
}

inline BOOL IsBlackSide(CHESSMANTYPE Type)
{
	return Type && !(Type >> SIDE_SHIFT);
}

inline BOOL IsSameSide(CHESSMANTYPE Type1, CHESSMANTYPE Type2)
{
	return Type1 && Type2 && Type1 >> SIDE_SHIFT == Type2 >> SIDE_SHIFT;
}

inline INT ToNoSide(CHESSMANTYPE Type)
{
	return Type - (Type >> SIDE_SHIFT << SIDE_SHIFT);
}

typedef struct
{
	INT x;
	INT y;
}CHESSMANPOSITION, * LPCHESSMANPOSITION;

typedef struct
{
	CHESSMANTYPE     Type;
	CHESSMANPOSITION From;
	CHESSMANPOSITION To;
	INT              nScore;
}CHESSMOVE, * LPCHESSMOVE;

void GetAllMove(
				BASECHESSBOARD& ChessBoard, 
				INT x, INT y, 
				BOOL fnbTextMove(BASECHESSBOARD& ChessBoard, INT nFromX, INT nFormY, INT nToX, INT nToY), 
				void fnAddMove(INT nFromX, INT nFormY, INT nToX, INT nToY) );
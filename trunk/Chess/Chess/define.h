#pragma once
  
#define CHESS_WIDTH  9
#define CHESS_HEIGHT 10

/*template<typename TYPE, UINT WIDTH, UINT HEIGHT>
struct BOARD
{
	TYPE gPosition[HEIGHT][WIDTH];
	inline void SetChessBoard(const TYPE gPosition[HEIGHT][WIDTH])
	{
		memcpy( this->gPosition, gPosition, sizeof(TYPE[HEIGHT][WIDTH]) );
	}

	inline void SetChessBoard(BOARD<TYPE, WIDTH, HEIGHT>* const pChessBoard)
	{
		SetChessBoard(pChessBoard->gPosition);
	}
};*/
template<typename TYPE>
struct CHESSBOARD :
	public BOARD<TYPE, CHESS_WIDTH, CHESS_HEIGHT>
{
	//T gPosition[CHESS_HEIGHT][CHESS_WIDTH];
};

/*typedef enum
{
	RED_SIDE   = 1,
	BLACK_SIDE = 0
}SEARCHSIDE;*/

#define RED_SIDE   POSITIVE_SIDE
#define BLACK_SIDE NEGATIVE_SIDE

typedef SEARCHSIDE CHESSSIDE;

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

#define CHESS <CHESSMANTYPE, CHESS_WIDTH, CHESS_HEIGHT>
typedef CHESSBOARD<CHESSMANTYPE> BASECHESSBOARD;

const SHORT CHESS_TYPE_SUBSCRIPT[] = {-1, 0, 1, 2, 3, 4, 5, 6, -1, 7, 8, 9, 10, 11, 12, 13};

inline BOOL IsSide(SEARCHSIDE Side, CHESSMANTYPE Type)
{
	return Type && (Type >> SIDE_SHIFT == Side);
}

/*inline BOOL IsRedSide(CHESSMANTYPE Type)
{
	return Type && (Type >> SIDE_SHIFT == RED_SIDE);
}

inline BOOL IsBlackSide(CHESSMANTYPE Type)
{
	return Type && (Type >> SIDE_SHIFT == BLACK_SIDE);
}*/

inline BOOL IsSameSide(CHESSMANTYPE Type1, CHESSMANTYPE Type2)
{
	return Type1 && Type2 && Type1 >> SIDE_SHIFT == Type2 >> SIDE_SHIFT;
}

inline INT ToNoSide(CHESSMANTYPE Type)
{
	return Type - (Type >> SIDE_SHIFT << SIDE_SHIFT);
}

typedef SEARCHPOSITION CHESSMANPOSITION;
typedef SEARCHMOVE     CHESSMOVE;
typedef LPSEARCHMOVE   LPCHESSMOVE;
/*typedef struct
{
	INT x;
	INT y;
}CHESSMANPOSITION, * LPCHESSMANPOSITION;

typedef struct
{
	//CHESSMANTYPE     Type;
	CHESSMANPOSITION From;
	CHESSMANPOSITION To;
	//INT              nScore;
}CHESSMOVE, * LPCHESSMOVE;*/
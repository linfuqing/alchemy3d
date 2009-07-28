/**
结构体说明:
R :只读
W :只写
RW:读写
N :封装
**/

#ifndef BASE_H
#define BASE_H

# include < math.h >

# define TRUE      1
# define FALSE     0
# define PI        3.1415926
# define TODEGREES 180 / PI
# define TORADIANS PI / 180

typedef double Number;

/*int toInt( Number n )
{
	return n;
}

char toChar( Number n )
{
	return toInt( n ) + 48;
}*/

#endif
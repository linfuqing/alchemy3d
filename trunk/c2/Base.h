#ifndef __BASE_H
#define __BASE_H

#include <math.h>

typedef unsigned char		UCHAR;
typedef unsigned char		BYTE;		// 8 bits[0-255]size:1Byte FF
typedef unsigned char * 	LPBYTE;

typedef unsigned short		USHORT;
typedef unsigned short		WORD;		// 16 bits[0-65535]size:2Bytes FFFF
typedef unsigned short *	LPWORD;

typedef unsigned int		DWORD;		// 32 bits[0-4294967295]size:4Bytes FFFFFFFF
typedef unsigned int *		LPDWORD;
typedef unsigned int		UINT32;

typedef unsigned long		ULONG;

#define TRUE			1
#define FALSE			0

#define VERTEX_SIZE		4
#define FACE_SIZE		9
#define USERADIANS TRUE

#define ABS(x)	(((x) < 0) ? -(x) : (((x) > 0) ? (x) : 0))
#define BOUND(x,a,b) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))
#define MAX(a, b) (a > b) ? (a) : (b)
#define MIN(a, b) (a < b) ? (a) : (b)
#define SWAP(a,b,t) {t=a; a=b; b=t;}

#define X_AXIS (NULL == g_X_AXIS) ? newVector3D( 1.0f, 0, 0, 1.0f ) : g_X_AXIS
#define Y_AXIS (NULL == g_Y_AXIS) ? newVector3D( 0, 1.0f, 0, 1.0f ) : g_Y_AXIS
#define Z_AXIS (NULL == g_Z_AXIS) ? newVector3D( 0, 0, 1.0f, 1.0f ) : g_Z_AXIS

#define INV_X_AXIS (NULL == g_INV_X_AXIS) ? newVector3D( - 1.0f, 0, 0, 1.0f ) : g_INV_X_AXIS
#define INV_Y_AXIS (NULL == g_INV_Y_AXIS) ? newVector3D( 0, - 1.0f, 0, 1.0f ) : g_INV_Y_AXIS
#define INV_Z_AXIS (NULL == g_INV_Z_AXIS) ? newVector3D( 0, 0, - 1.0f, 1.0f ) : g_INV_Z_AXIS

#define FCMP(a,b) ( (fabs(a-b) < EPSILON_E3) ? 1 : 0)

#endif
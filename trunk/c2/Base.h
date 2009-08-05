#ifndef __BASE_H
#define __BASE_H

#include <math.h>

typedef unsigned char		BYTE;		// 8 bits[0-15]size:1
typedef unsigned char * 	LPBYTE;		// 8 bits[0-15]size:1
typedef unsigned short		WORD;		// 16 bits[0-255]size:2
typedef unsigned short *	LPWORD;		// 16 bits[0-255]size:2
typedef unsigned int		DWORD;		// 32 bits[0-65535]size:4
typedef unsigned int *		LPDWORD;		// 32 bits[0-65535]size:4
typedef unsigned int		UINT32;
typedef unsigned long		ULONG;

#define TRUE			1
#define FALSE			0

#define VERTEX_SIZE		4
#define FACE_SIZE		9
#define USERADIANS TRUE

#define MATRIX_INVERSE_EPSILON		1e-14
#define MATRIX_EPSILON				1e-6

#define ABS(x)	(((x) < 0) ? -(x) : (((x) > 0) ? (x) : 0))
#define BOUND(x,a,b) (((x) < (a)) ? (a) : (((x) > (b)) ? (b) : (x)))
#define MAX(a, b) (a > b) ? (a) : (b)
#define MIN(a, b) (a < b) ? (a) : (b)

#define X_AXIS (NULL == g_X_AXIS) ? newVector3D( 1.0f, 0, 0, 1.0f ) : g_X_AXIS
#define Y_AXIS (NULL == g_Y_AXIS) ? newVector3D( 0, 1.0f, 0, 1.0f ) : g_Y_AXIS
#define Z_AXIS (NULL == g_Z_AXIS) ? newVector3D( 0, 0, 1.0f, 1.0f ) : g_Z_AXIS
#define INV_X_AXIS (NULL == g_INV_X_AXIS) ? newVector3D( - 1.0f, 0, 0, 1.0f ) : g_INV_X_AXIS
#define INV_Y_AXIS (NULL == g_INV_Y_AXIS) ? newVector3D( 0, - 1.0f, 0, 1.0f ) : g_INV_Y_AXIS
#define INV_Z_AXIS (NULL == g_INV_Z_AXIS) ? newVector3D( 0, 0, - 1.0f, 1.0f ) : g_INV_Z_AXIS

//Engine settings
//0：不进行背面剔除
//1：在世界空间进行背面测试
//2：在屏幕空间进行背面测试
int BACKFACE_CULLING_MODE = 1;

#endif
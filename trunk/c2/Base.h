#ifndef __BASE_H
#define __BASE_H

#include <math.h>

typedef unsigned char		byte;		// 8 bits
typedef unsigned short		word;		// 16 bits
typedef unsigned int		dword;		// 32 bits
typedef unsigned int		uint32;
typedef unsigned long		ulong;

#define TRUE			1
#define FALSE			0

#define VERTEX_SIZE		4
#define FACE_SIZE		9
#define USERADIANS TRUE

#define MATRIX_INVERSE_EPSILON		1e-14
#define MATRIX_EPSILON				1e-6

#define MAX(a, b) (a > b) ? (a) : (b)
#define MIN(a, b) (a < b) ? (a) : (b)

#define X_AXIS (NULL == g_X_AXIS) ? newVector3D( 1.0f, 0, 0, 1.0f ) : g_X_AXIS
#define Y_AXIS (NULL == g_Y_AXIS) ? newVector3D( 0, 1.0f, 0, 1.0f ) : g_Y_AXIS
#define Z_AXIS (NULL == g_Z_AXIS) ? newVector3D( 0, 0, 1.0f, 1.0f ) : g_Z_AXIS

//Engine settings
//0�������б����޳�
//1��������ռ���б������
//2������Ļ�ռ���б������
int BACKFACE_CULLING_MODE = 1;

//�Ƿ����õƹ�
//0��������
//1������
int LIGHT_ENABLE = 0;

#endif
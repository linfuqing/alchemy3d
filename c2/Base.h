#ifndef __BASE_H
#define __BASE_H

#include <math.h>

//# define INLINE

typedef unsigned char		UCHAR;
typedef unsigned char		BYTE;		// 8 bits[0-255]size:1Byte FF
typedef unsigned char * 	LPBYTE;

typedef unsigned short		USHORT;
typedef unsigned short		WORD;		// 16 bits[0-65535]size:2Bytes FFFF
typedef unsigned short *	LPWORD;

typedef unsigned int		DWORD;		// 32 bits[0-4294967295]size:4Bytes FFFFFFFF
typedef unsigned int *		LPDWORD;
typedef unsigned int		UINT;

typedef unsigned long		ULONG;

#define TRUE				1
#define FALSE				0

#define OFF                 - 1

#define USERADIANS			TRUE

#define VERTEX_SIZE			5
#define FACE_SIZE			12
#define MAX_LIGHTS			10

//获得成员在结构体中的偏移量
#ifdef  _WIN64
#define FPOS(s,m)   (size_t)( (ptrdiff_t)&(   ( (s *)0 )->m   ) )
#else
#define FPOS(s,m)   (size_t)&(   ( (s *) 0 )->m   )
#endif

#define X_AXIS (NULL == g_X_AXIS) ? newVector3D( 1.0f, 0, 0, 1.0f ) : g_X_AXIS
#define Y_AXIS (NULL == g_Y_AXIS) ? newVector3D( 0, 1.0f, 0, 1.0f ) : g_Y_AXIS
#define Z_AXIS (NULL == g_Z_AXIS) ? newVector3D( 0, 0, 1.0f, 1.0f ) : g_Z_AXIS

#define INV_X_AXIS (NULL == g_INV_X_AXIS) ? newVector3D( - 1.0f, 0, 0, 1.0f ) : g_INV_X_AXIS
#define INV_Y_AXIS (NULL == g_INV_Y_AXIS) ? newVector3D( 0, - 1.0f, 0, 1.0f ) : g_INV_Y_AXIS
#define INV_Z_AXIS (NULL == g_INV_Z_AXIS) ? newVector3D( 0, 0, - 1.0f, 1.0f ) : g_INV_Z_AXIS

//定义渲染模式
#define RENDER_NONE											0x000000	//不渲染
#define RENDER_WIREFRAME_TRIANGLE_32						0x000001	//线框模式

//带纹理光栅化
#define RENDER_TEXTRUED_TRIANGLE_INVZB_32					0x000002	//32位模式 + 1/z深度缓冲 + 纹理映射
#define RENDER_TEXTRUED_BILERP_TRIANGLE_INVZB_32			0x000004	//32位模式 + 1/z深度缓冲 + 双线性纹理
#define RENDER_TEXTRUED_TRIANGLE_FSINVZB_32					0x000008	//32位模式 + 1/z深度缓冲 + Flat光照模型 + 纹理映射
#define RENDER_TEXTRUED_TRIANGLE_GSINVZB_32					0x000010	//32位模式 + 1/z深度缓冲 + Gouraud光照模型 + 纹理映射
#define RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_INVZB_32		0x000020	//32位模式 + 1/z深度缓冲 + 透视矫正纹理映射
#define RENDER_TEXTRUED_PERSPECTIVE_TRIANGLELP_INVZB_32		0x000040	//32位模式 + 1/z深度缓冲 + 线性分段透视矫正纹理映射
#define RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_FSINVZB_32		0x000080	//32位模式 + 1/z深度缓冲 + 透视矫正纹理映射 + Flat光照模型
#define RENDER_TEXTRUED_PERSPECTIVE_TRIANGLELP_FSINVZB_32	0x000100	//32位模式 + 1/z深度缓冲 + 线性分段透视矫正纹理映射 + Flat光照模型

//不带纹理光栅化
#define RENDER_FLAT_TRIANGLE_32								0x000200	//32位Flat模式不带Z缓冲
#define RENDER_FLAT_TRIANGLEFP_32							0x000400	//32位Flat定点模式不带Z缓冲
#define RENDER_FLAT_TRIANGLE_INVZB_32						0x000800	//32位模式 + 1/z深度缓冲 + Flat光照模型
#define RENDER_GOURAUD_TRIANGLE_INVZB_32					0x001000	//32位模式 + 1/z深度缓冲 + Gouraud光照模型

#endif
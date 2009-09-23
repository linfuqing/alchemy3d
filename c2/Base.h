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

//��ó�Ա�ڽṹ���е�ƫ����
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

//������Ⱦģʽ
#define RENDER_NONE											0x000000	//����Ⱦ
#define RENDER_WIREFRAME_TRIANGLE_32						0x000001	//�߿�ģʽ

//�������դ��
#define RENDER_TEXTRUED_TRIANGLE_INVZB_32					0x000002	//32λģʽ + 1/z��Ȼ��� + ����ӳ��
#define RENDER_TEXTRUED_BILERP_TRIANGLE_INVZB_32			0x000004	//32λģʽ + 1/z��Ȼ��� + ˫��������
#define RENDER_TEXTRUED_TRIANGLE_FSINVZB_32					0x000008	//32λģʽ + 1/z��Ȼ��� + Flat����ģ�� + ����ӳ��
#define RENDER_TEXTRUED_TRIANGLE_GSINVZB_32					0x000010	//32λģʽ + 1/z��Ȼ��� + Gouraud����ģ�� + ����ӳ��
#define RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_INVZB_32		0x000020	//32λģʽ + 1/z��Ȼ��� + ͸�ӽ�������ӳ��
#define RENDER_TEXTRUED_PERSPECTIVE_TRIANGLELP_INVZB_32		0x000040	//32λģʽ + 1/z��Ȼ��� + ���Էֶ�͸�ӽ�������ӳ��
#define RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_FSINVZB_32		0x000080	//32λģʽ + 1/z��Ȼ��� + ͸�ӽ�������ӳ�� + Flat����ģ��
#define RENDER_TEXTRUED_PERSPECTIVE_TRIANGLELP_FSINVZB_32	0x000100	//32λģʽ + 1/z��Ȼ��� + ���Էֶ�͸�ӽ�������ӳ�� + Flat����ģ��

//���������դ��
#define RENDER_FLAT_TRIANGLE_32								0x000200	//32λFlatģʽ����Z����
#define RENDER_FLAT_TRIANGLEFP_32							0x000400	//32λFlat����ģʽ����Z����
#define RENDER_FLAT_TRIANGLE_INVZB_32						0x000800	//32λģʽ + 1/z��Ȼ��� + Flat����ģ��
#define RENDER_GOURAUD_TRIANGLE_INVZB_32					0x001000	//32λģʽ + 1/z��Ȼ��� + Gouraud����ģ��

#endif
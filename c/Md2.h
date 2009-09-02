#pragma once

# include "Entity.h"

typedef struct
{
  int ident;                  /* magic number: "IDP2" */ //ħ��Ӧ��ΪIDP2
  int version;                /* version: must be 8  */ //�汾������Ϊ8
  int skinwidth;              /* texture width */  //������
  int skinheight;             /* texture height */   //����ĸ߶�
  int framesize;              /* size in bytes of a frame */  //ÿ֡�Ĵ�С
  int num_skins;              /* number of skins */ //Ƥ��������
  int num_vertices;           /* number of vertices per frame */  //���������
  int num_st;                 /* number of texture coordinates */  //�������������
  int num_tris;               /* number of triangles */ //�����ε�����
  int num_frames;             /* number of frames */  //��֡��
  int offset_skins;           /* offset skin data */ //Ƥ����ƫ����
  int offset_st;              /* offset texture coordinate data */  //���������ƫ����
  int offset_tris;            /* offset triangle data */ //���������ݵ�ƫ����
  int offset_frames;          /* offset frame data */  //֡���ݵ�ƫ����
  int offset_end;             /* offset end of file */ //�ļ�β��ƫ����
}MD2Header;

typedef struct
{
	MD2Header * header;
	Entity    * entity;
}MD2;

md2_newMD2()
{
	MD2 * md2;

	if( ( md2 = ( MD2 * )malloc( MD2 ) ) == NULL || ( md2 -> header = ( MD2Header * )malloc( MD2Header ) ) == NULL )
	{
		return exit( 1 );
	}

	md2 -> entity = newEntity( 

int MD2_read( UCHAR ** buffer, MD2 * md2 )
{
	memcpy( & m -> header
}
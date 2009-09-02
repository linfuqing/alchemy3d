#pragma once

# include "Entity.h"

typedef struct
{
  int ident;                  /* magic number: "IDP2" */ //魔数应该为IDP2
  int version;                /* version: must be 8  */ //版本：必须为8
  int skinwidth;              /* texture width */  //纹理宽度
  int skinheight;             /* texture height */   //纹理的高度
  int framesize;              /* size in bytes of a frame */  //每帧的大小
  int num_skins;              /* number of skins */ //皮肤的数量
  int num_vertices;           /* number of vertices per frame */  //顶点的数量
  int num_st;                 /* number of texture coordinates */  //纹理坐标的数量
  int num_tris;               /* number of triangles */ //三角形的数量
  int num_frames;             /* number of frames */  //总帧数
  int offset_skins;           /* offset skin data */ //皮肤的偏移量
  int offset_st;              /* offset texture coordinate data */  //纹理坐标的偏移量
  int offset_tris;            /* offset triangle data */ //三角形数据的偏移量
  int offset_frames;          /* offset frame data */  //帧数据的偏移量
  int offset_end;             /* offset end of file */ //文件尾的偏移量
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
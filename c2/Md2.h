#pragma once

# include "Animation.h"

# define SKIN_NAME_LENGTH 64

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
  int num_glcmds;             /* number of opengl commands */ //opengl命令的数量
  int num_frames;             /* number of frames */  //总帧数
  int offset_skins;           /* offset skin data */ //皮肤的偏移量
  int offset_st;              /* offset texture coordinate data */  //纹理坐标的偏移量
  int offset_tris;            /* offset triangle data */ //三角形数据的偏移量
  int offset_frames;          /* offset frame data */  //帧数据的偏移量
  int offset_glcmds;          /* offset OpenGL command data */ //opengl命令数据的偏移量
  int offset_end;             /* offset end of file */ //文件尾的偏移量
}MD2Header;

typedef struct
{
	char name[SKIN_NAME_LENGTH];
}MD2Skin;

typedef struct
{
	MD2Skin     * skins;
	MD2Header     header;
	Movie       * movie;
}MD2;

MD2 * newMD2()
{
	MD2 * m;

	if( ( m = ( MD2 * )malloc( sizeof( MD2 ) ) ) == NULL )
	{
		exit( TRUE );
	}

	m -> entity = newEntity();

	return m;
}

int md2_read( UCHAR ** buffer, MD2 * m )
{
	UCHAR * pointer;

	unsigned int i;

	unsigned short vertexIndex[3], uvIndex[3];

	short u, v;

	Vector * uvs;

	Mesh   * mesh;

	memcpy( & ( m -> header ), * buffer, sizeof( MD2Header ) );

	//AS3_Trace(AS3_Int( m -> header.ident ));

	if( m -> header.ident != 844121161 || m -> header.version != 8 )
	{
		return FALSE;
	}
	
	if( ( m -> skin = ( MD2Skin * )malloc( sizeof( skin   ) * m -> header.num_skins ) ) == NULL
	||  ( uvs       = ( Vector  * )malloc( sizeof( Vector ) * m -> header.num_st    ) ) == NULL )
	{
		exit( TRUE );
	}

	for( i = 0; i < m -> header.num_skins; i ++ )
	{
		pointer = * buffer + m -> header.offset_skins + sizeof( Skin ) * i;

		memcpy( & ( m -> skins[i] ), pointer, sizeof( Skin ) );
	}

	for( i = 0; i < m -> header.num_st; i ++ )
	{
		pointer  = * buffer + m -> header.offset_st + sizeof( short ) * i * 2;

		memcpy( & u, pointer                  , sizeof( short ) );
		memcpy( & v, pointer + sizeof( short ), sizeof( short ) );

		uvs[i].u = u * .0 / m -> header.skinwidth;
		uvs[i].v = v * .0 / m -> header.skinheight;
	}

	mesh = newMesh( m -> header.num_vertices, m -> header.num_tris );

	for( i = 0; i < m -> header.num_tris; i ++ )
	{
		pointer = * buffer + m -> header.offset_tris + sizeof( short ) * i * 2 * 3;

		memcpy( vertexIndex, pointer                      , sizeof( short ) * 3 );

		memcpy( uvIndex,     pointer + sizeof( short ) * 3, sizeof( short ) * 3 );

		mesh_push_triangle( 
			mesh, 
			mesh -> vertices[vertexIndex[0]],
			mesh -> vertices[vertexIndex[1]],
			mesh -> vertices[vertexIndex[2]],
			uvs[uvIndex[0]],
			uvs[uvIndex[1]],
			uvs[uvIndex[2]],
			NULL );
	}

	for( i = 0; i < m -> header.num_frames; i ++ )
	{

	}

	return TRUE;
}
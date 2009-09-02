#pragma once

# include "Entity.h"

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
	Entity      * entity;
}MD2;

MD2 * newMD2( Entity * entity )
{
	MD2 * m;

	if( ( m = ( MD2 * )malloc( sizeof( MD2 ) ) ) == NULL )
	{
		exit( TRUE );
	}

	m -> skins  = NULL;
	m -> entity = entity != NULL ? entity : newEntity();

	return m;
}

int md2_read( UCHAR ** buffer, MD2 * m )
{
	UCHAR        * pointer, * vertexPointer, name[FRAME_NAME_LENGTH];

	int            i, j;

	unsigned short vertexIndex[3], uvIndex[3];

	unsigned char vx, vy, vz, normal;

	short          u, v;

	float          sx, sy, sz, tx, ty, tz;

	Vector *       uvs;

	Mesh   *       mesh;

	Frame  *       frames;

	Texture   *    texture;

	Animation *    animation;

	memcpy( & ( m -> header ), * buffer, sizeof( MD2Header ) );

	if( m -> header.ident != 844121161 || m -> header.version != 8 )
	{
		return FALSE;
	}
	
	if( ( m -> skins = ( MD2Skin * )malloc( sizeof( MD2Skin ) * m -> header.num_skins ) ) == NULL
	||  ( uvs        = ( Vector  * )malloc( sizeof( Vector  ) * m -> header.num_st    ) ) == NULL
	||  ( frames     = ( Frame   * )malloc( sizeof( Frame   ) * m -> header.num_frames) ) == NULL )
	{
		exit( TRUE );
	}

	for( i = 0; i < m -> header.num_skins; i ++ )
	{
		pointer = * buffer + m -> header.offset_skins + sizeof( MD2Skin ) * i;

		memcpy( & ( m -> skins[i] ), pointer, sizeof( MD2Skin ) );
	}

	for( i = 0; i < m -> header.num_st; i ++ )
	{
		pointer  = * buffer + m -> header.offset_st + sizeof( short ) * i * 2;

		memcpy( & u, pointer                  , sizeof( short ) );
		memcpy( & v, pointer + sizeof( short ), sizeof( short ) );

		uvs[i].u = ( float )u * .0 / ( m -> header.skinwidth );
		uvs[i].v = ( float )v * .0 / ( m -> header.skinheight );

		//AS3_Trace( AS3_Int( uvs[i].u ) );
	}

	mesh = m -> entity -> mesh ? m -> entity -> mesh : newMesh( m -> header.num_vertices, m -> header.num_tris,NULL );

	for( i = 0; i < m -> header.num_vertices; i ++ )
	{
		mesh_push_vertex( mesh, 0, 0, 0 );
	}

	for( i = 0; i < m -> header.num_tris; i ++ )
	{
		pointer = * buffer + m -> header.offset_tris + sizeof( short ) * i * 2 * 3;

		memcpy( vertexIndex, pointer                      , sizeof( short ) * 3 );

		memcpy( uvIndex,     pointer + sizeof( short ) * 3, sizeof( short ) * 3 );

		mesh_push_triangle( 
			mesh, 
			mesh -> vertices + vertexIndex[0],
			mesh -> vertices + vertexIndex[1],
			mesh -> vertices + vertexIndex[2],
			uvs + uvIndex[0],
			uvs + uvIndex[1],
			uvs + uvIndex[2],
			mesh -> texture );

		//AS3_Trace( AS3_Int( vertexIndex[0] ) );
	}

	m -> entity -> mesh = mesh;

	for( i = 0; i < m -> header.num_frames; i ++ )
	{
		pointer = * buffer + m -> header.offset_frames
			+ ( sizeof( float ) * 6 + sizeof( char ) * FRAME_NAME_LENGTH + sizeof( unsigned char ) * 4 * ( m -> header.num_vertices ) ) * i;

		memcpy( & sx, pointer,                       sizeof( float ) );
		memcpy( & sy, pointer + sizeof( float ),     sizeof( float ) );
		memcpy( & sz, pointer + sizeof( float ) * 2, sizeof( float ) );

		memcpy( & tx, pointer + sizeof( float ) * 3, sizeof( float ) );
		memcpy( & ty, pointer + sizeof( float ) * 4, sizeof( float ) );
		memcpy( & tz, pointer + sizeof( float ) * 5, sizeof( float ) );

		memcpy( name, pointer + sizeof( float ) * 6, sizeof( char ) * FRAME_NAME_LENGTH );

		strcpy( frames[i].name, name );

		if( ( frames[i].vertices = ( Vector3D * )malloc( sizeof( Vector3D ) * m -> header.num_vertices ) ) == NULL )
		{
			exit( TRUE );
		}

		for( j = 0; j < m -> header.num_vertices; j ++ )
		{
			vertexPointer = pointer + sizeof( float ) * 6 + sizeof( char ) * FRAME_NAME_LENGTH + sizeof( unsigned char ) * 4 * j;

			memcpy( & vx,     vertexPointer                              , sizeof( unsigned char ) );
			memcpy( & vy,     vertexPointer + sizeof( unsigned char )    , sizeof( unsigned char ) );
			memcpy( & vz,     vertexPointer + sizeof( unsigned char ) * 2, sizeof( unsigned char ) );

			//法向量索引，这里没有计算，需要可以添加上去．
			memcpy( & normal, vertexPointer + sizeof( unsigned char ) * 3, sizeof( unsigned char ) );

			frames[i].vertices[j].x = sx * vx + tx;
			frames[i].vertices[j].y = sy * vy + ty;
			frames[i].vertices[j].z = sz * vz + tz;
		}
	}

	animation = newAnimation( frames, m -> header.num_frames );

	animation_updateToFrame( animation, m -> entity, 0 );

	m -> entity -> movie = newMovie( animation, 1 );

	return TRUE;
}
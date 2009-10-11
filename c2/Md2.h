#ifndef __MD2_H
#define __MD2_H

#include "Entity.h"

#define SKIN_NAME_LENGTH 64

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
	m -> entity   = entity ? entity : newEntity();

	return m;
}

int md2_read( UCHAR ** buffer, MD2 * m, Material * material, Texture * texture, int render_mode, unsigned int fps )
{
	UCHAR        * pointer, * vertexPointer;

	char			name[FRAME_NAME_LENGTH];

	int            i, j;

	unsigned short vertexIndex[3], uvIndex[3];

	unsigned char vx, vy, vz, normal;

	short          u, v;

	float          sx, sy, sz, tx, ty, tz;

	int            duration = 1000 / fps;

	Vector *       uvs;

	Frame  *       frames;

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

		//uvs[i].u = ( float )(u * 1.0f / ( m -> header.skinwidth ));
		//uvs[i].v = ( float )(v * 1.0f / ( m -> header.skinheight ));

		uvs[i].u = u * 1.0f / m -> header.skinwidth;
		uvs[i].v = v * 1.0f / m -> header.skinheight;

		//printf("%f  ", uvs[i].u);
		//printf("%f\n", uvs[i].v);
	}

	m -> entity -> mesh = mesh_reBuild( m -> entity -> mesh, m -> header.num_vertices, m -> header.num_tris );

	for( i = 0; i < m -> header.num_vertices; i ++ )
	{
		mesh_push_vertex( m -> entity -> mesh, 0, 0, 0 );
	}

	for( i = 0; i < m -> header.num_tris; i ++ )
	{
		pointer = * buffer + m -> header.offset_tris + sizeof( unsigned short ) * i * 2 * 3;

		memcpy( vertexIndex, pointer                               , sizeof( unsigned short ) * 3 );

		memcpy( uvIndex,     pointer + sizeof( unsigned short ) * 3, sizeof( unsigned short ) * 3 );

		mesh_push_triangle( 
							m -> entity -> mesh, 
							(* m -> entity -> mesh -> vertices) + vertexIndex[0],
							(* m -> entity -> mesh -> vertices) + vertexIndex[1],
							(* m -> entity -> mesh -> vertices) + vertexIndex[2],
							uvs + uvIndex[0],
							uvs + uvIndex[1],
							uvs + uvIndex[2],
							material,
							texture,
							render_mode );
	}

	//mesh_setRenderMode( m -> mesh, render_mode );
	mesh_updateFaces( m -> entity -> mesh );
	mesh_updateVertices( m -> entity -> mesh );

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

		frames[i].length = m -> header.num_vertices;
		frames[i].time   = i * duration;

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

	animation = newMorphAnimation( m -> entity -> mesh, frames, m -> header.num_frames, ( m -> header.num_frames - 1 ) * duration );

	animation_morph_updateToFrame( animation, 0 );

	m -> entity -> animation = animation;
	//for ( j = 0; j < 8 ; j ++ )
	//{
	//	printf("%f %f %f\n", mesh->vertices[j].position->x, mesh->vertices[j].position->y, mesh->vertices[j].position->z);
	//}

	return TRUE;
}

#endif

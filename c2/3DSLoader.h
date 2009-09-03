#ifndef __3DSLOADER_H
#define __3DSLOADER_H

#define CHUNK_RGB1      	 0x0010	// 3 floats of RGB
#define CHUNK_RGB2      	 0x0011	// 3 bytes of RGB
#define CHUNK_RGB3			 0x0012	//?
#define CHUNK_AMOUNT         0x0030
#define CHUNK_MAIN      	 0x4D4D	//主块(==整个文件)
#define CHUNK_OBJMESH        0x3D3D	//ObjMesh
#define CHUNK_BKGCOLOR       0x1200
#define CHUNK_AMBCOLOR  	 0x2100
#define CHUNK_OBJBLOCK  	 0x4000	//物体块
#define CHUNK_TRIMESH   	 0x4100	//网格块
#define CHUNK_VERTLIST       0x4110	//顶点块
#define CHUNK_FACELIST       0x4120	//面列表块
#define CHUNK_FACEMAT        0x4130	//面材质块
#define CHUNK_MAPLIST        0x4140
#define CHUNK_SMOOLIST       0x4150
#define CHUNK_TRMATRIX       0x4160
#define CHUNK_LIGHT     	 0x4600
#define CHUNK_SPOTLIGHT      0x4610
#define CHUNK_CAMERA    	 0x4700
#define CHUNK_MATERIAL  	 0xAFFF	//材质块
#define CHUNK_MATNAME   	 0xA000
#define CHUNK_AMBIENT   	 0xA010
#define CHUNK_DIFFUSE   	 0xA020	//diffuse
#define CHUNK_SPECULAR  	 0xA030
#define CHUNK_SHININESS      0xA040
#define CHUNK_SHINE_STRENGTH 0xA041
#define CHUNK_DOUBLESIDED    0xA081
#define CHUNK_TEXTURE   	 0xA200	//贴图块
#define CHUNK_BUMPMAP   	 0xA230
#define CHUNK_MAPFILENAME	 0xA300
#define CHUNK_MAPOPTIONS     0xA351
#define CHUNK_TILINGU		 0xA354	//U方向缩放系数
#define CHUNK_TILINGV		 0xA356	//V方向缩放系数
#define CHUNK_OFFSETU		 0xA358	//U方向偏移系数
#define CHUNK_OFFSETV		 0xA35A	//V方向偏移系数
#define CHUNK_ROTATEW		 0xA35C	//W方向旋转角
#define CHUNK_KEYFRAMER 	 0xB000	//关键帧块

#include "Scene.h"
#include "Entity.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Vector.h"

DWORD A3DS_Chunk( UCHAR ** buffer, WORD wID, DWORD dwLength )
{
	WORD	wIDRead = 0;
	DWORD	dwBlockLength = 0;

	UCHAR	* end = 0;
	UCHAR	* tmp;

	end = ( * buffer ) + dwLength;
	tmp = ( * buffer );

	while( ( * buffer ) <= end )
	{
		//读取块ID
		memcpy( & wIDRead, ( * buffer ), sizeof( wIDRead ) );
		( * buffer ) += sizeof( wIDRead );

		//读取块长度
		memcpy( & dwBlockLength, ( * buffer ), sizeof( dwBlockLength ) );
		( * buffer ) += sizeof( dwBlockLength );

		if ( ( * buffer ) > end )
		{
			//恢复指针到原始位置
			( * buffer ) = tmp;

			return 0;
		}

		if( wID != wIDRead )
		{
			//如果不是指定块，跳向下一块
			( * buffer ) += dwBlockLength - sizeof( wIDRead ) - sizeof( dwBlockLength );
		}
		else
		{
			//否则返回块的长度
			return dwBlockLength - sizeof( wIDRead ) - sizeof( dwBlockLength );
		}
	}

	//找不到指定块则恢复指针到原始位置
	( * buffer ) = tmp;

	//找不到指定块返回0
	return 0;
}

//构造实体
Entity * A3DS_Chunk_Objblock_Handler( UCHAR ** buffer )
{
	UINT i = 0;

	WORD	wID;
	DWORD	dwBlockLength;

	UCHAR	* end = 0;

	Material * material;

	Entity * entity = newEntity();
	
	material = newMaterial( newFloatColor( 1.0f, 1.0f, 1.0f, 1.0f ),
		newFloatColor( 0.8f, 0.8f, 0.8f, 1.0f ),
		newFloatColor( 1.0f, 0.0f, 0.0f, 1.0f ),
		newFloatColor( 1.0f, 0.0f, 0.0f, 1.0f ),
		4.0f );

	//=========================读取名字=========================

	if( ( entity->name = ( char * )malloc( sizeof(char) * 20 ) ) == NULL ) exit( TRUE );

	while( ** buffer != '\0' )   
	{
		* ( entity->name ) = ( char )( * * buffer );

		entity->name++;

		(* buffer)++;

		i++;
	}

	* ( entity->name ) = '\0';

	(* buffer)++;

	entity->name -= i;

	//AS3_Trace(AS3_String(entity->name));

	//=========================结束读取名字=========================

	//读取块ID
	memcpy( & wID, ( * buffer ), sizeof( wID ) );
	( * buffer ) += sizeof( wID );

	//读取块长度
	memcpy( & dwBlockLength, ( * buffer ), sizeof( dwBlockLength ) );
	( * buffer ) += sizeof( dwBlockLength );

	end = ( * buffer ) + dwBlockLength;

	//=========================读取网格信息=========================

	if( wID == CHUNK_TRIMESH )
	{
		int bufferPitch, i = 0, j = 0;

		Mesh * mesh;

		WORD vNum;
		WORD fNum;
		WORD uvNum;

		float * vList, * uvList;
		WORD * fList;
		Vertex ** vertArr;
		Vector ** vectArr;

		while( ( * buffer ) <= end )
		{
			//读取块ID
			memcpy( & wID, ( * buffer ), sizeof( wID ) );
			( * buffer ) += sizeof( wID );

			//读取块长度
			memcpy( & dwBlockLength, ( * buffer ), sizeof( dwBlockLength ) );
			( * buffer ) += sizeof( dwBlockLength );

			switch( wID )
			{
				//顶点列表
				case CHUNK_VERTLIST:

					memcpy( & vNum, ( * buffer ), sizeof( vNum ) );
					( * buffer ) += sizeof( vNum );

					bufferPitch = vNum * sizeof( float ) * 3;

					if( ( vertArr = ( Vertex ** )malloc( vNum * sizeof( Vertex * ) ) ) == NULL ) exit( TRUE );

					if( ( vList = ( float * )malloc( bufferPitch ) ) == NULL ) exit( TRUE );
					memcpy( vList, ( * buffer ), bufferPitch );

					( * buffer ) += bufferPitch;

					break;

				//纹理坐标
				case CHUNK_MAPLIST:

					memcpy( & uvNum, ( * buffer ), sizeof( uvNum ) );
					( * buffer ) += sizeof( uvNum );

					bufferPitch = uvNum * sizeof( float ) * 2;

					if( ( vectArr = ( Vector ** )malloc( vNum * sizeof( Vector * ) ) ) == NULL ) exit( TRUE );

					if( ( uvList = ( float * )malloc( bufferPitch ) ) == NULL ) exit( TRUE );
					memcpy( uvList, ( * buffer ), bufferPitch );

					( * buffer ) += bufferPitch;

					break;

				//面列表
				case CHUNK_FACELIST:

					memcpy( & fNum, ( * buffer ), sizeof( fNum ) );
					( * buffer ) += sizeof( fNum );

					bufferPitch = fNum * sizeof( WORD ) * 4;

					if( ( fList = ( WORD * )malloc( bufferPitch ) ) == NULL ) exit( TRUE );
					memcpy( fList, ( * buffer ), bufferPitch );

					( * buffer ) += bufferPitch;

					break;

				//取得物体的中心坐标（目前没有用处）
				case CHUNK_TRMATRIX:
					( * buffer ) += dwBlockLength - sizeof( wID ) - sizeof( dwBlockLength );
					break;

				default:
					( * buffer ) += dwBlockLength - sizeof( wID ) - sizeof( dwBlockLength );
					break;
			}
		}

		//开始构造网格
		mesh = newMesh( vNum, fNum, NULL );

		//压入顶点
		for ( i = 0, j = 0; i < vNum * 3; i += 3, j ++)
		{
			mesh_push_vertex(mesh, vList[i], vList[i + 1], vList[i + 2]);
			vertArr[j] = & mesh->vertices[j];
		}

		//压入纹理坐标
		for ( i = 0, j = 0; i < vNum * 2; i += 2, j ++)
		{
			vectArr[j] = newVector( uvList[i], uvList[i + 1] );
		}

		//压入面
		for ( i = 0, j = 0; i < fNum * 4; i +=4, j += 2 )
		{
			mesh_push_triangle(mesh,
								vertArr[fList[i]],
								vertArr[(int)( fList[i + 1] )],
								vertArr[(int)( fList[i + 2] )],
								vectArr[(int)( fList[i] )],
								vectArr[(int)( fList[i + 1] )],
								vectArr[(int)( fList[i + 2] )],
								NULL);
		}

		mesh_setMaterial( mesh, material );
		entity->lightEnable = FALSE;
		entity_setMesh( entity, mesh );
		entity->mesh->render_mode = RENDER_WIREFRAME_TRIANGLE_32;
	}
	//=========================结束读取网格信息=========================

	return entity;
}

//buffer	指向缓冲区起始位置的指针
//length	缓冲区长度
void A3DS_LoadData( Entity * entity, UCHAR ** buffer, DWORD length )
{
	WORD	wID = 0;
	DWORD	dwLength = length;
	DWORD	dwBlockLength = 0;

	UCHAR	* end = 0;
	Entity	* node;

	//主编辑块
	if ( ! ( dwLength = A3DS_Chunk( buffer, CHUNK_MAIN, dwLength ) ) )
	{
		//不是3DS文件
		exit( TRUE );
	}

	if ( ! ( dwBlockLength = A3DS_Chunk( buffer, CHUNK_OBJMESH, dwLength ) ) )
	{
		//可能的文件I/O错误
		exit( TRUE );
	}

	end = ( * buffer ) + dwBlockLength;

	while( ( * buffer ) <= end )
	{
		//读取块ID
		memcpy( & wID, ( * buffer ), sizeof( wID ) );
		( * buffer ) += sizeof( wID );

		//读取块长度
		memcpy( & dwBlockLength, ( * buffer ), sizeof( dwBlockLength ) );
		( * buffer ) += sizeof( dwBlockLength );

		switch( wID )
		{
			case CHUNK_MATERIAL:
				break;

			case CHUNK_OBJMESH:
				break;

			case CHUNK_OBJBLOCK:
				node = A3DS_Chunk_Objblock_Handler( buffer );
				entity_addChild( entity, node );
				if ( entity->scene )scene_addEntity( entity->scene, node, entity );
				break;

			default:
				( * buffer ) += dwBlockLength - sizeof( wID ) - sizeof( dwBlockLength );
				break;
		}
	}
}

#endif
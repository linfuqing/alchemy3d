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
#include "Material.h"
#include "Texture.h"
#include "Triangle.h"

//面和材质的关联结构
typedef struct A3DS_T_List_TYP
{
	//名字
	char * name;

	//拥有该材质的面数量
	WORD wNum;

	//对应的面索引
	WORD * list;

	struct A3DS_T_List_TYP * next;

}A3DS_T_List;

typedef struct A3DS_Entity_TYP
{
	Entity * entity;

	A3DS_T_List * tList;

	struct A3DS_Entity_TYP * next;

}A3DS_Entity;

typedef struct A3DS_MaterialList_TYP
{
	char * name;

	float fOffsetU;
	float fOffsetV;

	float fTilingU;
	float fTilingV;

	float fRotationW;
	float cosT;
	float sinT;
	int isRotateW;

	Material * material;

	Texture * texture;

	struct A3DS_MaterialList_TYP * next;

}A3DS_MaterialList;

typedef struct A3DS_TYP
{
	int mNum, tNum;

	Entity * root;

	A3DS_Entity * children;

	A3DS_MaterialList * a3d_materialList;

}A3DS, * lpA3DS;

A3DS_T_List * newT_List()
{
	A3DS_T_List * tl;

	if( ( tl = ( A3DS_T_List * )malloc( sizeof(A3DS_T_List) ) ) == NULL ) exit( TRUE );

	tl->next = NULL;
	tl->list = NULL;
	tl->wNum = 0;
	tl->name = NULL;

	return tl;
}

A3DS_Entity * newA3DS_Entity()
{
	A3DS_Entity * a3ds_entity;

	if( ( a3ds_entity = ( A3DS_Entity * )malloc( sizeof(A3DS_Entity) ) ) == NULL ) exit( TRUE );

	a3ds_entity->entity = NULL;
	a3ds_entity->tList = NULL;
	a3ds_entity->next = NULL;

	return a3ds_entity;
}

A3DS_MaterialList * newA3D_MaterialList()
{
	A3DS_MaterialList * materialList;

	if( ( materialList = ( A3DS_MaterialList * )malloc( sizeof(A3DS_MaterialList) ) ) == NULL ) exit( TRUE );

	materialList->next = NULL;
	materialList->name = NULL;
	materialList->material = NULL;
	materialList->texture = NULL;
	materialList->fOffsetU = 0;
	materialList->fOffsetV = 0;
	materialList->fTilingU = 1;
	materialList->fTilingV = 1;
	materialList->fRotationW = 0;
	materialList->cosT = 1;
	materialList->sinT = 0;
	materialList->isRotateW = FALSE;

	return materialList;
}

void t_List_addFList( A3DS_T_List * head, A3DS_T_List * node )
{
	node->next = head->next;

	head->next = node;
}

void A3DS_AddChild( A3DS_Entity * head, A3DS_Entity * node )
{
	node->next = head->next;

	head->next = node;
}

void A3D_MaterialList_addMaterial( A3DS_MaterialList * head, A3DS_MaterialList * node )
{
	node->next = head->next;

	head->next = node;
}

//处理主块
DWORD A3DS_Main_Chunk_Handler( UCHAR ** buffer, WORD wID, DWORD dwLength )
{
	WORD	wIDRead = 0;
	DWORD	dwBlockLength = 0;

	UCHAR	* end = 0;
	UCHAR	* tmp;

	end = ( * buffer ) + dwLength;
	tmp = ( * buffer );

	while( ( * buffer ) < end )
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

//处理面块
void A3DS_Face_Chunk_Handler( UCHAR ** buffer, A3DS_T_List * tList, DWORD dwLength )
{
	UINT		i = 0;
	WORD		wID = 0;
	DWORD		dwBlockLength = 0;
	UCHAR		* end = NULL;

	end = ( * buffer ) + dwLength;

	while( ( * buffer ) < end )
	{
		//读取块ID
		memcpy( & wID, ( * buffer ), sizeof( wID ) );
		( * buffer ) += sizeof( wID );

		//读取块长度
		memcpy( & dwBlockLength, ( * buffer ), sizeof( dwBlockLength ) );
		( * buffer ) += sizeof( dwBlockLength );

		if( wID == CHUNK_FACEMAT )
		{
			int			bufferPitch = 0;
			WORD		wNum = 0;
			char		* name = NULL;
			WORD		* list;
			A3DS_T_List	* ntl;

			if( ( name = ( char * )malloc( sizeof(char) * 200 ) ) == NULL ) exit( TRUE );
				
			while( ** buffer != '\0' )
			{
				* name = ( char )( ** buffer );

				name ++;

				(* buffer) ++;

				i++;
			}

			* name = '\0';

			(* buffer) ++;

			name -= i;

			memcpy( & wNum, ( * buffer ), sizeof( wNum ) );
			( * buffer ) += sizeof( wNum );

			bufferPitch = wNum * sizeof( WORD );

			if( ( list = ( WORD * )malloc( bufferPitch ) ) == NULL ) exit( TRUE );
			memcpy( list, ( * buffer ), bufferPitch );

			( * buffer ) += bufferPitch;

			ntl = newT_List();

			ntl->list = list;
			ntl->name = name;
			ntl->wNum = wNum;

			t_List_addFList( tList, ntl );
		}
		else
		{
			( * buffer ) += dwBlockLength - sizeof( wID ) - sizeof( dwBlockLength );
		}
	}
}

INLINE void A3DS_TranslateTexcoord( Triangle * face, float ou, float ov, float tu, float tv, float cosW, float sinW )
{
	float u0, u1, u2, v0, v1, v2;

	u0 = face->uv[0]->u - ou - 0.5f;
	v0 = face->uv[0]->v - ov - 0.5f;
	u1 = face->uv[1]->u - ou - 0.5f;
	v1 = face->uv[1]->v - ov - 0.5f;
	u2 = face->uv[2]->u - ou - 0.5f;
	v2 = face->uv[2]->v - ov - 0.5f;

	face->uv[0]->u = ( cosW * u0 - sinW * v0) * tu + 0.5f;
	face->uv[0]->v = ( sinW * u0 + cosW * v0) * tv + 0.5f;
	face->uv[1]->u = ( cosW * u1 - sinW * v1) * tu + 0.5f;
	face->uv[1]->v = ( sinW * u1 + cosW * v1) * tv + 0.5f;
	face->uv[2]->u = ( cosW * u2 - sinW * v2) * tu + 0.5f;
	face->uv[2]->v = ( sinW * u2 + cosW * v2) * tv + 0.5f;
}

INLINE void A3DS_TranslateTexcoord2( Triangle * face, float ou, float ov, float tu, float tv )
{
	float u0, u1, u2, v0, v1, v2;

	u0 = face->uv[0]->u;
	v0 = face->uv[0]->v;
	u1 = face->uv[1]->u;
	v1 = face->uv[1]->v;
	u2 = face->uv[2]->u;
	v2 = face->uv[2]->v;

	//先计算旋转后的纹理坐标
	face->uv[0]->u = ( u0 - ou -0.5f ) * tu + 0.5f;
	face->uv[0]->v = ( v0 - ov -0.5f ) * tv + 0.5f;
	face->uv[1]->u = ( u1 - ou -0.5f ) * tu + 0.5f;
	face->uv[1]->v = ( v1 - ov -0.5f ) * tv + 0.5f;
	face->uv[2]->u = ( u2 - ou -0.5f ) * tu + 0.5f;
	face->uv[2]->v = ( v2 - ov -0.5f ) * tv + 0.5f;
}

//处理实体块
void A3DS_Objblock_Chunk_Handler( UCHAR ** buffer, A3DS * a3ds  )
{
	UINT		i = 0;
	WORD		wID = 0;
	DWORD		dwBlockLength = 0;
	UCHAR		* end = NULL;
	A3DS_Entity	* child;

	Entity * entity = newEntity();
	A3DS_T_List * tList = newT_List();

	//=========================读取名字=========================

	if( ( entity->name = ( char * )malloc( sizeof(char) * 200 ) ) == NULL ) exit( TRUE );

	while( ** buffer != '\0' )
	{
		* ( entity->name ) = ( char )( ** buffer );

		entity->name ++;

		(* buffer) ++;

		i ++;
	}

	* ( entity->name ) = '\0';

	(* buffer) ++;

	entity->name -= i;

	//=========================结束读取名字=========================

	//读取块ID
	memcpy( & wID, ( * buffer ), sizeof( wID ) );
	( * buffer ) += sizeof( wID );

	//读取块长度
	memcpy( & dwBlockLength, ( * buffer ), sizeof( dwBlockLength ) );
	( * buffer ) += sizeof( dwBlockLength );

	end = ( * buffer ) + dwBlockLength - sizeof( wID ) - sizeof( dwBlockLength );

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

		while( ( * buffer ) < end )
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

					A3DS_Face_Chunk_Handler( buffer, tList, dwBlockLength - sizeof( wID ) - sizeof( dwBlockLength ) - sizeof( fNum ) - bufferPitch );

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
		mesh = newMesh( vNum, fNum );

		//压入顶点
		for ( i = 0, j = 0; i < vNum * 3; i += 3, j ++)
		{
			mesh_push_vertex(mesh, - vList[i], vList[i + 2], vList[i + 1]);
			vertArr[j] = & mesh->vertices[j];
		}

		//压入纹理坐标
		for ( i = 0, j = 0; i < uvNum * 2; i += 2, j ++)
		{
			vectArr[j] = newVector( uvList[i], uvList[i + 1] );
		}

		//压入面
		for ( i = 0; i < fNum * 4; i += 4 )
		{
			mesh_push_triangle(mesh,
								vertArr[fList[i + 2]],
								vertArr[fList[i + 1]],
								vertArr[fList[i + 0]],
								vectArr[fList[i + 2]],
								vectArr[fList[i + 1]],
								vectArr[fList[i + 0]],
								NULL,
								NULL,
								RENDER_FLAT_TRIANGLE_INVZB_32 );
		}

		entity_setMesh( entity, mesh );

		free( vList );
		free( uvList );
		free( fList );

		free( vertArr );
		free( vectArr );

		//=========================处理材质=========================
		if ( a3ds->a3d_materialList->next )
		{
			if ( tList->next )
			{
				A3DS_T_List * ntList;
				A3DS_MaterialList * nml;
				Triangle * face;

				ntList = tList->next;

				while ( ntList )
				{
					nml = a3ds->a3d_materialList->next;

					while ( nml )
					{
						if ( strcmp( nml->name, ntList->name ) == 0 )
						{
							if ( nml->texture )
							{
								for ( i = 0; i < ntList->wNum; i ++ )
								{
									face = & entity->mesh->faces[ntList->list[i]];

									face->texture = nml->texture;
									face->material = nml->material;
									face->render_mode = RENDER_GOURAUD_TRIANGLE_INVZB_32;

									if ( nml->isRotateW )
									{
										A3DS_TranslateTexcoord( face,
																nml->fOffsetU,
																nml->fOffsetV,
																nml->fTilingU,
																nml->fTilingV,
																nml->cosT,
																nml->sinT );
									}
									else
									{
										A3DS_TranslateTexcoord2( face,
																nml->fOffsetU,
																nml->fOffsetV,
																nml->fTilingU,
																nml->fTilingV );
									}
								}
							}
							else
							{
								for ( i = 0; i < ntList->wNum; i ++ )
								{
									entity->mesh->faces[ntList->list[i]].texture = NULL;
									entity->mesh->faces[ntList->list[i]].material = nml->material;
									entity->mesh->faces[ntList->list[i]].render_mode = RENDER_GOURAUD_TRIANGLE_INVZB_32;
								}
							}
						}

						nml = nml->next;
					}

					ntList = ntList->next;
				}
			}
		}
		else
		{
			Material * material;

			material = newMaterial( NULL, NULL, NULL, NULL, 64.0f );
			material->name = "DefaultMat";

			for ( i = 0; i < entity->mesh->nFaces; i ++ )
			{
				entity->mesh->faces[i].material = material;
				entity->mesh->faces[i].texture = NULL;
				entity->mesh->faces[i].render_mode = RENDER_GOURAUD_TRIANGLE_INVZB_32;
			}
		}
	}

	entity->mesh->lightEnable = TRUE;

	child = newA3DS_Entity();

	child->entity = entity;
	child->tList = tList;

	A3DS_AddChild( a3ds->children, child );
	
	entity_addChild( a3ds->root, entity );

	if ( a3ds->root->scene ) scene_addEntity( a3ds->root->scene, entity, a3ds->root );
}

//处理颜色块
INLINE void A3DS_Color_Chunk_Handler( UCHAR ** buffer, WORD wID, float * r, float * g, float * b )
{
	if( wID == CHUNK_RGB1 )
	{
		memcpy( r, ( * buffer ), sizeof( * r ) );
		( * buffer ) += sizeof( * r );

		memcpy( g, ( * buffer ), sizeof( * g ) );
		( * buffer ) += sizeof( * g );

		memcpy( b, ( * buffer ), sizeof( * b ) );
		( * buffer ) += sizeof( * b );
	}
	else if( wID == CHUNK_RGB2 )
	{
		UCHAR rb, gb, bb;

		memcpy( & rb, ( * buffer ), sizeof( rb ) );
		( * buffer ) += sizeof( rb );

		memcpy( & gb, ( * buffer ), sizeof( gb ) );
		( * buffer ) += sizeof( gb );

		memcpy( & bb, ( * buffer ), sizeof( bb ) );
		( * buffer ) += sizeof( bb );

		* r = (float)rb / 255.0f;
		* g = (float)gb / 255.0f;
		* b = (float)bb / 255.0f;
	}
}

//处理纹理块
Texture * A3DS_Texture_Chunk_Handler( UCHAR ** buffer, A3DS_MaterialList * head, A3DS_MaterialList * a3ds_m, DWORD dwLength )
{
	UINT	i = 0;
	WORD	wID = 0;
	DWORD	dwBlockLength = 0;
	UCHAR	* end = NULL;
	int		found = FALSE;

	Texture	* texture = NULL;

	char	* name = NULL;

	end = ( * buffer ) + dwLength;

	while( ( * buffer ) < end )
	{
		//读取块ID
		memcpy( & wID, ( * buffer ), sizeof( wID ) );
		( * buffer ) += sizeof( wID );

		//读取块长度
		memcpy( & dwBlockLength, ( * buffer ), sizeof( dwBlockLength ) );
		( * buffer ) += sizeof( dwBlockLength );
		
		switch( wID )
		{
			//纹理文件名
			case CHUNK_MAPFILENAME:

				if( ( name = ( char * )malloc( sizeof(char) * 200 ) ) == NULL ) exit( TRUE );
				
				while( ** buffer != '\0' )
				{
					* name = ( char )( ** buffer );

					name ++;

					(* buffer) ++;

					 i++;
				}

				* name = '\0';

				(* buffer) ++;

				name -= i;

				head = head->next;

				while ( head )
				{
					//如果已经存在该纹理
					if ( strcmp( name, head->texture->name ) == 0 )
					{
						found = TRUE;
						break;
					}

					head = head->next;
				}

				if ( ! found ) texture = newTexture( name );
				else texture = head->texture;

				break;

			//以下为局部纹理映射参数
			case CHUNK_OFFSETU:

				memcpy( & a3ds_m->fOffsetU, ( * buffer ), sizeof( float ) );
				( * buffer ) += sizeof( float );
				break;

			case CHUNK_OFFSETV:

				memcpy( & a3ds_m->fOffsetV, ( * buffer ), sizeof( float ) );
				a3ds_m->fOffsetV = -(a3ds_m->fOffsetV);
				( * buffer ) += sizeof( float );
				break;

			case CHUNK_TILINGU:

				memcpy( & a3ds_m->fTilingU, ( * buffer ), sizeof( float ) );
				( * buffer ) += sizeof( float );
				break;

			case CHUNK_TILINGV:

				memcpy( & a3ds_m->fTilingV, ( * buffer ), sizeof( float ) );
				( * buffer ) += sizeof( float );
				break;

			case CHUNK_ROTATEW:

				memcpy( & a3ds_m->fRotationW, ( * buffer ), sizeof( float ) );
				a3ds_m->fRotationW = -(a3ds_m->fRotationW);
				( * buffer ) += sizeof( float );
				break;

			default:

				( * buffer ) += dwBlockLength - sizeof( wID ) - sizeof( dwBlockLength );
				break;
		}
	}

	if( fabs( a3ds_m->fRotationW ) >= 0.00001f )
	{
		float theta = DEG2RAD( a3ds_m->fRotationW );
		a3ds_m->cosT = cosf( theta );
		a3ds_m->sinT = sinf( theta );
		a3ds_m->isRotateW = TRUE;
	}

	return texture;
}

//处理材质块
void A3DS_Material_Chunk_Handler( UCHAR ** buffer, lpA3DS a3ds, DWORD dwLength )
{
	UINT	i = 0;
	WORD	wID = 0;
	WORD	wID1 = 0;
	DWORD	dwBlockLength = 0;
	DWORD	dwBlockLength1 = 0;
	UCHAR	* end = NULL;

	float		r, g, b;
	char		* name = NULL;
	FloatColor	* ambient = NULL;
	FloatColor	* diffuse = NULL;
	FloatColor	* specular = NULL;
	Texture		* texture = NULL;

	A3DS_MaterialList * a3ds_m = newA3D_MaterialList();

	end = ( * buffer ) + dwLength;

	while( ( * buffer ) < end )
	{
		//读取块ID
		memcpy( & wID, ( * buffer ), sizeof( wID ) );
		( * buffer ) += sizeof( wID );

		//读取块长度
		memcpy( & dwBlockLength, ( * buffer ), sizeof( dwBlockLength ) );
		( * buffer ) += sizeof( dwBlockLength );
		
		switch( wID )
		{
			case CHUNK_MATNAME:

				if( ( name = ( char * )malloc( sizeof(char) * 200 ) ) == NULL ) exit( TRUE );
				
				while( ** buffer != '\0' )
				{
					* name = ( char )( ** buffer );

					name ++;

					(* buffer) ++;

					 i++;
				}

				* name = '\0';

				(* buffer) ++;

				name -= i;

				break;

			case CHUNK_AMBIENT:

				memcpy( & wID1, ( * buffer ), sizeof( wID1 ) );
				( * buffer ) += sizeof( wID1 );

				memcpy( & dwBlockLength1, ( * buffer ), sizeof( dwBlockLength1 ) );
				( * buffer ) += sizeof( dwBlockLength1 );

				A3DS_Color_Chunk_Handler( buffer, wID1, &r, &g, &b );

				ambient = newFloatColor( r, g, b, 1.0f );

				break;

			case CHUNK_DIFFUSE:

				memcpy( & wID1, ( * buffer ), sizeof( wID1 ) );
				( * buffer ) += sizeof( wID1 );

				memcpy( & dwBlockLength1, ( * buffer ), sizeof( dwBlockLength1 ) );
				( * buffer ) += sizeof( dwBlockLength1 );

				A3DS_Color_Chunk_Handler( buffer, wID1, &r, &g, &b );

				diffuse = newFloatColor( r, g, b, 1.0f );

				break;

			case CHUNK_SPECULAR:

				memcpy( & wID1, ( * buffer ), sizeof( wID1 ) );
				( * buffer ) += sizeof( wID1 );

				memcpy( & dwBlockLength1, ( * buffer ), sizeof( dwBlockLength1 ) );
				( * buffer ) += sizeof( dwBlockLength1 );

				A3DS_Color_Chunk_Handler( buffer, wID1, &r, &g, &b );

				specular = newFloatColor( r, g, b, 1.0f );

				break;

			case CHUNK_TEXTURE:

				texture = A3DS_Texture_Chunk_Handler( buffer, a3ds->a3d_materialList, a3ds_m, dwBlockLength- sizeof( wID ) - sizeof( dwBlockLength ) );
				
				a3ds->tNum ++;

				break;

			default:
				( * buffer ) += dwBlockLength - sizeof( wID ) - sizeof( dwBlockLength );
				break;
		}
	}
	
	a3ds_m->material = newMaterial( ambient, diffuse, specular, NULL, 64.0f );
	a3ds_m->material->name = name;
	a3ds_m->name = name;
	a3ds_m->texture = texture;

	a3ds->mNum ++;

	A3D_MaterialList_addMaterial( a3ds->a3d_materialList, a3ds_m );
}

A3DS * A3DS_Create( Entity * entity, UCHAR ** buffer, DWORD length )
{
	WORD	wID = 0;
	DWORD	dwLength = length;
	DWORD	dwBlockLength = 0;

	UCHAR	* end = 0;
	lpA3DS a3ds = NULL;

	//主编辑块
	if ( ! ( dwLength = A3DS_Main_Chunk_Handler( buffer, CHUNK_MAIN, dwLength ) ) )
	{
		//不是3DS文件
		exit( TRUE );
	}

	if ( ! ( dwBlockLength = A3DS_Main_Chunk_Handler( buffer, CHUNK_OBJMESH, dwLength ) ) )
	{
		//可能的文件I/O错误
		exit( TRUE );
	}
	
	if( ( a3ds = ( lpA3DS )malloc( sizeof( A3DS ) ) ) == NULL ) exit( TRUE );

	a3ds->root = entity;
	a3ds->children = newA3DS_Entity();
	a3ds->a3d_materialList = newA3D_MaterialList();
	a3ds->mNum = a3ds->tNum = 0;

	end = ( * buffer ) + dwBlockLength;

	while( ( * buffer ) < end )
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

				A3DS_Material_Chunk_Handler( buffer, a3ds, dwBlockLength - sizeof(wID) - sizeof(dwBlockLength) );
				break;

			case CHUNK_OBJMESH:

				( * buffer ) += dwBlockLength - sizeof( wID ) - sizeof( dwBlockLength );
				break;

			case CHUNK_OBJBLOCK:

				A3DS_Objblock_Chunk_Handler( buffer, a3ds );
				break;

			default:

				( * buffer ) += dwBlockLength - sizeof( wID ) - sizeof( dwBlockLength );
				break;
		}
	}

	return a3ds;
}

void A3DS_Dispose( A3DS * a3ds )
{
	A3DS_Entity * node;
	A3DS_T_List * tList, * ntList;
	A3DS_MaterialList * ml, * nml;

	node = a3ds->children->next;

	while ( node )
	{
		ntList = node->tList->next;

		while ( ntList )
		{
			tList = ntList;

			ntList = ntList->next;

			free( tList );
		}

		free( node->tList );

		node = node->next;
	}

	nml = a3ds->a3d_materialList;

	while ( nml )
	{
		ml = nml;

		nml = nml->next;

		free( ml );
	}
}
#endif
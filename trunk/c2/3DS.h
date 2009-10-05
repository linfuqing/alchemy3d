#ifndef __3DS_H
#define __3DS_H

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

#define MESH_USINGTEXCOORD	0x00000001

#include "Scene.h"
#include "Entity.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Vector.h"
#include "Material.h"
#include "Texture.h"
#include "Triangle.h"

typedef struct A3DS_T_List_TYP
{
	//名字
	char * name;

	//拥有该材质的面数量
	WORD wNum;

	//对应的面索引
	DWORD * list;

	struct A3DS_T_List_TYP * next;

}A3DS_T_List;

typedef struct A3DS_Entity_TYP
{
	DWORD dwFlags;

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

	DWORD render_mode;

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
	a3ds_entity->dwFlags = 0;

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

void A3DS_TranslateTexcoord( Triangle * face, float ou, float ov, float tu, float tv, float cosW, float sinW )
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

void A3DS_TranslateTexcoord2( Triangle * face, float ou, float ov, float tu, float tv )
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


Texture * A3DS_Texture_Chunk_Handler( FILE * file, A3DS_MaterialList * head, A3DS_MaterialList * a3ds_m, DWORD dwLength )
{
	int		iCount = dwLength, i = 0;
	WORD	wID;
	DWORD	dwBlockLength;
	DWORD	dwNextBlock = (DWORD)(ftell( file )) + dwLength;

	Texture	* texture = NULL;

	char	* name;

	while( iCount > 0 )
	{
		//块头
		fread( (char*)&wID, sizeof(wID), 1, file );
		fread( (char*)&dwBlockLength, sizeof(dwBlockLength), 1, file );

		switch( wID )
		{
			case CHUNK_MAPFILENAME:		//纹理文件名
				{
					if( ( name = ( char * )calloc( 200, sizeof(char) ) ) == NULL ) exit( TRUE );

					fread( name, 1, 1, file );

					for( i = 0; name[i]!=0; i++ )
						fread( name+i+1, 1, 1, file );
				}
				break;
			case CHUNK_OFFSETU:		//以下为局部纹理映射参数
				fread( (char*)&(a3ds_m->fOffsetU), sizeof(float), 1, file );
				break;
			case CHUNK_OFFSETV:
				fread( (char*)&(a3ds_m->fOffsetV), sizeof(float), 1, file );
				a3ds_m->fOffsetV = -(a3ds_m->fOffsetV);
				break;
			case CHUNK_TILINGU:
				fread( (char*)&(a3ds_m->fTilingU), sizeof(float), 1, file );
				break;
			case CHUNK_TILINGV:
				fread( (char*)&(a3ds_m->fTilingV), sizeof(float), 1, file );
				break;
			case CHUNK_ROTATEW:
				fread( (char*)&(a3ds_m->fRotationW), sizeof(float), 1, file );
				a3ds_m->fRotationW = -(a3ds_m->fRotationW);
			default:
				fseek( file, dwBlockLength - sizeof(wID) - sizeof(dwBlockLength), SEEK_CUR );
		}
		iCount -= dwBlockLength;
	}

	fseek( file, dwNextBlock, SEEK_SET );

	texture = newTexture( name );

	if( fabs( a3ds_m->fRotationW ) >= 0.00001f )
	{
		float theta = DEG2RAD( a3ds_m->fRotationW );
		a3ds_m->cosT = cosf( theta );
		a3ds_m->sinT = sinf( theta );
		a3ds_m->isRotateW = TRUE;
	}

	return texture;
}

int A3DS_Color_Chunk_Handler( FILE * file, WORD wID, float * r, float * g, float * b )
{
	if( wID == CHUNK_RGB1 )			//3 floats of rgb
	{
		fread( (char*)r, sizeof(*r), 1, file );
		fread( (char*)g, sizeof(*r), 1, file );
		fread( (char*)b, sizeof(*r), 1, file );
	}
	else if( wID == CHUNK_RGB2 )	//3 bytes of rgb
	{
		BYTE rb, gb, bb;

		fread( (char*)&rb, sizeof(rb), 1, file );
		fread( (char*)&gb, sizeof(gb), 1, file );
		fread( (char*)&bb, sizeof(bb), 1, file );

		* r = (float)rb / 255.0f;
		* g = (float)gb / 255.0f;
		* b = (float)bb / 255.0f;
	}
	else
		return FALSE;
	return TRUE;
}

void A3DS_Material_Chunk_Handler( FILE * file, lpA3DS a3ds, DWORD dwLength )
{
	int			iCount, i = 0;
	WORD		wID, wID1;
	DWORD		dwBlockLength, dwBlockLength1;
	DWORD		dwNextBlock = (DWORD)(ftell( file )) + dwLength;

	float		r, g, b;
	char		* name;
	FloatColor	* ambient = NULL;
	FloatColor	* diffuse = NULL;
	FloatColor	* specular = NULL;
	Texture		* texture = NULL;

	A3DS_MaterialList * a3ds_m = newA3D_MaterialList();

	iCount	= dwLength;
	
	while( iCount > 0 )
	{
		//块头
		fread( (char*)&wID, sizeof(wID), 1, file );
		fread( (char*)&dwBlockLength, sizeof(dwBlockLength), 1, file );

		switch( wID )
		{
			case CHUNK_MATNAME:		//材质名
				{
					if( ( name = ( char * )calloc( 200, sizeof(char) ) ) == NULL ) exit( TRUE );

					fread( name, 1, 1, file );

					for( i = 0; name[i]!=0; i++ )
						fread( (char*)name+i+1, 1, 1, file );
				}
				break;
			case CHUNK_AMBIENT:		//材质参数，下同
				{
					fread( (char*)&wID1, sizeof( wID1 ), 1, file );
					fread( (char*)&dwBlockLength1, sizeof(dwBlockLength1), 1, file );

					A3DS_Color_Chunk_Handler( file, wID1, &r, &g, &b );

					ambient = newFloatColor( r, g, b, 1.0f );
				}
				break;
			case CHUNK_DIFFUSE:
				{
					fread( (char*)&wID1, sizeof( wID1 ), 1, file );
					fread( (char*)&dwBlockLength1, sizeof(dwBlockLength1), 1, file );

					A3DS_Color_Chunk_Handler( file, wID1, &r, &g, &b );

					diffuse = newFloatColor( r, g, b, 1.0f );
				}
				break;
			case CHUNK_SPECULAR:
				{
					fread( (char*)&wID1, sizeof( wID1 ), 1, file );
					fread( (char*)&dwBlockLength1, sizeof(dwBlockLength1), 1, file );

					A3DS_Color_Chunk_Handler( file, wID1, &r, &g, &b );

					specular = newFloatColor( r, g, b, 1.0f );
				}
				break;
			case CHUNK_TEXTURE:
				//纹理块
				texture = A3DS_Texture_Chunk_Handler( file, a3ds->a3d_materialList, a3ds_m, dwBlockLength );

				a3ds->tNum ++;

				break;
			default:
				fseek( file, dwBlockLength - sizeof(wID) - sizeof(dwBlockLength), SEEK_CUR );
				break;
		}//end switch
		iCount -= dwBlockLength;
	}

	//跳到下一块
	fseek( file, dwNextBlock, SEEK_SET );

	a3ds_m->material = newMaterial( ambient, diffuse, specular, NULL, 64.0f );
	a3ds_m->name = name;
	a3ds_m->material->name = name;
	a3ds_m->texture = texture;

	a3ds->mNum ++;

	A3D_MaterialList_addMaterial( a3ds->a3d_materialList, a3ds_m );
}

DWORD A3DS_Face_Chunk_Handler( FILE * file, A3DS_T_List * tl, WORD ** pwFace, DWORD dwLength )
{
	int		iCount = dwLength;
	WORD	wID1;
	DWORD	dwBlockLength1;

	WORD	wNum;
	DWORD	dwBeginPos	= ftell( file );

	fread( (char*)&wNum, sizeof(wNum), 1, file );
	iCount -= sizeof(wNum);

	if( feof( file ) || iCount < 0 )
	{
		fseek( file, dwBeginPos, SEEK_SET );
		return FALSE;
	}

	if( ( ( * pwFace) = ( WORD * )malloc( 4 * wNum * sizeof( WORD ) ) ) == NULL ) exit( TRUE );

	fread( ( char * )( * pwFace ), sizeof(WORD), wNum * 4, file );
	iCount -= sizeof(WORD) * wNum * 4;

	if( feof( file ) || iCount < 0 )
	{
		fseek( file, dwBeginPos, SEEK_SET );
		free( pwFace );
		return FALSE;
	}

	while( iCount>0 && !(feof( file )) )
	{
		fread( (char*)&wID1, sizeof(wID1), 1, file );
		iCount -= sizeof(wID1);

		fread( (char*)&dwBlockLength1, sizeof( dwBlockLength1), 1, file );
		iCount -= sizeof(dwBlockLength1);

		//仅处理面材质块
		if( wID1 == CHUNK_FACEMAT )
		{
			DWORD		i;
			char		* name;
			WORD		wFaceNum;
			WORD		* pTemp;
			DWORD		* pFaceList;

			if( ( name = ( char * )calloc( 200, sizeof(char) ) ) == NULL ) exit( TRUE );

			fread( name, 1, 1, file );
			iCount --;

			for( i = 0; name[i] != 0; i ++ )
			{
				fread( name+i+1, 1, 1, file );
				iCount --;
			}

			fread( (char*)&wFaceNum, sizeof(wFaceNum), 1, file );
			iCount -= sizeof(wFaceNum);

			if( ( pTemp = ( WORD * )malloc( wFaceNum * sizeof(WORD)) ) == NULL ) exit( TRUE );
			if( ( pFaceList = ( DWORD * )malloc( wFaceNum * sizeof(DWORD) ) ) == NULL ) exit( TRUE );

			fread( (char*)pTemp, sizeof(WORD), wFaceNum, file );
			iCount -= sizeof(WORD) * wFaceNum;

			for( i = 0; i < wFaceNum; i ++ )
				pFaceList[i] = pTemp[i];

			free( pTemp );

			//tl = newT_List();
			tl->name = name;
			tl->list = pFaceList;
			tl->wNum = wFaceNum;

			//t_List_addFList( tList, tl );
		}
		else
		{
			fseek( file, dwBlockLength1 - sizeof(wID1) - sizeof(dwBlockLength1), SEEK_CUR );
			iCount -= dwBlockLength1 - sizeof(wID1) - sizeof(dwBlockLength1);
		}
	}

	fseek( file, dwBeginPos + dwLength, SEEK_SET);

	return wNum;
}

DWORD A3DS_Main_Chunk_Handler( FILE * file, WORD wID, DWORD dwLength )
{
	WORD	wIDRead = 0;
	DWORD	dwBlockLength = 0, dwTemp, dwLengthToEnd, dwReadCount, dwOffset, dwBeginPos;
	int		iCounter;

	dwTemp = ftell( file );
	fseek( file, 0, SEEK_END );
	dwLengthToEnd = (DWORD)ftell( file ) - dwTemp;
	fseek( file, dwTemp, SEEK_SET );

	if( dwLength > dwLengthToEnd || (!dwLength) )
		dwLength = dwLengthToEnd;

	iCounter	= dwLength;
	dwBeginPos	= ftell( file );
	dwReadCount = 0;

	//当处于寻找范围内，执行循环
	//此处的文件操作均未使用异常检测
	while( iCounter > 0 )
	{
		//读取块ID
		fread( (char*)&wIDRead, sizeof(wIDRead), 1, file );
		iCounter -= sizeof(wIDRead);

		//读取块长度
		fread( (char*)&dwBlockLength, sizeof(dwBlockLength), 1, file );
		iCounter -= sizeof(dwBlockLength);

		if( iCounter <= 0 || feof( file ) )
		{
			fseek( file, dwBeginPos, SEEK_SET );
			return 0;
		}

		if( wID != wIDRead )
		{
			//如果不是指定块，跳向下一块
			dwOffset = dwBlockLength - sizeof(wID) - sizeof(dwBlockLength);
			iCounter -= dwOffset;

			if( iCounter <= 0 )
				break;

			fseek( file, dwOffset, SEEK_CUR );

			if( feof( file ) )
				break;
		}
		else
			//否则返回块的长度
			return dwBlockLength - sizeof( dwBlockLength ) - sizeof( wID );
	}

	//若找不到则返回初始位置
	fseek( file, dwBeginPos, SEEK_SET );

	return 0;
}

Mesh * A3DS_BuildMeshes( int vNum, int fNum, int uvNum, float * pfVertex, float * pfVector, WORD * pwFace )
{
	Mesh * mesh;
	Vertex ** vertArr;
	Vector ** vectArr;
	int i, j, k;

	//读取所有顶点
	if( ( vertArr = ( Vertex ** )malloc( vNum * sizeof( Vertex * ) ) ) == NULL ) exit( TRUE );

	if( ( vectArr = ( Vector ** )malloc( uvNum * sizeof( Vector * ) ) ) == NULL ) exit( TRUE );

	mesh = newMesh( vNum, fNum );

	for ( i = 0, j = 0, k = 0; i < vNum * 3; i += 3, j ++, k += 2)
	{
		mesh_push_vertex(mesh, - pfVertex[i], pfVertex[i + 2], pfVertex[i + 1]);
		vertArr[j] = mesh->vertices[j];		
		vectArr[j] = newVector( pfVector[k], pfVector[k + 1] );
	}

	for ( i = 0; i < fNum * 4; i += 4 )
	{
		mesh_push_triangle(	mesh,	vertArr[pwFace[i + 2]], vertArr[pwFace[i + 1]], vertArr[pwFace[i + 0]],
									vectArr[pwFace[i + 2]], vectArr[pwFace[i + 1]], vectArr[pwFace[i + 0]],
									NULL, NULL, RENDER_FLAT_TRIANGLE_INVZB_32 );
	}

	free( vertArr );
	free( vectArr );

	return mesh;
}

void A3DS_Objblock_Chunk_Handler( FILE * file, A3DS * a3ds, DWORD dwLength  )
{
	int		iCount, i = 0;
	WORD	wID;
	DWORD	dwBlockLength;
	DWORD	dwNextChunk	= (DWORD)(ftell( file )) + dwLength;

	A3DS_Entity		* child = newA3DS_Entity();
	Entity			* entity = newEntity();
	A3DS_T_List		* tList = newT_List();

	iCount = dwLength;

	if( ( entity->name = ( char * )calloc( 200, sizeof(char) ) ) == NULL ) exit( TRUE );

	fread( entity->name, 1, 1, file );
	iCount --;

	for( ; entity->name[i] != 0; i ++ )
	{
		fread( entity->name + i + 1, 1, 1, file );
		iCount --;
	}

	//接着读取块名
	fread( (char*)&wID, sizeof(wID), 1, file );
	iCount -= sizeof(wID);

	fread( (char*)&dwBlockLength, sizeof( dwBlockLength ), 1, file );
	iCount -= sizeof(dwBlockLength);

	if( feof( file ) || iCount <=0 )
	{
		fseek( file, dwNextChunk - dwLength, SEEK_SET );
		return;
	}
	//如果是网格块
	if( wID == CHUNK_TRIMESH )
	{
		Mesh * mesh;

		WORD vNum = 0;
		WORD fNum = 0;
		WORD uvNum = 0;

		float * pfVertex, * pfVector, * a;
		WORD * pwFace;
		A3DS_T_List * tList;
		Vertex * vtxCenter;

		while( iCount >0 )
		{
			//读取块头
			fread( (char*)&wID, sizeof( wID ), 1, file );
			fread( (char*)&dwBlockLength, sizeof( dwBlockLength ), 1, file );

			switch( wID )
			{
				case CHUNK_VERTLIST:		//顶点列表

					fread( (char*)&vNum, sizeof(vNum), 1, file );

					if( ( pfVertex = ( float * )malloc( 3 * vNum * sizeof( float ) ) ) == NULL ) exit( TRUE );

					fread( (char*)pfVertex, sizeof(float), 3 * vNum, file );

					break;

				case CHUNK_MAPLIST:

					//数目（==顶点数目）这里假定文件数据是正确的
					fread( (char*)&uvNum, sizeof(uvNum), 1, file );

					if( ( vNum != 0 ) && ( uvNum != vNum ) )
						break;

					if( ( pfVector = ( float * )malloc( 2 * uvNum * sizeof( float ) ) ) == NULL ) exit( TRUE );

					//读取所有顶点的纹理坐标
					fread( (char*)pfVector, sizeof(float), 2 * uvNum, file );

					child->dwFlags |= MESH_USINGTEXCOORD;

					break;

				case CHUNK_FACELIST:
					//读取面信息
					tList = newT_List();

					fNum = A3DS_Face_Chunk_Handler( file, tList, & pwFace, dwBlockLength - sizeof(wID) - sizeof(dwBlockLength) );

					break;

				case CHUNK_TRMATRIX:

					fseek( file, sizeof(float) * 3 * 3, SEEK_CUR );

					if( ( a = ( float * )malloc( 3 * sizeof( float ) ) ) == NULL ) exit( TRUE );

					fread( (char*)a, sizeof(float), 3, file );

					vtxCenter = newVertex( -a[0], a[2], a[1] );

					break;

				default:

					fseek( file, dwBlockLength - sizeof(wID) - sizeof( dwBlockLength ), SEEK_CUR );

					break;
			}
			
			iCount -= dwBlockLength;
		}
		
		//开始构造网格
		mesh = A3DS_BuildMeshes( vNum, fNum, uvNum, pfVertex, pfVector, pwFace );

		entity_setMesh( entity, mesh );

		free( pwFace );
		free( pfVector );
		free( pfVertex );

		//=========================处理材质=========================
		if ( a3ds->a3d_materialList->next )
		{
			if ( tList )
			{
				A3DS_MaterialList * nml;
				Triangle * face;

				nml = a3ds->a3d_materialList->next;

				if ( nml->texture ) mesh->textureState = TEX_NOT_READY;

				while ( nml )
				{
					if ( strcmp( nml->name, tList->name ) == 0 )
					{
						if ( nml->texture )
						{
							for ( i = 0; i < tList->wNum; i ++ )
							{
								face = entity->mesh->faces[tList->list[i]];

								face->texture = nml->texture;
								face->material = nml->material;
								face->render_mode = a3ds->render_mode;

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
							for ( i = 0; i < tList->wNum; i ++ )
							{
								entity->mesh->faces[tList->list[i]]->texture = NULL;
								entity->mesh->faces[tList->list[i]]->material = nml->material;
								entity->mesh->faces[tList->list[i]]->render_mode = a3ds->render_mode;
							}
						}
					}

					nml = nml->next;
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
				entity->mesh->faces[i]->material = material;
				entity->mesh->faces[i]->texture = NULL;
				entity->mesh->faces[i]->render_mode = a3ds->render_mode;
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

	fseek( file, dwNextChunk, SEEK_SET );
}

A3DS * A3DS_Create( FILE * file, Entity * root, DWORD render_mode )
{
	WORD	wID = 0;
	DWORD	dwBlockLength = 0, dwLength = 0, dwNextBlock = 0, dwDataLength = 0;
	int		iCount;

	lpA3DS a3ds = NULL;

	fseek( file, 0, SEEK_END );
	dwLength = ftell( file );
	rewind( file );

	//主编辑块
	if ( ! ( dwLength = A3DS_Main_Chunk_Handler( file, CHUNK_MAIN, dwLength ) ) )
	{
		//不是3DS文件
		fclose( file );
		exit( TRUE );
	}

	if ( ! ( dwBlockLength = A3DS_Main_Chunk_Handler( file, CHUNK_OBJMESH, dwLength ) ) )
	{
		//可能的文件I/O错误
		fclose( file );
		exit( TRUE );
	}

	//构造新的a3ds对象
	if( ( a3ds = ( lpA3DS )malloc( sizeof( A3DS ) ) ) == NULL ) exit( TRUE );

	a3ds->root = root;
	a3ds->render_mode = render_mode;
	a3ds->children = newA3DS_Entity();
	a3ds->a3d_materialList = newA3D_MaterialList();
	a3ds->mNum = a3ds->tNum = 0;

	iCount = dwBlockLength;

	while( iCount > 0 )
	{
		dwNextBlock = ftell( file );
		//读取块头
		fread( (char*)&wID, sizeof(wID), 1, file );

		fread( (char*)&dwBlockLength, sizeof(dwBlockLength), 1, file );

		dwNextBlock += dwBlockLength;

		dwDataLength = dwBlockLength - sizeof(wID) - sizeof(dwBlockLength);

		switch( wID )
		{
			case CHUNK_MATERIAL:

				A3DS_Material_Chunk_Handler( file, a3ds, dwDataLength );
				
				break;

			case CHUNK_OBJBLOCK:

				A3DS_Objblock_Chunk_Handler( file, a3ds, dwDataLength );
				
				break;

			default:

				break;
		}

		fseek( file, dwNextBlock, SEEK_SET );
		iCount -= dwBlockLength;
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
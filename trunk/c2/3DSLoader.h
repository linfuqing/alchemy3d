#ifndef __3DSLOADER_H
#define __3DSLOADER_H

#define CHUNK_RGB1      	 0x0010	// 3 floats of RGB
#define CHUNK_RGB2      	 0x0011	// 3 bytes of RGB
#define CHUNK_RGB3			 0x0012	//?
#define CHUNK_AMOUNT         0x0030
#define CHUNK_MAIN      	 0x4D4D	//����(==�����ļ�)
#define CHUNK_OBJMESH        0x3D3D	//ObjMesh
#define CHUNK_BKGCOLOR       0x1200
#define CHUNK_AMBCOLOR  	 0x2100
#define CHUNK_OBJBLOCK  	 0x4000	//�����
#define CHUNK_TRIMESH   	 0x4100	//�����
#define CHUNK_VERTLIST       0x4110	//�����
#define CHUNK_FACELIST       0x4120	//���б��
#define CHUNK_FACEMAT        0x4130	//����ʿ�
#define CHUNK_MAPLIST        0x4140
#define CHUNK_SMOOLIST       0x4150
#define CHUNK_TRMATRIX       0x4160
#define CHUNK_LIGHT     	 0x4600
#define CHUNK_SPOTLIGHT      0x4610
#define CHUNK_CAMERA    	 0x4700
#define CHUNK_MATERIAL  	 0xAFFF	//���ʿ�
#define CHUNK_MATNAME   	 0xA000
#define CHUNK_AMBIENT   	 0xA010
#define CHUNK_DIFFUSE   	 0xA020	//diffuse
#define CHUNK_SPECULAR  	 0xA030
#define CHUNK_SHININESS      0xA040
#define CHUNK_SHINE_STRENGTH 0xA041
#define CHUNK_DOUBLESIDED    0xA081
#define CHUNK_TEXTURE   	 0xA200	//��ͼ��
#define CHUNK_BUMPMAP   	 0xA230
#define CHUNK_MAPFILENAME	 0xA300
#define CHUNK_MAPOPTIONS     0xA351
#define CHUNK_TILINGU		 0xA354	//U��������ϵ��
#define CHUNK_TILINGV		 0xA356	//V��������ϵ��
#define CHUNK_OFFSETU		 0xA358	//U����ƫ��ϵ��
#define CHUNK_OFFSETV		 0xA35A	//V����ƫ��ϵ��
#define CHUNK_ROTATEW		 0xA35C	//W������ת��
#define CHUNK_KEYFRAMER 	 0xB000	//�ؼ�֡��

#include "Scene.h"
#include "Entity.h"
#include "Mesh.h"
#include "Vertex.h"
#include "Vector.h"
#include "Material.h"

DWORD A3DS_Main_Chunk_Handler( UCHAR ** buffer, WORD wID, DWORD dwLength )
{
	WORD	wIDRead = 0;
	DWORD	dwBlockLength = 0;

	UCHAR	* end = 0;
	UCHAR	* tmp;

	end = ( * buffer ) + dwLength;
	tmp = ( * buffer );

	while( ( * buffer ) <= end )
	{
		//��ȡ��ID
		memcpy( & wIDRead, ( * buffer ), sizeof( wIDRead ) );
		( * buffer ) += sizeof( wIDRead );

		//��ȡ�鳤��
		memcpy( & dwBlockLength, ( * buffer ), sizeof( dwBlockLength ) );
		( * buffer ) += sizeof( dwBlockLength );

		if ( ( * buffer ) > end )
		{
			//�ָ�ָ�뵽ԭʼλ��
			( * buffer ) = tmp;

			return 0;
		}

		if( wID != wIDRead )
		{
			//�������ָ���飬������һ��
			( * buffer ) += dwBlockLength - sizeof( wIDRead ) - sizeof( dwBlockLength );
		}
		else
		{
			//���򷵻ؿ�ĳ���
			return dwBlockLength - sizeof( wIDRead ) - sizeof( dwBlockLength );
		}
	}

	//�Ҳ���ָ������ָ�ָ�뵽ԭʼλ��
	( * buffer ) = tmp;

	//�Ҳ���ָ���鷵��0
	return 0;
}

//����ʵ��
Entity * A3DS_Objblock_Chunk_Handler( UCHAR ** buffer )
{
	UINT	i = 0;
	WORD	wID = 0;
	DWORD	dwBlockLength = 0;
	UCHAR	* end = NULL;

	Material * material;

	Entity * entity = newEntity();
	
	material = newMaterial( newFloatColor( 1.0f, 1.0f, 1.0f, 1.0f ),
		newFloatColor( 0.8f, 0.8f, 0.8f, 1.0f ),
		newFloatColor( 1.0f, 0.0f, 0.0f, 1.0f ),
		newFloatColor( 1.0f, 0.0f, 0.0f, 1.0f ),
		4.0f );

	//=========================��ȡ����=========================

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

	//=========================������ȡ����=========================

	//��ȡ��ID
	memcpy( & wID, ( * buffer ), sizeof( wID ) );
	( * buffer ) += sizeof( wID );

	//��ȡ�鳤��
	memcpy( & dwBlockLength, ( * buffer ), sizeof( dwBlockLength ) );
	( * buffer ) += sizeof( dwBlockLength );

	end = ( * buffer ) + dwBlockLength;

	//=========================��ȡ������Ϣ=========================

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
			//��ȡ��ID
			memcpy( & wID, ( * buffer ), sizeof( wID ) );
			( * buffer ) += sizeof( wID );

			//��ȡ�鳤��
			memcpy( & dwBlockLength, ( * buffer ), sizeof( dwBlockLength ) );
			( * buffer ) += sizeof( dwBlockLength );

			switch( wID )
			{
				//�����б�
				case CHUNK_VERTLIST:

					memcpy( & vNum, ( * buffer ), sizeof( vNum ) );
					( * buffer ) += sizeof( vNum );

					bufferPitch = vNum * sizeof( float ) * 3;

					if( ( vertArr = ( Vertex ** )malloc( vNum * sizeof( Vertex * ) ) ) == NULL ) exit( TRUE );

					if( ( vList = ( float * )malloc( bufferPitch ) ) == NULL ) exit( TRUE );
					memcpy( vList, ( * buffer ), bufferPitch );

					( * buffer ) += bufferPitch;

					break;

				//��������
				case CHUNK_MAPLIST:

					memcpy( & uvNum, ( * buffer ), sizeof( uvNum ) );
					( * buffer ) += sizeof( uvNum );

					bufferPitch = uvNum * sizeof( float ) * 2;

					if( ( vectArr = ( Vector ** )malloc( vNum * sizeof( Vector * ) ) ) == NULL ) exit( TRUE );

					if( ( uvList = ( float * )malloc( bufferPitch ) ) == NULL ) exit( TRUE );
					memcpy( uvList, ( * buffer ), bufferPitch );

					( * buffer ) += bufferPitch;

					break;

				//���б�
				case CHUNK_FACELIST:

					memcpy( & fNum, ( * buffer ), sizeof( fNum ) );
					( * buffer ) += sizeof( fNum );

					bufferPitch = fNum * sizeof( WORD ) * 4;

					if( ( fList = ( WORD * )malloc( bufferPitch ) ) == NULL ) exit( TRUE );
					memcpy( fList, ( * buffer ), bufferPitch );

					( * buffer ) += bufferPitch;

					break;

				//ȡ��������������꣨Ŀǰû���ô���
				case CHUNK_TRMATRIX:
					( * buffer ) += dwBlockLength - sizeof( wID ) - sizeof( dwBlockLength );
					break;

				default:
					( * buffer ) += dwBlockLength - sizeof( wID ) - sizeof( dwBlockLength );
					break;
			}
		}

		//��ʼ��������
		mesh = newMesh( vNum, fNum, NULL );

		//ѹ�붥��
		for ( i = 0, j = 0; i < vNum * 3; i += 3, j ++)
		{
			mesh_push_vertex(mesh, vList[i], vList[i + 1], vList[i + 2]);
			vertArr[j] = & mesh->vertices[j];
		}

		//ѹ����������
		for ( i = 0, j = 0; i < vNum * 2; i += 2, j ++)
		{
			vectArr[j] = newVector( uvList[i], uvList[i + 1] );
		}

		//ѹ����
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

		free( vList );
		free( uvList );
		free( fList );

		free( vertArr );
		free( vectArr );
	}
	//=========================������ȡ������Ϣ=========================

	return entity;
}

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

void A3DS_Texture_Chunk_Handler( UCHAR ** buffer, DWORD dwLength )
{
	UINT	i = 0;
	WORD	wID = 0;
	DWORD	dwBlockLength = 0;
	UCHAR	* end = NULL;

	char	* name = NULL;

	end = ( * buffer ) + dwLength;

	while( ( * buffer ) <= end )
	{
		//��ȡ��ID
		memcpy( & wID, ( * buffer ), sizeof( wID ) );
		( * buffer ) += sizeof( wID );

		//��ȡ�鳤��
		memcpy( & dwBlockLength, ( * buffer ), sizeof( dwBlockLength ) );
		( * buffer ) += sizeof( dwBlockLength );
		
		switch( wID )
		{
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

				break;

			case CHUNK_OFFSETU:
				break;

			case CHUNK_OFFSETV:
				break;

			case CHUNK_TILINGU:
				break;

			case CHUNK_TILINGV:
				break;

			case CHUNK_ROTATEW:
				break;

			default:
				( * buffer ) += dwBlockLength - sizeof( wID ) - sizeof( dwBlockLength );
				break;
		}
	}
}

Material * A3DS_Material_Chunk_Handler( UCHAR ** buffer, DWORD dwLength )
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
	Material	* material = NULL;

	end = ( * buffer ) + dwLength;

	while( ( * buffer ) <= end )
	{
		//��ȡ��ID
		memcpy( & wID, ( * buffer ), sizeof( wID ) );
		( * buffer ) += sizeof( wID );

		//��ȡ�鳤��
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

				ambient = newFloatColor( r, g, b,  1.0f );

				break;

			case CHUNK_DIFFUSE:

				memcpy( & wID1, ( * buffer ), sizeof( wID1 ) );
				( * buffer ) += sizeof( wID1 );

				memcpy( & dwBlockLength1, ( * buffer ), sizeof( dwBlockLength1 ) );
				( * buffer ) += sizeof( dwBlockLength1 );

				A3DS_Color_Chunk_Handler( buffer, wID1, &r, &g, &b );

				diffuse = newFloatColor( r, g, b,  1.0f );

				break;

			case CHUNK_SPECULAR:

				memcpy( & wID1, ( * buffer ), sizeof( wID1 ) );
				( * buffer ) += sizeof( wID1 );

				memcpy( & dwBlockLength1, ( * buffer ), sizeof( dwBlockLength1 ) );
				( * buffer ) += sizeof( dwBlockLength1 );

				A3DS_Color_Chunk_Handler( buffer, wID1, &r, &g, &b );

				specular = newFloatColor( r, g, b,  1.0f );

				break;

			case CHUNK_TEXTURE:
				A3DS_Texture_Chunk_Handler( buffer, dwBlockLength );
				break;

			default:
				( * buffer ) += dwBlockLength - sizeof( wID ) - sizeof( dwBlockLength );
				break;
		}
	}

	material = newMaterial( ambient, diffuse, specular, NULL, 64.0f );
	material->name = name;

	return material;
}
//buffer	ָ�򻺳�����ʼλ�õ�ָ��
//length	����������
void A3DS_LoadData( Entity * entity, UCHAR ** buffer, DWORD length )
{
	WORD	wID = 0;
	DWORD	dwLength = length;
	DWORD	dwBlockLength = 0;

	UCHAR	* end = 0;
	Entity	* node;

	//���༭��
	if ( ! ( dwLength = A3DS_Main_Chunk_Handler( buffer, CHUNK_MAIN, dwLength ) ) )
	{
		//����3DS�ļ�
		exit( TRUE );
	}

	if ( ! ( dwBlockLength = A3DS_Main_Chunk_Handler( buffer, CHUNK_OBJMESH, dwLength ) ) )
	{
		//���ܵ��ļ�I/O����
		exit( TRUE );
	}

	end = ( * buffer ) + dwBlockLength;

	while( ( * buffer ) <= end )
	{
		//��ȡ��ID
		memcpy( & wID, ( * buffer ), sizeof( wID ) );
		( * buffer ) += sizeof( wID );

		//��ȡ�鳤��
		memcpy( & dwBlockLength, ( * buffer ), sizeof( dwBlockLength ) );
		( * buffer ) += sizeof( dwBlockLength );

		switch( wID )
		{
			case CHUNK_MATERIAL:
				A3DS_Material_Chunk_Handler( buffer, dwBlockLength );
				break;

			case CHUNK_OBJMESH:
				break;

			case CHUNK_OBJBLOCK:
				node = A3DS_Objblock_Chunk_Handler( buffer );
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
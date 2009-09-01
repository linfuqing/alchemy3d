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

#include "Entity.h"

DWORD A3DS_Chunk( DWORD ** buffer_ptr, WORD wID, DWORD dwLength )
{
	DWORD	*buffer = *buffer_ptr;
	WORD	wIDRead = 0;
	DWORD	length = 0;
	DWORD	end = 0;
	DWORD	dwBlockLength = 0;
	DWORD	* tmp;

	end = buffer + dwLength;
	tmp = buffer;

	while( buffer <= end )
	{
		//读取块ID
		memcpy( & wIDRead, buffer, sizeof( wIDRead ) );
		buffer += sizeof( wIDRead );

		//读取块长度
		memcpy( & dwBlockLength, buffer, sizeof( dwBlockLength ) );
		buffer += sizeof( dwBlockLength );

		/*AS3_Trace(AS3_Int(wIDRead));
		AS3_Trace(AS3_Int(dwBlockLength));*/

		if ( buffer > end )
		{
			//恢复指针到原始位置
			buffer = tmp;

			return 0;
		}

		if( wID != wIDRead )
		{
			//如果不是指定块，跳向下一块
			buffer += dwBlockLength - sizeof( wIDRead ) - sizeof( dwBlockLength );
		}
		else
		{
			//否则返回块的长度
			return dwBlockLength - sizeof( wIDRead ) - sizeof( dwBlockLength );
		}
	}

	//找不到指定块则恢复指针到原始位置
	buffer = tmp;
	//找不到指定块返回0
	return 0;
}

void A3DS_LoadData( DWORD ** buffer_ptr, DWORD length )
{
	DWORD	*buffer = *buffer_ptr;
	DWORD	dwLength = length;
	DWORD	dwEditBlockLength = 0;

	Entity	*entity = NULL;
	Mesh	*mesh = NULL;

	DWORD	end = 0;
	WORD	wID = 0;
	DWORD	dwBlockLength = 0;

	//主编辑块
	if ( ! ( dwLength = A3DS_Chunk( buffer_ptr, CHUNK_MAIN, dwLength ) ) )
	{
		//不是3DS文件
		exit( TRUE );
	}

	if ( ! ( dwEditBlockLength = A3DS_Chunk( buffer_ptr, CHUNK_OBJMESH, dwLength ) ) )
	{
		//可能的文件I/O错误
		exit( TRUE );
	}

	end = buffer + dwEditBlockLength;

	while( buffer <= end )
	{
		//读取块ID
		memcpy( & wID, buffer, sizeof( wID ) );
		buffer += sizeof( wID );

		//读取块长度
		memcpy( & dwBlockLength, buffer, sizeof( dwBlockLength ) );
		buffer += sizeof( dwBlockLength );

		switch( wID )
		{
			case CHUNK_MATERIAL:
				break;

			case CHUNK_OBJMESH:
				break;

			case CHUNK_OBJBLOCK:
				/*do
				{
				* entity->name = (char*)buffer;
				++dest;
				++entity->name;
				}while(chunk_char[0]!='\0');*/

				break;

			default:
				buffer += dwBlockLength - sizeof( wID ) - sizeof( dwBlockLength );
				break;
		}
	}
}

#endif
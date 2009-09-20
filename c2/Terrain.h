#ifndef __TERRAIN_H
#define __TERRAIN_H

# include "Entity.h"

Entity * newTerrain( Entity * entity, Texture * map, float width, float height, float maxHeight, Material * material, Texture * texture, DWORD render_mode )
{
	/*int i, j, index, _width = map -> width - 1, _height = map -> height - 1, wh = _width * _height;

	float xOffset = width / ( float )_width, yOffset = height / ( float )_height;

	Vector * uvs;

	if( ( uvs = ( Vector * )malloc( sizeof( Vector ) * map -> wh ) ) == NULL )
	{
		exit( TRUE );
	}

	mesh = mesh_reBuild( mesh, map -> wh, wh * 2 );

	for( i = 0; i < map -> height; i ++ )
	{
		for( j = 0; j < map -> width; j ++ )
		{
			index = j + i * map -> width;

			mesh_push_vertex( 
				mesh, 
				j         * xOffset + width  / 2, 
				maxHeight * ( float )( map -> pRGBABuffer[index] ) / 255, 
				i         * yOffset + height / 2 );

			uvs[index].u = j * xOffset;
			uvs[index].v = i * yOffset;
		}
	}

	for( i = 0; i < wh; i ++ )
	{
		index = i % _width + map -> width * i / _width;

		mesh_push_triangle( 
			mesh, 
			mesh -> vertices + index,
			mesh -> vertices + index + map -> width,
			mesh -> vertices + index + map -> width + 1,
			uvs + index,
			uvs + index + map -> width,
			uvs + index + map -> width + 1,
			material,
			texture,
			RENDER_WIREFRAME_TRIANGLE_32 );

		mesh_push_triangle( 
			mesh, 
			mesh -> vertices + index,
			mesh -> vertices + index + map -> width + 1,
			mesh -> vertices + index                + 1,
			uvs + index,
			uvs + index + map -> width + 1,
			uvs + index                + 1,
			material,
			texture,
			RENDER_WIREFRAME_TRIANGLE_32 );
	}*/

	int i, * buffer = ( int * )map -> pRGBABuffer, wh;

	float tmp;

	//Matrix3D rotation;
	//
	//rotationMatrix3D( & rotation, 90, 1 );

	//float xOffset = width / ( map -> width - 1 ), yOffset = height / ( map -> height - 1 );

	entity -> mesh = newPlane( entity -> mesh, material, texture, width, height, map -> width - 1, map -> height - 1, render_mode );

	wh = map -> height * map -> width;

	for( i = 0; i < wh; i ++ )
	{
		entity -> mesh -> vertices[i].position -> z = ( buffer[i] & 0x0000FF ) / 255.0f * maxHeight - maxHeight / 2;

		//AS3_Trace( AS3_Number( mesh -> vertices[k].position -> z ) );

		//matrix3D_transformVector_self( & rotation, mesh -> vertices[k].position );

		SWAP( entity -> mesh->vertices[i].position->y, entity -> mesh->vertices[i].position->z, tmp );

		//entity -> mesh->vertices[i].position->y = - entity -> mesh->vertices[i].position->y;
	}

	entity -> type          = ENTITY_TYPE_MESH_TERRAIN;

	entity -> width         = width;
	entity -> height        = height;

	entity -> widthSegment  = map -> width - 1;
	entity -> heightSegment = map -> height - 1;

	return entity;
}

#endif
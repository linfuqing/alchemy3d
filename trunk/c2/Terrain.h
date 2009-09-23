#ifndef __TERRAIN_H
#define __TERRAIN_H

# include "Entity.h"

# define TERRAIN_ADDRESS_MODE_NONE    0
# define TERRAIN_ADDRESS_MODE_WRAP    1
# define TERRAIN_ADDRESS_MODE_MIRROR  2

typedef struct TerrainTree
{
	BYTE * data;

	struct TerrainTree * tl, * tr, * bl, * br;

}TerrainTree;

TerrainTree * newTerrainTree()
{
	TerrainTree * t;

	if( ( t = ( TerrainTree * )malloc( sizeof( TerrainTree ) ) ) == NULL ) exit( TRUE );

	t->data = t->tl = t->tr = t->bl = t->br = NULL;

	return t;
}

int buildTerrainTree( Mesh * m, int depth )
{
}

Entity * newTerrain( Entity * entity, Texture * map, float width, float height, float maxHeight, int addressMode,  Material * material, Texture * texture, DWORD render_mode )
{
	int i, * buffer = ( int * )map->pRGBABuffer, wh, widthSegments = map -> width - 1, heightSegments = map->height - 1;

	float tmp;

	entity->mesh = newPlane( entity->mesh, material, texture, width, height, widthSegments, heightSegments, render_mode );

	wh = map->height * map->width;

	for( i = 0; i < wh; i ++ )
	{
		entity->mesh->vertices[i].position->z = ( buffer[i] & 0x0000FF ) / 255.0f * maxHeight - maxHeight / 2;

		SWAP( entity->mesh->vertices[i].position->y, entity->mesh->vertices[i].position->z, tmp );
	}

	if( addressMode == TERRAIN_ADDRESS_MODE_WRAP )
	{
		for( i = 0; i < entity -> mesh -> nFaces; i += 2 )
		{
			entity -> mesh -> faces[i    ].uv[0] -> x = 0.0f;
			entity -> mesh -> faces[i    ].uv[0] -> y = 0.0f;

			entity -> mesh -> faces[i    ].uv[1] -> x = 0.0f;
			entity -> mesh -> faces[i    ].uv[1] -> y = 1.0f;

			entity -> mesh -> faces[i    ].uv[2] -> x = 1.0f;
			entity -> mesh -> faces[i    ].uv[2] -> y = 0.0f;

			entity -> mesh -> faces[i + 1].uv[0] -> x = 1.0f;
			entity -> mesh -> faces[i + 1].uv[0] -> y = 0.0f;

			entity -> mesh -> faces[i + 1].uv[1] -> x = 1.0f;
			entity -> mesh -> faces[i + 1].uv[1] -> y = 1.0f;

			entity -> mesh -> faces[i + 1].uv[2] -> x = 0.0f;
			entity -> mesh -> faces[i + 1].uv[2] -> y = 1.0f;
		}
	}
	else if( addressMode == TERRAIN_ADDRESS_MODE_MIRROR )
	{
		int ib, jb,
			j, k = 0;

		float vx0, vx1, vx2, vx3, 
			  vy0, vy1, vy2, vy3;

		for(  i = 0; i < widthSegments; i ++ )
		{
			for(  j = 0; j < heightSegments; j ++ )
			{
				ib = i % 2;
				jb = j % 2;

				if( jb )
				{
					vx0 = 1;
					vx1 = 0;
					vx2 = 1;
					vx3 = 0;
				}
				else
				{
					vx0 = 0;
					vx1 = 1;
					vx2 = 0;
					vx3 = 1;
				}

				if( ib )
				{
					vy0 = 1;
					vy1 = 1;
					vy2 = 0;
					vy3 = 0;
				}
				else
				{
					vy0 = 0;
					vy1 = 0;
					vy2 = 1;
					vy3 = 1;
				}

				entity -> mesh -> faces[k].uv[0] -> x = vx0;
				entity -> mesh -> faces[k].uv[0] -> y = vy0;

				entity -> mesh -> faces[k].uv[1] -> x = vx2;
				entity -> mesh -> faces[k].uv[1] -> y = vy2;

				entity -> mesh -> faces[k].uv[2] -> x = vx1;
				entity -> mesh -> faces[k].uv[2] -> y = vy1;

				k ++;

				entity -> mesh -> faces[k].uv[0] -> x = vx1;
				entity -> mesh -> faces[k].uv[0] -> y = vy1;

				entity -> mesh -> faces[k].uv[1] -> x = vx3;
				entity -> mesh -> faces[k].uv[1] -> y = vy3;

				entity -> mesh -> faces[k].uv[2] -> x = vx2;
				entity -> mesh -> faces[k].uv[2] -> y = vy2;

				k ++;
			}
		}
	}

	entity->mesh->type			= ENTITY_TYPE_MESH_TERRAIN;

	entity->mesh->widthSegment	= map->width - 1;
	entity->mesh->heightSegment	= map->height - 1;

	return entity;
}

#endif
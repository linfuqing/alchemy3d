#ifndef __TERRAIN_H
#define __TERRAIN_H

# include "Entity.h"

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

Entity * newTerrain( Entity * entity, Texture * map, float width, float height, float maxHeight, Material * material, Texture * texture, DWORD render_mode )
{
	int i, * buffer = ( int * )map->pRGBABuffer, wh;

	float tmp;

	entity->mesh = newPlane( entity->mesh, material, texture, width, height, map->width - 1, map->height - 1, render_mode );

	wh = map->height * map->width;

	for( i = 0; i < wh; i ++ )
	{
		entity->mesh->vertices[i].position->z = ( buffer[i] & 0x0000FF ) / 255.0f * maxHeight - maxHeight / 2;

		SWAP( entity->mesh->vertices[i].position->y, entity->mesh->vertices[i].position->z, tmp );
	}

	entity->mesh->type			= ENTITY_TYPE_MESH_TERRAIN;

	entity->mesh->widthSegment	= map->width - 1;
	entity->mesh->heightSegment	= map->height - 1;

	return entity;
}

#endif
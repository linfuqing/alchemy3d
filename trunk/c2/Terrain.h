#ifndef __TERRAIN_H
#define __TERRAIN_H

#include "AABB.h"
#include "Triangle.h"
#include "Entity.h"

# define TERRAIN_ADDRESS_MODE_NONE		0
# define TERRAIN_ADDRESS_MODE_WRAP		1
# define TERRAIN_ADDRESS_MODE_MIRROR	2

# define TERRAIN_TREE_DEPTH				3

typedef struct TerrainData
{
	int nFaces;

	Triangle * faces;

	AABB * aabb;

}TerrainData;

typedef struct TerrainTree
{
	BYTE * data;

	struct TerrainTree * tl, * tr, * bl, * br;

}TerrainTree;

TerrainData * newTerrainData()
{
	TerrainData * t;

	if( ( t = ( TerrainData * )malloc( sizeof( TerrainData ) ) ) == NULL ) exit( TRUE );

	t->nFaces = 0;
	t->faces = NULL;
	t->aabb = NULL;

	return t;
}

TerrainTree * newTerrainTree()
{
	TerrainTree * t;

	if( ( t = ( TerrainTree * )malloc( sizeof( TerrainTree ) ) ) == NULL ) exit( TRUE );

	t->tl = t->tr = t->bl = t->br = NULL;

	t->data = NULL;

	return t;
}

TerrainTree * buildTerrainTreeRoot( Mesh * m )
{
	TerrainTree * root = newTerrainTree();
	TerrainData * data = newTerrainData();

	data->aabb = m->aabb;
	data->nFaces = m->nFaces;
	data->faces = m->faces;

	root->data = ( BYTE * )data;

	return root;
}

//构造四叉树
int buildTerrainTree( TerrainTree * parent, int maxdepth )
{
	//级数递减
	if( -- maxdepth >= 0 )
	{
		int i;
		float t0x, t0y, t1x, t1y, t2x, t2y;
		Triangle * face;

		TerrainData * pr_data = ( ( TerrainData * )parent->data );
		TerrainData * tl_data = newTerrainData();
		TerrainData * tr_data = newTerrainData();
		TerrainData * bl_data = newTerrainData();
		TerrainData * br_data = newTerrainData();

		parent->tl = newTerrainTree();
		parent->tr = newTerrainTree();
		parent->bl = newTerrainTree();
		parent->br = newTerrainTree();

		t0x = pr_data->aabb->min->x;
		t1x = ( pr_data->aabb->min->x + pr_data->aabb->max->x ) * .5f;
		t2x = pr_data->aabb->max->x;

		t0y = pr_data->aabb->min->y;
		t1y = ( pr_data->aabb->min->y + pr_data->aabb->max->y ) * .5f;
		t2y = pr_data->aabb->max->y;

		/*tl_data->aabb = newAABB();
		tl_data->aabb->min->x = pr_data->aabb->min->x;
		tl_data->aabb->max->x = ( pr_data->aabb->min->x + pr_data->aabb->max->x ) * .5f;
		tl_data->aabb->min->z = ( pr_data->aabb->min->z + pr_data->aabb->max->z ) * .5f;
		tl_data->aabb->max->z = pr_data->aabb->max->z;
		
		tr_data->aabb = newAABB();
		tr_data->aabb->min->x = ( pr_data->aabb->min->x + pr_data->aabb->max->x ) * .5f;
		tr_data->aabb->max->x = pr_data->aabb->max->x;
		tr_data->aabb->min->z = ( pr_data->aabb->min->z + pr_data->aabb->max->z ) * .5f;
		tr_data->aabb->max->z = pr_data->aabb->max->z;
		
		bl_data->aabb = newAABB();
		bl_data->aabb->min->x = pr_data->aabb->min->x;
		bl_data->aabb->max->x = ( pr_data->aabb->min->x + pr_data->aabb->max->x ) * .5f;
		bl_data->aabb->min->z = pr_data->aabb->min->z;
		bl_data->aabb->max->z = ( pr_data->aabb->min->z + pr_data->aabb->max->z ) * .5f;
		
		br_data->aabb = newAABB();
		br_data->aabb->min->x = ( pr_data->aabb->min->x + pr_data->aabb->max->x ) * .5f;
		br_data->aabb->max->x = pr_data->aabb->max->x;
		br_data->aabb->min->z = pr_data->aabb->min->z;
		br_data->aabb->max->z = ( pr_data->aabb->min->z + pr_data->aabb->max->z ) * .5f;*/

		for ( i = 0; i < pr_data->nFaces; i ++ )
		{
			face = & pr_data->faces[i];

			//如果在左上区间
			if( ( face->vertex[0]->position->x > t0x && face->vertex[0]->position->y > t0y &&
				face->vertex[0]->position->x < t1x && face->vertex[0]->position->y < t1y ) ||
				( face->vertex[1]->position->x > t0x && face->vertex[1]->position->y > t0y &&
				face->vertex[1]->position->x < t1x && face->vertex[1]->position->y < t1y ) ||
				( face->vertex[2]->position->x > t0x && face->vertex[2]->position->y > t0y &&
				face->vertex[2]->position->x < t1x && face->vertex[2]->position->y < t1y ) )
			{
				
			}

			//如果在右上区间
			if( ( face->vertex[0]->position->x > t1x && face->vertex[0]->position->y > t0y &&
				face->vertex[0]->position->x < t2x && face->vertex[0]->position->y < t1y ) ||
				( face->vertex[1]->position->x > t1x && face->vertex[1]->position->y > t0y &&
				face->vertex[1]->position->x < t2x && face->vertex[1]->position->y < t1y ) ||
				( face->vertex[2]->position->x > t1x && face->vertex[2]->position->y > t0y &&
				face->vertex[2]->position->x < t2x && face->vertex[2]->position->y < t1y ) )
			{
				
			}

			//如果在左下区间
			if( ( face->vertex[0]->position->x > t0x && face->vertex[0]->position->y > t1y &&
				face->vertex[0]->position->x < t1x && face->vertex[0]->position->y < t2y ) ||
				( face->vertex[1]->position->x > t0x && face->vertex[1]->position->y > t1y &&
				face->vertex[1]->position->x < t1x && face->vertex[1]->position->y < t2y ) ||
				( face->vertex[2]->position->x > t0x && face->vertex[2]->position->y > t1y &&
				face->vertex[2]->position->x < t1x && face->vertex[2]->position->y < t2y ) )
			{
				
			}

			//如果在右下区间
			if( ( face->vertex[0]->position->x > t1x && face->vertex[0]->position->y > t1y &&
				face->vertex[0]->position->x < t2x && face->vertex[0]->position->y < t2y ) ||
				( face->vertex[1]->position->x > t1x && face->vertex[1]->position->y > t1y &&
				face->vertex[1]->position->x < t2x && face->vertex[1]->position->y < t2y ) ||
				( face->vertex[2]->position->x > t1x && face->vertex[2]->position->y > t1y &&
				face->vertex[2]->position->x < t2x && face->vertex[2]->position->y < t2y ) )
			{
				
			}
		}
	}

	return TRUE;
}

Mesh * newTerrain( Mesh * base, Texture * map, float width, float height, float maxHeight, int addressMode,  Material * material, Texture * texture, DWORD render_mode )
{
	int i, * buffer = ( int * )map->pRGBABuffer, wh, widthSegments = map -> width - 1, heightSegments = map->height - 1;

	float tmp;

	TerrainTree * root;

	base = newPlane( base, material, texture, width, height, widthSegments, heightSegments, render_mode );

	wh = map->height * map->width;

	aabb_empty( base->aabb );

	for( i = 0; i < wh; i ++ )
	{
		base->vertices[i].position->z = ( (int)( ( buffer[i] & 0x0000FF ) * maxHeight ) >> 8 ) - maxHeight * .5f;

		SWAP( base->vertices[i].position->y, base->vertices[i].position->z, tmp );

		aabb_add_3D( base->aabb, base->vertices[i].position );
	}

	if( addressMode == TERRAIN_ADDRESS_MODE_WRAP )
	{
		for( i = 0; i < base -> nFaces; i += 2 )
		{
			base -> faces[i    ].uv[0] -> x = 0.0f;
			base -> faces[i    ].uv[0] -> y = 0.0f;

			base -> faces[i    ].uv[1] -> x = 0.0f;
			base -> faces[i    ].uv[1] -> y = 1.0f;

			base -> faces[i    ].uv[2] -> x = 1.0f;
			base -> faces[i    ].uv[2] -> y = 0.0f;

			base -> faces[i + 1].uv[0] -> x = 1.0f;
			base -> faces[i + 1].uv[0] -> y = 0.0f;

			base -> faces[i + 1].uv[1] -> x = 1.0f;
			base -> faces[i + 1].uv[1] -> y = 1.0f;

			base -> faces[i + 1].uv[2] -> x = 0.0f;
			base -> faces[i + 1].uv[2] -> y = 1.0f;
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

				base -> faces[k].uv[0] -> x = vx0;
				base -> faces[k].uv[0] -> y = vy0;

				base -> faces[k].uv[1] -> x = vx2;
				base -> faces[k].uv[1] -> y = vy2;

				base -> faces[k].uv[2] -> x = vx1;
				base -> faces[k].uv[2] -> y = vy1;

				k ++;

				base -> faces[k].uv[0] -> x = vx1;
				base -> faces[k].uv[0] -> y = vy1;

				base -> faces[k].uv[1] -> x = vx3;
				base -> faces[k].uv[1] -> y = vy3;

				base -> faces[k].uv[2] -> x = vx2;
				base -> faces[k].uv[2] -> y = vy2;

				k ++;
			}
		}
	}

	root = buildTerrainTreeRoot( base );

	buildTerrainTree( root, TERRAIN_TREE_DEPTH );

	base->type			= ENTITY_TYPE_MESH_TERRAIN;
	base->widthSegment	= map->width - 1;
	base->heightSegment	= map->height - 1;

	return base;
}

#endif

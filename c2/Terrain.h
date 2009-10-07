#ifndef __TERRAIN_H
#define __TERRAIN_H

#include "AABB.h"
#include "Triangle.h"
#include "Entity.h"
#include "Primitives.h"

#define TERRAIN_ADDRESS_MODE_NONE		0
#define TERRAIN_ADDRESS_MODE_WRAP		1
#define TERRAIN_ADDRESS_MODE_MIRROR		2

Mesh * newTerrain( Mesh * base, Texture * map, float width, float height, float maxHeight, int addressMode,  Material * material, Texture * texture, DWORD render_mode )
{
	int i, * buffer = ( int * )map->pRGBABuffer, wh, widthSegments = map -> width - 1, heightSegments = map->height - 1;

	float tmp;

	base = newPlane( base, material, texture, width, height, widthSegments, heightSegments, render_mode );

	wh = map->height * map->width;

	for( i = 0; i < wh; i ++ )
	{
		base->vertices[i]->position->z = ( (int)( ( buffer[i] & 0x0000FF ) * maxHeight ) >> 8 ) - maxHeight * .5f;

		SWAP( base->vertices[i]->position->y, base->vertices[i]->position->z, tmp );

		//base->vertices[i]->position->y = - base->vertices[i]->position->y;
	}

	if( addressMode == TERRAIN_ADDRESS_MODE_WRAP )
	{
		for( i = 0; i < base -> nFaces; i += 2 )
		{
			base -> faces[i    ]->uv[0] -> x = 0.0f;
			base -> faces[i    ]->uv[0] -> y = 0.0f;

			base -> faces[i    ]->uv[1] -> x = 0.0f;
			base -> faces[i    ]->uv[1] -> y = 1.0f;

			base -> faces[i    ]->uv[2] -> x = 1.0f;
			base -> faces[i    ]->uv[2] -> y = 0.0f;

			base -> faces[i + 1]->uv[0] -> x = 1.0f;
			base -> faces[i + 1]->uv[0] -> y = 0.0f;

			base -> faces[i + 1]->uv[1] -> x = 1.0f;
			base -> faces[i + 1]->uv[1] -> y = 1.0f;

			base -> faces[i + 1]->uv[2] -> x = 0.0f;
			base -> faces[i + 1]->uv[2] -> y = 1.0f;
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

				base -> faces[k]->uv[0] -> x = vx0;
				base -> faces[k]->uv[0] -> y = vy0;

				base -> faces[k]->uv[1] -> x = vx2;
				base -> faces[k]->uv[1] -> y = vy2;

				base -> faces[k]->uv[2] -> x = vx1;
				base -> faces[k]->uv[2] -> y = vy1;

				k ++;

				base -> faces[k]->uv[0] -> x = vx1;
				base -> faces[k]->uv[0] -> y = vy1;

				base -> faces[k]->uv[1] -> x = vx3;
				base -> faces[k]->uv[1] -> y = vy3;

				base -> faces[k]->uv[2] -> x = vx2;
				base -> faces[k]->uv[2] -> y = vy2;

				k ++;
			}
		}
	}

	base->type			= ENTITY_TYPE_MESH_TERRAIN;
	base->widthSegment	= map->width - 1;
	base->heightSegment	= map->height - 1;

	return base;
}

float terrain_getHeight( Mesh * terrain, float x, float z )
{
	if( terrain -> type == ENTITY_TYPE_MESH_TERRAIN )
	{
		float w = terrain->octree->data->aabb->max->x - terrain->octree->data->aabb->min->x;
		float h = terrain->octree->data->aabb->max->z - terrain->octree->data->aabb->min->z;
		float textureX = w  * .5f,
			  textureY = h  * .5f,
		
			  iW = terrain -> widthSegment / w,
			  iH = terrain ->heightSegment / h,

			  tx = ( x + textureX ) * iW, 
			  tz = ( z + textureY ) * iH;

		int //gridX = terrain ->  widthSegment + 1,
			gridZ = terrain->heightSegment + 1,

			ix = ( int )tx, 
			iz = ( int )tz, 

			_x = ix + 1, 
			_z = iz + 1, 
			
			aIndex, bIndex, cIndex;

		Vector3D normal;

		if( ( tx -= ix ) + ( tz -= iz ) > 1 )
		{
			aIndex = ix * gridZ + iz;
			cIndex = ix * gridZ + _z;
			bIndex = _x * gridZ + iz;
		}
		else
		{
			aIndex = _x * gridZ + _z;
			cIndex = _x * gridZ + iz;
			bIndex = ix * gridZ + _z;
		}

		triangle_normal( & normal, terrain -> vertices[cIndex], terrain -> vertices[bIndex], terrain -> vertices[aIndex] );

		return - ( x * normal.x + z * normal.z - vector3D_dotProduct( & normal, terrain -> vertices[aIndex]->position ) ) / normal.y;
	}

	return 0;
}

INLINE void terrain_trace( Entity * terrain, SceneNode * target, int type )
{
	if( terrain->mesh->type == ENTITY_TYPE_MESH_TERRAIN )
	{
		float w = terrain->mesh->octree->data->aabb->max->x - terrain->mesh->octree->data->aabb->min->x;
		float h = terrain->mesh->octree->data->aabb->max->z - terrain->mesh->octree->data->aabb->min->z;
		float textureX = w  * .5f,
			  textureY = h  * .5f,
		
			  iW = terrain->mesh->widthSegment / w,
			  iH = terrain->mesh->heightSegment / h,

			  tx, tz,
					   
			  height;

		int //gridX = terrain ->  widthSegment + 1,
			gridZ = terrain->mesh->heightSegment + 1,
			ix, iz, _x, _z, aIndex, bIndex, cIndex;

		Vector3D normal;

		SceneNode * ep = target;

		while( ep != NULL )
		{
			if( ep -> entity->mesh->type == ENTITY_TYPE_MESH_TERRAIN
			||  ep -> entity -> parent != terrain -> parent
			||	ep -> entity -> position -> x < - textureX 
			||  ep -> entity -> position -> x >   textureX
			||	ep -> entity -> position -> z < - textureY 
			||  ep -> entity -> position -> z >   textureY )
			{
				ep = ep -> next;
				continue;
			}

			tx = ( ep -> entity -> position -> x + textureX ) * iW;
			tz = ( ep -> entity -> position -> z + textureY ) * iH;

			ix = ( int )tx;
			iz = ( int )tz;
					
			_x = ix + 1;
			_z = iz + 1;

			if( ( tx -= ix ) + ( tz -= iz ) > 1 )
			{
				aIndex = ix * gridZ + iz;
				cIndex = ix * gridZ + _z;
				bIndex = _x * gridZ + iz;
			}
			else
			{
				aIndex = _x * gridZ + _z;
				cIndex = _x * gridZ + iz;
				bIndex = ix * gridZ + _z;
			}

			/*b = entity -> mesh -> vertices[bIndex].position -> y - entity -> mesh -> vertices[aIndex].position -> y;
			c = entity -> mesh -> vertices[cIndex].position -> y - entity -> mesh -> vertices[aIndex].position -> y;

			//////
			///�����㷨1
			///
			///�����β�ֵ
			///�����������������ͶӰʼ��Ϊֱ�ǵ���������
			///�ʵ�ͶӰ�߶�Ϊ:����2(c-b)+bx+by
			//////
			height = invSqrt( 2 ) * tx * ( c - b ) + b * tx + b * tz + entity -> mesh -> vertices[aIndex].position -> y;*/

			//////
			///�����㷨2:
			///
			///ƽ���㷨
			//////
			triangle_normal( & normal, terrain -> mesh -> vertices[cIndex], terrain -> mesh -> vertices[bIndex], terrain -> mesh -> vertices[aIndex] );

			height = - ( ep -> entity -> position -> x * normal.x + ep -> entity -> position -> z * normal.z - vector3D_dotProduct( & normal, terrain -> mesh -> vertices[aIndex]->position ) ) / normal.y;

			/////
			///�����㷨3
			///
			///ƽ��ֵ
			/////
			/*ep -> entity -> position -> y = - ( entity -> mesh -> vertices[aIndex].position -> y
										      +   entity -> mesh -> vertices[bIndex].position -> y
										      +   entity -> mesh -> vertices[cIndex].position -> y
										        ) * .333333333333333333333333f;*/

			ep -> entity -> position -> y = height - mesh_minY( ep->entity->mesh ) * ep -> entity -> scale -> y;
			//AS3_Trace( AS3_Number( ep->entity->mesh->octree->data->aabb->min->y ) );
			ep = ep -> next;
		}
	}
}


INLINE void terrain_sceneTrace( SceneNode * scene )
{
	SceneNode * sceneNode = scene;

	while( sceneNode )
	{
		terrain_trace( sceneNode -> entity, scene, 0 );

		sceneNode = sceneNode->next;
	}
}

#endif

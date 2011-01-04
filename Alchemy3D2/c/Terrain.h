#ifndef __TERRAIN_H
#define __TERRAIN_H

#include "Camera.h"
#include "Primitives.h"
#include "Bitmap.h"

#define TERRAIN_TYPE_NONE     0
#define TERRAIN_TYPE_TRACE    1
#define TERRAIN_TYPE_BOUNDARY 2

#define TERRAIN_ADDRESS_MODE_NONE   0
#define TERRAIN_ADDRESS_MODE_WRAP   1
#define TERRAIN_ADDRESS_MODE_MIRROR 2

typedef struct Terrain
{
	int widthSegment;
	int heightSegment;
	Entity* parent;
}Terrain;

Terrain * newTerrain( Entity * base, Bitmap * map, float width, float height, float maxHeight,  Material * material, Texture * texture, DWORD render_mode, int addressMode )
{
	Terrain* terrain;
	DWORD i;
	
#ifdef RGB565

	USHORT * buffer = ( USHORT * )map->pRGBABuffer, wh, widthSegments = map -> width - 1, heightSegments = map->height - 1;

#else

	DWORD * buffer = ( DWORD * )map->pRGBABuffer, wh, widthSegments = map -> width - 1, heightSegments = map->height - 1;

#endif

	float tmp;

	//int addressMode = 2;

	base -> mesh = newPlane( base -> mesh, material, texture, width, height, widthSegments, heightSegments, render_mode );

	wh = map->height * map->width;

	for( i = 0; i < wh; i ++ )
	{

#ifdef RGB565

		base -> mesh ->vertices[i]->position->z = ( (int)( ( RGB565TOARGB888( map->alpha[i], ((buffer[i]) >> 11) & 0x1f, ((buffer[i]) >> 5) & 0x3f, (buffer[i]) & 0x1f ) & 0x0000FF ) * maxHeight ) >> 8 ) - maxHeight * .5f;

#else

		base -> mesh ->vertices[i]->position->z = ( (int)( ( buffer[i] & 0x0000FF ) * maxHeight ) >> 8 ) - maxHeight * .5f;

#endif

		SWAP( base ->mesh->vertices[i]->position->y, base->mesh->vertices[i]->position->z, tmp );

		//base->vertices[i]->position->y = - base->vertices[i]->position->y;
	}

	if( addressMode == TERRAIN_ADDRESS_MODE_WRAP )
	{
		for( i = 0; i < base -> mesh -> nFaces; i += 2 )
		{
			base -> mesh -> faces[i    ]->uv[0] -> u = 0.0f;
			base -> mesh -> faces[i    ]->uv[0] -> v = 1.0f;

			base -> mesh -> faces[i    ]->uv[1] -> u = 1.0f;
			base -> mesh -> faces[i    ]->uv[1] -> v = 1.0f;

			base -> mesh -> faces[i    ]->uv[2] -> u = 0.0f;
			base -> mesh -> faces[i    ]->uv[2] -> v = 0.0f;

			base -> mesh -> faces[i + 1]->uv[0] -> u = 1.0f;
			base -> mesh -> faces[i + 1]->uv[0] -> v = 0.0f;

			base -> mesh -> faces[i + 1]->uv[1] -> u = 0.0f;
			base -> mesh -> faces[i + 1]->uv[1] -> v = 0.0f;

			base -> mesh -> faces[i + 1]->uv[2] -> u = 1.0f;
			base -> mesh -> faces[i + 1]->uv[2] -> v = 1.0f;
		}
	}
	else if( addressMode == TERRAIN_ADDRESS_MODE_MIRROR )
	{
		DWORD ib, jb,
			j, k = 0;

		float vx0, vx1, vx2, vx3, 
			vy0, vy1, vy2, vy3;

		for(  i = 0; i < widthSegments; i ++ )
		{
			for(  j = 0; j < heightSegments; j ++ )
			{
				ib = i & 1;
				jb = j & 1;

				if( ib )
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

				if( jb )
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

				base -> mesh -> faces[k]->uv[0] -> u = vx2;
				base -> mesh -> faces[k]->uv[0] -> v = vy2;

				base -> mesh -> faces[k]->uv[1] -> u = vx3;
				base -> mesh -> faces[k]->uv[1] -> v = vy3;

				base -> mesh -> faces[k]->uv[2] -> u = vx0;
				base -> mesh -> faces[k]->uv[2] -> v = vy0;

				k ++;

				base -> mesh -> faces[k]->uv[0] -> u = vx1;
				base -> mesh -> faces[k]->uv[0] -> v = vy1;

				base -> mesh -> faces[k]->uv[1] -> u = vx0;
				base -> mesh -> faces[k]->uv[1] -> v = vy0;

				base -> mesh -> faces[k]->uv[2] -> u = vx3;
				base -> mesh -> faces[k]->uv[2] -> v = vy3;

				k ++;
			}
		}
	}

	/*if( address )
	{
		
		DWORD j, k = 0;

		for(  i = 0; i < widthSegments; i ++ )
		{
			for(  j = 0; j < heightSegments; j ++ )
			{
				base -> mesh -> faces[k]->uv[0] -> x = (float)i;
				base -> mesh -> faces[k]->uv[0] -> y = (float)(heightSegments - j);

				base -> mesh -> faces[k]->uv[1] -> x = (float)(i + 1);
				base -> mesh -> faces[k]->uv[1] -> y = (float)(heightSegments - j);

				base -> mesh -> faces[k]->uv[2] -> x = (float)(i);
				base -> mesh -> faces[k]->uv[2] -> y = (float)(heightSegments - j - 1);

				k ++;

				base -> mesh -> faces[k]->uv[0] -> x = (float)(i + 1);
				base -> mesh -> faces[k]->uv[0] -> y = (float)(heightSegments - j - 1);

				base -> mesh -> faces[k]->uv[1] -> x = (float)(i);
				base -> mesh -> faces[k]->uv[1] -> y = (float)(heightSegments - j - 1);

				base -> mesh -> faces[k]->uv[2] -> x = (float)(i + 1);
				base -> mesh -> faces[k]->uv[2] -> y = (float)(heightSegments - j);

				k ++;
			}
		}
	}*/

	//base -> mesh->type			= ENTITY_TYPE_MESH_TERRAIN;
	//base -> mesh->widthSegment	= map->width - 1;
	//base -> mesh->heightSegment	= map->height - 1;

	//base -> animation = newTerrainTraceAnimation( base, map->width - 1, map->height - 1 );

	if( ( terrain = ( Terrain * )malloc( sizeof( Terrain ) ) ) == NULL ) 
		exit( TRUE );

	terrain->widthSegment  = map->width - 1;
	terrain->heightSegment = map->height - 1;
	terrain->parent        = base;

	return terrain;
}

float terrain_getHeight( Terrain * terrain, float x, float z )
{
	float x0 = terrain->parent->mesh->octree->data->aabb->min->x,
		  x1 = terrain->parent->mesh->octree->data->aabb->max->x,
		  z0 = terrain->parent->mesh->octree->data->aabb->min->z,
		  z1 = terrain->parent->mesh->octree->data->aabb->max->z,

		  localX = (x - terrain->parent->world->m41) / terrain->parent->world->m11,
		  localZ = (z - terrain->parent->world->m43) / terrain->parent->world->m33,

		  tx = (localX - x0) * terrain->widthSegment / (x1 - x0),
		  tz = terrain->heightSegment - (localZ - z0) * terrain->heightSegment / (z1 - z0),

		  height;

	int //gridX = terrain ->  widthSegment + 1,
		gridZ = terrain->heightSegment + 1,

		ix = ( int )tx, 
		iz = ( int )tz, 
		
		aIndex, bIndex, cIndex;

	Vector3D normal;

	if( ( tx -= ix ) + ( tz -= iz ) > 1 )
	{
		cIndex = (ix + 1) * gridZ + iz;
		bIndex = ix * gridZ + iz + 1;
		aIndex = cIndex + 1;
	}
	else
	{
		aIndex = ix * gridZ + iz;
		cIndex = aIndex + 1;
		bIndex = (ix + 1) * gridZ + iz;
	}

	triangle_normal( & normal, terrain->parent->mesh->vertices[aIndex], terrain->parent->mesh->vertices[bIndex], terrain->parent->mesh->vertices[cIndex] );

	height = ( vector3D_dotProduct( & normal, terrain->parent->mesh->vertices[aIndex]->position ) 
			- localX * normal.x 
			- localZ * normal.z ) / normal.y;

	return height * terrain->parent->world->m22 + terrain->parent->world->m42;
}

INLINE void terrain_trace(Terrain * terrain, SceneNode * target)
{
	float	tx, tz,
	x0 = terrain->parent->mesh->octree->data->aabb->min->x,
	x1 = terrain->parent->mesh->octree->data->aabb->max->x,
	z0 = terrain->parent->mesh->octree->data->aabb->min->z,
	z1 = terrain->parent->mesh->octree->data->aabb->max->z;

	float height, minY;

	int gridZ = terrain->heightSegment + 1,
		ix, iz, aIndex, bIndex, cIndex;

	Vector3D normal, local;

	SceneNode * ep = target;

	while( ep )
	{
		if(!ep->entity->visible)
		{
			ep = entity_getTreeTail(ep);

			ep = ep->next;

			continue;
		}

		if(ep->entity->isBone)
		{
			ep = ep -> next;

			continue;
		}

		if(ep->entity->parent)
		{
			matrix4x4_getPosition(ep->entity->position, ep->entity->transform);
			matrix4x4_transformVector(&local, ep->entity->parent->world, ep->entity->position);

			vector3D_subtract_self(&local, ep->entity->position);

			vector3D_add_self(ep->entity->position, &local);

			ep->entity->position->w = local.w;

			aabb_setToTranslationBox(ep->entity->worldAABB, ep->entity->worldAABB, &local);

			//if(ep->entity->mesh)
			//	aabb_setToTranslationBox(ep->entity->mesh->octree->data->worldAABB, ep->entity->aabb, &local);
		}

		//if(! ep->entity->mesh || ! ep->entity->mesh->terrainTrace || ep->entity == terrain->parent)
		if(!ep->entity->terrainTrace || ep->entity == terrain->parent)
		{
			ep = ep -> next;

			continue;
		}

		//把实体位置变换到地形的私有坐标系
		matrix4x4_transformVector( & local, terrain->parent->worldInvert, ep->entity->w_pos );

		if(	local.x < x0 
		||  local.x > x1
		||	local.z < z0 
		||  local.z > z1 )
		{
			ep = ep -> next;

			continue;
		}

		tx = ( local.x - x0 ) * terrain->widthSegment / ( x1 - x0 );
		tz = terrain->heightSegment - ( local.z - z0 ) * terrain->heightSegment / ( z1 - z0 );

		ix = ( int )tx;
		iz = ( int )tz;

		if( ( tx -= ix ) + ( tz -= iz ) > 1 )
		{
			cIndex = (ix + 1) * gridZ + iz;
			bIndex = ix * gridZ + iz + 1;
			aIndex = cIndex + 1;
		}
		else
		{
			aIndex = ix * gridZ + iz;
			cIndex = aIndex + 1;
			bIndex = (ix + 1) * gridZ + iz;
		}

		/*b = entity -> mesh -> vertices[bIndex].position -> y - entity -> mesh -> vertices[aIndex].position -> y;
		c = entity -> mesh -> vertices[cIndex].position -> y - entity -> mesh -> vertices[aIndex].position -> y;

		//////
		///跟踪算法1
		///
		///三角形插值
		///根据网格地形三角形投影始终为直角等腰三角形
		///故点投影高度为:根号2(c-b)+bx+by
		//////
		height = invSqrt( 2 ) * tx * ( c - b ) + b * tx + b * tz + entity -> mesh -> vertices[aIndex].position -> y;*/

		//////
		///跟踪算法2:
		///
		///平面算法
		//////
		triangle_normal( & normal, terrain->parent->mesh->vertices[aIndex], terrain->parent->mesh->vertices[bIndex], terrain->parent->mesh->vertices[cIndex] );

		height = ( vector3D_dotProduct( & normal, terrain->parent->mesh->vertices[aIndex]->position ) 
				- local.x * normal.x 
				- local.z * normal.z ) / normal.y;

		/////
		///跟踪算法3
		///
		///平均值
		/////
		/*local.y = - ( entity -> mesh -> vertices[aIndex].position -> y
									      +   entity -> mesh -> vertices[bIndex].position -> y
									      +   entity -> mesh -> vertices[cIndex].position -> y
									        ) * .333333333333333333333333f;*/

		//local.y = height - ( ep->entity->mesh ? mesh_minY( ep->entity->mesh ) * ep -> entity -> scale -> y : 0 );

		//localAABB.min = & normal;
		//localAABB.max = & local;

		//aabb_zero( & localAABB );

		//aabb_setToTransformedBox( & localAABB, ep->entity->mesh->octree->data->aabb, animation->container->worldInvert );

		if( aabb_isEmpty(ep->entity->worldAABB) )
			minY = local.y;
		else
		{
			minY = terrain->parent->worldInvert->m42;

			if ( terrain->parent->worldInvert->m12 > 0.0f )
			{
				//minY += terrain->parent->worldInvert->m12 * ep->entity->mesh->octree->data->worldAABB->min->x;
				minY += terrain->parent->worldInvert->m12 * ep->entity->worldAABB->min->x;
			}
			else
			{
				//minY += terrain->parent->worldInvert->m12 * ep->entity->mesh->octree->data->worldAABB->max->x;
				minY += terrain->parent->worldInvert->m12 * ep->entity->worldAABB->max->x;
			}

			if ( terrain->parent->worldInvert->m22 > 0.0f )
			{
				//minY += terrain->parent->worldInvert->m22 * ep->entity->mesh->octree->data->worldAABB->min->y;
				minY += terrain->parent->worldInvert->m22 * ep->entity->worldAABB->min->y;
			}
			else
			{
				//minY += terrain->parent->worldInvert->m22 * ep->entity->mesh->octree->data->worldAABB->max->y;
				minY += terrain->parent->worldInvert->m22 * ep->entity->worldAABB->max->y;
			}

			if ( terrain->parent->worldInvert->m32 > 0.0f )
			{
				//minY += terrain->parent->worldInvert->m32 * ep->entity->mesh->octree->data->worldAABB->min->z;
				minY += terrain->parent->worldInvert->m32 * ep->entity->worldAABB->min->z;
			}
			else
			{
				//minY += terrain->parent->worldInvert->m32 * ep->entity->mesh->octree->data->worldAABB->max->z;
				minY += terrain->parent->worldInvert->m32 * ep->entity->worldAABB->max->z;
			}
		}

		//if(!ep->entity->matrixDirty)
		//	entity_decompose(ep->entity);

		if( ep->entity->terrainTrace == TERRAIN_TYPE_TRACE || (ep->entity->terrainTrace == TERRAIN_TYPE_BOUNDARY && height > minY) )
		{
			height -= minY;
			height *= terrain->parent->world->m22;

			ep->entity->worldAABB->min->y += height;
			ep->entity->worldAABB->max->y += height;

			ep->entity->world->m42 += height;
			ep->entity->w_pos->y = ep->entity->world->m42;
			ep->entity->worldInvert->m42 = - (ep->entity->world->m41 * ep->entity->world->m12 + ep->entity->world->m42 * ep->entity->world->m22 + ep->entity->world->m43 * ep->entity->world->m32);

			ep->entity->transform->m42 += height / ep->entity->world->m22 * ep->entity->transform->m22;
			ep->entity->position->y = ep->entity->transform->m42;
		}

		ep = ep -> next;
	}
}

INLINE void terrain_traceCamera(Terrain * terrain, Camera * target)
{
	Quaternion qua;
	Matrix4x4 quaMtr;
	Vector3D temp;
	float rotationX, rotationY;

	float height;

	float minY;

	if( target->eye->transform->m41 <= terrain->parent->worldAABB->min->x || 
		target->eye->transform->m41 >= terrain->parent->worldAABB->max->x ||
		target->eye->transform->m43 <= terrain->parent->worldAABB->min->z ||
		target->eye->transform->m43 >= terrain->parent->worldAABB->max->z )
		return;
		
	height = terrain_getHeight(terrain, target->eye->transform->m41, target->eye->transform->m43);

	minY = target->eye->transform->m42;

	if( !aabb_isEmpty(target->eye->worldAABB) )
		minY = target->eye->worldAABB->min->y;

	if( target->eye->terrainTrace == TERRAIN_TYPE_TRACE || (target->eye->terrainTrace == TERRAIN_TYPE_BOUNDARY && height > minY) )
	{
		height -= minY;

		target->eye->transform->m42 += height;

		target->eye->w_pos->y = target->eye->transform->m42;

		matrix4x4_getPosition(target->eye->position, target->eye->transform);

		if(target->isUVN)
		{
			//lookAt(target->eye->transform, target->eye->position, target->target, Y_AXIS);
			vector3D_subtract(&temp, target->target, target->eye->position);

			temp.x = DEG2RAD(temp.x);
			temp.y = DEG2RAD(temp.y);
			temp.z = DEG2RAD(temp.z);

			vector3D_directionToRotation(&rotationX, &rotationY, &temp);

			target->eye->direction->x += (rotationX - target->eye->direction->x) / target->step;
			target->eye->direction->y += (rotationY - target->eye->direction->y) / target->step;

			//单位化
			matrix4x4_identity( target->eye->transform );
			//缩放
			matrix4x4_appendScale( target->eye->transform, target->eye->scale->x, - target->eye->scale->y, target->eye->scale->z );
			//旋转
			matrix4x4_append_self( target->eye->transform, quaternoin_toMatrix( &quaMtr, quaternoin_setFromEuler( &qua, DEG2RAD( target->eye->direction->y ), DEG2RAD( target->eye->direction->x ), DEG2RAD( target->eye->direction->z ) ) ) );
			//位移
			matrix4x4_appendTranslation( target->eye->transform, target->eye->position->x, target->eye->position->y, target->eye->position->z );

			matrix4x4_copy(target->eye->world,  target->eye->transform);

			matrix4x4_getPosition(target->eye->w_pos, target->eye->world);

			matrix4x4_fastInvert(target->eye->world);

			aabb_setToTransformedBox(target->eye->worldAABB, target->eye->aabb, target->eye->transform);
		}
		else
		{
			target->eye->worldAABB->min->y += height;
			target->eye->worldAABB->max->y += height;

			target->eye->world->m42 = - (target->eye->transform->m41 * target->eye->world->m12 + target->eye->transform->m42 * target->eye->world->m22 + target->eye->transform->m43 * target->eye->world->m32);
		}
	}
}

#endif

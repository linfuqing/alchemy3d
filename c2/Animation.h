#ifndef __ANIMATION_H
#define __ANIMATION_H

//# include <time.h>
# include <ctype.h>

#include "Entity.h"

#define FRAME_NAME_LENGTH 16

typedef enum
{
	Morph,
	TerrainTrace,
	TextureCoordinates,
	Noise,
	MatrixStack
}AnimationType;

typedef struct
{
	char         name[FRAME_NAME_LENGTH];

	int          time;

	Vector3D   * vertices;

	unsigned int length;
}Frame;

typedef struct Animation
{
	AnimationType  type;

	int            loop;
	int            isPlay;

	union
	{
		Mesh   * parent;
		Entity * container;
	};

	union
	{
		struct
		{
			Frame        * frames;
			char           currentFrameName[FRAME_NAME_LENGTH];
			int            dirty;
			int            maxTime;
			int            minTime;
			int            durationTime;
			int            startTime;
			unsigned int   length;
		};

		struct
		{
			int  widthSegment;
			int heightSegment;
		};
	};

	struct Animation * next;
}Animation;

typedef struct
{
	Animation  * animation;
	unsigned int length;
}Movie;

Animation   * newMorphAnimation( Mesh * parent, Frame * frames, unsigned int length, int duration )
{
	Animation * a;

	if( ( a = ( Animation * ) malloc( sizeof( Animation ) ) ) == NULL )
	{
		exit( TRUE );
	}

	a -> type                = Morph;
	a -> parent              = parent;
	a -> frames              = frames;
	a -> dirty               = FALSE;
	a -> maxTime             = duration;
	a -> minTime             = 0;
	a -> length              = length;
	a -> loop                = TRUE;
	a -> isPlay              = TRUE;
	a -> startTime           = 0;
	a -> durationTime        = duration;
	a -> next                = NULL;
	//a -> currentFrameIndex = 0;

	return a;
}

///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////Morph//////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void animation_morph_updateToFrame( Animation * animation, unsigned int keyFrame )
{
	DWORD i;

	if( animation -> length == 0 || animation -> length < keyFrame || animation -> parent == NULL )
	{
		return;
	}

	for( i = 0; i < ( animation -> parent -> nVertices ); i ++ )
	{
		animation -> parent -> vertices[i]->position -> x = animation -> frames[keyFrame].vertices[i].x;
		animation -> parent -> vertices[i]->position -> y = animation -> frames[keyFrame].vertices[i].y;
		animation -> parent -> vertices[i]->position -> z = animation -> frames[keyFrame].vertices[i].z;
	}
}

void animation_morph_updateToTime( Animation * animation, float timeAlpha )
{
	float frameAlpha;
	unsigned int   currentFrameIndex = ( unsigned int )floor( ( animation -> length - 1 ) * timeAlpha );
	DWORD i;

	if( animation -> length == 0 || animation -> parent == NULL  )
	{
		return;
	}

	currentFrameIndex = currentFrameIndex < ( animation -> length - 1 ) ? currentFrameIndex : 0;

	frameAlpha        = ( timeAlpha *   ( animation -> durationTime )     - animation -> frames[currentFrameIndex].time ) / 
					    ( animation -> frames[currentFrameIndex + 1].time - animation -> frames[currentFrameIndex].time );

	frameAlpha        = frameAlpha > 1 ? 1 : ( frameAlpha < 0 ? 0 : frameAlpha );

	//AS3_Trace( AS3_Number( timeAlpha ) );
	//AS3_Trace( AS3_Number( frameAlpha ) );

	for( i = 0; i < ( animation -> parent -> nVertices ); i ++ )
	{
		animation -> parent -> vertices[i]->position -> x = animation -> frames[currentFrameIndex].vertices[i].x + frameAlpha * ( animation -> frames[currentFrameIndex + 1].vertices[i].x - animation -> frames[currentFrameIndex].vertices[i].x );
		animation -> parent -> vertices[i]->position -> y = animation -> frames[currentFrameIndex].vertices[i].y + frameAlpha * ( animation -> frames[currentFrameIndex + 1].vertices[i].y - animation -> frames[currentFrameIndex].vertices[i].y );
		animation -> parent -> vertices[i]->position -> z = animation -> frames[currentFrameIndex].vertices[i].z + frameAlpha * ( animation -> frames[currentFrameIndex + 1].vertices[i].z - animation -> frames[currentFrameIndex].vertices[i].z );
	}

	animation->parent->v_dirty = TRUE;
}

//可优化函数,将动作指令存储到内存中
int animation_morph_updateToName( Animation * animation, char name[FRAME_NAME_LENGTH] )
{
	DWORD i;
	int min = OFF, max = OFF, match;
	char * ch = NULL;

	if( !strcmp( name, "all" ) )
	{
		min = 0;
		max = animation -> durationTime;
	}
	else
	{
		for( i = 0; i < animation -> length; i ++ )
		{
			if( ( ch = strstr( animation -> frames[i].name, name ) ) == animation -> frames[i].name )
			{
				ch += strlen( name );

				match = TRUE;
			
				while( * ch != '\0' )
				{
					if( !isdigit( ( int )( * ch ++ ) ) )
					{
						match = FALSE;
						break;
					}
				}

				if( match )
				{
					min = min == OFF ? i : min;
					max = i;
				}
			}
			else if( min != OFF )
			{
				break;
			}
		}
	}

	if( min != OFF )
	{
		animation -> minTime = animation -> frames[min].time;
		animation -> maxTime = animation -> frames[max].time;

		return TRUE;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////TerrainTrace///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

Animation * newTerrainTraceAnimation( Entity * container, int widthSegment, int heightSegment )
{
	Animation * a;

	if( ( a = ( Animation * ) malloc( sizeof( Animation ) ) ) == NULL )
	{
		exit( TRUE );
	}

	a -> type          = TerrainTrace;
	a -> container     = container;
	a -> widthSegment  = widthSegment;
	a -> heightSegment = heightSegment;
	a -> next          = NULL;

	return a;
}

void animation_terrainTrace_update( Animation * animation )
{
	if( animation->container->scene )
	{
		float	tx, tz,
				x0 = animation->container->mesh->octree->data->aabb->min->x,
				x1 = animation->container->mesh->octree->data->aabb->max->x,
				z0 = animation->container->mesh->octree->data->aabb->min->z,
				z1 = animation->container->mesh->octree->data->aabb->max->z;

		float height, minY;

		int gridZ = animation->container->mesh->heightSegment + 1,
			ix, iz, aIndex, bIndex, cIndex;

		Vector3D normal, local;

		SceneNode * ep = animation->container->scene->nodes;

		while( ep )
		{
			if(	! ep->entity->mesh || ! ep->entity->mesh->terrainTrace || ep->entity->mesh->type == ENTITY_TYPE_MESH_TERRAIN )
			{
				ep = ep -> next;

				continue;
			}

			//把实体位置变换到地形的私有坐标系
			matrix3D_transformVector( & local, animation->container->worldInvert, ep->entity->w_pos );

			if(	local.x < x0 
			||  local.x > x1
			||	local.z < z0 
			||  local.z > z1 )
			{
				ep = ep -> next;

				continue;
			}

			tx = ( local.x - x0 ) * animation->container->mesh->widthSegment / ( x1 - x0 );
			tz = animation->container->mesh->heightSegment - ( local.z - z0 ) * animation->container->mesh->heightSegment / ( z1 - z0 );

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
			triangle_normal( & normal, animation->container->mesh->vertices[aIndex], animation->container->mesh->vertices[bIndex], animation->container->mesh->vertices[cIndex] );

			height = ( vector3D_dotProduct( & normal, animation->container->mesh->vertices[aIndex]->position ) 
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

			minY = animation->container->worldInvert->m42;

			if ( animation->container->worldInvert->m12 > 0.0f )
			{
				minY += animation->container->worldInvert->m12 * ep->entity->mesh->octree->data->worldAABB->min->x;
			}
			else
			{
				minY += animation->container->worldInvert->m12 * ep->entity->mesh->octree->data->worldAABB->max->x;
			}

			if ( animation->container->worldInvert->m22 > 0.0f )
			{
				minY += animation->container->worldInvert->m22 * ep->entity->mesh->octree->data->worldAABB->min->y;
			}
			else
			{
				minY += animation->container->worldInvert->m22 * ep->entity->mesh->octree->data->worldAABB->max->y;
			}

			if ( animation->container->worldInvert->m32 > 0.0f )
			{
				minY += animation->container->worldInvert->m32 * ep->entity->mesh->octree->data->worldAABB->min->z;
			}
			else
			{
				minY += animation->container->worldInvert->m32 * ep->entity->mesh->octree->data->worldAABB->max->z;
			}

			ep->entity->position->y += ( height - minY );

			ep->entity->transformDirty = TRUE;

			ep = ep -> next;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////TextureCoordinates/////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

Animation * newTextureCoordinatesAnimation( Mesh * parent, float widthSegment, float heightSegment )
{
	Animation * a;

	if( ( a = ( Animation * ) malloc( sizeof( Animation ) ) ) == NULL )
	{
		exit( TRUE );
	}

	a -> type          = TextureCoordinates;
	a -> parent        = parent;

	return a;
}

void animation_textureCoordinates_update( Animation * animation )
{
	DWORD i;

	for( i = 0; i < animation -> parent -> nFaces; i ++ )
	{
		animation -> parent -> faces[i] -> uv[0] -> u += .001f;
		animation -> parent -> faces[i] -> uv[0] -> v += .001f;

		animation -> parent -> faces[i] -> uv[1] -> u += .001f;
		animation -> parent -> faces[i] -> uv[1] -> v += .001f;

		animation -> parent -> faces[i] -> uv[2] -> u += .001f;
		animation -> parent -> faces[i] -> uv[2] -> v += .001f;

		animation -> parent -> faces[i] -> uvTransformed = FALSE;
	}
}

void animation_update( Animation * animation, int time )
{
	Animation * ap = animation;

	while( ap != NULL )
	{
		switch( ap -> type )
		{
		case Morph:

			if( ap -> dirty )
			{
				ap -> dirty = FALSE;

				animation_morph_updateToName( ap, ap -> currentFrameName );
			}

			if( ap -> isPlay )
			{
				//animation -> currentFrameIndex = ( animation -> currentFrameIndex ) > ( animation -> length ) ? 0 : ( animation -> currentFrameIndex );

				//animation_updateToFrame( animation, animation -> currentFrameIndex );

				//animation -> currentFrameIndex ++;

				int elapsed  = time - animation -> startTime + animation -> minTime;

				if( elapsed > animation -> maxTime )
				{
					animation -> startTime = time;

					elapsed                = animation -> minTime;

					animation -> isPlay    = animation -> loop ? TRUE : FALSE;
				}

				animation_morph_updateToTime( animation, elapsed * 1.0f / animation -> durationTime );
			}

			break;

		case TerrainTrace:

			animation_terrainTrace_update( animation );

			break;

		case TextureCoordinates:
			break;

		case Noise:
			break;

		case MatrixStack:
			break;

		default:
			break;
		}

		ap = ap -> next;
	}
}

#endif

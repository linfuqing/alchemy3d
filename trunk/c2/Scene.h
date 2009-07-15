#ifndef __SCENE_H
#define __SCENE_H

#include "Entity.h"
#include "Light.h"

typedef struct Lights
{
	Light * light;

	struct Lights * next;
}Lights;

typedef struct SceneNode
{
	Entity * entity;

	struct SceneNode * next;
}SceneNode;

typedef struct Scene
{
	int nNodes, nVertices, nFaces, nLights, isAttached;

	struct SceneNode * nodes;

	struct Lights * lights;
}Scene;

Scene * newScene()
{
	Scene * scene;

	if( ( scene = ( Scene * )malloc( sizeof( Scene ) ) ) == NULL )
	{
		exit( TRUE );
	}

	scene->nFaces = scene->nNodes = scene->nVertices = 0;
	scene->nodes = NULL;
	scene->lights = NULL;
	scene->isAttached = FALSE;

	return scene;
}

//------------------------Entity----------------------
SceneNode * scene_findEntity( SceneNode * node, Entity * entity )
{   
	node = node->next;

	if ( node == NULL )
	{
		return NULL;
	}
	else
	{
		if ( entity == node->entity )
		{
			return node;
		}
		else
		{
			return scene_findEntity( node->next, entity );
		}
	}
}

void scene_addEntity(Scene * scene, Entity * entity, Entity * parent)
{
	SceneNode * p, * n;

	if (parent != NULL)
	{
		p = scene_findEntity(scene->nodes, parent);

		if (p == NULL)
		{
			exit(TRUE);
		}

		entity->parent = parent;
	}
	else
	{
		p = scene->nodes;

		if (NULL != p)
		{
			while( NULL != p->next )
			{
				p = p->next;
			}
		}
	}

	if( ( n = ( SceneNode * )malloc( sizeof( SceneNode ) ) ) == NULL )
	{
		exit( TRUE );
	}

	n->entity = entity;

	if (NULL == p)
	{
		n->next = NULL;

		scene->nodes = n;
	}
	else
	{
		n->next = p->next;

		p->next = n;
	}

	scene->nNodes ++;
	scene->nFaces += n->entity->mesh->nFaces;
	scene->nVertices += n->entity->mesh->nVertices;
}

int scene_removeEntityAt( Scene * scene, int i )
{
	SceneNode * p, * s;

	int j = 1;

	if( i > scene->nNodes - 1)
	{
		//printf( "删除位置参数错误!" );
		return FALSE;
	}
	else if ( 0 == i)
	{
		s = scene->nodes;

		scene->nodes = s->next;
	}
	else
	{
		p = scene->nodes;

		for (; j < i; j ++)
		{
			p = p->next;
		}

		s = p->next;

		p->next = s->next;
	}

	scene->nFaces --;

	free(s);

	return TRUE;
}
//------------------------end Entity----------------------

//------------------------Light----------------------
int scene_addLight(Scene * scene, Light * light)
{
	Lights * p, * q;

	p = scene->lights;

	if (NULL != p)
	{
		while( NULL != p->next )
		{
			p = p->next;
		}
	}

	if( ( q = ( Lights * )malloc( sizeof( Lights ) ) ) == NULL )
	{
		exit( TRUE );
	}

	q->light = light;

	q->next = NULL;

	if (NULL == p)
	{
		scene->lights = q;
	}
	else
	{
		p->next = q;
	}

	scene->nLights ++;

	return TRUE;
}

int scene_removeLightAt( Scene * scene, int i )
{
	Lights * p, * s;

	int j = 1;

	if( i > scene->nLights - 1)
	{
		//printf( "删除位置参数错误!" );
		return FALSE;
	}
	else if ( 0 == i)
	{
		s = scene->lights;

		scene->lights = s->next;
	}
	else
	{
		p = scene->lights;

		for (; j < i; j ++)
		{
			p = p->next;
		}

		s = p->next;

		p->next = s->next;
	}

	scene->nLights --;

	free(s);

	return TRUE;
}
//------------------------end Light----------------------

void scene_destroy( Scene * * head )
{
	SceneNode * p, * p1;

	p = (* head)->nodes;

	while( p != NULL )
	{
		p1 = p;

		p = p->next;

		free( p1 );
	}

	free(* head);

	* head = NULL;
}

void scene_update(Scene * scene)
{
	AABB * aabb;

	SceneNode * p;
	Entity * entity;
	VertexNode * v;
	Matrix3D * world;

	Lights * lights;
	Light * light;
	Vector3D lightToLocal, dstns;
	Color a, d;

	p = scene->nodes;

	//遍历场景
	do
	{
		entity = p->entity;

		//===================更新矩阵===================
		world = entity->world;

		entity_updateTransform(entity);	//更新本地矩阵

		if( NULL != entity->parent )
		{
			matrix3D_append(world, entity->parent->world);	//连接父世界矩阵
		}

		if ( NULL != entity->mesh )
		{
			//===================计算AABB===================
			aabb = entity->mesh->worldAABB;

			matrix3D_transformVector( world, aabb->min );
			matrix3D_transformVector( world, aabb->max );

			aabb_createFromSelf( & aabb );

			v = entity->mesh->vertices->nodes;

			//===================顶点变换===================
			while ( NULL != v )
			{
				vector3D_copy( v->vertex->worldPosition, v->vertex->position );

				matrix3D_transformVector( world, v->vertex->worldPosition );	//变换顶点到世界坐标系

				//===================计算光照和顶点颜色值===================
				lights = scene->lights;

				while ( LIGHT_ENABLE == 1 && NULL != lights )
				{
					light = lights->light;

					entity_updateTransform(light->source);

					vector3D_copy( & lightToLocal, light->source->worldPosition );

					matrix3D_transformVector( entity->worldInvert, & lightToLocal );

					vector3D_subtract( & dstns, & lightToLocal, v->vertex->position );

					vector3D_normalize( & dstns );

					//环境光
					color_append( & a, entity->material->ambient, light->ambient );
					//漫反射
					color_append( & d, entity->material->diffuse, light->diffuse );
					//反射光线（看到的颜色）
					v->vertex->color = color_add_self( & a, color_scaleBy_self( & d, vector3D_dotProduct( & dstns, v->vertex->normal ) ) );

					lights = lights->next;
				}

				v = v -> next;
			}
		}

		p = p->next;
	}
	while( NULL != p );
}

#endif
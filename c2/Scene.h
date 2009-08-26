#ifndef __SCENE_H
#define __SCENE_H

#include "Entity.h"
#include "Light.h"

typedef struct Scene
{
	int nNodes, nVertices, nFaces, nLights, lightOn, isAttached, dirty;

	struct SceneNode * nodes;

	struct Viewport * viewport;

	struct Lights * lights;
}Scene;

Scene * newScene()
{
	Scene * scene;

	if( ( scene = ( Scene * )malloc( sizeof( Scene ) ) ) == NULL )
	{
		exit( TRUE );
	}

	scene->nFaces = scene->nNodes = scene->nVertices = scene->nLights = scene->lightOn = 0;
	scene->nodes = NULL;
	scene->lights = NULL;
	scene->dirty = FALSE;
	scene->isAttached = FALSE;

	return scene;
}

//------------------------Entity----------------------
SceneNode * scene_findEntity( SceneNode * node, Entity * entity )
{
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

SceneNode * scene_findEntity2( SceneNode * node, Entity * entity )
{
	if ( node == NULL )
	{
		return NULL;
	}
	else
	{
		if ( entity == node->next->entity )
		{
			return node;
		}
		else
		{
			return scene_findEntity( node->next, entity );
		}
	}
}

void scene_addEntity( Scene * scene, Entity * entity, Entity * parent )
{
	SceneNode * sceneNode, * n;

	if (parent != NULL)
	{
		sceneNode = scene_findEntity( scene->nodes, parent );

		if (sceneNode == NULL)
		{
			exit(TRUE);
		}

		entity->parent = parent;
		entity_addChild( parent, entity );
	}
	else
	{
		sceneNode = scene->nodes;

		if (NULL != sceneNode)
		{
			while( NULL != sceneNode->next )
			{
				sceneNode = sceneNode->next;
			}
		}
	}

	if( ( n = ( SceneNode * )malloc( sizeof( SceneNode ) ) ) == NULL )
	{
		exit( TRUE );
	}

	n->entity = entity;
	n->entity->scene = scene;

	if (NULL == sceneNode)
	{
		n->next = NULL;

		scene->nodes = n;
	}
	else
	{
		n->next = sceneNode->next;

		sceneNode->next = n;
	}

	scene->nNodes ++;
	scene->dirty = TRUE;

	if ( NULL != entity->mesh )
	{
		scene->nFaces += n->entity->mesh->nFaces;
		scene->nVertices += n->entity->mesh->nVertices;
	}
}

void scene_removeEntity( Scene * scene, Entity * entity )
{
	SceneNode * sceneNode, * s;

	if ( NULL != entity->children )
	{
		s = entity->children;

		while ( NULL != s )
		{
			scene_removeEntity( scene, s->entity );

			s = s->next;
		}
	}

	sceneNode = scene->nodes;

	if ( sceneNode->entity == entity )
	{
		scene->nodes = scene->nodes->next;
	}
	else
	{
		s = scene_findEntity2( sceneNode, entity );

		s->next = s->next->next;
	}

	entity->scene = NULL;

	scene->nNodes --;
	scene->dirty = TRUE;
	scene->nFaces -= entity->mesh->nFaces;
	scene->nVertices -= entity->mesh->nVertices;
}

int scene_removeEntityAt( Scene * scene, int i )
{
	SceneNode * sceneNode, * s;

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
		sceneNode = scene->nodes;

		for (; j < i; j ++)
		{
			sceneNode = sceneNode->next;
		}

		s = sceneNode->next;

		sceneNode->next = s->next;
	}

	scene->nNodes --;
	scene->dirty = TRUE;
	scene->nFaces -= s->entity->mesh->nFaces;
	scene->nVertices -= s->entity->mesh->nVertices;

	return TRUE;
}
//------------------------end Entity----------------------

//------------------------Light----------------------
int scene_addLight(Scene * scene, Light * light)
{
	Lights * lights, * q;

	lights = scene->lights;

	if (NULL != lights)
	{
		while( NULL != lights->next )
		{
			lights = lights->next;
		}
	}

	if( ( q = ( Lights * )malloc( sizeof( Lights ) ) ) == NULL )
	{
		exit( TRUE );
	}

	q->light = light;

	q->next = NULL;

	if (NULL == lights)
	{
		scene->lights = q;
	}
	else
	{
		lights->next = q;
	}

	scene->nLights ++;

	return TRUE;
}

Lights * scene_findLight( Lights * node, Light * light )
{
	if ( node == NULL )
	{
		return NULL;
	}
	else
	{
		if ( light == node->next->light )
		{
			return node;
		}
		else
		{
			return scene_findLight( node->next, light );
		}
	}
}

void scene_removeLight( Scene * scene, Light * light )
{
	Lights * lights, * s;

	lights = scene->lights;

	if ( lights->light == light )
	{
		scene->lights = scene->lights->next;
	}
	else
	{
		s = scene_findLight( lights, light );

		s->next = s->next->next;
	}

	scene->nLights --;
}

int scene_removeLightAt( Scene * scene, int i )
{
	Lights * lights, * s;

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
		lights = scene->lights;

		for (; j < i; j ++)
		{
			lights = lights->next;
		}

		s = lights->next;

		lights->next = s->next;
	}

	scene->nLights --;

	return TRUE;
}
//------------------------end Light----------------------

void scene_dispose( Scene * head )
{
	/*SceneNode * sceneNode, * p1;
	Lights * lights, * p2;

	sceneNode = (* head)->nodes;

	while( sceneNode != NULL )
	{
		p1 = sceneNode;

		sceneNode = sceneNode->next;

		free( p1 );
	}

	lights = (* head)->lights;

	while( lights != NULL )
	{
		p2 = lights;

		lights = lights->next;

		free( p2 );
	}*/
	SceneNode * sceneNode = head->nodes;
	Lights * lights = head->lights;

	while ( NULL != sceneNode)
	{
		sceneNode->entity = NULL;
		sceneNode = sceneNode->next;
		sceneNode->next = NULL;
	}

	while ( NULL != lights)
	{
		lights->light = NULL;
		lights = lights->next;
		lights->next = NULL;
	}

	free(head);

	head = NULL;
}

//更新场景
void scene_project(Scene * scene)
{
	SceneNode * sceneNode;
	Entity * entity;
	Vertex * v;
	int i = 0;
#ifdef __AS3__
	int j = 0;
	float * meshBuffer;
#endif

	sceneNode = scene->nodes;

	//遍历场景结点，更新实体
	while( NULL != sceneNode )
	{
		//==================实体===================
		entity = sceneNode->entity;

		//更新实体矩阵
		entity_updateTransform(entity);

		//如果实体有网格
		if ( NULL != entity->mesh )
		{
			//===================顶点===================
			//遍历顶点
			v = entity->mesh->vertices;
#ifdef __AS3__
			meshBuffer = entity->mesh->meshBuffer;

			for( i = 0, j = 0; i < entity->mesh->nVertices; i ++, j += VERTEX_SIZE)
			{
				//如果顶点局部坐标发生改变
				if ( TRUE == entity->mesh->dirty )
				{
					v[i].position->x = meshBuffer[j];
					v[i].position->y = meshBuffer[j + 1];
					v[i].position->z = meshBuffer[j + 2];
				}

				matrix3D_transformVector( v[i].w_pos, entity->world, v[i].position );
			}
#else
			for( i = 0; i < entity->mesh->nVertices; i ++)
			{
				//用世界矩阵把顶点变换到世界坐标系
				matrix3D_transformVector( v[i].w_pos, entity->world, v[i].position );
			}
#endif
		}

		sceneNode = sceneNode->next;
	}//end entity
}

//void scene_updateBeforeRender( Scene * scene )
//{
//}

void scene_updateAfterRender( Scene * scene )
{
	SceneNode * sceneNode;

	sceneNode = scene->nodes;

	//遍历场景结点，更新实体
	while( NULL != sceneNode )
	{
		entity_updateAfterRender( sceneNode->entity );

		sceneNode = sceneNode->next;
	}

	scene->lightOn = FALSE;
	scene->dirty = FALSE;
}

#endif
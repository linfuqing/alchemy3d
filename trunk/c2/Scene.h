#ifndef __SCENE_H
#define __SCENE_H

#include <malloc.h>

#include "Entity.h"
#include "Light.h"
#include "Fog.h"

typedef struct Scene
{
	int nLights, dirty;

	DWORD nNodes, nVertices, nFaces,nRenderList, nClippList, nCullList;

	struct SceneNode * nodes;

	struct Viewport * viewport;

	struct Lights * lights;

	struct Fog * fog;

}Scene;

Scene * newScene()
{
	Scene * scene;

	if( ( scene = ( Scene * )malloc( sizeof( Scene ) ) ) == NULL )
	{
		exit( TRUE );
	}

	scene->nFaces		= scene->nNodes = scene->nVertices = scene->nLights = 0;
	scene->nRenderList	= scene->nClippList = scene->nCullList = 0;
	scene->nodes		= NULL;
	scene->lights		= NULL;
	scene->fog			= NULL;
	scene->dirty		= FALSE;

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
	SceneNode * sceneNode, * n, * child;

	if (parent != NULL)
	{
		if ( ! parent->scene ) exit( TRUE );

		sceneNode = scene_findEntity( scene->nodes, parent );

		if (sceneNode == NULL)
		{
			exit(TRUE);
		}

		if ( ! entity->parent ) entity_addChild( parent, entity );
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

	//扫描孩子列表，查看是否有孩子没有加入场景列表，是则添加之
	child = entity->children;

	while ( NULL != child )
	{
		if ( ! child->entity->scene )
		{
			scene_addEntity( scene, child->entity, entity );
		}

		child = child->next;
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

int scene_removeEntityAt( Scene * scene, DWORD i )
{
	SceneNode * sceneNode, * s;

	DWORD j = 1;

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

	if ( scene->nLights > MAX_LIGHTS ) exit( TRUE );

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

//------------------------Fog----------------------
int scene_addFog(Scene * scene, Fog * fog)
{
	if ( scene->fog ) return 0;

	scene->fog = fog;

	return 1;
}

int scene_removeFog(Scene * scene, Fog * fog)
{
	if ( ! scene->fog ) return 0;

	scene->fog = NULL;

	return 1;
}
//------------------------end Fog----------------------

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

void scene_updateAfterRender( Scene * scene )
{
	SceneNode * sceneNode;

	sceneNode = scene->nodes;

	scene->nNodes = scene->nFaces = scene->nRenderList = scene->nClippList = scene->nCullList = 0;

	while( NULL != sceneNode )
	{
		scene->nNodes ++;
		scene->nFaces += sceneNode->entity->mesh->nFaces;
		scene->nRenderList += sceneNode->entity->mesh->nRenderList;
		scene->nClippList += sceneNode->entity->mesh->nClippList;
		scene->nCullList += sceneNode->entity->mesh->nCullList;

		entity_updateAfterRender( sceneNode->entity );

		sceneNode = sceneNode->next;
	}

	scene->dirty = FALSE;
}

#endif
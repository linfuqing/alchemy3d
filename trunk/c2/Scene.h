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

	scene->nFaces = scene->nNodes = scene->nVertices = scene->nLights = 0;
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
	SceneNode * sceneNode, * n;

	if (parent != NULL)
	{
		sceneNode = scene_findEntity(scene->nodes, parent);

		if (sceneNode == NULL)
		{
			exit(TRUE);
		}

		entity->parent = parent;
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
	scene->nFaces += n->entity->mesh->nFaces;
	scene->nVertices += n->entity->mesh->nVertices;
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

	scene->nFaces --;

	free(s);

	return TRUE;
}
//------------------------end Entity----------------------

//------------------------Light----------------------
int scene_addLight(Scene * scene, Light * light)
{
	Lights * sceneNode, * q;

	sceneNode = scene->lights;

	if (NULL != sceneNode)
	{
		while( NULL != sceneNode->next )
		{
			sceneNode = sceneNode->next;
		}
	}

	if( ( q = ( Lights * )malloc( sizeof( Lights ) ) ) == NULL )
	{
		exit( TRUE );
	}

	q->light = light;

	q->next = NULL;

	if (NULL == sceneNode)
	{
		scene->lights = q;
	}
	else
	{
		sceneNode->next = q;
	}

	scene->nLights ++;

	return TRUE;
}

int scene_removeLightAt( Scene * scene, int i )
{
	Lights * sceneNode, * s;

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
		sceneNode = scene->lights;

		for (; j < i; j ++)
		{
			sceneNode = sceneNode->next;
		}

		s = sceneNode->next;

		sceneNode->next = s->next;
	}

	scene->nLights --;

	free(s);

	return TRUE;
}
//------------------------end Light----------------------

void scene_destroy( Scene * * head )
{
	SceneNode * sceneNode, * p1;

	sceneNode = (* head)->nodes;

	while( sceneNode != NULL )
	{
		p1 = sceneNode;

		sceneNode = sceneNode->next;

		free( p1 );
	}

	free(* head);

	* head = NULL;
}

void scene_update(Scene * scene)
{
	AABB * aabb;

	SceneNode * sceneNode;
	Entity * entity;
	VertexNode * vNode;
	Matrix3D * world;

	Lights * lights;
	Light * light;
	Vector3D dstns, * lightToLocals;
	Color d;
	float dot;
	int i = 0;

	if( ( lightToLocals = ( Vector3D * )malloc( sizeof( Vector3D ) * scene->nLights ) ) == NULL )
	{
		exit( TRUE );
	}

	//===================更新光源位置===================
	lights = scene->lights;
	//遍历光源
	while ( LIGHT_ENABLE == 1 && NULL != lights)
	{
		light = lights->light;

		light_updateTransform(light);

		lights = lights->next;
	}

	sceneNode = scene->nodes;

	//遍历场景结点
	while( NULL != sceneNode )
	{
		entity = sceneNode->entity;

		//===================更新矩阵===================
		world = entity->world;

		entity_updateTransform(entity);	//更新本地矩阵

		if( NULL != entity->parent )
		{
			matrix3D_append(world, entity->parent->world);	//连接父世界矩阵
		}

		//===================更新光源位置===================
		lights = scene->lights;

		//遍历光源
		while ( LIGHT_ENABLE == 1 && NULL != lights)
		{
			light = lights->light;

			vector3D_copy( & lightToLocals[i], light->source->worldPosition );

			matrix3D_transformVector( entity->worldInvert, & lightToLocals[i] );

			lights = lights->next;

			i ++;
		}

		i = 0;

		if ( NULL != entity->mesh )
		{
			//===================计算AABB===================
			aabb = entity->mesh->worldAABB;

			matrix3D_transformVector( world, aabb->min );
			matrix3D_transformVector( world, aabb->max );

			aabb_createFromSelf( & aabb );

			vNode = entity->mesh->vertices->nodes;

			//===================顶点变换===================
			//遍历顶点
			while ( NULL != vNode )
			{
				vector3D_copy( vNode->vertex->worldPosition, vNode->vertex->position );

				matrix3D_transformVector( world, vNode->vertex->worldPosition );	//变换顶点到世界坐标系

				//===================计算光照和顶点颜色值===================
				lights = scene->lights;
				//遍历光源
				while ( LIGHT_ENABLE == 1 &&  entity->material != NULL && NULL != lights)
				{
					vector3D_subtract( & dstns, & lightToLocals[i], vNode->vertex->position );

					vector3D_normalize( & dstns );
					/*AS3_Trace(AS3_String("normal"));
					AS3_Trace(AS3_Number(dstns.z));*/

					//环境光
					color_append( vNode->vertex->color, entity->material->ambient, light->ambient );					
					/*AS3_Trace(AS3_String("vcolor"));
					AS3_Trace(AS3_Number(vNode->vertex->color->red));*/

					dot = vector3D_dotProduct( & dstns, vNode->vertex->normal );

					if ( dot >= 0.0f )
					{
						//漫反射
						color_append( & d, entity->material->diffuse, light->diffuse );
						/*AS3_Trace(AS3_String("di.color"));
						AS3_Trace(AS3_Number(d.red));*/

						//反射光
						color_add_self( vNode->vertex->color, color_scaleBy_self( & d, dot, dot, dot, 1 ) );
					}
					/*AS3_Trace(AS3_String("fanshe.color"));
					AS3_Trace(AS3_Number(vNode->vertex->color->red));
					AS3_Trace(AS3_Number(xxx));*/

					lights = lights->next;
				}

				vNode = vNode -> next;
			}
		}

		sceneNode = sceneNode->next;
	}

	free( lightToLocals );
}

#endif
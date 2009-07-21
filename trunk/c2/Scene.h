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

//更新场景
void scene_update(Scene * scene)
{
	AABB * aabb, * worldAabb;

	SceneNode * sceneNode;
	Entity * entity;
	VertexNode * vNode;
	Matrix3D * world;

	Vector3D * lightDirs;
	Lights * lights;
	Light * light;
	Color d;
	float dot;
	int i = 0;

	//此数组用于记录以本地作为参考点的光源方向
	if( ( lightDirs = ( Vector3D * )malloc( sizeof( Vector3D ) * scene->nLights ) ) == NULL )
	{
		exit( TRUE );
	}

	//==================光源===================
	lights = scene->lights;
	//遍历光源
	while ( LIGHT_ENABLE == 1 && NULL != lights)
	{
		light = lights->light;

		//更新光源矩阵
		light_updateTransform(light);

		lights = lights->next;
	}

	sceneNode = scene->nodes;

	//遍历场景结点，更新实体
	while( NULL != sceneNode )
	{
		//==================实体===================
		entity = sceneNode->entity;
		world = entity->world;

		//更新实体矩阵
		entity_updateTransform(entity);

		//如果存在父结点，则连接父结点世界矩阵
		if( NULL != entity->parent )
			matrix3D_append_self(world, entity->parent->world);

		//更新光源位置
		lights = scene->lights;

		//遍历光源
		while ( LIGHT_ENABLE == 1 && NULL != lights)
		{
			light = lights->light;

			//用实体的世界逆矩阵变换光源的位置得到以实体为参考点的光源的方向，保存在数组
			matrix3D_transformVector( & lightDirs[i], entity->worldInvert, light->source->worldPosition );

			vector3D_normalize( & lightDirs[i] );

			lights = lights->next;

			i ++;
		}

		//如果实体有网格
		if ( NULL != entity->mesh )
		{
			//===================AABB===================
			aabb = entity->mesh->aabb;
			worldAabb = entity->mesh->worldAABB;

			//用实体的世界矩阵变换它的AABB到世界坐标系
			matrix3D_transformVector( worldAabb->min, world, aabb->min );
			matrix3D_transformVector( worldAabb->max, world, aabb->max );

			//根据变换后的AABB重新构造新的AABB（这也就是实体经过旋转、位移、缩放后的新AABB）
			aabb_createFromSelf( & worldAabb );

			//===================顶点===================
			vNode = entity->mesh->vertices->nodes;
			//遍历顶点
			while ( NULL != vNode )
			{
				i = 0;
				//用实体的世界矩阵把顶点变换到世界坐标系
				matrix3D_transformVector( vNode->vertex->worldPosition, world, vNode->vertex->position );

				//===================光照和顶点颜色值===================
				lights = scene->lights;
				//遍历光源
				while ( LIGHT_ENABLE == 1 &&  NULL != entity->material && NULL != lights)
				{
					//计算环境光（光源颜色乘以材质颜色），保存在顶点颜色
					color_append( vNode->vertex->color, entity->material->ambient, light->ambient );

					/*vector3D_subtract( & dstns, & lightDirs[i], vNode->vertex->position );
					vector3D_normalize( & dstns );
					dot = vector3D_dotProduct( & dstns, vNode->vertex->normal );*/

					//光源和顶点的夹角
					dot = vector3D_dotProduct( & lightDirs[i], vNode->vertex->normal );

					//如果夹角大于0，即夹角范围在(-90, 90)之间
					if ( dot > 0.0f )
					{
						//漫反射（光源漫反射颜色乘以材质漫反射颜色），保存在临时颜色值d
						color_append( & d, entity->material->diffuse, light->diffuse );

						//反射光（根据夹角大小对 d 进行颜色的缩放，最后和顶点当前颜色值相加）
						color_add_self( vNode->vertex->color, color_scaleBy_self( & d, dot, dot, dot, 1 ) );
					}

					lights = lights->next;

					//i ++;
				}

				vNode = vNode -> next;
			}
		}

		sceneNode = sceneNode->next;
	}

	free( lightDirs );
	lightDirs = NULL;
}

#endif
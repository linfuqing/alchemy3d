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
		//printf( "ɾ��λ�ò�������!" );
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
		//printf( "ɾ��λ�ò�������!" );
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

//���³���
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

	//���������ڼ�¼�Ա�����Ϊ�ο���Ĺ�Դ����
	if( ( lightDirs = ( Vector3D * )malloc( sizeof( Vector3D ) * scene->nLights ) ) == NULL )
	{
		exit( TRUE );
	}

	//==================��Դ===================
	lights = scene->lights;
	//������Դ
	while ( LIGHT_ENABLE == 1 && NULL != lights)
	{
		light = lights->light;

		//���¹�Դ����
		light_updateTransform(light);

		lights = lights->next;
	}

	sceneNode = scene->nodes;

	//����������㣬����ʵ��
	while( NULL != sceneNode )
	{
		//==================ʵ��===================
		entity = sceneNode->entity;
		world = entity->world;

		//����ʵ�����
		entity_updateTransform(entity);

		//������ڸ���㣬�����Ӹ�����������
		if( NULL != entity->parent )
			matrix3D_append_self(world, entity->parent->world);

		//���¹�Դλ��
		lights = scene->lights;

		//������Դ
		while ( LIGHT_ENABLE == 1 && NULL != lights)
		{
			light = lights->light;

			//��ʵ������������任��Դ��λ�õõ���ʵ��Ϊ�ο���Ĺ�Դ�ķ��򣬱���������
			matrix3D_transformVector( & lightDirs[i], entity->worldInvert, light->source->worldPosition );

			vector3D_normalize( & lightDirs[i] );

			lights = lights->next;

			i ++;
		}

		//���ʵ��������
		if ( NULL != entity->mesh )
		{
			//===================AABB===================
			aabb = entity->mesh->aabb;
			worldAabb = entity->mesh->worldAABB;

			//��ʵ����������任����AABB����������ϵ
			matrix3D_transformVector( worldAabb->min, world, aabb->min );
			matrix3D_transformVector( worldAabb->max, world, aabb->max );

			//���ݱ任���AABB���¹����µ�AABB����Ҳ����ʵ�徭����ת��λ�ơ����ź����AABB��
			aabb_createFromSelf( & worldAabb );

			//===================����===================
			vNode = entity->mesh->vertices->nodes;
			//��������
			while ( NULL != vNode )
			{
				i = 0;
				//��ʵ����������Ѷ���任����������ϵ
				matrix3D_transformVector( vNode->vertex->worldPosition, world, vNode->vertex->position );

				//===================���պͶ�����ɫֵ===================
				lights = scene->lights;
				//������Դ
				while ( LIGHT_ENABLE == 1 &&  NULL != entity->material && NULL != lights)
				{
					//���㻷���⣨��Դ��ɫ���Բ�����ɫ���������ڶ�����ɫ
					color_append( vNode->vertex->color, entity->material->ambient, light->ambient );

					/*vector3D_subtract( & dstns, & lightDirs[i], vNode->vertex->position );
					vector3D_normalize( & dstns );
					dot = vector3D_dotProduct( & dstns, vNode->vertex->normal );*/

					//��Դ�Ͷ���ļн�
					dot = vector3D_dotProduct( & lightDirs[i], vNode->vertex->normal );

					//����нǴ���0�����нǷ�Χ��(-90, 90)֮��
					if ( dot > 0.0f )
					{
						//�����䣨��Դ��������ɫ���Բ�����������ɫ������������ʱ��ɫֵd
						color_append( & d, entity->material->diffuse, light->diffuse );

						//����⣨���ݼнǴ�С�� d ������ɫ�����ţ����Ͷ��㵱ǰ��ɫֵ��ӣ�
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
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

	Vector3D vFDist, * vLightsToObject, vLightToVertex, vVertexToLight;
	Lights * lights;
	Light * light;
	Color * fColor, * lastColor, outPutColor;
	float dot, fAttenuCoef, fc1, fc2, fDist, fSpotFactor;
	int i;

	lastColor = newColor( 0.0f, 0.0f, 0.0f, 0.0f );
	fColor = newColor( 0.0f, 0.0f, 0.0f, 0.0f );

	//���������ڼ�¼�Ա�����Ϊ�ο���Ĺ�Դ����
	if( ( vLightsToObject = ( Vector3D * )calloc( scene->nLights, sizeof( Vector3D ) ) ) == NULL )
	{
		exit( TRUE );
	}

	//==================��Դ===================
	lights = scene->lights;

	//������Դ
	while ( NULL != lights)
	{
		if ( TRUE == lights->light->bOnOff )
		{
			//���¹�Դ����
			light_updateTransform(lights->light);
		}

		lights = lights->next;
	}

	sceneNode = scene->nodes;

	//����������㣬����ʵ��
	while( NULL != sceneNode )
	{
		i = 0;

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
		while ( NULL != lights)
		{
			if ( TRUE == lights->light->bOnOff )
			{
				//��ʵ������������任��Դ��λ�õõ���ʵ��Ϊ�ο���Ĺ�Դ��λ�ã�����������
				matrix3D_transformVector( & vLightsToObject[i], entity->worldInvert, lights->light->source->worldPosition );
			}

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
				//���������Ѷ���任����������ϵ
				matrix3D_transformVector( vNode->vertex->worldPosition, world, vNode->vertex->position );

				//===================���պͶ�����ɫֵ===================
				//����
				color_zero( lastColor );
				color_zero( fColor );

				//������Ч�ʺ���ʵ��֮��ȡ��һ��ƽ�⣬������ȫ�ֻ�����Ĺ��ף�ֱ��ʹ�ò��ʵĻ����ⷴ��ϵ����Ϊ������ɫ
				if ( NULL != entity->material )
					color_add_self( lastColor, entity->material->ambient );

				lights = scene->lights;

				//�����ڹ�Դ�Ĺ���
				while ( NULL != entity->material && NULL != lights)
				{
					//�����Դ�ǹرյ�
					if ( FALSE == lights->light->bOnOff )
					{
						lights = lights->next;

						i ++;

						continue;
					}

					light = lights->light;	

					//������ø߼������㷨
					if ( light->mode == HIGH_MODE )
					{
						//��һ, ����˥��ϵ��

						//˥��ϵ��.����1.0��˥��
						fAttenuCoef = 1.0f;

						//����ǵ��Դ
						if ( light->type == POINT_LIGHT )
						{
							//����˥��ϵ��
							fAttenuCoef = light->attenuation0;

							//һ��˥��ϵ�������˥��ϵ��
							fc1 = light->attenuation1;
							fc2 = light->attenuation2;

							if ( ( fc1 > 0.0001f ) || ( fc2 > 0.0001f ) )
							{
								//�󶥵�����Դ�ľ���
								vector3D_subtract( & vFDist, & vLightsToObject[i], vNode->vertex->position );
								fDist = vector3D_lengthSquared( & vFDist );

								//����һ�κͶ�������
								fAttenuCoef += (fc1 * sqrtf( fDist ) + fc2 * fDist);
							}

							if ( fAttenuCoef < 0.0001f ) fAttenuCoef = 0.0001f;
							fAttenuCoef = 1.0f / fAttenuCoef;

							//˥��ϵ�����ô���1.0
							fAttenuCoef = MIN( 1.0f,  fAttenuCoef );
						}

						//����۹�����
						
						//�۹�����, һ����Դ�ľ۹�����Ϊ 1.0f, (��ɢ���Ϊ180��)
						fSpotFactor = 1.0f;

						//�۹����������:��һ, ��ԴΪ�۹��; �ڶ�, ��ķ�ɢ���С�ڻ����90��
						if ( ( light->type == SPOT_LIGHT ) && ( light->spotCutoff < 90.0001f ) )
						{
							//��Դ����ʵλ���Ѿ�Ϊ(xLight,  yLight,  zLight), 
							//�����Դ�������۹�Ч��
							
							//����: �۹�λ��ָ�����䶥��
							vector3D_subtract( & vLightToVertex, vNode->vertex->position, & vLightsToObject[i] );

							//��λ��
							vector3D_normalize( & vLightToVertex );

							//�۹����䷽��(�Ѿ���һ����λ����) �� ���� vLightToVertex �нǵ�����
							dot = vector3D_dotProduct( & vLightToVertex, light->source->direction );

							//�������λ�ڹ�׶֮��, �򲻻��о۹�������䵽������
							if( dot < light->spotCutoff )
								fSpotFactor = 0.0f;
							else
							{
								//���þ۹�ָ�����м���
								fSpotFactor = powf( dot, light->spotExp );
							}
						}
						// �������Թ�Դ�Ĺ���(�����Ѿ����㹻��������)

						//���뻷�����䲿��:
						color_append( fColor, entity->material->ambient, light->ambient );

						//���, ���������䲿��
				
						//����ָ���Դ������
						vector3D_subtract( & vVertexToLight, & vLightsToObject[i], vNode->vertex->position );
						
						//�����ԴΪƽ�й�Դ(��λ��Դ)
						if ( light->type == DIRECTIONAL_LIGHT )
						{
							//��Դ��λ�þ������䷽��, �����������Դ���������ǹ�Դλ���������෴����
							vVertexToLight.x = -vLightsToObject[i].x;
							vVertexToLight.y = -vLightsToObject[i].y;
							vVertexToLight.z = -vLightsToObject[i].z;
						}
						
						//��λ��
						vector3D_normalize( & vVertexToLight );

						//���㷨�������� vVertexToLight �����ļнǵ�����
						//���㷨��Ӧ�ǵ�λ����, ���ڽ�ģʱ�Ѿ�����뱣֤��
						dot = vector3D_dotProduct( & vVertexToLight, vNode->vertex->normal );

						if ( dot > 0.0f )
						{
							//���������䲿�ֵĹ���
							color_add_self( fColor, color_scaleBy_self( color_append( & outPutColor, entity->material->diffuse, light->diffuse ), dot, dot, dot, 1 ) );
						}

						//����߹ⲿ����Ҫ���ӿڽ��м���-------------

						//������˥���;۹����ӣ��� j ���������ĵڸ����������:
						fSpotFactor *= fAttenuCoef;

						color_scaleBy_self( fColor, fSpotFactor, fSpotFactor, fSpotFactor, 1.0f );

						//�ۼ��������ɫ:
						color_add_self( lastColor, fColor );
					}
					else
					{
						vector3D_normalize( & vLightsToObject[i] );

						//��Դ�Ͷ���ļн�
						dot = vector3D_dotProduct( & vLightsToObject[i], vNode->vertex->normal2 );

						//���뻷�����䲿��
						color_append( fColor, entity->material->ambient, light->ambient );


						//����нǴ���0�����нǷ�Χ��(-90, 90)֮��
						if ( dot > 0.0f )
						{
							//�����䲿�ֵĹ���
							color_add_self( fColor, color_scaleBy_self( color_append( & outPutColor, entity->material->diffuse, light->diffuse ), dot, dot, dot, 1 ) );
						}

						//�ۼ��������ɫ:
						color_add_self( lastColor, fColor );
					}

					lights = lights->next;

					i ++;
				}//end light

				//����ɫ��һ������
				color_normalize( lastColor );

				//��ɫ��ֵ
				color_copy( vNode->vertex->color, lastColor );

				//����alpha, ����򵥵��ò�������������������
				vNode->vertex->color->alpha = entity->material->diffuse->alpha;

				vNode = vNode -> next;
			}//end vertex
		}

		sceneNode = sceneNode->next;
	}//end entity

	free( vLightsToObject );
	vLightsToObject = NULL;

	free( lastColor );
	lastColor = NULL;

	free( fColor );
	fColor = NULL;
}

#endif
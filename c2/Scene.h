#ifndef __SCENE_H
#define __SCENE_H

#include "Entity.h"
#include "Light.h"

typedef struct Scene
{
	int nNodes, nVertices, nFaces, nLights, lightOn, isAttached, dirty;

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
	scene->nFaces += n->entity->mesh->nFaces;
	scene->nVertices += n->entity->mesh->nVertices;
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

	free(head);

	head = NULL;
}

//更新场景
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

	//此数组用于记录以本地作为参考点的光源方向
	if( ( vLightsToObject = ( Vector3D * )calloc( scene->nLights, sizeof( Vector3D ) ) ) == NULL )
	{
		exit( TRUE );
	}

	//==================光源===================
	lights = scene->lights;

	//遍历光源
	while ( NULL != lights)
	{
		if ( TRUE == lights->light->bOnOff )
		{
			//更新光源矩阵
			light_updateTransform(lights->light);
		}

		lights = lights->next;
	}

	sceneNode = scene->nodes;

	//遍历场景结点，更新实体
	while( NULL != sceneNode )
	{
		i = 0;

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
		while ( NULL != lights)
		{
			if ( TRUE == lights->light->bOnOff )
			{
				//用实体的世界逆矩阵变换光源的位置得到以实体为参考点的光源的位置，保存在数组
				matrix3D_transformVector( & vLightsToObject[i], entity->worldInvert, lights->light->source->worldPosition );
			}

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
				//用世界矩阵把顶点变换到世界坐标系
				matrix3D_transformVector( vNode->vertex->worldPosition, world, vNode->vertex->position );

				//===================光照和顶点颜色值===================
				//清零
				color_zero( lastColor );
				color_zero( fColor );

				//基于在效率和真实感之间取得一个平衡，不考虑全局环境光的贡献，直接使用材质的环境光反射系数作为最终颜色
				if ( NULL != entity->material )
					color_add_self( lastColor, entity->material->ambient );

				lights = scene->lights;

				//来自于光源的贡献
				//如果有光源
				while ( NULL != entity->material && NULL != lights)
				{
					//如果光源是关闭的
					if ( FALSE == lights->light->bOnOff )
					{
						lights = lights->next;

						i ++;

						continue;
					}

					scene->lightOn = TRUE;

					light = lights->light;	

					//如果启用高级光照算法
					if ( light->mode == HIGH_MODE )
					{
						//其一, 计算衰减系数

						//衰减系数.等于1.0则不衰减
						fAttenuCoef = 1.0f;

						//如果是点光源
						if ( light->type == POINT_LIGHT )
						{
							//常数衰减系数
							fAttenuCoef = light->attenuation0;

							//一次衰减系数与二次衰减系数
							fc1 = light->attenuation1;
							fc2 = light->attenuation2;

							if ( ( fc1 > 0.0001f ) || ( fc2 > 0.0001f ) )
							{
								//求顶点至光源的距离
								vector3D_subtract( & vFDist, & vLightsToObject[i], vNode->vertex->position );
								fDist = vector3D_lengthSquared( & vFDist );

								//加入一次和二次因子
								fAttenuCoef += (fc1 * sqrtf( fDist ) + fc2 * fDist);
							}

							if ( fAttenuCoef < 0.0001f ) fAttenuCoef = 0.0001f;
							fAttenuCoef = 1.0f / fAttenuCoef;

							//衰减系数不得大于1.0
							fAttenuCoef = MIN( 1.0f,  fAttenuCoef );
						}

						//计算聚光因子
						
						//聚光因子, 一般点光源的聚光因子为 1.0f, (发散半角为180度)
						fSpotFactor = 1.0f;

						//聚光产生的条件:第一, 光源为聚光灯; 第二, 光的发散半角小于或等于90度
						if ( ( light->type == SPOT_LIGHT ) && ( light->spotCutoff < 90.0001f ) )
						{
							//光源的真实位置已经为(xLight,  yLight,  zLight), 
							//定向光源不产生聚光效果
							
							//向量: 聚光位置指向照射顶点
							vector3D_subtract( & vLightToVertex, vNode->vertex->position, & vLightsToObject[i] );

							//单位化
							vector3D_normalize( & vLightToVertex );

							//聚光照射方向(已经是一个单位向量) 与 向量 vLightToVertex 夹角的余弦
							dot = vector3D_dotProduct( & vLightToVertex, light->source->direction );

							//如果顶点位于光锥之外, 则不会有聚光光线照射到物体上
							if( dot < light->spotCutoff )
								fSpotFactor = 0.0f;
							else
							{
								//利用聚光指数进行计算
								fSpotFactor = powf( dot, light->spotExp );
							}
						}
						// 计算来自光源的贡献(现在已经有足够的条件了)

						//加入环境反射部分:
						color_append( fColor, entity->material->ambient, light->ambient );

						//其次, 计算漫反射部分
				
						//顶点指向光源的向量
						vector3D_subtract( & vVertexToLight, & vLightsToObject[i], vNode->vertex->position );
						
						//如果光源为平行光源(定位光源)
						if ( light->type == DIRECTIONAL_LIGHT )
						{
							//光源的位置就是照射方向, 因而顶点至光源的向量就是光源位置向量的相反向量
							vVertexToLight.x = -vLightsToObject[i].x;
							vVertexToLight.y = -vLightsToObject[i].y;
							vVertexToLight.z = -vLightsToObject[i].z;
						}
						
						//单位化
						vector3D_normalize( & vVertexToLight );

						//顶点法线向量与 vVertexToLight 向量的夹角的余弦
						//顶点法线应是单位向量, 这在建模时已经或必须保证的
						dot = vector3D_dotProduct( & vVertexToLight, vNode->vertex->normal );

						if ( dot > 0.0f )
						{
							//加入漫反射部分的贡献
							color_add_self( fColor, color_scaleBy_self( color_append( & outPutColor, entity->material->diffuse, light->diffuse ), dot, dot, dot, 1 ) );
						}

						//计算高光部分需要在视口进行计算-------------

						//最后乘以衰减和聚光因子，第 j 个光对物体的第个顶点的照射:
						fSpotFactor *= fAttenuCoef;

						color_scaleBy_self( fColor, fSpotFactor, fSpotFactor, fSpotFactor, 1.0f );

						//累加至最后颜色:
						color_add_self( lastColor, fColor );
					}
					else
					{
						vector3D_normalize( & vLightsToObject[i] );

						//光源和顶点的夹角
						dot = vector3D_dotProduct( & vLightsToObject[i], vNode->vertex->normal2 );

						//加入环境反射部分
						color_append( fColor, entity->material->ambient, light->ambient );


						//如果夹角大于0，即夹角范围在(-90, 90)之间
						if ( dot > 0.0f )
						{
							//漫反射部分的贡献
							color_add_self( fColor, color_scaleBy_self( color_append( & outPutColor, entity->material->diffuse, light->diffuse ), dot, dot, dot, 1 ) );
						}

						//累加至最后颜色:
						color_add_self( lastColor, fColor );
					}

					lights = lights->next;

					i ++;
				}//end light

				//作颜色归一化处理
				color_normalize( lastColor );

				//颜色赋值
				color_copy( vNode->vertex->color, lastColor );

				//对于alpha, 这里简单地用材质漫反射属性来代替
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

void scene_updateAfterRender( Scene * scene )
{
	scene->lightOn = FALSE;
	scene->dirty = FALSE;
}

#endif
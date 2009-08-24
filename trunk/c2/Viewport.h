#ifndef __VIEWPORT_H
#define __VIEWPORT_H

#include <malloc.h>
#include <float.h>

#include "Entity.h"
#include "Scene.h"
#include "Camera.h"
#include "Matrix3D.h"
#include "Vector3D.h"
#include "Clipping.h"

void viewport_resize( Viewport * viewport, float width, float height )
{
	int wh = (int)width * (int)height;

	viewport->width = width;
	viewport->height = height;
	viewport->offsetX = width * 0.5f;
	viewport->offsetY = height * 0.5f;
	viewport->aspect = width / height;
	viewport->wh = wh;
	viewport->dirty = TRUE;

	if ( NULL != viewport->mixedChannel ) free( viewport->mixedChannel );
	if ( NULL != viewport->zBuffer ) free( viewport->zBuffer );

	//初始化缓冲区
	if( ( viewport->mixedChannel	= ( LPDWORD )malloc( sizeof( DWORD ) * wh ) ) == NULL ) exit( TRUE );
	if( ( viewport->zBuffer			= ( float * )malloc( sizeof( float ) * wh ) ) == NULL ) exit( TRUE );
}

void view_attachCamera( Viewport * viewport, Camera * camera)
{
	viewport->camera = camera;
	viewport->dirty = TRUE;
}

void view_attachScene( Viewport * viewport, Scene * scene)
{
	viewport->scene = scene;
	viewport->dirty = TRUE;
}

RenderList * initializeRenderList( int number )
{
	int i = 0;

	RenderList * renderList, * lastRenderList;

	lastRenderList = NULL;

	for ( ; i < number; i ++ )
	{
		if( ( renderList = ( RenderList * )malloc( sizeof( RenderList ) ) ) == NULL ) exit( TRUE );

		renderList->polygon = NULL;

		renderList->next = lastRenderList;

		if ( lastRenderList ) lastRenderList->pre = renderList;

		lastRenderList = renderList;
	}

	return renderList;
}

RenderVList * initializeRenderVList( int number )
{
	int i = 0;

	RenderVList * renderVList, * lastRenderVList;

	lastRenderVList = NULL;

	for ( ; i < number; i ++ )
	{
		if( ( renderVList = ( RenderVList * )malloc( sizeof( RenderVList ) ) ) == NULL ) exit( TRUE );

		renderVList->vertex = NULL;

		renderVList->next = lastRenderVList;

		if ( lastRenderVList ) lastRenderVList->pre = renderVList;

		lastRenderVList = renderVList;
	}

	return renderVList;
}

Viewport * newViewport( float width, float height, Scene * scene, Camera * camera )
{
	Viewport * viewport;

	if( (viewport = ( Viewport * )malloc( sizeof( Viewport ) ) ) == NULL) exit( TRUE );

	viewport->mixedChannel = NULL;

	viewport->zBuffer = NULL;

	viewport_resize(viewport, width, height);
	
	viewport->camera = camera;

	viewport->scene = scene;

	camera->isAttached = scene->isAttached = TRUE;

	//构造渲染列表
	viewport->renderList = initializeRenderList( scene->nFaces + 2 );

	//构造裁剪列表
	viewport->clippedList = initializeRenderList( scene->nFaces + 2 );

	viewport->nRenderList = 0;

	return viewport;
}

void viewport_updateBeforeRender( Viewport * viewport )
{
	float * zBuf = viewport->zBuffer;

	LPDWORD mixedChannel = viewport->mixedChannel;

	int wh = viewport->wh;

	RenderList * renderList;

	int m = 0;

	//初始化缓冲区
	for ( ; m < wh; m ++ )
	{
		zBuf[m] = FLT_MAX;
		mixedChannel[m] = 0;
	}

	//如果场景有改变
	if ( TRUE == viewport->scene->dirty )
	{
		RenderList * lastRenderList = viewport->renderList;

		//释放渲染列表
		while ( NULL != lastRenderList )
		{
			renderList = lastRenderList;

			lastRenderList = lastRenderList->next;

			free( renderList );
		}

		//重新构造渲染列表
		viewport->renderList = initializeRenderList( viewport->scene->nFaces + 2 );
	}
	else
	{
		renderList = viewport->renderList->next;

		//重置渲染列表
		while ( NULL != renderList )
		{
			renderList->polygon = NULL;

			renderList = renderList->next;
		}
	}

	viewport->nRenderList = 0;
}

void viewport_project( Viewport * viewport )
{
	Scene * scene;
	SceneNode * sceneNode;
	Camera * camera;
	Entity * entity;
	Matrix3D view_projection;
	float worldZNear, worldZFar;
	RenderList * rl_ptr, * cl_ptr, * renderList;

	//光照变量
	Vertex * vs;
	Material * material;
	Lights * lights;
	Light * light;
	Vector3D vFDist, * vLightsToObject, vLightToVertex, vVertexToLight, vVertexToCamera;
	FloatColor fColor, lastColor, mColor, outPutColor;
	float dot, fAttenuCoef, fc1, fc2, fDist, fSpotFactor, fShine, fShineFactor;

	int l = 0, j = 0;

	scene = viewport->scene;
	camera = viewport->camera;
	worldZNear = camera->near + camera->eye->position->z;
	worldZFar = camera->far + camera->eye->position->z;

	//如果摄像机或视口的属性改变
	//则重新计算投影矩阵
	if ( TRUE == camera->fnfDirty || TRUE == viewport->dirty)
	{
		getPerspectiveFovLH( camera->projectionMatrix, camera, viewport->aspect );

		camera->fnfDirty = viewport->dirty = FALSE;
	}

	//如果有光源
	//此数组用于记录以本地作为参考点的光源方向
	if ( NULL != scene->lights )
		if( ( vLightsToObject = ( Vector3D * )calloc( scene->nLights, sizeof( Vector3D ) ) ) == NULL ) exit( TRUE );

	matrix3D_append4x4( & view_projection, camera->eye->world, camera->projectionMatrix );

	//把渲染列表的指针指向表头的下一个结点
	rl_ptr = viewport->renderList->next;
	cl_ptr = viewport->clippedList->next;

	sceneNode = scene->nodes;

	//遍历实体
	while( NULL != sceneNode && NULL != sceneNode->entity->mesh )
	{
		entity = sceneNode->entity;

		//连接摄像机矩阵
		//matrix3D_append( entity->view, entity->world, camera->eye->world );
		//matrix3D_getPosition( entity->viewPosition, entity->view );

		//连接透视投影矩阵
		matrix3D_append4x4( entity->projection, entity->world, & view_projection );
		matrix3D_getPosition( entity->CVVPosition, entity->projection );
		vector3D_project( entity->CVVPosition );

		mesh_transformNewAABB( entity->mesh->worldAABB, entity->world, entity->mesh->aabb );

		//由于透视投影使AABB在Z方向发生扭曲，因此无法用投影矩阵对viewAABB进行变换，必须重新构造基于CVV的AABB
		mesh_transformNewAABB( entity->mesh->CVVAABB, entity->projection, entity->mesh->aabb );

		matrix3D_transformVector( entity->viewerToLocal, entity->worldInvert, camera->eye->position );

		//基于包围盒的视锥体剔除
		if ( frustumCulling( entity, worldZNear, worldZFar, & rl_ptr, & cl_ptr ) > 0 )
		{
			sceneNode = sceneNode->next;

			continue;
		}

		//=======================实体没有离屏=======================

		//========================光照计算======================

		//光源索引为零
		l = 0;

		material = entity->material;

		//更新光源位置
		lights = scene->lights;

		//遍历光源
		while ( NULL != lights )
		{
			if ( TRUE == lights->light->bOnOff )
			{
				//用实体的世界逆矩阵变换光源的位置得到以实体为参考点的光源的位置，保存在数组
				matrix3D_transformVector( & vLightsToObject[l], entity->worldInvert, lights->light->source->worldPosition );
			}

			lights = lights->next;

			l ++;
		}

		//基于在效率和真实感之间取得一个平衡，不考虑全局环境光的贡献，直接使用材质的环境光反射系数作为最终颜色
		floatColor_zero( & mColor );
		floatColor_add_self( & mColor, material->ambient );

		//遍历顶点
		renderList = viewport->renderList->next;

		while ( NULL != renderList && NULL != renderList->polygon )
		{
			for ( j = 0; j < 3; j ++)
			{
				vs = renderList->polygon->vertex[j];

				if ( TRUE == vs->transformed ) continue;

				//===================光照和顶点颜色值===================

				//光源索引为零
				l = 0;

				floatColor_identity( & lastColor);

				if ( NULL != material )
				{
					//复制材质颜色到最终颜色
					floatColor_copy( & lastColor, & mColor );
					//临时颜色清零
					floatColor_zero( & fColor );

					lights = scene->lights;

					//来自于光源的贡献
					//如果有光源
					while ( NULL != lights)
					{
						//如果光源是关闭的
						if ( FALSE == lights->light->bOnOff )
						{
							lights = lights->next;

							l ++;

							continue;
						}

						scene->lightOn = TRUE;

						light = lights->light;	

						//如果启用高级光照算法
						if ( light->mode == HIGH_MODE || light->mode == MID_MODE)
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
									vector3D_subtract( & vFDist, & vLightsToObject[l], vs->position );
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
								vector3D_subtract( & vLightToVertex, vs->position, & vLightsToObject[l] );

								//单位化
								vector3D_normalize( & vLightToVertex );

								//聚光照射方向(已经是一个单位向量) 与 向量 vLightToVertex 夹角的余弦
								dot = vector3D_dotProduct( & vLightToVertex, light->source->direction );

								//如果顶点位于光锥之外, 则不会有聚光光线照射到物体上
								if ( dot < light->spotCutoff )
									fSpotFactor = 0.0f;
								else
								{
									//利用聚光指数进行计算
									fSpotFactor = powf( dot, light->spotExp );
								}
							}
							// 计算来自光源的贡献(现在已经有足够的条件了)

							//加入环境反射部分:
							floatColor_append( & fColor, material->ambient, light->ambient );

							//其次, 计算漫反射部分

							//顶点指向光源的向量
							vector3D_subtract( & vVertexToLight, & vLightsToObject[l], vs->position );

							//如果光源为平行光源(定位光源)
							if ( light->type == DIRECTIONAL_LIGHT )
							{
								//光源的位置就是照射方向, 因而顶点至光源的向量就是光源位置向量的相反向量
								vVertexToLight.x = -vLightsToObject[l].x;
								vVertexToLight.y = -vLightsToObject[l].y;
								vVertexToLight.z = -vLightsToObject[l].z;
							}

							//单位化
							vector3D_normalize( & vVertexToLight );

							//顶点法线向量与 vVertexToLight 向量的夹角的余弦
							//顶点法线应是单位向量, 这在建模时已经或必须保证的
							dot = vector3D_dotProduct( & vVertexToLight, vs->normal );

							if ( dot > 0.0f )
							{
								//加入漫反射部分的贡献
								floatColor_add_self( & fColor, floatColor_scaleBy_self( floatColor_append( & outPutColor, material->diffuse, light->diffuse ), dot, dot, dot, 1 ) );

								//计算高光部分
								if ( light->mode == HIGH_MODE )
								{
									vector3D_subtract( & vVertexToCamera, entity->viewerToLocal, vs->position );
									vector3D_normalize( & vVertexToCamera );
									vector3D_add_self( & vVertexToCamera, & vVertexToLight );
									vector3D_normalize( & vVertexToCamera );

									//光度因子基数:与顶点法线作点积
									fShine = vector3D_dotProduct( & vVertexToCamera, vs->normal );

									if ( fShine > 0.0f )
									{
										fShineFactor = powf(fShine, material->power);

										//加入高光部分的贡献
										floatColor_add_self( & fColor, floatColor_scaleBy_self( floatColor_append( & outPutColor, material->specular, light->specular ), fShineFactor, fShineFactor, fShineFactor, 1 ) );
									}
								}
							}

							//最后乘以衰减和聚光因子，第 j 个光对物体的第个顶点的照射:
							fSpotFactor *= fAttenuCoef;

							floatColor_scaleBy_self( & fColor, fSpotFactor, fSpotFactor, fSpotFactor, 1.0f );

							//累加至最后颜色:
							floatColor_add_self( & lastColor, & fColor );
						}
						else
						{
							vector3D_normalize( & vLightsToObject[l] );

							//光源和顶点的夹角
							dot = vector3D_dotProduct( & vLightsToObject[l], vs->normalLength );

							//加入环境反射部分
							floatColor_append( & fColor, material->ambient, light->ambient );


							//如果夹角大于0，即夹角范围在(-90, 90)之间
							if ( dot > 0.0f )
							{
								//漫反射部分的贡献
								floatColor_add_self( & fColor, floatColor_scaleBy_self( floatColor_append( & outPutColor, material->diffuse, light->diffuse ), dot, dot, dot, 1 ) );
							}

							//累加至最后颜色:
							floatColor_add_self( & lastColor, & fColor );
						}

						lights = lights->next;

						l ++;
					}//end light

					//作颜色归一化处理
					floatColor_normalize( & lastColor );

					//对于alpha, 这里简单地用材质漫反射属性来代替
					lastColor.alpha = material->diffuse->alpha;
				}

				//把顶点变换到CVV
				matrix3D_transformVector( vs->viewPosition, & view_projection, vs->worldPosition );

				vs->viewPosition->x += 0.5f;
				vs->viewPosition->y += 0.5f;

				vs->viewPosition->x *= viewport->width;
				vs->viewPosition->y *= viewport->height;

				vs->color->red = (WORD)(lastColor.red * 255.0f);
				vs->color->green = (WORD)(lastColor.green * 255.0f);
				vs->color->blue = (WORD)(lastColor.blue * 255.0f);
				vs->color->alpha = (WORD)(lastColor.alpha * 255.0f);

				vs->transformed = TRUE;
			}

			renderList = renderList->next;
		}
		
		sceneNode = sceneNode->next;
	}

	if ( NULL != scene->lights )
	{
		free( vLightsToObject );
		vLightsToObject = NULL;
	}
}

INLINE void getMixedColor( WORD a, WORD r, WORD g, WORD b, int u, int v, int pos, Texture * texture, LPDWORD mixedChannel );
void wireframe_rasterize( Viewport * view, Vertex * ver0, Vertex * ver1, Vertex * ver2 );
void triangle_rasterize( Viewport * view, Vertex * ver0, Vertex * ver1, Vertex * ver2 );
void triangle_rasterize_texture( Viewport * view, Vertex * ver0, Vertex * ver1, Vertex * ver2, Texture * texture );
void triangle_rasterize_light( Viewport * view, Vertex * ver0, Vertex * ver1, Vertex * ver2 );
void triangle_rasterize_light_texture( Viewport * view, Vertex * ver0, Vertex * ver1, Vertex * ver2, Texture * texture );

void viewport_render(Viewport * view)
{
	Scene * scene;

	Triangle * face;

	Vertex ** vertex;

	RenderList * rl;

	scene = view->scene;

	rl = view->renderList->next;

	while ( NULL != rl && NULL != rl->polygon )
	{
		face = rl->polygon;

		vertex = face->vertex;

		if ( NULL == scene->lights)
		{
			if ( NULL == face->texture )
			{
				triangle_rasterize( view, vertex[0], vertex[1], vertex[2] );
				//wireframe_rasterize( view, vertex[0], vertex[1], vertex[2] );
			}
			else
			{
				triangle_rasterize_texture( view, vertex[0], vertex[1], vertex[2], face->texture );
			}
		}
		else
		{
			if ( NULL == face->texture )
			{
				triangle_rasterize_light( view, vertex[0], vertex[1], vertex[2] );
			}
			else
			{
				triangle_rasterize_light_texture( view,  vertex[0], vertex[1], vertex[2], face->texture );
			}
		}

		rl = rl->next;
	}
}

#endif
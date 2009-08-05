#ifndef __VIEWPORT_H
#define __VIEWPORT_H

#include <malloc.h>
#include <float.h>

#include "Entity.h"
#include "Scene.h"
#include "Camera.h"
#include "Matrix3D.h"
#include "Vector3D.h"

typedef struct RenderList
{
	Entity * entity;

	float screenDepth;
}RenderList;

typedef struct Viewport
{
	int wh, dirty, nRenderVertex, nRenderList;

	float width, height, offsetX, offsetY, aspect;

	//深度缓冲区
	float * zBuffer;

	//颜色缓冲区
	DWORD * colorBuffer;

	//纹理缓冲区
	DWORD * textureBuffer;

	struct Scene * scene;

	struct Camera * camera;

	struct RenderList * renderList;

	RenderVertex * screenVertices;
}Viewport;

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

	if ( NULL != viewport->colorBuffer ) free( viewport->colorBuffer );
	if ( NULL != viewport->textureBuffer ) free( viewport->textureBuffer );
	if ( NULL != viewport->zBuffer ) free( viewport->zBuffer );

	//初始化缓冲区
	if( ( viewport->colorBuffer = ( DWORD * )malloc( sizeof( DWORD ) * wh ) ) == NULL ) exit( TRUE );
	if( ( viewport->textureBuffer = ( DWORD * )malloc( sizeof( DWORD ) * wh ) ) == NULL ) exit( TRUE );
	if( ( viewport->zBuffer = ( float * )malloc( sizeof( float ) * wh ) ) == NULL ) exit( TRUE );
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

Viewport * newViewport( float width, float height, Scene * scene, Camera * camera )
{
	Viewport * viewport;

	if( (viewport = ( Viewport * )malloc( sizeof( Viewport ) ) ) == NULL)
	{
		exit( TRUE );
	}

	viewport->colorBuffer = NULL;
	viewport->textureBuffer = NULL;
	viewport->zBuffer = NULL;

	viewport_resize(viewport, width, height);
	
	viewport->camera = camera;
	viewport->scene = scene;

	camera->isAttached = scene->isAttached = TRUE;

	viewport->nRenderVertex = viewport->nRenderList = 0;

	if( ( viewport->screenVertices = ( RenderVertex * )malloc( sizeof( RenderVertex ) * scene->nVertices ) ) == NULL )
	{
		exit( TRUE );
	}

	if( ( viewport->renderList = ( RenderList * )malloc( sizeof( RenderList ) * scene->nNodes ) ) == NULL )
	{
		exit( TRUE );
	}

	return viewport;
}

void viewport_updateBeforeRender( Viewport * viewport )
{
	float * zBuf = viewport->zBuffer;
	DWORD * colorBuffer = viewport->colorBuffer;
	DWORD * textureBuffer = viewport->textureBuffer;

	int wh = viewport->wh;

	int m = 0;

	for ( ; m < wh; m ++ )
	{
		zBuf[m] = FLT_MAX;
		colorBuffer[m] = 0;
		textureBuffer[m] = 0;
	}

	if ( TRUE == viewport->scene->dirty )
	{
		free( viewport->screenVertices );
		free( viewport->renderList );

		if( ( viewport->screenVertices = ( RenderVertex * )malloc( sizeof( RenderVertex ) * viewport->scene->nVertices ) ) == NULL )
		{
			exit( TRUE );
		}

		if( ( viewport->renderList = ( RenderList * )malloc( sizeof( RenderList ) * viewport->scene->nNodes ) ) == NULL )
		{
			exit( TRUE );
		}
	}

	viewport->nRenderList = 0;
}

void viewport_updateAfterRender( Viewport * viewport )
{
	
}

//基于AABB包围盒的视空间裁剪
//输出码为非零侧物体离屏
INLINE int viewport_viewCulling(Matrix3D * proj_matrix, AABB aabb)
{
	Vector3D * min, * max;
	Vector3D T_L_F_V;
	Vector3D T_R_F_V;
	Vector3D T_L_B_V;
	Vector3D T_R_B_V;
	Vector3D B_L_F_V;
	Vector3D B_R_F_V;
	Vector3D B_L_B_V;
	Vector3D B_R_B_V;
	AABB * tmpAABB = newAABB();

	int code = 0;

	min = aabb.min;
	max = aabb.max;

	T_L_F_V.x = min->x;	T_L_F_V.y = max->y;	T_L_F_V.z = min->z;	T_L_F_V.w = 1;
	T_R_F_V.x = max->x;	T_R_F_V.y = max->y;	T_R_F_V.z = min->z;	T_R_F_V.w = 1;
	T_L_B_V.x = min->x;	T_L_B_V.y = max->y;	T_L_B_V.z = max->z;	T_L_B_V.w = 1;
	T_R_B_V.x = max->x;	T_R_B_V.y = max->y;	T_R_B_V.z = max->z;	T_R_B_V.w = 1;
	B_L_F_V.x = min->x;	B_L_F_V.y = min->y;	B_L_F_V.z = min->z;	B_L_F_V.w = 1;
	B_R_F_V.x = max->x;	B_R_F_V.y = min->y;	B_R_F_V.z = min->z;	B_R_F_V.w = 1;
	B_L_B_V.x = min->x;	B_L_B_V.y = min->y;	B_L_B_V.z = max->z;	B_L_B_V.w = 1;
	B_R_B_V.x = max->x;	B_R_B_V.y = min->y;	B_R_B_V.z = max->z;	B_R_B_V.w = 1;

	matrix3D_transformVector_self( proj_matrix, & T_L_F_V );
	matrix3D_transformVector_self( proj_matrix, & T_R_F_V );
	matrix3D_transformVector_self( proj_matrix, & T_L_B_V );
	matrix3D_transformVector_self( proj_matrix, & T_R_B_V );
	matrix3D_transformVector_self( proj_matrix, & B_L_F_V );
	matrix3D_transformVector_self( proj_matrix, & B_R_F_V );
	matrix3D_transformVector_self( proj_matrix, & B_L_B_V );
	matrix3D_transformVector_self( proj_matrix, & B_R_B_V );

	aabb_empty( tmpAABB );

	aabb_add( tmpAABB, & T_L_F_V );
	aabb_add( tmpAABB, & T_R_F_V );
	aabb_add( tmpAABB, & T_L_B_V );
	aabb_add( tmpAABB, & T_R_B_V );
	aabb_add( tmpAABB, & B_L_F_V );
	aabb_add( tmpAABB, & B_R_F_V );
	aabb_add( tmpAABB, & B_L_B_V );
	aabb_add( tmpAABB, & B_R_B_V );

	if ( tmpAABB->max->x < -1 )	code |= 0x01;
	if ( tmpAABB->min->x > 1 )	code |= 0x02;
	if ( tmpAABB->max->y < -1 )	code |= 0x04;
	if ( tmpAABB->min->y > 1 )	code |= 0x08;
	if ( tmpAABB->max->z < 0 )	code |= 0x10;
	if ( tmpAABB->min->z > 1 )	code |= 0x20;

	return code;
}

void viewport_project( Viewport * viewport )
{
	Scene * scene;
	SceneNode * sceneNode;
	Camera * camera;
	Entity * entity;
	Matrix3D view, * proj_matrixPtr;
	Vector3D viewPosition;
	Vertex * vs;
	RenderList * renderList;
	RenderVertex * screenVertices;
	Material * material;
	Polygon * faces;
	//
	Lights * lights;
	Light * light;
	Vector3D vFDist, * vLightsToObject, vLightToVertex, vVertexToLight, vVertexToCamera;
	Color fColor, lastColor, mColor, outPutColor;
	float dot, fAttenuCoef, fc1, fc2, fDist, fSpotFactor, fShine, fShineFactor;

	int i = 0, j = 0, n = 0, m = 0, l = 0;

	scene = viewport->scene;
	camera = viewport->camera;

	//如果摄像机或视口的属性改变
	//则重新计算投影矩阵
	if ( TRUE == camera->fnfDirty || TRUE == viewport->dirty)
	{
		getPerspectiveFovLH( camera->projectionMatrix, camera, viewport->aspect );

		camera->fnfDirty = viewport->dirty = FALSE;
	}
	
	proj_matrixPtr = camera->projectionMatrix;

	renderList = viewport->renderList;

	color_zero( & mColor );
	color_zero( & lastColor );
	color_zero( & fColor );

	//如果有光源
	if ( NULL != scene->lights )
	{
		//此数组用于记录以本地作为参考点的光源方向
		if( ( vLightsToObject = ( Vector3D * )calloc( scene->nLights, sizeof( Vector3D ) ) ) == NULL )
		{
			exit( TRUE );
		}

		vVertexToCamera.x = camera->eye->worldPosition->x;
		vVertexToCamera.y = camera->eye->worldPosition->y;
		vVertexToCamera.z = camera->eye->worldPosition->z;

		vector3D_normalize( & vVertexToCamera );
	}

	sceneNode = scene->nodes;

	//遍历实体
	while( NULL != sceneNode && NULL != sceneNode->entity->mesh )
	{
		entity = sceneNode->entity;
		//连接摄像机矩阵
		matrix3D_append( & view, entity->world, camera->eye->world );
		//连接投影矩阵
		matrix3D_append4x4( & view, proj_matrixPtr );

		//视空间裁剪
		if ( viewport_viewCulling( & view, * entity->mesh->aabb ) > 0 )
		{
			sceneNode = sceneNode->next;

			continue;
		}

		//========================如果物体没有离屏则继续如下操作======================

		//加入渲染列表并按深度进行排序
		//计算实体的视空间坐标
		matrix3D_transformVector( & viewPosition, & view, entity->position );

		if ( n == 0)
		{
			renderList[0].entity = entity;
			renderList[0].screenDepth = viewPosition.z;
		}
		else
		{
			for ( m = 0; m < n; m ++ )
			{
				if ( viewPosition.z < renderList[m].screenDepth )
				{
					renderList[n].entity = renderList[m].entity;
					renderList[n].screenDepth = renderList[m].screenDepth;
					renderList[m].entity = entity;
					renderList[m].screenDepth = viewPosition.z;

					break;
				}
			}

			if ( m == n )
			{
				renderList[m].entity = entity;
				renderList[m].screenDepth = viewPosition.z;
			}
		}

		viewport->nRenderList ++;

		//背面剔除
		//总共：9乘法+1除法+6加减+1开方
		//比较屏幕测试背面剔除
		//顶点到裁剪空间的代价：12乘法+12加法
		//顶点到屏幕空间的代价：2乘法+2加法
		//总共：14乘法+14加法
		if ( BACKFACE_CULLING_MODE == 1 )
		{
			Vector3D eyeToLocal, d;

			matrix3D_transformVector( & eyeToLocal, entity->worldInvert, camera->eye->position );

			//遍历面
			faces = entity->mesh->faces;

			for( j = 0; j < entity->mesh->nFaces; j ++)
			{
				vector3D_subtract( & d, & eyeToLocal, faces[j].center );	//3减法

				vector3D_normalize( & d );	//3乘法+3乘法+1开方+1除法

				//如果夹角大于90或小于-90时
				if ( vector3D_dotProduct( & d, faces[j].normal ) <= 0.0f )	//3乘法+3加法
				{
					polygon_setBackFace( & faces[j] );
				}
			}
		}//背面剔除完成

		//光源索引为零
		l = 0;

		material = entity->material;

		if ( NULL != material )
		{
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
			color_zero( & mColor );
			color_add_self( & mColor, material->ambient );
		}

		//遍历顶点
		vs = entity->mesh->vertices;

		for( j = 0; j < entity->mesh->nVertices; j ++)
		{
			//如果所有关联面都是背向摄像机
			if ( vs[j].nContectedFaces == vs[j].bFlag )
			{
				continue;
			}

			//===================光照和顶点颜色值===================

			//光源索引为零
			l = 0;

			color_identity( & lastColor);

			if ( NULL != material )
			{
				//复制材质颜色到最终颜色
				color_copy( & lastColor, & mColor );
				//临时颜色清零
				color_zero( & fColor );

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
								vector3D_subtract( & vFDist, & vLightsToObject[l], vs[j].position );
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
							vector3D_subtract( & vLightToVertex, vs[j].position, & vLightsToObject[l] );

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
						color_append( & fColor, material->ambient, light->ambient );

						//其次, 计算漫反射部分

						//顶点指向光源的向量
						vector3D_subtract( & vVertexToLight, & vLightsToObject[l], vs[j].position );

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
						dot = vector3D_dotProduct( & vVertexToLight, vs[j].normal );

						if ( dot > 0.0f )
						{
							//加入漫反射部分的贡献
							color_add_self( & fColor, color_scaleBy_self( color_append( & outPutColor, material->diffuse, light->diffuse ), dot, dot, dot, 1 ) );
						}

						//计算高光部分
						if ( light->mode == HIGH_MODE )
						{
							if ( dot > 0.0f )
							{
								vector3D_add_self( & vVertexToCamera, & vVertexToLight );
								vector3D_normalize( & vVertexToCamera );

								//光度因子基数:与顶点法线作点积
								fShine = vector3D_dotProduct( & vVertexToCamera, vs[j].normal );

								if ( fShine > 0.0f )
								{
									fShineFactor = powf(fShine, material->power);

									//加入高光部分的贡献
									color_add_self( & fColor, color_scaleBy_self( color_append( & outPutColor, material->specular, light->specular ), fShineFactor, fShineFactor, fShineFactor, 1 ) );
								}
							}
						}

						//最后乘以衰减和聚光因子，第 j 个光对物体的第个顶点的照射:
						fSpotFactor *= fAttenuCoef;

						color_scaleBy_self( & fColor, fSpotFactor, fSpotFactor, fSpotFactor, 1.0f );

						//累加至最后颜色:
						color_add_self( & lastColor, & fColor );
					}
					else
					{
						vector3D_normalize( & vLightsToObject[l] );

						//光源和顶点的夹角
						dot = vector3D_dotProduct( & vLightsToObject[l], vs[j].normal2 );

						//加入环境反射部分
						color_append( & fColor, material->ambient, light->ambient );


						//如果夹角大于0，即夹角范围在(-90, 90)之间
						if ( dot > 0.0f )
						{
							//漫反射部分的贡献
							color_add_self( & fColor, color_scaleBy_self( color_append( & outPutColor, material->diffuse, light->diffuse ), dot, dot, dot, 1 ) );
						}

						//累加至最后颜色:
						color_add_self( & lastColor, & fColor );
					}

					lights = lights->next;

					l ++;
				}//end light

				//作颜色归一化处理
				color_normalize( & lastColor );

				//对于alpha, 这里简单地用材质漫反射属性来代替
				lastColor.alpha = material->diffuse->alpha;
			}

			vs[j].index = i;

			screenVertices = viewport->screenVertices;

			//把顶点变换到屏幕坐标系
			matrix3D_transformVector( & viewPosition, & view, vs[j].position );

			screenVertices[i].x = (viewPosition.x + 0.5f) * viewport->width;
			screenVertices[i].y = (viewPosition.y + 0.5f) * viewport->height;
			screenVertices[i].z = viewPosition.z;
			screenVertices[i].u = vs[j].uv->x;
			screenVertices[i].v = vs[j].uv->y;

			if ( NULL != entity->texture )
			{
				screenVertices[i].r = lastColor.red;
				screenVertices[i].g = lastColor.green;
				screenVertices[i].b = lastColor.blue;
				screenVertices[i].a = lastColor.alpha;
			}
			else
			{
				screenVertices[i].r = lastColor.red * 255.0f;
				screenVertices[i].g = lastColor.green * 255.0f;
				screenVertices[i].b = lastColor.blue * 255.0f;
				screenVertices[i].a = lastColor.alpha * 255.0f;
			}

			i ++;

			viewport->nRenderVertex  = i;
		}//结束遍历顶点

		sceneNode = sceneNode->next;

		n ++;
	}//结束遍历实体

	if ( NULL != scene->lights )
	{
		free( vLightsToObject );
		vLightsToObject = NULL;
	}
}

void triangle_rasterize( Viewport * view, RenderVertex * ver0, RenderVertex * ver1, RenderVertex * ver2 );
void triangle_rasterize_texture( Viewport * view, RenderVertex * ver0, RenderVertex * ver1, RenderVertex * ver2, Texture * texture );
void triangle_rasterize_lightOff_texture( Viewport * view, RenderVertex * ver0, RenderVertex * ver1, RenderVertex * ver2, Texture * texture );
void triangle_rasterize_light( Viewport * view, RenderVertex * ver0, RenderVertex * ver1, RenderVertex * ver2 );
void triangle_rasterize_light_texture( Viewport * view, RenderVertex * ver0, RenderVertex * ver1, RenderVertex * ver2, Texture * texture );

void viewport_render(Viewport * view)
{
	Scene * scene;
	Entity * entity;
	Polygon * faces;
	Vertex ** vertex;
	int i = 0, j = 0;

	scene = view->scene;

	//遍历实体
	for ( i = 0; i < view->nRenderList; i ++ )
	{
		entity = view->renderList[i].entity;

		//遍历面
		faces = entity->mesh->faces;

		for( j = 0; j < entity->mesh->nFaces; j ++)
		{
			//如果面背向摄像机，不用光栅化
			if ( TRUE == faces[j].isBackFace )
			{
				polygon_resetBackFace( & faces[j] );

				continue;
			}

			vertex = faces[j].vertex;

			if ( NULL == scene->lights)
			{
				if ( NULL == entity->texture )
				{
					triangle_rasterize( view,
						& view->screenVertices[vertex[0]->index],
						& view->screenVertices[vertex[1]->index],
						& view->screenVertices[vertex[2]->index] );
				}
				else
				{
					triangle_rasterize_texture( view,
						& view->screenVertices[vertex[0]->index],
						& view->screenVertices[vertex[1]->index],
						& view->screenVertices[vertex[2]->index],
						entity->texture );
				}
			}
			else
			{
				/*if ( scene->lightOn == FALSE )
				{
					if ( NULL == entity->texture )
					{
						triangle_rasterize( view,
											& view->screenVertices[vertex[0]->index],
											& view->screenVertices[vertex[1]->index],
											& view->screenVertices[vertex[2]->index] );
					}
					else
					{
						triangle_rasterize_lightOff_texture( view,
												& view->screenVertices[vertex[0]->index],
												& view->screenVertices[vertex[1]->index],
												& view->screenVertices[vertex[2]->index],
												entity->texture );
					}
				}
				else
				{*/
					if ( NULL == entity->texture )
					{
						triangle_rasterize_light( view,
												& view->screenVertices[vertex[0]->index],
												& view->screenVertices[vertex[1]->index],
												& view->screenVertices[vertex[2]->index] );
					}
					else
					{
						triangle_rasterize_light_texture( view, 
														& view->screenVertices[vertex[0]->index],
														& view->screenVertices[vertex[1]->index],
														& view->screenVertices[vertex[2]->index],
														entity->texture );
					}
				//}
			}

			polygon_resetBackFace( & faces[j] );
		}
	}
}

#endif
#ifndef __VIEWPORT_H
#define __VIEWPORT_H

#include <malloc.h>
#include <float.h>

#include "Entity.h"
#include "Scene.h"
#include "Camera.h"
#include "Matrix3D.h"
#include "Vector3D.h"
#include "RenderFGTINVZB.h"
#include "RenderWF.h"

#define NUM_PER_RL_INIT 50

typedef struct RenderList
{
	Triangle * polygon;

	struct RenderList * next;
	struct RenderList * pre;
}RenderList;

typedef struct Viewport
{
	//高宽积，是否更改属性，渲染数，裁剪数，剔除数
	int wh, dirty, nRenderList, nClippList, nCullList;

	//视口宽高和宽高比
	int width, height;

	float aspect;

	//深度缓冲区
	LPBYTE zBuffer;

	//视频缓冲区
	LPBYTE videoBuffer;

	struct Scene * scene;

	struct Camera * camera;

	struct RenderList * renderList, * clippedList;
}Viewport;

void viewport_resize( Viewport * viewport, int width, int height )
{
	int wh = width * height;

	viewport->width = width;
	viewport->height = height;
	viewport->aspect = (float)width / (float)height;
	viewport->wh = wh;
	viewport->dirty = TRUE;

	if ( NULL != viewport->videoBuffer )	free( viewport->videoBuffer );
	if ( NULL != viewport->zBuffer )		free( viewport->zBuffer );

	//初始化缓冲区
	if( ( viewport->videoBuffer		= ( LPBYTE )calloc( wh * sizeof( DWORD ), sizeof( BYTE ) ) ) == NULL ) exit( TRUE );
	if( ( viewport->zBuffer			= ( LPBYTE )calloc( wh * sizeof( DWORD ), sizeof( BYTE ) ) ) == NULL ) exit( TRUE );
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

Viewport * newViewport( int width, int height, Scene * scene, Camera * camera )
{
	Viewport * viewport;

	if( (viewport = ( Viewport * )malloc( sizeof( Viewport ) ) ) == NULL) exit( TRUE );

	viewport->videoBuffer = NULL;

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
	LPDWORD zBuf = ( LPDWORD )viewport->zBuffer;

	LPDWORD videoBuffer = ( LPDWORD )viewport->videoBuffer;

	int wh = viewport->wh;

	RenderList * renderList;

	int m = 0;

	//初始化缓冲区
#ifdef __NOT_AS3__
	
	Mem_Set_QUAD( ( void * )zBuf, 0, wh );
	memset( videoBuffer, 0, wh );

#else

	for ( ; m < wh; m ++ )
	{
		zBuf[m] = 0;
		videoBuffer[m] = 0;
	}

#endif

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

	viewport->nRenderList = viewport->nClippList = viewport->nCullList = 0;
}

INLINE void extendRenderList( RenderList ** rl_ptr, int length )
{
	RenderList * newRL;

	newRL = initializeRenderList( length );

	newRL->pre = * rl_ptr;

	(* rl_ptr)->next = newRL;
}

INLINE void insertFaceToList( RenderList ** rl_ptr, Triangle * face)
{
	//如果到达渲染列表尾部，则插入NUM_PER_RL_INIT个新的结点(注意保留表尾)
	if ( NULL == (* rl_ptr)->next )
		extendRenderList( rl_ptr, NUM_PER_RL_INIT );

	( * rl_ptr )->polygon = face;
	( * rl_ptr ) = ( * rl_ptr )->next;
}

//基于AABB包围盒的视空间裁剪
//viewport		视口
//camera		摄像机
//entity		当前要处理的实体
//rl_ptr		当前渲染列表的指针
//cl_ptr		当前裁剪列表的指针
void frustumClipping( Viewport * viewport, Entity * entity, float near, RenderList ** rl_ptr, RenderList ** cl_ptr )
{
	int j, zCode0, zCode1, zCode2, verts_out, crossNear = FALSE;

	Vector3D viewerToLocal;

	Triangle * face, * face1, * face2;

	int v0, v1, v2;

	Vertex * tmpVert1, * tmpVert2;

	float t1, t2,  xi, yi, ui, vi, x01i, x02i, y01i, y02i, u01i, u02i, v01i, v02i, dx, dy, dz;

	Vertex * ver1_0, * ver1_1, * ver1_2, * ver2_0, * ver2_1, * ver2_2;

	//测试包围盒是否穿越近截面
	if ( entity->mesh->CVVAABB->max->w > near && entity->mesh->CVVAABB->min->w < near ) crossNear = TRUE;

	//不离屏则继续
	//遍历面
	for( j = 0; j < entity->mesh->nFaces; j ++)
	{
		zCode0 = zCode1 = zCode2 = 0;

		//越界标记置0
		verts_out = FALSE;

		face = & entity->mesh->faces[j];

		face1 = face2 = NULL;

		tmpVert1 = tmpVert2 = NULL;

		//背面剔除
		//线框模式将不会进行背面剔除
		if ( entity->mesh->render_mode != RENDER_WIREFRAME_TRIANGLE_32 )
		{
			vector3D_subtract( & viewerToLocal, entity->viewerToLocal, face->center );

			vector3D_normalize( & viewerToLocal );

			//如果夹角大于90或小于-90时，即背向摄像机
			if ( vector3D_dotProduct( & viewerToLocal, face->normal ) < 0.0f )
			{
				viewport->nCullList ++;

				continue;
			}
		}

		//如果包围盒穿越近截面，则进一步检测面是否穿越近截面
		if ( TRUE == crossNear )
		{
			if ( face->vertex[0]->w_pos->z <= near )
			{
				verts_out ++;

				zCode0 = 0x01;
			}

			if ( face->vertex[1]->w_pos->z <= near )
			{
				verts_out ++;

				zCode1 = 0x02;
			}

			if ( face->vertex[2]->w_pos->z <= near )
			{
				verts_out ++;

				zCode2 = 0x04;
			}

			//输出码为非零则面穿越近截面
			if ( zCode0 | zCode1 | zCode2 )
			{
				//开始裁剪
				//分两种情况
				//1、只有一个顶点在近截面内侧
				if ( verts_out == 2 )
				{
					//累计裁剪多边形
					viewport->nClippList ++;

					//检查当前裁剪列表指针的面指针是否为空
					//如果是则恢复为原始信息
					if ( ( * cl_ptr )->polygon )
					{
						face1 = ( * cl_ptr )->polygon;

						triangle_copy( face1, face );

						( * cl_ptr ) = ( * cl_ptr )->next;
					}
					//否则创建一个新三角形
					else
					{
						//克隆一个面
						face1 = triangle_clone( face );

						//插入裁剪列表
						insertFaceToList( cl_ptr, face1 );
					}

					//找出位于内侧的顶点
					if ( zCode0 == 0x01 && zCode1 == 0x02 )
					{
						v0 = 2;	v1 = 0;	v2 = 1;
					}
					else if ( zCode1 == 0x02 && zCode2 == 0x04 )
					{
						v0 = 0;	v1 = 1;	v2 = 2;
					}
					else
					{
						v0 = 1;	v1 = 2;	v2 = 0;
					}

					ver1_0 = face1->vertex[v0];
					ver1_1 = face1->vertex[v1];
					ver1_2 = face1->vertex[v2];

					//对各边裁剪

					//=======================对v0->v1边进行裁剪=======================
					dx = ver1_1->w_pos->x - ver1_0->w_pos->x;
					dy = ver1_1->w_pos->y - ver1_0->w_pos->y;
					dz = ver1_1->w_pos->z - ver1_0->w_pos->z;

					t1 = ( ( near - ver1_0->w_pos->z ) / dz );

					//计算交点x、y坐标
					xi = ver1_0->w_pos->x + dx * t1;
					yi = ver1_0->w_pos->y + dy * t1;

					//用交点覆盖原来的顶点
					ver1_1->w_pos->x = xi;
					ver1_1->w_pos->y = yi;
					ver1_1->w_pos->z = near;

					//=======================对v0->v2边进行裁剪=======================
					dx = ver1_2->w_pos->x - ver1_0->w_pos->x;
					dy = ver1_2->w_pos->y - ver1_0->w_pos->y;
					dz = ver1_2->w_pos->z - ver1_0->w_pos->z;

					t2 = ( ( near - ver1_0->w_pos->z ) / dz );

					//计算交点x、y坐标
					xi = ver1_0->w_pos->x + dx * t2;
					yi = ver1_0->w_pos->y + dy * t2;

					//用交点覆盖原来的顶点
					ver1_2->w_pos->x = xi;
					ver1_2->w_pos->y = yi;
					ver1_2->w_pos->z = near;

					//检查多边形是否带纹理
					//如果有，则对纹理坐标进行裁剪
					if ( NULL != face->texture )
					{
						ui = ver1_0->uv->u + ( ver1_1->uv->u - ver1_0->uv->u ) * t1;
						vi = ver1_0->uv->v + ( ver1_1->uv->v - ver1_0->uv->v ) * t1;

						ver1_1->uv->u = ui;
						ver1_1->uv->v = vi;

						ui = ver1_0->uv->u + ( ver1_2->uv->u - ver1_0->uv->u ) * t2;
						vi = ver1_0->uv->v + ( ver1_2->uv->v - ver1_0->uv->v ) * t2;

						ver1_2->uv->u = ui;
						ver1_2->uv->v = vi;
					}
				}
				else if ( verts_out == 1 )
				{
					//三角形被裁剪后为四边形，需要分割为两个三角形

					//累计裁剪多边形
					viewport->nClippList += 2;

					//检查当前裁剪列表指针的面指针是否为空
					//如果是则恢复为原始信息
					if ( ( * cl_ptr )->polygon )
					{
						face1 = ( * cl_ptr )->polygon;

						triangle_copy( face1, face );

						( * cl_ptr ) = ( * cl_ptr )->next;
					}
					//否则创建一个新三角形
					else
					{
						//克隆一个面
						face1 = triangle_clone( face );

						//插入裁剪列表
						insertFaceToList( cl_ptr, face1 );
					}

					if ( ( * cl_ptr )->polygon )
					{
						face2 = ( * cl_ptr )->polygon;

						triangle_copy( face2, face );

						( * cl_ptr ) = ( * cl_ptr )->next;
					}
					//否则创建一个新三角形
					else
					{
						//克隆一个面
						face2 = triangle_clone( face );

						//插入裁剪列表
						insertFaceToList( cl_ptr, face2 );
					}

					//找出位于外侧的顶点
					if ( zCode0 == 0x01 )
					{
						v0 = 0;	v1 = 1;	v2 = 2;
					}
					else if ( zCode1 == 0x02 )
					{
						v0 = 1;	v1 = 2;	v2 = 0;
					}
					else
					{
						v0 = 2;	v1 = 0;	v2 = 1;
					}

					ver1_0 = face1->vertex[v0];
					ver1_1 = face1->vertex[v1];
					ver1_2 = face1->vertex[v2];

					ver2_0 = face2->vertex[v0];
					ver2_1 = face2->vertex[v1];
					ver2_2 = face2->vertex[v2];

					//对各边裁剪

					//=======================对v0->v1边进行裁剪=======================
					dx = ver1_1->w_pos->x - ver1_0->w_pos->x;
					dy = ver1_1->w_pos->y - ver1_0->w_pos->y;
					dz = ver1_1->w_pos->z - ver1_0->w_pos->z;

					t1 = ( ( near - ver1_0->w_pos->z ) / dz );

					x01i = ver1_0->w_pos->x + dx * t1;
					y01i = ver1_0->w_pos->y + dy * t1;

					//=======================对v0->v2边进行裁剪=======================
					dx = ver1_2->w_pos->x - ver1_0->w_pos->x;
					dy = ver1_2->w_pos->y - ver1_0->w_pos->y;
					dz = ver1_2->w_pos->z - ver1_0->w_pos->z;

					t2 = ( ( near - ver1_0->w_pos->z ) / dz );

					x02i = ver1_0->w_pos->x + dx * t2;
					y02i = ver1_0->w_pos->y + dy * t2;

					//计算出交点后需要用交点1覆盖原来的三角形顶点
					//分割后的第一个三角形

					//用交点1覆盖原来三角形的顶点0
					ver1_0->w_pos->x = x01i;
					ver1_0->w_pos->y = y01i;
					ver1_0->w_pos->z = near;

					//用交点1覆盖新三角形的顶点1，交点2覆盖顶点0
					ver2_1->w_pos->x = x01i;
					ver2_1->w_pos->y = y01i;
					ver2_1->w_pos->z = near;

					ver2_0->w_pos->x = x02i;
					ver2_0->w_pos->y = y02i;
					ver2_0->w_pos->z = near;

					//检查多边形是否带纹理
					//如果有，则对纹理坐标进行裁剪
					if ( NULL != face->texture )
					{
						u01i = ver1_0->uv->u + ( ver1_1->uv->u - ver1_0->uv->u ) * t1;
						v01i = ver1_0->uv->v + ( ver1_1->uv->v - ver1_0->uv->v ) * t1;

						u02i = ver1_0->uv->u + ( ver1_2->uv->u - ver1_0->uv->u ) * t2;
						v02i = ver1_0->uv->v + ( ver1_2->uv->v - ver1_0->uv->v ) * t2;

						//覆盖原来的纹理坐标
						ver1_0->uv->u = u01i;
						ver1_0->uv->v = v01i;

						ver2_1->uv->u = u01i;
						ver2_1->uv->v = v01i;

						ver2_0->uv->u = u02i;
						ver2_0->uv->v = v02i;
					}

					//计算顶点法线
				}
				//所有顶点在近截面外则
				else
				{
					//累计剔除多边形
					viewport->nCullList ++;

					continue;
				}
			}
		}

		//插入新建的面
		//如果没有新面，则插入原来的面
		if ( NULL == face1 && NULL == face2 )
		{
			insertFaceToList( rl_ptr, face );

			viewport->nRenderList ++;
		}
		//否则插入新面
		else
		{
			if ( NULL != face1 )
			{
				insertFaceToList( rl_ptr, face1 );

				viewport->nRenderList ++;
			}

			if ( NULL != face2 )
			{
				insertFaceToList( rl_ptr, face2 );

				viewport->nRenderList ++;
			}
		}
	}
}

void viewport_project( Viewport * viewport )
{
	Scene * scene;
	SceneNode * sceneNode;
	Camera * camera;
	Entity * entity;
	float zn, worldZFar;
	RenderList * curr_rl_ptr, * rl_ptr, * cl_ptr, * renderList;

	//光照变量
	Vertex * vs;
	Material * material;
	Lights * lights;
	Light * light;
	Vector3D vFDist, vLightToVertex, vVertexToLight, vVertexToCamera;
	FloatColor fColor, lastColor, mColor, outPutColor;
	float dot, fAttenuCoef, fc1, fc2, fDist, fSpotFactor, fShine, fShineFactor;

	float invw;

	int l = 0, j = 0, code = 0;

	//如果有光源
	//此数组用于记录以本地作为参考点的光源方向
	//if ( NULL != scene->lights ) if( ( vLightsToObject = ( Vector3D * )calloc( scene->nLights, sizeof( Vector3D ) ) ) == NULL ) exit( TRUE );
	Vector3D vLightsToObject[MAX_LIGHTS];

#ifdef __AS3__
	int k = 0;
	float * meshBuffer;
#endif

	scene = viewport->scene;
	camera = viewport->camera;
	zn = camera->near + camera->eye->position->z;
	worldZFar = camera->far + camera->eye->position->z;

	//把渲染列表的指针指向表头的下一个结点
	rl_ptr = viewport->renderList->next;
	cl_ptr = viewport->clippedList->next;

	//如果摄像机或视口的属性改变
	//则重新计算投影矩阵
	if ( TRUE == camera->fnfDirty || TRUE == viewport->dirty)
	{
		getPerspectiveFovLH( camera->projectionMatrix, camera, viewport->aspect );

		camera->fnfDirty = viewport->dirty = FALSE;
	}

	//matrix3D_append4x4( & projection, camera->eye->world, camera->projectionMatrix );

	//遍历实体
	sceneNode = scene->nodes;
	while( NULL != sceneNode )
	{
		entity = sceneNode->entity;

		//更新实体的局部和世界坐标
		entity_updateTransform(entity);

		if ( NULL != entity->mesh )
		{
			code = 0;

			matrix3D_append( entity->view, entity->world, camera->eye->world );

			//连接透视投影矩阵
			matrix3D_append4x4( entity->projection, entity->view, camera->projectionMatrix );

			//构造基于CVV的AABB
			mesh_transformNewAABB( entity->mesh->CVVAABB, entity->projection, entity->mesh->aabb );

			//基于包围盒的视锥体剔除
			if ( entity->mesh->CVVAABB->max->x < -1 )				code |= 0x01;
			if ( entity->mesh->CVVAABB->min->x > 1 )				code |= 0x02;
			if ( entity->mesh->CVVAABB->max->y < -1 )				code |= 0x04;
			if ( entity->mesh->CVVAABB->min->y > 1 )				code |= 0x08;
			if ( entity->mesh->CVVAABB->max->w <= camera->near )	code |= 0x10;
			if ( entity->mesh->CVVAABB->min->w >= camera->far )		code |= 0x20;

			//输出码为非零侧实体离屏
			if ( code > 0 )
			{
				//累计离屏多边形
				viewport->nCullList += entity->mesh->nFaces;

				sceneNode = sceneNode->next;

				continue;
			}

			//计算视点在实体的局部坐标
			matrix3D_transformVector( entity->viewerToLocal, entity->worldInvert, camera->eye->position );

			//遍历顶点
#ifdef __AS3__
			meshBuffer = entity->mesh->meshBuffer;

			for( j = 0, k = 0; j < entity->mesh->nVertices; j ++, k += VERTEX_SIZE)
			{
#else
			for ( j = 0; j < entity->mesh->nVertices; j ++)
			{
#endif
				vs = & entity->mesh->vertices[j];

#ifdef __AS3__
				//如果顶点局部坐标发生改变
				if ( TRUE == entity->mesh->dirty )
				{
					vs->position->x = meshBuffer[k];
					vs->position->y = meshBuffer[k + 1];
					vs->position->z = meshBuffer[k + 2];
				}
#endif

				//把顶点变换到视空间，这里fix_inv_z标记1/z，并以4.28定点数表示
				matrix3D_transformVector( vs->w_pos, entity->view, vs->position );
			}

			//记录当前的渲染列表指针
			curr_rl_ptr	= rl_ptr;

			//多边形裁剪
			frustumClipping( viewport, entity, camera->near, & rl_ptr, & cl_ptr );

			//========================光照计算======================

			//光源索引为零
			l = 0;

			material = entity->mesh->material;

			//更新光源位置
			lights = scene->lights;

			//遍历光源
			while ( NULL != lights && entity->lightEnable )
			{
				if ( TRUE == lights->light->bOnOff )
				{
					//用实体的世界逆矩阵变换光源的位置得到以实体为参考点的光源的位置，保存在数组
					matrix3D_transformVector( & vLightsToObject[l], entity->worldInvert, lights->light->source->w_pos );
				}

				lights = lights->next;

				l ++;
			}

			//基于在效率和真实感之间取得一个平衡，不考虑全局环境光的贡献，直接使用材质的环境光反射系数作为最终颜色
			floatColor_zero( & mColor );
			floatColor_add_self( & mColor, material->ambient );

			//遍历渲染列表
			renderList = curr_rl_ptr;

			while ( NULL != renderList && NULL != renderList->polygon )
			{
				//为多边形标记渲染模式
				renderList->polygon->render_mode = entity->mesh->render_mode;

				//遍历顶点
				for ( j = 0; j < 3; j ++)
				{
					vs = renderList->polygon->vertex[j];

					//如果顶点已经变换或计算，则直接进入下一个顶点
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
						while ( NULL != lights && entity->lightEnable )
						{
							//如果光源是关闭的
							if ( FALSE == lights->light->bOnOff )
							{
								lights = lights->next;

								l ++;

								continue;
							}

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

					matrix3D_transformVector( vs->s_pos, camera->projectionMatrix, vs->w_pos );

					invw = 1.0f / vs->s_pos->w;

					vs->s_pos->x *= invw;
					vs->s_pos->y *= invw;

					vs->s_pos->x += 0.5f;
					vs->s_pos->y += 0.5f;

					vs->s_pos->x *= viewport->width;
					vs->s_pos->y *= viewport->height;

					vs->fix_inv_z = (1 << FIXP28_SHIFT) / (int)(vs->s_pos->w + 0.5f );

					vs->color->red = (BYTE)(lastColor.red * 255.0f);
					vs->color->green = (BYTE)(lastColor.green * 255.0f);
					vs->color->blue = (BYTE)(lastColor.blue * 255.0f);
					vs->color->alpha = (BYTE)(lastColor.alpha * 255.0f);

					vs->transformed = TRUE;
				}

				renderList = renderList->next;
			}
		}//end NULL != mesh

		sceneNode = sceneNode->next;
	}
}

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

		//选择渲染器
		switch ( face->render_mode )
		{
			case RENDER_WIREFRAME_TRIANGLE_32 :
				Draw_Wireframe_Triangle_32( face, view->videoBuffer, view->width * sizeof( DWORD ), 0, view->width - 1, 0, view->height - 1 );
				break;

			case RENDER_TEXTRUED_TRIANGLE_INVZB_32 :
				Draw_Textured_Triangle_INVZB_32( face, view->videoBuffer, view->width * sizeof( DWORD ), view->zBuffer, view->width * sizeof( DWORD ), 0, view->width - 1, 0, view->height - 1 );
				break;

			case RENDER_TEXTRUED_BILERP_TRIANGLE_INVZB_32:
				Draw_Textured_Bilerp_Triangle_INVZB_32( face, view->videoBuffer, view->width * sizeof( DWORD ), view->zBuffer, view->width * sizeof( DWORD ), 0, view->width - 1, 0, view->height - 1 );
				break;

			case RENDER_TEXTRUED_TRIANGLE_FSINVZB_32:
				Draw_Textured_Triangle_FSINVZB_32( face, view->videoBuffer, view->width * sizeof( DWORD ), view->zBuffer, view->width * sizeof( DWORD ), 0, view->width - 1, 0, view->height - 1 );
				break;

			case RENDER_TEXTRUED_TRIANGLE_GSINVZB_32:
				Draw_Textured_Triangle_GSINVZB_32( face, view->videoBuffer, view->width * sizeof( DWORD ), view->zBuffer, view->width * sizeof( DWORD ), 0, view->width - 1, 0, view->height - 1 );
				break;

			case RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_INVZB_32:
				Draw_Textured_Perspective_Triangle_INVZB_32( face, view->videoBuffer, view->width * sizeof( DWORD ), view->zBuffer, view->width * sizeof( DWORD ), 0, view->width - 1, 0, view->height - 1 );
				break;

			case RENDER_TEXTRUED_PERSPECTIVE_TRIANGLELP_INVZB_32:
				Draw_Textured_PerspectiveLP_Triangle_INVZB_32( face, view->videoBuffer, view->width * sizeof( DWORD ), view->zBuffer, view->width * sizeof( DWORD ), 0, view->width - 1, 0, view->height - 1 );
				break;

			case RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_FSINVZB_32:
				Draw_Textured_Perspective_Triangle_FSINVZB_32( face, view->videoBuffer, view->width * sizeof( DWORD ), view->zBuffer, view->width * sizeof( DWORD ), 0, view->width - 1, 0, view->height - 1 );
				break;

			case RENDER_TEXTRUED_PERSPECTIVE_TRIANGLELP_FSINVZB_32:
				Draw_Textured_PerspectiveLP_Triangle_FSINVZB_32( face, view->videoBuffer, view->width * sizeof( DWORD ), view->zBuffer, view->width * sizeof( DWORD ), 0, view->width - 1, 0, view->height - 1 );
				break;

			case RENDER_FLAT_TRIANGLE_32 :
				Draw_Flat_Triangle_32( face, view->videoBuffer, view->width * sizeof( DWORD ), 0, view->width - 1, 0, view->height - 1 );
				break;

			case RENDER_FLAT_TRIANGLEFP_32 :
				Draw_Flat_TriangleFP_32( face, view->videoBuffer, view->width * sizeof( DWORD ), 0, view->width - 1, 0, view->height - 1 );
				break;

			case RENDER_FLAT_TRIANGLE_INVZB_32:
				Draw_Flat_Triangle_INVZB_32( face, view->videoBuffer, view->width * sizeof( DWORD ), view->zBuffer, view->width * sizeof( DWORD ), 0, view->width - 1, 0, view->height - 1 );
				break;

			case RENDER_GOURAUD_TRIANGLE_INVZB_32:
				Draw_Gouraud_Triangle_INVZB_32( face, view->videoBuffer, view->width * sizeof( DWORD ), view->zBuffer, view->width * sizeof( DWORD ), 0, view->width - 1, 0, view->height - 1 );
				break;
		}

		rl = rl->next;
	}
}

#endif
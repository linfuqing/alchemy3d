#ifndef __VIEWPORT_H
#define __VIEWPORT_H

#include <malloc.h>
#include <float.h>

#include "Animation.h"
#include "Entity.h"
#include "Scene.h"
#include "Camera.h"
#include "Matrix3D.h"
#include "Vector3D.h"
#include "Terrain.h"

typedef struct Viewport
{
	//高宽积，是否更改属性，渲染数，裁剪数，剔除数
	int wh, dirty, nClippList, nCullList;

	//视口宽高和宽高比
	int width, height, mempitch, zpitch;

	float aspect;

	//深度缓冲区
	LPBYTE zBuffer;

	//视频缓冲区
	LPBYTE videoBuffer;

	struct Scene * scene;

	struct Camera * camera;

}Viewport;

void viewport_resize( Viewport * viewport, int width, int height )
{
	int wh = width * height;

	viewport->width = width;
	viewport->height = height;
	viewport->aspect = (float)width / (float)height;
	viewport->wh = wh;
	viewport->mempitch = width * sizeof( DWORD );
	viewport->zpitch = width * sizeof( DWORD );
	viewport->dirty = TRUE;

	if ( NULL != viewport->videoBuffer )
	{
		free( viewport->videoBuffer );

		memset( viewport->videoBuffer, 0, sizeof( BYTE ) );
	}
	if ( NULL != viewport->zBuffer )
	{
		free( viewport->zBuffer );
	
		memset( viewport->zBuffer, 0, sizeof( BYTE ) );
	}

	//初始化缓冲区
	if( ( viewport->videoBuffer		= ( LPBYTE )calloc( wh * sizeof( DWORD ), sizeof( BYTE ) ) ) == NULL ) exit( TRUE );
	if( ( viewport->zBuffer			= ( LPBYTE )calloc( wh * sizeof( DWORD ), sizeof( BYTE ) ) ) == NULL ) exit( TRUE );
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

	return viewport;
}

//渲染前更新
void viewport_updateBeforeRender( Viewport * viewport )
{
#ifdef __NOT_AS3__

	LPDWORD zBuf = ( LPDWORD )viewport->zBuffer;

	LPDWORD videoBuffer = ( LPDWORD )viewport->videoBuffer;

	int wh = viewport->wh;

	int m = 0;
	
	Mem_Set_QUAD( ( void * )zBuf, 0, wh );
	Mem_Set_QUAD( ( void * )videoBuffer, 0, wh );

#endif

	if ( viewport->scene->fog && ( ! viewport->scene->fog->ready || viewport->camera->fnfDirty ) )
	{
		buildFogTable( viewport->scene->fog, viewport->camera->far - viewport->camera->near );

		viewport->scene->fog->ready = TRUE;
	}

	//terrain_sceneTrace( viewport -> scene -> nodes );
}

INLINE void viewport_updateAfterRender( Viewport * viewport )
{
	viewport->camera->fnfDirty = viewport->dirty = FALSE;

	scene_updateAfterRender( viewport->scene );
}

//基于AABB包围盒的视空间裁剪
//viewport		视口
//camera		摄像机
//entity		当前要处理的实体
//octreeData	八叉树
//rl_ptr		当前渲染列表的指针
//cl_ptr		当前裁剪列表的指针
void frustumClipping( Camera * camera, Entity * entity, OctreeData * octreeData, RenderList ** rl_ptr, RenderList ** cl_ptr )
{
	int j, zCode0, zCode1, zCode2, verts_out, v0, v1, v2;
	float t1, t2,  xi, yi, ui, vi, x01i, x02i, y01i, y02i, u01i, u02i, v01i, v02i, dx, dy, dz, near = camera->near, far = camera->far;
	Vector3D viewerToLocal;
	Triangle * face, * face1, * face2;
	Vertex * tmpVert1, * tmpVert2, * ver1_0, * ver1_1, * ver1_2, * ver2_0, * ver2_1, * ver2_2;
	Mesh * mesh = entity->mesh;

	//遍历面
	for ( j = 0; j < octreeData->nFaces; j ++)
	{
		face = octreeData->faces[j];

		zCode0 = zCode1 = zCode2 = 0;

		//越界标记置0
		verts_out = FALSE;

		face1 = face2 = NULL;

		tmpVert1 = tmpVert2 = NULL;

		//背面剔除
		//线框模式将不进行背面剔除
		if ( face->render_mode != RENDER_WIREFRAME_TRIANGLE_32 )
		{
			//如果夹角大于90或小于-90时，即背向摄像机
			if ( triangle_backFaceCulling( face, & viewerToLocal, entity->viewerToLocal ) )
			{
				mesh->nCullList ++;

				continue;
			}
		}

		triangle_transform( entity->world, entity->projection, face );

		if (face->vertex[0]->v_pos->x < - face->vertex[0]->v_pos->w &&
			face->vertex[1]->v_pos->x < - face->vertex[1]->v_pos->w &&
			face->vertex[2]->v_pos->x < - face->vertex[2]->v_pos->w )
		{
			mesh->nCullList ++;

			face->vertex[0]->transformed = FALSE;
			face->vertex[1]->transformed = FALSE;
			face->vertex[2]->transformed = FALSE;

			continue;
		}

		if (face->vertex[0]->v_pos->x > face->vertex[0]->v_pos->w &&
			face->vertex[1]->v_pos->x > face->vertex[1]->v_pos->w &&
			face->vertex[2]->v_pos->x > face->vertex[2]->v_pos->w )
		{
			mesh->nCullList ++;

			face->vertex[0]->transformed = FALSE;
			face->vertex[1]->transformed = FALSE;
			face->vertex[2]->transformed = FALSE;

			continue;
		}

		if (face->vertex[0]->v_pos->y < - face->vertex[0]->v_pos->w &&
			face->vertex[1]->v_pos->y < - face->vertex[1]->v_pos->w &&
			face->vertex[2]->v_pos->y < - face->vertex[2]->v_pos->w )
		{
			mesh->nCullList ++;

			face->vertex[0]->transformed = FALSE;
			face->vertex[1]->transformed = FALSE;
			face->vertex[2]->transformed = FALSE;

			continue;
		}

		if (face->vertex[0]->v_pos->y > face->vertex[0]->v_pos->w &&
			face->vertex[1]->v_pos->y > face->vertex[1]->v_pos->w &&
			face->vertex[2]->v_pos->y > face->vertex[2]->v_pos->w )
		{
			mesh->nCullList ++;

			face->vertex[0]->transformed = FALSE;
			face->vertex[1]->transformed = FALSE;
			face->vertex[2]->transformed = FALSE;

			continue;
		}

		if (face->vertex[0]->v_pos->w > far ||
			face->vertex[1]->v_pos->w > far ||
			face->vertex[2]->v_pos->w > far )
		{
			mesh->nCullList ++;

			face->vertex[0]->transformed = FALSE;
			face->vertex[1]->transformed = FALSE;
			face->vertex[2]->transformed = FALSE;

			continue;
		}

		if ( face->vertex[0]->v_pos->w < near )
		{
			verts_out ++;

			zCode0 = 0x01;
		}

		if ( face->vertex[1]->v_pos->w < near )
		{
			verts_out ++;

			zCode1 = 0x02;
		}

		if ( face->vertex[2]->v_pos->w < near )
		{
			verts_out ++;

			zCode2 = 0x04;
		}

		if ( verts_out == 3 )
		{
			mesh->nCullList ++;

			face->vertex[0]->transformed = FALSE;
			face->vertex[1]->transformed = FALSE;
			face->vertex[2]->transformed = FALSE;

			continue;
		}
		//开始裁剪
		//分两种情况
		//1、只有一个顶点在近截面内侧
		else if ( verts_out == 2 )
		{
			//累计裁剪多边形
			mesh->nClippList ++;

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
				renderList_push( cl_ptr, face1 );
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
			dx = ver1_1->v_pos->x - ver1_0->v_pos->x;
			dy = ver1_1->v_pos->y - ver1_0->v_pos->y;
			dz = ver1_1->v_pos->w - ver1_0->v_pos->w;

			t1 = ( ( near - ver1_0->v_pos->w ) / dz );

			//计算交点x、y坐标
			xi = ver1_0->v_pos->x + dx * t1;
			yi = ver1_0->v_pos->y + dy * t1;

			//用交点覆盖原来的顶点
			ver1_1->v_pos->x = xi;
			ver1_1->v_pos->y = yi;
			ver1_1->v_pos->w = near;
			ver1_1->s_pos->x = xi / near;
			ver1_1->s_pos->y = yi / near;

			//=======================对v0->v2边进行裁剪=======================
			dx = ver1_2->v_pos->x - ver1_0->v_pos->x;
			dy = ver1_2->v_pos->y - ver1_0->v_pos->y;
			dz = ver1_2->v_pos->w - ver1_0->v_pos->w;

			t2 = ( ( near - ver1_0->v_pos->w ) / dz );

			//计算交点x、y坐标
			xi = ver1_0->v_pos->x + dx * t2;
			yi = ver1_0->v_pos->y + dy * t2;

			//用交点覆盖原来的顶点
			ver1_2->v_pos->x = xi;
			ver1_2->v_pos->y = yi;
			ver1_2->v_pos->w = near;
			ver1_2->s_pos->x = xi / near;
			ver1_2->s_pos->y = yi / near;

			//检查多边形是否带纹理
			//如果有，则对纹理坐标进行裁剪
			if ( face->texture && face->texture->mipmaps )
			{
				ui = face1->uv[v0]->u + ( face1->uv[v1]->u - face1->uv[v0]->u ) * t1;
				vi = face1->uv[v0]->v + ( face1->uv[v1]->v - face1->uv[v0]->v ) * t1;

				face1->uv[v1]->u = ui;
				face1->uv[v1]->v = vi;

				ui = face1->uv[v0]->u + ( face1->uv[v2]->u - face1->uv[v0]->u ) * t2;
				vi = face1->uv[v0]->v + ( face1->uv[v2]->v - face1->uv[v0]->v ) * t2;

				face1->uv[v2]->u = ui;
				face1->uv[v2]->v = vi;

				face1->uvTransformed = FALSE;
			}
			
			face->vertex[0]->transformed = FALSE;
			face->vertex[1]->transformed = FALSE;
			face->vertex[2]->transformed = FALSE;
		}
		else if ( verts_out == 1 )
		{
			//三角形被裁剪后为四边形，需要分割为两个三角形

			//累计裁剪多边形
			mesh->nClippList += 2;

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
				renderList_push( cl_ptr, face1 );
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
				renderList_push( cl_ptr, face2 );
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
			dx = ver1_1->v_pos->x - ver1_0->v_pos->x;
			dy = ver1_1->v_pos->y - ver1_0->v_pos->y;
			dz = ver1_1->v_pos->w - ver1_0->v_pos->w;

			t1 = ( ( near - ver1_0->v_pos->w ) / dz );

			x01i = ver1_0->v_pos->x + dx * t1;
			y01i = ver1_0->v_pos->y + dy * t1;

			//=======================对v0->v2边进行裁剪=======================
			dx = ver1_2->v_pos->x - ver1_0->v_pos->x;
			dy = ver1_2->v_pos->y - ver1_0->v_pos->y;
			dz = ver1_2->v_pos->w - ver1_0->v_pos->w;

			t2 = ( ( near - ver1_0->v_pos->w ) / dz );

			x02i = ver1_0->v_pos->x + dx * t2;
			y02i = ver1_0->v_pos->y + dy * t2;

			//计算出交点后需要用交点1覆盖原来的三角形顶点
			//分割后的第一个三角形

			//用交点1覆盖原来三角形的顶点0
			ver1_0->v_pos->x = x01i;
			ver1_0->v_pos->y = y01i;
			ver1_0->v_pos->w = near;
			ver1_0->s_pos->x = x01i / near;
			ver1_0->s_pos->y = y01i / near;

			//用交点1覆盖新三角形的顶点1，交点2覆盖顶点0
			ver2_1->v_pos->x = x01i;
			ver2_1->v_pos->y = y01i;
			ver2_1->v_pos->w = near;
			ver2_1->s_pos->x = x01i / near;
			ver2_1->s_pos->y = y01i / near;

			ver2_0->v_pos->x = x02i;
			ver2_0->v_pos->y = y02i;
			ver2_0->v_pos->w = near;
			ver2_0->s_pos->x = x02i / near;
			ver2_0->s_pos->y = y02i / near;

			//检查多边形是否带纹理
			//如果有，则对纹理坐标进行裁剪
			if ( face->texture && face->texture->mipmaps )
			{
				u01i = face1->uv[v0]->u + ( face1->uv[v1]->u - face1->uv[v0]->u ) * t1;
				v01i = face1->uv[v0]->v + ( face1->uv[v1]->v - face1->uv[v0]->v ) * t1;

				u02i = face1->uv[v0]->u + ( face1->uv[v2]->u - face1->uv[v0]->u ) * t2;
				v02i = face1->uv[v0]->v + ( face1->uv[v2]->v - face1->uv[v0]->v ) * t2;

				//覆盖原来的纹理坐标
				face1->uv[v0]->u = u01i;
				face1->uv[v0]->v = v01i;

				face2->uv[v1]->u = u01i;
				face2->uv[v1]->v = v01i;

				face2->uv[v0]->u = u02i;
				face2->uv[v0]->v = v02i;

				face1->uvTransformed = FALSE;
				face2->uvTransformed = FALSE;
			}

			face->vertex[0]->transformed = FALSE;
			face->vertex[1]->transformed = FALSE;
			face->vertex[2]->transformed = FALSE;
		}

		//插入新建的面
		//如果没有新面，则插入原来的面
		if ( NULL == face1 && NULL == face2 )
		{
			renderList_push( rl_ptr, face );

			mesh->nRenderList ++;
		}
		//否则插入新面
		else
		{
			if ( NULL != face1 )
			{
				renderList_push( rl_ptr, face1 );

				mesh->nRenderList ++;
			}

			if ( NULL != face2 )
			{
				renderList_push( rl_ptr, face2 );

				mesh->nRenderList ++;
			}
		}
	}
}

int octree_culling( Camera * camera, Entity * entity, Octree * octree, RenderList ** rl_ptr, RenderList ** cl_ptr )
{
	int code = 0, noChild = 0;

	AABB * aabb;
	
	aabb_setToTransformedBox( octree->data->worldAABB, octree->data->aabb, entity->world );
	aabb_setToTransformedBox_CVV( octree->data->CVVAABB, octree->data->aabb, entity->projection );

	aabb = octree->data->CVVAABB;

	if ( aabb->max->x <= -aabb->max->w )	code |= 0x01;
	if ( aabb->min->x >= aabb->max->w )		code |= 0x02;
	if ( aabb->max->y <= -aabb->max->w )	code |= 0x04;
	if ( aabb->min->y >= aabb->max->w )		code |= 0x08;
	if ( aabb->max->w <= camera->near )		code |= 0x10;
	if ( aabb->min->w >= camera->far )		code |= 0x20;

	//输出码为非零侧包围盒离屏
	if ( code > 0 )
	{
		entity->mesh->nCullList += octree->data->nFaces;

		return 0;
	}
	else
	{
		code = 0;

		if ( aabb->max->x <= aabb->max->w )		code |= 0x01;
		if ( aabb->min->x >= -aabb->max->w )	code |= 0x02;
		if ( aabb->max->y <= aabb->max->w )		code |= 0x04;
		if ( aabb->min->y >= -aabb->max->w )	code |= 0x08;
		if ( aabb->max->w <= camera->far )		code |= 0x10;
		if ( aabb->min->w >= camera->near )		code |= 0x20;

		//如果包围盒完全在视锥体内
		//直接把所有多边形添加到渲染列表
		if ( code == 0x3f )
		{
			Triangle * face;

			Vector3D viewerToLocal;

			for ( code = 0; code < octree->data->nFaces; code ++ )
			{
				face = octree->data->faces[code];

				if ( triangle_backFaceCulling( face, & viewerToLocal, entity->viewerToLocal ) )
				{
					entity->mesh->nCullList ++;

					continue;
				}

				triangle_transform( entity->world, entity->projection, face );

				renderList_push( rl_ptr, face );

				entity->mesh->nRenderList ++;
			}

			return 2;
		}
		//否则包围盒将穿越任一边界
		//交给裁剪函数处理
		else
		{
			//如果有子结点则继续递归
			if ( octree->tlb )	code = octree_culling( camera, entity, octree->tlb, rl_ptr, cl_ptr );
			else				noChild |= 0x01;

			if ( octree->tlf )	code = octree_culling( camera, entity, octree->tlf, rl_ptr, cl_ptr );
			else				noChild |= 0x02;

			if ( octree->trb )	code = octree_culling( camera, entity, octree->trb, rl_ptr, cl_ptr );
			else				noChild |= 0x04;

			if ( octree->trf )	code = octree_culling( camera, entity, octree->trf, rl_ptr, cl_ptr );
			else				noChild |= 0x08;

			if ( octree->blb )	code = octree_culling( camera, entity, octree->blb, rl_ptr, cl_ptr );
			else				noChild |= 0x10;

			if ( octree->blf )	code = octree_culling( camera, entity, octree->blf, rl_ptr, cl_ptr );
			else				noChild |= 0x20;

			if ( octree->brb )	code = octree_culling( camera, entity, octree->brb, rl_ptr, cl_ptr );
			else				noChild |= 0x40;

			if ( octree->brf )	code = octree_culling( camera, entity, octree->brf, rl_ptr, cl_ptr );
			else				noChild |= 0x80;

			if ( noChild == 0xff ) frustumClipping( camera, entity, octree->data, rl_ptr, cl_ptr );
		}
	}

	return 1;
}

void viewport_lightting( Viewport * viewport )
{
	Entity * entity;
	RenderList * renderList;
	SceneNode * sceneNode;
	Mesh * mesh;

	//光照变量
	Vertex * vs;
	Material * material;
	Lights * lights;
	Light * light;
	Vector3D vFDist, vLightToVertex, vVertexToLight, vVertexToCamera;
	FloatColor fColor, lastColor, outPutColor;
	float dot, fAttenuCoef, fc1, fc2, fDist, fSpotFactor, fShine, fShineFactor;
	DWORD l = 0, j = 0, i = 0;
	//如果有光源
	//此数组用于记录以本地作为参考点的光源方向
	Vector3D vLightsToObject[MAX_LIGHTS];

	//雾化变量
	//Fog * fog = viewport->scene->fog;
	//float f_concentration;

	//遍历实体
	sceneNode = viewport->scene->nodes;

	while( sceneNode )
	{
		entity = sceneNode->entity;
		mesh = entity->mesh;

		if ( mesh && mesh->nFaces && mesh->nVertices )
		{
			//光源索引为零
			l = 0;

			//更新光源位置
			lights = viewport->scene->lights;

			//遍历光源
			while ( NULL != lights && mesh->lightEnable )
			{
				if ( TRUE == lights->light->bOnOff )
				{
					//用实体的世界逆矩阵变换光源的位置得到以实体为参考点的光源的位置，保存在数组
					matrix3D_transformVector( & vLightsToObject[l], entity->worldInvert, lights->light->source->w_pos );
				}

				lights = lights->next;

				l ++;
			}

			//遍历渲染列表
			renderList = mesh->renderList->next;

			for ( i = 0; i < mesh->nRenderList; i ++ )
			{
				material = renderList->polygon->material;

				//遍历顶点
				for ( j = 0; j < 3; j ++)
				{
					vs = renderList->polygon->vertex[j];

					//如果顶点已经变换或计算，则直接进入下一个顶点
					if ( vs->transformed == 2 ) continue;

					vs->s_pos->x = ( vs->s_pos->x + 1.0f ) * viewport->width * 0.5f;
					vs->s_pos->y = ( vs->s_pos->y + 1.0f ) * viewport->height * 0.5f;
					vs->s_pos->w = vs->v_pos->w;

					vs->fix_inv_z = ( 1 << FIXP28_SHIFT ) / (int)( vs->v_pos->w + 0.5f );

					//===================光照和顶点颜色值===================

					//光源索引为零
					l = 0;

					floatColor_identity( & lastColor);

					if ( NULL != material )
					{
						//基于在效率和真实感之间取得一个平衡，不考虑全局环境光的贡献，直接使用材质的环境光反射系数作为最终颜色
						//复制材质颜色到最终颜色
						floatColor_copy( & lastColor, material->ambient );

						//临时颜色清零
						floatColor_zero( & fColor );

						lights = viewport->scene->lights;

						//来自于光源的贡献
						//如果有光源
						while ( NULL != lights && mesh->lightEnable )
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
									fDist = vector3D_lengthSquared( vector3D_subtract( & vFDist, & vLightsToObject[l], vs->position ) );

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
								vector3D_subtract( & vLightToVertex, & vLightsToObject[l], vs->position );

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

							lights = lights->next;

							l ++;
						}//end light

						//if ( fog && fog->ready && mesh->fogEnable )
						//{
						//	f_concentration = fog->fog_table[(int)vs->v_pos->w];

						//	AS3_Trace(AS3_Number(vs->v_pos->w));

						//	floatColor_add( & fColor, fog->global, & lastColor );
						//	floatColor_copy( & fColor, fog->global );

						//	floatColor_scaleBy_self( & fColor, f_concentration, f_concentration, f_concentration, 1.0f );

						//	floatColor_add_self( & lastColor, & fColor );
						//}

						//作颜色归一化处理
						floatColor_normalize( & lastColor );

						//对于alpha, 这里简单地用材质漫反射属性来代替
						lastColor.alpha = material->diffuse->alpha;
					}

					vs->color->red = (BYTE)(lastColor.red * 255.0f);
					vs->color->green = (BYTE)(lastColor.green * 255.0f);
					vs->color->blue = (BYTE)(lastColor.blue * 255.0f);
					vs->color->alpha = (BYTE)(lastColor.alpha * 255.0f);

					vs->transformed = 2;
				}

				renderList = renderList->next;
			}
		}

		sceneNode = sceneNode->next;
	}
}

void viewport_project( Viewport * viewport, int time )
{
	Scene * scene;
	SceneNode * sceneNode;
	Camera * camera;
	Entity * entity;
	RenderList * rl, * cl;

	scene = viewport->scene;
	camera = viewport->camera;

	//更新摄像机矩阵
	camera_updateTransform( camera );

	//如果摄像机或视口的属性改变
	//则重新计算投影矩阵
	if ( TRUE == camera->fnfDirty || TRUE == viewport->dirty )
		getPerspectiveFovLH( camera->projectionMatrix, camera, viewport->aspect );

	//遍历实体
	sceneNode = scene->nodes;

	while( sceneNode )
	{
		entity = sceneNode->entity;

		if( entity->animation )
			animation_update( entity->animation, time );

		entity_updateTransform(entity);

		if ( entity->mesh && entity -> mesh -> nFaces && entity -> mesh -> nVertices )
		{
			//重新计算法向量以及构造八叉树（如果顶点是静态的只会执行一次）
			mesh_updateMesh( entity->mesh );

			matrix3D_append( entity->view, entity->world, camera->eye->world );

			//连接透视投影矩阵
			matrix3D_append4x4( entity->projection, entity->view, camera->projectionMatrix );

			//计算视点在实体的局部坐标
			matrix3D_transformVector( entity->viewerToLocal, entity->worldInvert, camera->eye->position );

			rl = entity->mesh->renderList->next;
			cl = entity->mesh->clippedList->next;

			//八叉
			if ( ! octree_culling( camera, entity, entity->mesh->octree, & rl, & cl ) )
			{
				sceneNode = sceneNode->next;

				continue;
			}
		}

		sceneNode = sceneNode->next;
	}

	viewport_lightting( viewport );
}

#include "RenderFGTINVZB.h"
#include "RenderWF.h"

void viewport_render( Viewport * viewport )
{
	DWORD tmiplevels, miplevel, i;
	Entity * entity;
	RenderList * rl;
	SceneNode * sceneNode;
	Mesh * mesh;
	Triangle * face;
	Bitmap * bitmap;
	float minZ;

	//遍历实体
	sceneNode = viewport->scene->nodes;

	while( sceneNode )
	{
		entity = sceneNode->entity;
		mesh = entity->mesh;

		if ( mesh && mesh->nFaces && mesh->nVertices )
		{
			//遍历渲染列表
			rl = mesh->renderList->next;

			for ( i = 0; i < mesh->nRenderList; i ++ )
			{
				face = rl->polygon;

				minZ = face->vertex[0]->v_pos->w;
				minZ = MIN( minZ, face->vertex[1]->v_pos->w );
				minZ = MIN( minZ, face->vertex[2]->v_pos->w );

				//AS3_Trace(AS3_Number(face->texture->perspective_dist));

				if ( face->texture && face->texture->mipmaps )
				{
					if ( mesh->useMipmap && mesh->mip_dist )
					{
						tmiplevels = logbase2ofx[face->texture->mipmaps[0]->width];

						miplevel = (int)(tmiplevels * minZ / mesh->mip_dist);

						if ( miplevel > tmiplevels ) miplevel = tmiplevels;

						bitmap = face->texture->mipmaps[miplevel];

						if ( miplevel != face->miplevel || ! face->uvTransformed )
						{
							triangle_setUV( face, bitmap->width, bitmap->height, face->texture->addressMode );

							face->miplevel = miplevel;

							face->uvTransformed = TRUE;
						}
					}
					else
					{
						bitmap = face->texture->mipmaps[0];

						if ( ! face->uvTransformed )
						{
							triangle_setUV( face, bitmap->width, bitmap->height, face->texture->addressMode );

							face->uvTransformed = TRUE;
						}
					}

					switch ( face->render_mode )
					{
						case RENDER_WIREFRAME_TRIANGLE_32:
							Draw_Wireframe_Triangle_32( face, viewport );
							break;

						case RENDER_TEXTRUED_TRIANGLE_INVZB_32:
							Draw_Textured_Triangle_INVZB_32( face, viewport );
							break;

						case RENDER_TEXTRUED_BILERP_TRIANGLE_INVZB_32:
							Draw_Textured_Bilerp_Triangle_INVZB_32( face, viewport );
							break;

						case RENDER_TEXTRUED_TRIANGLE_FSINVZB_32:
							Draw_Textured_Triangle_FSINVZB_32( face, viewport );
							break;

						case RENDER_TEXTRUED_TRIANGLE_GSINVZB_32:
							Draw_Textured_Triangle_GSINVZB_32( face, viewport );
							break;

						case RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_FSINVZB_32:

							if ( minZ < face->texture->perspective_dist )
								Draw_Textured_Perspective_Triangle_FSINVZB_32( face, viewport );
							else
								Draw_Textured_Triangle_FSINVZB_32( face, viewport );
							break;

						case RENDER_TEXTRUED_PERSPECTIVE_TRIANGLELP_FSINVZB_32:

							if ( minZ < face->texture->perspective_dist )
								Draw_Textured_PerspectiveLP_Triangle_FSINVZB_32( face, viewport );
							else
								Draw_Textured_Triangle_FSINVZB_32( face, viewport );
							break;

						case RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_GSINVZB_32:

							if ( minZ < face->texture->perspective_dist )
								Draw_Textured_Perspective_Triangle_GSINVZB_32( face, viewport );
							else
								Draw_Textured_Triangle_GSINVZB_32( face, viewport );
							break;

						case RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_INVZB_32:

							if ( minZ < face->texture->perspective_dist )
								Draw_Textured_Perspective_Triangle_INVZB_32( face, viewport );
							else
								Draw_Textured_Triangle_INVZB_32( face, viewport );
							break;

						case RENDER_TEXTRUED_PERSPECTIVE_TRIANGLELP_INVZB_32:

							if ( minZ < face->texture->perspective_dist )
								Draw_Textured_PerspectiveLP_Triangle_INVZB_32( face, viewport );
							else
								Draw_Textured_Triangle_INVZB_32( face, viewport );
							break;

						case RENDER_FLAT_TRIANGLE_32 :
							Draw_Flat_Triangle_32( face, viewport );
							break;

						case RENDER_FLAT_TRIANGLEFP_32 :
							Draw_Flat_TriangleFP_32( face, viewport );
							break;

						case RENDER_FLAT_TRIANGLE_INVZB_32:
							Draw_Flat_Triangle_INVZB_32( face, viewport );
							break;

						case RENDER_GOURAUD_TRIANGLE_INVZB_32:
							Draw_Gouraud_Triangle_INVZB_32( face, viewport );
							break;

						case RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_FOG_GSINVZB_32:

							if ( viewport->scene->fog && mesh->fogEnable )
							{
								if ( minZ < face->texture->perspective_dist )
									Draw_Textured_Perspective_Triangle_FOG_GSINVZB_32( face, viewport );
								else
									Draw_Textured_Triangle_FOG_GSINVZB_32( face, viewport );
							}
							else
							{
								if ( minZ < face->texture->perspective_dist )
									Draw_Textured_Perspective_Triangle_GSINVZB_32( face, viewport );
								else
									Draw_Textured_Triangle_GSINVZB_32( face, viewport );
							}
							break;
					}
				}
				else
				{
					switch ( face->render_mode )
					{
						case RENDER_WIREFRAME_TRIANGLE_32:
							Draw_Wireframe_Triangle_32( face, viewport );
							break;

						case RENDER_FLAT_TRIANGLE_32 :
							Draw_Flat_Triangle_32( face, viewport );
							break;

						case RENDER_FLAT_TRIANGLEFP_32 :
							Draw_Flat_TriangleFP_32( face, viewport );
							break;

						case RENDER_FLAT_TRIANGLE_INVZB_32:
							Draw_Flat_Triangle_INVZB_32( face, viewport );
							break;

						case RENDER_GOURAUD_TRIANGLE_INVZB_32:
							Draw_Gouraud_Triangle_INVZB_32( face, viewport );
							break;
					}
				}

				face->vertex[0]->transformed = FALSE;
				face->vertex[1]->transformed = FALSE;
				face->vertex[2]->transformed = FALSE;

				rl->polygon = NULL;

				rl = rl->next;
			}
		}

		sceneNode = sceneNode->next;
	}
}

#endif
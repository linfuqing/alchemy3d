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
#include "RenderList.h"

typedef struct Viewport
{
	//�߿�����Ƿ�������ԣ���Ⱦ�����ü������޳���
	int wh, dirty, nClippList, nCullList, nRenderList;

	//�ӿڿ�ߺͿ�߱�
	int width, height, mempitch, zpitch;

	float aspect;

	//��Ȼ�����
	LPBYTE zBuffer;

	//��Ƶ������
	LPBYTE videoBuffer;

	struct Scene * scene;

	struct Camera * camera;

	RenderList * renderList, * tailList;

	//RenderList * renderList2;

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
		memset( viewport->videoBuffer, 0, sizeof( BYTE ) );

		free( viewport->videoBuffer );
	}
	if ( NULL != viewport->zBuffer )
	{	
		memset( viewport->zBuffer, 0, sizeof( BYTE ) );

		free( viewport->zBuffer );
	}

	//��ʼ��������
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

	viewport->renderList = initializeRenderList( scene->nFaces + 2 );
	//viewport->renderList2 = NULL;

	viewport->nRenderList = 0;

	return viewport;
}

//��Ⱦǰ����
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
		buildFogTable( viewport->scene->fog, viewport->camera->far );

		viewport->scene->fog->ready = TRUE;
	}
}

INLINE void viewport_updateAfterRender( Viewport * viewport )
{
	viewport->camera->fnfDirty = viewport->dirty = FALSE;

	viewport->nRenderList = 0;

	scene_updateAfterRender( viewport->scene );
}

//����AABB��Χ�е��ӿռ�ü�
//viewport		�ӿ�
//camera		�����
//entity		��ǰҪ�����ʵ��
//octreeData	�˲���
//rl_ptr		��ǰ��Ⱦ�б��ָ��
//cl_ptr		��ǰ�ü��б��ָ��
void frustumClipping( Camera * camera, Entity * entity, OctreeData * octreeData, RenderList ** rl_ptr, RenderList ** cl_ptr, RenderList ** vl_ptr )
{
	int j, zCode0, zCode1, zCode2, verts_out, v0, v1, v2;
	float t1, t2,  xi, yi, ui, vi, x01i, x02i, y01i, y02i, u01i, u02i, v01i, v02i, dx, dy, dz, near = camera->near, far = camera->far;
	Vector3D viewerToLocal;
	Triangle * face, * face1, * face2;
	Vertex * tmpVert1, * tmpVert2, * ver1_0, * ver1_1, * ver1_2, * ver2_0, * ver2_1, * ver2_2;
	Mesh * mesh = entity->mesh;

	//������
	for ( j = 0; j < octreeData->nFaces; j ++)
	{
		face = octreeData->faces[j];

		zCode0 = zCode1 = zCode2 = 0;

		//Խ������0
		verts_out = FALSE;

		face1 = face2 = NULL;

		tmpVert1 = tmpVert2 = NULL;

		//�����޳�
		//�߿�ģʽ�������б����޳�
		if ( face->render_mode != RENDER_WIREFRAME_TRIANGLE_32 )
		{
			//����нǴ���90��С��-90ʱ�������������
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
		//��ʼ�ü�
		//���������
		//1��ֻ��һ�������ڽ������ڲ�
		else if ( verts_out == 2 )
		{
			//�ۼƲü������
			mesh->nClippList ++;

			//��鵱ǰ�ü��б�ָ�����ָ���Ƿ�Ϊ��
			//�������ָ�Ϊԭʼ��Ϣ
			if ( ( * cl_ptr )->polygon )
			{
				face1 = ( * cl_ptr )->polygon;

				triangle_copy( face1, face );

				( * cl_ptr ) = ( * cl_ptr )->next;
			}
			//���򴴽�һ����������
			else
			{
				//��¡һ����
				face1 = triangle_clone( face );

				//����ü��б�
				renderList_push( cl_ptr, face1 );
			}

			//�ҳ�λ���ڲ�Ķ���
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

			//�Ը��߲ü�

			//=======================��v0->v1�߽��вü�=======================
			dx = ver1_1->v_pos->x - ver1_0->v_pos->x;
			dy = ver1_1->v_pos->y - ver1_0->v_pos->y;
			dz = ver1_1->v_pos->w - ver1_0->v_pos->w;

			t1 = ( ( near - ver1_0->v_pos->w ) / dz );

			//���㽻��x��y����
			xi = ver1_0->v_pos->x + dx * t1;
			yi = ver1_0->v_pos->y + dy * t1;

			//�ý��㸲��ԭ���Ķ���
			ver1_1->v_pos->x = xi;
			ver1_1->v_pos->y = yi;
			ver1_1->v_pos->w = near;
			ver1_1->s_pos->x = xi / near;
			ver1_1->s_pos->y = yi / near;

			//=======================��v0->v2�߽��вü�=======================
			dx = ver1_2->v_pos->x - ver1_0->v_pos->x;
			dy = ver1_2->v_pos->y - ver1_0->v_pos->y;
			dz = ver1_2->v_pos->w - ver1_0->v_pos->w;

			t2 = ( ( near - ver1_0->v_pos->w ) / dz );

			//���㽻��x��y����
			xi = ver1_0->v_pos->x + dx * t2;
			yi = ver1_0->v_pos->y + dy * t2;

			//�ý��㸲��ԭ���Ķ���
			ver1_2->v_pos->x = xi;
			ver1_2->v_pos->y = yi;
			ver1_2->v_pos->w = near;
			ver1_2->s_pos->x = xi / near;
			ver1_2->s_pos->y = yi / near;

			//��������Ƿ������
			//����У��������������вü�
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
			//�����α��ü���Ϊ�ı��Σ���Ҫ�ָ�Ϊ����������

			//�ۼƲü������
			mesh->nClippList += 2;

			//��鵱ǰ�ü��б�ָ�����ָ���Ƿ�Ϊ��
			//�������ָ�Ϊԭʼ��Ϣ
			if ( ( * cl_ptr )->polygon )
			{
				face1 = ( * cl_ptr )->polygon;

				triangle_copy( face1, face );

				( * cl_ptr ) = ( * cl_ptr )->next;
			}
			//���򴴽�һ����������
			else
			{
				//��¡һ����
				face1 = triangle_clone( face );

				//����ü��б�
				renderList_push( cl_ptr, face1 );
			}

			if ( ( * cl_ptr )->polygon )
			{
				face2 = ( * cl_ptr )->polygon;

				triangle_copy( face2, face );

				( * cl_ptr ) = ( * cl_ptr )->next;
			}
			//���򴴽�һ����������
			else
			{
				//��¡һ����
				face2 = triangle_clone( face );

				//����ü��б�
				renderList_push( cl_ptr, face2 );
			}

			//�ҳ�λ�����Ķ���
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

			//�Ը��߲ü�

			//=======================��v0->v1�߽��вü�=======================
			dx = ver1_1->v_pos->x - ver1_0->v_pos->x;
			dy = ver1_1->v_pos->y - ver1_0->v_pos->y;
			dz = ver1_1->v_pos->w - ver1_0->v_pos->w;

			t1 = ( ( near - ver1_0->v_pos->w ) / dz );

			x01i = ver1_0->v_pos->x + dx * t1;
			y01i = ver1_0->v_pos->y + dy * t1;

			//=======================��v0->v2�߽��вü�=======================
			dx = ver1_2->v_pos->x - ver1_0->v_pos->x;
			dy = ver1_2->v_pos->y - ver1_0->v_pos->y;
			dz = ver1_2->v_pos->w - ver1_0->v_pos->w;

			t2 = ( ( near - ver1_0->v_pos->w ) / dz );

			x02i = ver1_0->v_pos->x + dx * t2;
			y02i = ver1_0->v_pos->y + dy * t2;

			//������������Ҫ�ý���1����ԭ���������ζ���
			//�ָ��ĵ�һ��������

			//�ý���1����ԭ�������εĶ���0
			ver1_0->v_pos->x = x01i;
			ver1_0->v_pos->y = y01i;
			ver1_0->v_pos->w = near;
			ver1_0->s_pos->x = x01i / near;
			ver1_0->s_pos->y = y01i / near;

			//�ý���1�����������εĶ���1������2���Ƕ���0
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

			//��������Ƿ������
			//����У��������������вü�
			if ( face->texture && face->texture->mipmaps )
			{
				u01i = face1->uv[v0]->u + ( face1->uv[v1]->u - face1->uv[v0]->u ) * t1;
				v01i = face1->uv[v0]->v + ( face1->uv[v1]->v - face1->uv[v0]->v ) * t1;

				u02i = face1->uv[v0]->u + ( face1->uv[v2]->u - face1->uv[v0]->u ) * t2;
				v02i = face1->uv[v0]->v + ( face1->uv[v2]->v - face1->uv[v0]->v ) * t2;

				//����ԭ������������
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

		//�����½�����
		//���û�����棬�����ԭ������
		if ( NULL == face1 && NULL == face2 )
		{
			renderList_push( rl_ptr, face );
			renderList_push( vl_ptr, face );

			mesh->nRenderList ++;
		}
		//�����������
		else
		{
			if ( NULL != face1 )
			{
				renderList_push( rl_ptr, face1 );
				renderList_push( vl_ptr, face1 );

				mesh->nRenderList ++;
			}

			if ( NULL != face2 )
			{
				renderList_push( rl_ptr, face2 );
				renderList_push( vl_ptr, face2 );

				mesh->nRenderList ++;
			}
		}
	}
}

int octree_culling( Camera * camera, Entity * entity, Octree * octree, RenderList ** rl_ptr, RenderList ** cl_ptr, RenderList ** vl_ptr )
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

	//�����Ϊ������Χ������
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

		//�����Χ����ȫ����׶����
		//ֱ�Ӱ����ж������ӵ���Ⱦ�б�
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
				renderList_push( vl_ptr, face );

				entity->mesh->nRenderList ++;
			}

			return 2;
		}
		//�����Χ�н���Խ��һ�߽�
		//�����ü���������
		else
		{
			//������ӽ��������ݹ�
			if ( octree->tlb )	code = octree_culling( camera, entity, octree->tlb, rl_ptr, cl_ptr, vl_ptr );
			else				noChild |= 0x01;

			if ( octree->tlf )	code = octree_culling( camera, entity, octree->tlf, rl_ptr, cl_ptr, vl_ptr );
			else				noChild |= 0x02;

			if ( octree->trb )	code = octree_culling( camera, entity, octree->trb, rl_ptr, cl_ptr, vl_ptr );
			else				noChild |= 0x04;

			if ( octree->trf )	code = octree_culling( camera, entity, octree->trf, rl_ptr, cl_ptr, vl_ptr );
			else				noChild |= 0x08;

			if ( octree->blb )	code = octree_culling( camera, entity, octree->blb, rl_ptr, cl_ptr, vl_ptr );
			else				noChild |= 0x10;

			if ( octree->blf )	code = octree_culling( camera, entity, octree->blf, rl_ptr, cl_ptr, vl_ptr );
			else				noChild |= 0x20;

			if ( octree->brb )	code = octree_culling( camera, entity, octree->brb, rl_ptr, cl_ptr, vl_ptr );
			else				noChild |= 0x40;

			if ( octree->brf )	code = octree_culling( camera, entity, octree->brf, rl_ptr, cl_ptr, vl_ptr );
			else				noChild |= 0x80;

			if ( noChild == 0xff ) frustumClipping( camera, entity, octree->data, rl_ptr, cl_ptr, vl_ptr );
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

	//���ձ���
	Vertex * vs;
	Material * material;
	Lights * lights;
	Light * light;
	Vector3D vFDist, vLightToVertex, vVertexToLight, vVertexToCamera;
	ColorValue fColor, lastColor, outPutColor;
	float dot, fAttenuCoef, fc1, fc2, fDist, fSpotFactor, fShine, fShineFactor;
	DWORD l = 0, j = 0, i = 0, tmiplevels, miplevel;
	//
	Triangle * face;
	Bitmap * bitmap;
	//����й�Դ
	//���������ڼ�¼�Ա�����Ϊ�ο���Ĺ�Դ����
	Vector3D vLightsToObject[MAX_LIGHTS];

	//������
	//Fog * fog = viewport->scene->fog;
	//float f_concentration;

	//����ʵ��
	sceneNode = viewport->scene->nodes;

	while( sceneNode )
	{
		entity = sceneNode->entity;
		mesh = entity->mesh;

		if ( mesh && mesh->nFaces && mesh->nVertices )
		{
			//��Դ����Ϊ��
			l = 0;

			//���¹�Դλ��
			lights = viewport->scene->lights;

			//������Դ
			while ( NULL != lights && mesh->lightEnable )
			{
				if ( TRUE == lights->light->bOnOff )
				{
					//��ʵ������������任��Դ��λ�õõ���ʵ��Ϊ�ο���Ĺ�Դ��λ�ã�����������
					matrix3D_transformVector( & vLightsToObject[l], entity->worldInvert, lights->light->source->w_pos );
				}

				lights = lights->next;

				l ++;
			}

			//������Ⱦ�б�
			renderList = mesh->renderList->next;

			for ( i = 0; i < mesh->nRenderList; i ++ )
			{
				face = renderList->polygon;

				face->fogEnable = mesh->fogEnable;
				face->lightEnable = mesh->lightEnable;

				material = face->material;

				//��������
				for ( j = 0; j < 3; j ++)
				{
					vs = face->vertex[j];

					//����������������ӣ����������ƽ�����
					face->depth += vs->v_pos->w;

					//��������Ѿ��任����㣬��ֱ�ӽ�����һ������
					if ( vs->transformed == 2 ) continue;

					vs->s_pos->x = ( vs->s_pos->x + 1.0f ) * viewport->width * 0.5f;
					vs->s_pos->y = ( vs->s_pos->y + 1.0f ) * viewport->height * 0.5f;
					vs->s_pos->w = vs->v_pos->w;

					vs->fix_inv_z = ( 1 << FIXP28_SHIFT ) / (int)( vs->v_pos->w + 0.5f );

					//===================���պͶ�����ɫֵ===================

					//��Դ����Ϊ��
					l = 0;

					colorValue_identity( & lastColor);

					if ( NULL != material )
					{
						//������Ч�ʺ���ʵ��֮��ȡ��һ��ƽ�⣬������ȫ�ֻ�����Ĺ��ף�ֱ��ʹ�ò��ʵĻ����ⷴ��ϵ����Ϊ������ɫ
						//���Ʋ�����ɫ��������ɫ
						colorValue_copy( & lastColor, material->ambient );

						//��ʱ��ɫ����
						colorValue_zero( & fColor );

						lights = viewport->scene->lights;

						//�����ڹ�Դ�Ĺ���
						//����й�Դ
						while ( NULL != lights && mesh->lightEnable )
						{
							//�����Դ�ǹرյ�
							if ( FALSE == lights->light->bOnOff )
							{
								lights = lights->next;

								l ++;

								continue;
							}

							light = lights->light;	

							//������ø߼������㷨
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
									fDist = vector3D_lengthSquared( vector3D_subtract( & vFDist, & vLightsToObject[l], vs->position ) );

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
								vector3D_subtract( & vLightToVertex, & vLightsToObject[l], vs->position );

								//��λ��
								vector3D_normalize( & vLightToVertex );

								//�۹����䷽��(�Ѿ���һ����λ����) �� ���� vLightToVertex �нǵ�����
								dot = vector3D_dotProduct( & vLightToVertex, light->source->direction );

								//�������λ�ڹ�׶֮��, �򲻻��о۹�������䵽������
								if ( dot < light->spotCutoff )
									fSpotFactor = 0.0f;
								else
								{
									//���þ۹�ָ�����м���
									fSpotFactor = powf( dot, light->spotExp );
								}
							}
							// �������Թ�Դ�Ĺ���(�����Ѿ����㹻��������)

							//���뻷�����䲿��:
							colorValue_append( & fColor, material->ambient, light->ambient );

							//���, ���������䲿��

							//����ָ���Դ������
							vector3D_subtract( & vVertexToLight, & vLightsToObject[l], vs->position );

							//�����ԴΪƽ�й�Դ(��λ��Դ)
							if ( light->type == DIRECTIONAL_LIGHT )
							{
								//��Դ��λ�þ������䷽��, �����������Դ���������ǹ�Դλ���������෴����
								vVertexToLight.x = -vLightsToObject[l].x;
								vVertexToLight.y = -vLightsToObject[l].y;
								vVertexToLight.z = -vLightsToObject[l].z;
							}

							//��λ��
							vector3D_normalize( & vVertexToLight );

							//���㷨�������� vVertexToLight �����ļнǵ�����
							//���㷨��Ӧ�ǵ�λ����, ���ڽ�ģʱ�Ѿ�����뱣֤��
							dot = vector3D_dotProduct( & vVertexToLight, vs->normal );

							if ( dot > 0.0f )
							{
								//���������䲿�ֵĹ���
								colorValue_add_self( & fColor, colorValue_scaleBy_self( colorValue_append( & outPutColor, material->diffuse, light->diffuse ), dot, dot, dot, 1 ) );

								//����߹ⲿ��
								if ( light->mode == HIGH_MODE )
								{
									vector3D_subtract( & vVertexToCamera, entity->viewerToLocal, vs->position );
									vector3D_normalize( & vVertexToCamera );
									vector3D_add_self( & vVertexToCamera, & vVertexToLight );
									vector3D_normalize( & vVertexToCamera );

									//������ӻ���:�붥�㷨�������
									fShine = vector3D_dotProduct( & vVertexToCamera, vs->normal );

									if ( fShine > 0.0f )
									{
										fShineFactor = powf(fShine, material->power);

										//����߹ⲿ�ֵĹ���
										colorValue_add_self( & fColor, colorValue_scaleBy_self( colorValue_append( & outPutColor, material->specular, light->specular ), fShineFactor, fShineFactor, fShineFactor, 1 ) );
									}
								}
							}

							//������˥���;۹����ӣ��� j ���������ĵڸ����������:
							fSpotFactor *= fAttenuCoef;

							colorValue_scaleBy_self( & fColor, fSpotFactor, fSpotFactor, fSpotFactor, 1.0f );

							//�ۼ��������ɫ:
							colorValue_add_self( & lastColor, & fColor );

							lights = lights->next;

							l ++;
						}//end light

						//if ( fog && fog->ready && mesh->fogEnable )
						//{
						//	f_concentration = fog->fog_table[(int)vs->v_pos->w];

						//	AS3_Trace(AS3_Number(vs->v_pos->w));

						//	colorValue_add( & fColor, fog->global, & lastColor );
						//	colorValue_copy( & fColor, fog->global );

						//	colorValue_scaleBy_self( & fColor, f_concentration, f_concentration, f_concentration, 1.0f );

						//	colorValue_add_self( & lastColor, & fColor );
						//}

						//����ɫ��һ������
						colorValue_normalize( & lastColor );

						//����alpha, ����򵥵��ò�������������������
						lastColor.alpha = material->diffuse->alpha;
					}

#ifdef RGB565
					vs->color->red = (BYTE)(lastColor.red * 31.0f);
					vs->color->green = (BYTE)(lastColor.green * 63.0f);
					vs->color->blue = (BYTE)(lastColor.blue * 31.0f);
					vs->color->alpha = (BYTE)(lastColor.blue * 255.0f);
#else
					vs->color->red = (BYTE)(lastColor.red * 255.0f);
					vs->color->green = (BYTE)(lastColor.green * 255.0f);
					vs->color->blue = (BYTE)(lastColor.blue * 255.0f);
					vs->color->alpha = (BYTE)(lastColor.alpha * 255.0f);
#endif

					vs->transformed = 2;
				}//���մ������

				//ƽ�����
				face->depth *= .333333333333f;

				//mipmap����
				if ( face->texture && face->texture->mipmaps )
				{
					if ( mesh->useMipmap && mesh->mip_dist )
					{
						tmiplevels = logbase2ofx[face->texture->mipmaps[0]->width];

						miplevel = (int)(tmiplevels * face->vertex[0]->v_pos->w / mesh->mip_dist);

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
				}

				//viewport->renderList2 = renderList;
				//viewport->renderList2++;

				viewport->nRenderList ++ ;
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
	RenderList * rl, * cl, * vl;

	scene = viewport->scene;
	camera = viewport->camera;

	//�������������
	camera_updateTransform( camera );

	//�����������ӿڵ����Ըı�
	//�����¼���ͶӰ����
	if ( TRUE == camera->fnfDirty || TRUE == viewport->dirty )
		getPerspectiveFovLH( camera->projectionMatrix, camera, viewport->aspect );

	//����
	sceneNode = scene->nodes;

	while( sceneNode )
	{
		entity = sceneNode->entity;

		if( entity->animation ) animation_update( entity->animation, time );

		sceneNode = sceneNode->next;
	}

	vl = viewport->renderList->next;
	
	sceneNode = scene->nodes;

	while( sceneNode )
	{
		entity = sceneNode->entity;

		entity_updateTransform(entity);

		if ( entity->mesh && entity -> mesh -> nFaces && entity -> mesh -> nVertices )
		{
			//���¼��㷨�����Լ�����˲�������������Ǿ�̬��ֻ��ִ��һ�Σ�
			mesh_updateMesh( entity->mesh );

			matrix3D_append( entity->view, entity->world, camera->eye->world );

			//����͸��ͶӰ����
			matrix3D_append4x4( entity->projection, entity->view, camera->projectionMatrix );

			//�����ӵ���ʵ��ľֲ�����
			matrix3D_transformVector( entity->viewerToLocal, entity->worldInvert, camera->eye->position );

			rl = entity->mesh->renderList->next;
			cl = entity->mesh->clippedList->next;

			//�˲�
			if ( ! octree_culling( camera, entity, entity->mesh->octree, & rl, & cl, & vl ) )
			{
				sceneNode = sceneNode->next;

				continue;
			}
		}

		viewport->tailList = vl->pre;

		sceneNode = sceneNode->next;
	}

	//if ( viewport->renderList2 ) free ( viewport->renderList2 );

	//if( ( viewport->renderList2 = ( RenderList * )malloc( sizeof( RenderList ) * viewport->nRenderList ) ) == NULL ) exit( TRUE );

	viewport_lightting( viewport );

	//viewport->renderList2 -= viewport->nRenderList;
}

//int compare_z( const void * arg1, const void * arg2 )
//{
//	float min1, min2;
//
//	Triangle * face1, * face2;
//
//	face1 = ( ( RenderList * )arg1 )->polygon;
//	face2 = ( ( RenderList * )arg2 )->polygon;
//
//	min1 = face1->vertex[0]->v_pos->w;
//	min1 = MIN( min1, face1->vertex[1]->v_pos->w );
//	min1 = MIN( min1, face1->vertex[2]->v_pos->w );
//
//	min2 = face2->vertex[0]->v_pos->w;
//	min2 = MIN( min2, face2->vertex[1]->v_pos->w );
//	min2 = MIN( min2, face2->vertex[2]->v_pos->w );
//
//	if ( min1 > min2 )
//		return -1;
//	else if ( min1 < min2 )
//		return 1;
//	else
//		return 0;
//}

#include "RenderFGTINVZB.h"
#include "RenderWF.h"

void viewport_render( Viewport * viewport )
{
	RenderList * rl;
	Triangle * face;

	//qsort( viewport->renderList2, viewport->nRenderList, sizeof( RenderList ), compare_z );
	//renderList_quickSort( & viewport->renderList, & viewport->tailList );

	rl = viewport->renderList->next;

	while( rl && rl->polygon )
	{
		face = rl->polygon;

		//printf("%d\n", rl->k);
		//printf("%f\n", face->depth);

		if ( face->texture && face->texture->mipmaps )
		{
			switch ( face->render_mode )
			{
			case RENDER_WIREFRAME_TRIANGLE_32:
#ifdef RGB565
				//Draw_Wireframe_Triangle_16( face, viewport );
#else
				Draw_Wireframe_Triangle_32( face, viewport );
#endif
				break;

			case RENDER_TEXTRUED_TRIANGLE_INVZB_32:
#ifdef RGB565
				Draw_Textured_Triangle_INVZB_16( face, viewport );
#else
				Draw_Textured_Triangle_INVZB_32( face, viewport );
#endif
				break;

			case RENDER_TEXTRUED_BILERP_TRIANGLE_INVZB_32:
#ifdef RGB565
				Draw_Textured_Bilerp_Triangle_INVZB_16( face, viewport );
#else
				Draw_Textured_Bilerp_Triangle_INVZB_32( face, viewport );
#endif
				break;

			case RENDER_TEXTRUED_TRIANGLE_FSINVZB_32:
#ifdef RGB565
				Draw_Textured_Triangle_FSINVZB_16( face, viewport );
#else
				Draw_Textured_Triangle_FSINVZB_32( face, viewport );
#endif
				break;

			case RENDER_TEXTRUED_TRIANGLE_GSINVZB_32:
#ifdef RGB565
				Draw_Textured_Triangle_GSINVZB_16( face, viewport );
#else
				Draw_Textured_Triangle_GSINVZB_32( face, viewport );
#endif
				break;

			case RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_FSINVZB_32:
#ifdef RGB565
				if ( face->depth < face->texture->perspective_dist )
					Draw_Textured_Perspective_Triangle_FSINVZB_16( face, viewport );
				else
					Draw_Textured_Triangle_FSINVZB_16( face, viewport );
#else

				if ( face->depth < face->texture->perspective_dist )
					Draw_Textured_Perspective_Triangle_FSINVZB_32( face, viewport );
				else
					Draw_Textured_Triangle_FSINVZB_32( face, viewport );
#endif
				break;

			case RENDER_TEXTRUED_PERSPECTIVE_TRIANGLELP_FSINVZB_32:
#ifdef RGB565
				if ( face->depth < face->texture->perspective_dist )
					Draw_Textured_PerspectiveLP_Triangle_FSINVZB_16( face, viewport );
				else
					Draw_Textured_Triangle_FSINVZB_16( face, viewport );
#else

				if ( face->depth < face->texture->perspective_dist )
					Draw_Textured_PerspectiveLP_Triangle_FSINVZB_32( face, viewport );
				else
					Draw_Textured_Triangle_FSINVZB_32( face, viewport );
#endif
				break;

			case RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_GSINVZB_32:
#ifdef RGB565
				if ( face->depth < face->texture->perspective_dist )
					Draw_Textured_Perspective_Triangle_GSINVZB_16( face, viewport );
				else
					Draw_Textured_Triangle_GSINVZB_16( face, viewport );
#else

				if ( face->depth < face->texture->perspective_dist )
					Draw_Textured_Perspective_Triangle_GSINVZB_32( face, viewport );
				else
					Draw_Textured_Triangle_GSINVZB_32( face, viewport );
#endif
				break;

			case RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_INVZB_32:
#ifdef RGB565
				if ( face->depth < face->texture->perspective_dist )
					Draw_Textured_Perspective_Triangle_INVZB_16( face, viewport );
				else
					Draw_Textured_Triangle_INVZB_16( face, viewport );
#else

				if ( face->depth < face->texture->perspective_dist )
					Draw_Textured_Perspective_Triangle_INVZB_32( face, viewport );
				else
					Draw_Textured_Triangle_INVZB_32( face, viewport );
#endif
				break;

			case RENDER_TEXTRUED_PERSPECTIVE_TRIANGLELP_INVZB_32:
#ifdef RGB565
				if ( face->depth < face->texture->perspective_dist )
					Draw_Textured_PerspectiveLP_Triangle_INVZB_16( face, viewport );
				else
					Draw_Textured_Triangle_INVZB_16( face, viewport );
#else

				if ( face->depth < face->texture->perspective_dist )
					Draw_Textured_PerspectiveLP_Triangle_INVZB_32( face, viewport );
				else
					Draw_Textured_Triangle_INVZB_32( face, viewport );
#endif
				break;

			case RENDER_FLAT_TRIANGLE_32 :
#ifdef RGB565
				//Draw_Flat_Triangle_16( face, viewport );
#else
				Draw_Flat_Triangle_32( face, viewport );
#endif
				break;

			case RENDER_FLAT_TRIANGLEFP_32 :
#ifdef RGB565
				//Draw_Flat_TriangleFP_16( face, viewport );
#else
				Draw_Flat_TriangleFP_32( face, viewport );
#endif
				break;

			case RENDER_FLAT_TRIANGLE_INVZB_32:
#ifdef RGB565
				Draw_Flat_Triangle_INVZB_16( face, viewport );
#else
				Draw_Flat_Triangle_INVZB_32( face, viewport );
#endif
				break;

			case RENDER_GOURAUD_TRIANGLE_INVZB_32:
#ifdef RGB565
				Draw_Gouraud_Triangle_INVZB_16( face, viewport );
#else
				Draw_Gouraud_Triangle_INVZB_32( face, viewport );
#endif
				break;

			case RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_FOG_GSINVZB_32:
#ifdef RGB565
				if ( viewport->scene->fog && face->fogEnable )
				{
					if ( face->depth < face->texture->perspective_dist )
						Draw_Textured_Perspective_Triangle_FOG_GSINVZB_16( face, viewport );
					else
						Draw_Textured_Triangle_FOG_GSINVZB_16( face, viewport );
				}
				else
				{
					if ( face->depth < face->texture->perspective_dist )
						Draw_Textured_Perspective_Triangle_GSINVZB_16( face, viewport );
					else
						Draw_Textured_Triangle_GSINVZB_16( face, viewport );
				}
#else

				if ( viewport->scene->fog && face->fogEnable )
				{
					if ( face->depth < face->texture->perspective_dist )
						Draw_Textured_Perspective_Triangle_FOG_GSINVZB_32( face, viewport );
					else
						Draw_Textured_Triangle_FOG_GSINVZB_32( face, viewport );
				}
				else
				{
					if ( face->depth < face->texture->perspective_dist )
						Draw_Textured_Perspective_Triangle_GSINVZB_32( face, viewport );
					else
						Draw_Textured_Triangle_GSINVZB_32( face, viewport );
				}
#endif
				break;

			case RENDER_TEXTRUED_TRIANGLE_GSINVZB_ALPHA_32:

				Draw_Gouraud_Triangle_INVZB_Alpha_32( face, viewport );

				break;
			}
		}
		else
		{
			switch ( face->render_mode )
			{
			case RENDER_WIREFRAME_TRIANGLE_32:
#ifdef RGB565
				//Draw_Wireframe_Triangle_16( face, viewport );
#else
				Draw_Wireframe_Triangle_32( face, viewport );
#endif
				break;

			case RENDER_FLAT_TRIANGLE_32 :
#ifdef RGB565
				//Draw_Flat_Triangle_16( face, viewport );
#else
				Draw_Flat_Triangle_32( face, viewport );
#endif
				break;

			case RENDER_FLAT_TRIANGLEFP_32 :
#ifdef RGB565
				//Draw_Flat_TriangleFP_16( face, viewport );
#else
				Draw_Flat_TriangleFP_32( face, viewport );
#endif
				break;

			case RENDER_FLAT_TRIANGLE_INVZB_32:
#ifdef RGB565
				Draw_Flat_Triangle_INVZB_16( face, viewport );
#else
				Draw_Flat_Triangle_INVZB_32( face, viewport );
#endif
				break;

			case RENDER_GOURAUD_TRIANGLE_INVZB_32:
#ifdef RGB565
				Draw_Gouraud_Triangle_INVZB_16( face, viewport );
#else
				Draw_Gouraud_Triangle_INVZB_32( face, viewport );
#endif
				break;

			case RENDER_TEXTRUED_TRIANGLE_GSINVZB_ALPHA_32:

				Draw_Gouraud_Triangle_INVZB_Alpha_32( face, viewport );

				break;
			}
		}

		face->vertex[0]->transformed = FALSE;
		face->vertex[1]->transformed = FALSE;
		face->vertex[2]->transformed = FALSE;

		rl->polygon = NULL;
		rl->k = 0;

		rl = rl->next;
	}
}

#endif
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
#include "SkinMeshController.h"

typedef struct Viewport
{
	//�߿�����Ƿ�������ԣ���Ⱦ�����ü������޳���
	int wh, dirty, nClippList, nCullList, nRenderList;

	//�ӿڿ�ߺͿ�߱�
	int width, height, mempitch, zpitch, stencilpitch;

	float aspect;

	int sortTransList;

	int numFrames;

	//int mouseX, mouseY;

	int useAlphaTable;
	
	//RenderList * hitObject;
	//TrianglePool* trianglePool;

	AABB* aabb;

	//��Ȼ�����
	LPBYTE zBuffer;

	//��Ƶ������
	LPBYTE videoBuffer;

	LPBYTE stencilBuffer;

	struct Scene * scene;

	struct Camera * camera;

	RenderList * renderList;

	RenderList * transList, * tailList;

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
	viewport->stencilpitch = width * sizeof(DWORD);
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

	if ( NULL != viewport->stencilBuffer )
		free( viewport->stencilBuffer );

	//��ʼ��������
	if( ( viewport->videoBuffer	  = ( LPBYTE )malloc( wh * sizeof( DWORD )   /* , sizeof( BYTE )*/ ) ) == NULL ) exit( TRUE );
	if( ( viewport->zBuffer		  = ( LPBYTE )malloc( wh * sizeof( DWORD )    /*, sizeof( BYTE )*/ ) ) == NULL ) exit( TRUE );
	if( ( viewport->stencilBuffer = ( LPBYTE )malloc( wh * sizeof(RenderList*) ) ) == NULL ) exit( TRUE );
}

Viewport * newViewport( int width, int height, Scene * scene, Camera * camera )
{
	Viewport * viewport;

	if( (viewport = ( Viewport * )malloc( sizeof( Viewport ) ) ) == NULL) exit( TRUE );

	viewport->videoBuffer = NULL;
	viewport->zBuffer = NULL;
	viewport->stencilBuffer = NULL;

	viewport_resize(viewport, width, height);
	
	viewport->camera = camera;
	viewport->scene = scene;

	viewport->renderList = initializeRenderList( scene->nFaces + 2 );
	viewport->transList = initializeRenderList( scene->nFaces + 2 );

	//viewport->trianglePool = newTrianglePool();

	viewport->nRenderList = 0;

	viewport->useAlphaTable = FALSE;
	viewport->sortTransList = FALSE;

	//viewport->hitObject = NULL;

	viewport->aabb = newAABB();

	viewport->numFrames = OFF;

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
		fog_buildTable( viewport->scene->fog, viewport->camera->far );

		if(!viewport->useAlphaTable)
		{
			buildAlphaTable(&alphaTable);

			viewport->useAlphaTable = TRUE;
		}

		viewport->scene->fog->ready = TRUE;
	}

	//scene_updateBeforeRender(viewport->scene);
}

INLINE void viewport_updateAfterRender( Viewport * viewport )
{
	viewport->camera->fnfDirty = viewport->dirty = FALSE;

	viewport->nRenderList = 0;

	//trianglePool_empty(viewport->trianglePool);

	scene_updateAfterRender( viewport->scene );
}

//����AABB��Χ�е��ӿռ�ü�
//viewport		�ӿ�
//camera		�����
//entity		��ǰҪ�����ʵ��
//octreeData	�˲���
//rl_ptr		��ǰ��Ⱦ�б��ָ��
//cl_ptr		��ǰ�ü��б��ָ��
void frustumClipping( Camera * camera, SceneNode * sceneNode, OctreeData * octreeData, RenderList ** rl_ptr, RenderList ** cl_ptr, RenderList ** vl_ptr, RenderList ** tl_ptr )
{
	int j, zCode0, zCode1, zCode2, verts_out, v0, v1, v2;
	float t1, t2,  xi, yi, ui, vi, x01i, x02i, y01i, y02i, u01i, u02i, v01i, v02i, dx, dy, dz, near = camera->near, far = camera->far;
	Vector3D viewerToLocal;
	Triangle * face, * face1, * face2;
	Vertex * tmpVert1, * tmpVert2, * ver1_0, * ver1_1, * ver1_2, * ver2_0, * ver2_1, * ver2_2;
	Mesh * mesh = sceneNode -> entity->mesh;

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
			if ( (!face->material->doubleSide) && triangle_backFaceCulling( face, & viewerToLocal, sceneNode -> entity->viewerToLocal ) )
			{
				mesh->nCullList ++;

				continue;
			}
		}

		triangle_transform(sceneNode -> entity->projection, face);

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
				renderList_push( cl_ptr, face1,sceneNode );
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

				face1->uvState = FALSE;
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
				renderList_push( cl_ptr, face1, sceneNode );
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
				renderList_push( cl_ptr, face2, sceneNode );
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

				face1->uvState = FALSE;
				face2->uvState = FALSE;
			}

			face->vertex[0]->transformed = FALSE;
			face->vertex[1]->transformed = FALSE;
			face->vertex[2]->transformed = FALSE;
		}

		//�����½�����
		//���û�����棬�����ԭ������
		if ( NULL == face1 && NULL == face2 )
		{
			renderList_push( rl_ptr, face, sceneNode );

			if ( RENDER_MODE_CONTAIN_ALPHA(face->render_mode) )
				renderList_push( tl_ptr, face, sceneNode );
			else
				renderList_push( vl_ptr, face, sceneNode );

			mesh->nRenderList ++;
		}
		//�����������
		else
		{
			if ( NULL != face1 )
			{
				renderList_push( rl_ptr, face1, sceneNode );

				if ( RENDER_MODE_CONTAIN_ALPHA(face->render_mode) )
					renderList_push( tl_ptr, face1, sceneNode );
				else
					renderList_push( vl_ptr, face1, sceneNode );

				mesh->nRenderList ++;
			}

			if ( NULL != face2 )
			{
				renderList_push( rl_ptr, face2, sceneNode );

				if ( RENDER_MODE_CONTAIN_ALPHA(face->render_mode) )
					renderList_push( tl_ptr, face2, sceneNode );
				else
					renderList_push( vl_ptr, face2, sceneNode );

				mesh->nRenderList ++;
			}
		}
	}
}

int octree_culling( Camera * camera, SceneNode * sceneNode, Octree * octree, RenderList ** rl_ptr, RenderList ** cl_ptr, RenderList ** vl_ptr, RenderList ** tl_ptr )
{
	int code = 0, noChild = 0;

	AABB * aabb;
	
	//aabb_setToTransformedBox( octree->data->worldAABB, octree->data->aabb, sceneNode -> entity->world );
	aabb_setToTransformedBox_CVV( octree->data->CVVAABB, octree->data->aabb, sceneNode -> entity->projection );

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
		sceneNode -> entity->mesh->nCullList += octree->data->nFaces;

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

		if(sceneNode->entity->morphChannel && sceneNode->entity->parentAnimation)
		{
			morphChannel_update(sceneNode->entity->morphChannel, sceneNode->entity->mesh, sceneNode->entity->parentAnimation);

			noChild = TRUE;
		}

		if(sceneNode->entity->mesh->skinMeshController)
		{
			skinMeshController_updateSkinnedMesh(sceneNode->entity->mesh->skinMeshController, sceneNode->entity->mesh);

			noChild = TRUE;
		}

		//�����Χ����ȫ����׶����
		//ֱ�Ӱ����ж������ӵ���Ⱦ�б�
		if ( code == 0x3f )
		{
			Triangle * face;

			Vector3D viewerToLocal;

			for ( code = 0; code < octree->data->nFaces; code ++ )
			{
				face = octree->data->faces[code];

				if ( (!face->material->doubleSide) && triangle_backFaceCulling( face, & viewerToLocal, sceneNode -> entity->viewerToLocal ) )
				{
					sceneNode -> entity->mesh->nCullList ++;

					continue;
				}

				//triangle_transform( sceneNode -> entity->world, sceneNode -> entity->projection, face );
				triangle_transform(sceneNode -> entity->projection, face);

				renderList_push( rl_ptr, face, sceneNode );

				if ( RENDER_MODE_CONTAIN_ALPHA(face->render_mode) )
					renderList_push( tl_ptr, face, sceneNode );
				else
					renderList_push( vl_ptr, face, sceneNode );

				sceneNode -> entity->mesh->nRenderList ++;
			}

			return 2;
		}
		else if(noChild)
			frustumClipping( camera, sceneNode, octree->data, rl_ptr, cl_ptr, vl_ptr, tl_ptr );
		//�����Χ�н���Խ��һ�߽�
		//�����ü���������
		else
		{
			//������ӽ��������ݹ�
			if ( octree->tlb )	code = octree_culling( camera, sceneNode, octree->tlb, rl_ptr, cl_ptr, vl_ptr, tl_ptr );
			else				noChild |= 0x01;

			if ( octree->tlf )	code = octree_culling( camera, sceneNode, octree->tlf, rl_ptr, cl_ptr, vl_ptr, tl_ptr );
			else				noChild |= 0x02;

			if ( octree->trb )	code = octree_culling( camera, sceneNode, octree->trb, rl_ptr, cl_ptr, vl_ptr, tl_ptr );
			else				noChild |= 0x04;

			if ( octree->trf )	code = octree_culling( camera, sceneNode, octree->trf, rl_ptr, cl_ptr, vl_ptr, tl_ptr );
			else				noChild |= 0x08;

			if ( octree->blb )	code = octree_culling( camera, sceneNode, octree->blb, rl_ptr, cl_ptr, vl_ptr, tl_ptr );
			else				noChild |= 0x10;

			if ( octree->blf )	code = octree_culling( camera, sceneNode, octree->blf, rl_ptr, cl_ptr, vl_ptr, tl_ptr );
			else				noChild |= 0x20;

			if ( octree->brb )	code = octree_culling( camera, sceneNode, octree->brb, rl_ptr, cl_ptr, vl_ptr, tl_ptr );
			else				noChild |= 0x40;

			if ( octree->brf )	code = octree_culling( camera, sceneNode, octree->brf, rl_ptr, cl_ptr, vl_ptr, tl_ptr );
			else				noChild |= 0x80;

			if ( noChild == 0xff ) frustumClipping( camera, sceneNode, octree->data, rl_ptr, cl_ptr, vl_ptr, tl_ptr );
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
	int fColor_r, fColor_g, fColor_b,
		lastColor_r, lastColor_g, lastColor_b;
	float dot, fAttenuCoef, fc1, fc2, fDist, fSpotFactor, fShine, fShineFactor;
	DWORD l = 0, j = 0, i = 0;
	//
	Triangle * face;
	BYTE tmiplevels, miplevel;

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

		if(!entity->visible)
		{
			sceneNode = entity_getTreeTail(sceneNode);

			sceneNode = sceneNode->next;

			continue;
		}

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
					matrix4x4_transformVector( & vLightsToObject[l], entity->worldInvert, lights->light->source->w_pos );
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
					face->depth += (int)(vs->v_pos->w);

					//��������Ѿ��任����㣬��ֱ�ӽ�����һ������
					if ( vs->transformed == 2 ) continue;

					vs->s_pos->x = ( vs->s_pos->x + 1.0f ) * viewport->width * 0.5f;
					vs->s_pos->y = ( vs->s_pos->y + 1.0f ) * viewport->height * 0.5f;
					vs->s_pos->w = vs->v_pos->w;

					vs->fix_inv_z = ( 1 << FIXP28_SHIFT ) / (int)( vs->v_pos->w + 0.5f ) + ( (viewport->numFrames & 7) << FIXP28_SHIFT );

					//===================���պͶ�����ɫֵ===================

					//��Դ����Ϊ��
					l = 0;

					if ( material )
					{
						//������Ч�ʺ���ʵ��֮��ȡ��һ��ƽ�⣬������ȫ�ֻ�����Ĺ��ף�ֱ��ʹ�ò��ʵĻ����ⷴ��ϵ����Ϊ������ɫ
						//���Ʋ�����ɫ��������ɫ
						lastColor_r = material->ambient->red;
						lastColor_g = material->ambient->green;
						lastColor_b = material->ambient->blue;

						//��ʱ��ɫ����
						fColor_r = fColor_g = fColor_b = 0;

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

								if ( ( fc1 > 0.0001f ) || ( fc2 > 0.00001f ) )
								{
									//�󶥵�����Դ�ľ���
									vFDist.x = vLightsToObject[l].x - vs->position->x;
									vFDist.y = vLightsToObject[l].y - vs->position->y;
									vFDist.z = vLightsToObject[l].z - vs->position->z;

									fDist = vFDist.x * vFDist.x + vFDist.y * vFDist.y + vFDist.z * vFDist.z;

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
								vLightToVertex.x = vLightsToObject[l].x - vs->position->x;
								vLightToVertex.y = vLightsToObject[l].y - vs->position->y;
								vLightToVertex.z = vLightsToObject[l].z - vs->position->z;

								//��λ��
								vector3D_normalize( & vLightToVertex );

								//�۹����䷽��(�Ѿ���һ����λ����) �� ���� vLightToVertex �нǵ�����
								dot = vLightToVertex.x * light->source->direction->x + vLightToVertex.y * light->source->direction->y + vLightToVertex.z * light->source->direction->z;

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
							fColor_r = (int)( (material->ambient->red   * light->ambient->red  ) >> FIXP8_SHIFT );
							fColor_g = (int)( (material->ambient->green * light->ambient->green) >> FIXP8_SHIFT );
							fColor_b = (int)( (material->ambient->blue  * light->ambient->blue ) >> FIXP8_SHIFT );

							//���, ���������䲿��

							//����ָ���Դ������
							vVertexToLight.x = vLightsToObject[l].x - vs->position->x;
							vVertexToLight.y = vLightsToObject[l].y - vs->position->y;
							vVertexToLight.z = vLightsToObject[l].z - vs->position->z;

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
							dot = vVertexToLight.x * vs->normal->x + vVertexToLight.y * vs->normal->y + vVertexToLight.z * vs->normal->z;

							if ( dot > 0.0f )
							{
								//���������䲿�ֵĹ���
								fColor_r += ( (int)(material->diffuse->red   * light->diffuse->red   * dot ) ) >> FIXP8_SHIFT;
								fColor_g += ( (int)(material->diffuse->green * light->diffuse->green * dot ) ) >> FIXP8_SHIFT;
								fColor_b += ( (int)(material->diffuse->blue  * light->diffuse->blue  * dot ) ) >> FIXP8_SHIFT;

								//����߹ⲿ��
								if ( light->mode == HIGH_MODE )
								{
									vVertexToCamera.x = entity->viewerToLocal->x - vs->position->x;
									vVertexToCamera.y = entity->viewerToLocal->y - vs->position->y;
									vVertexToCamera.z = entity->viewerToLocal->z - vs->position->z;

									vector3D_normalize( & vVertexToCamera );

									vVertexToCamera.x += vVertexToLight.x;
									vVertexToCamera.y += vVertexToLight.y;
									vVertexToCamera.z += vVertexToLight.z;

									vector3D_normalize( & vVertexToCamera );

									//������ӻ���:�붥�㷨�������
									fShine = vVertexToCamera.x * vs->normal->x + vVertexToCamera.y * vs->normal->y + vVertexToCamera.z * vs->normal->z;

									if ( fShine > 0.0f )
									{
										fShineFactor = powf(fShine, material->power);

										//����߹ⲿ�ֵĹ���
										fColor_r += ( (int)(material->specular->red   * light->specular->red   * fShineFactor) ) >> FIXP8_SHIFT;
										fColor_g += ( (int)(material->specular->green * light->specular->green * fShineFactor) ) >> FIXP8_SHIFT;
										fColor_b += ( (int)(material->specular->blue  * light->specular->blue  * fShineFactor) ) >> FIXP8_SHIFT;
									}
								}
							}

							//������˥���;۹����ӣ��� j ���������ĵڸ����������:
							fSpotFactor *= fAttenuCoef;

							//�ۼ��������ɫ:
							lastColor_r += (int)(fColor_r * fSpotFactor);
							lastColor_g += (int)(fColor_g * fSpotFactor);
							lastColor_b += (int)(fColor_b * fSpotFactor);

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
						lastColor_r = MAX( lastColor_r, 0 );
						lastColor_g = MAX( lastColor_g, 0 );
						lastColor_b = MAX( lastColor_b, 0 );

						lastColor_r = MIN( lastColor_r, 255 );
						lastColor_g = MIN( lastColor_g, 255 );
						lastColor_b = MIN( lastColor_b, 255 );
					}

#ifdef RGB565
					vs->color->red = lastColor.red >> 3;
					vs->color->green = lastColor.green >> 2;
					vs->color->blue = lastColor.blue >> 3;
					vs->color->alpha = material->diffuse->alpha;
#else
					vs->color->red = lastColor_r;
					vs->color->green = lastColor_g;
					vs->color->blue = lastColor_b;

					//����alpha, ����򵥵��ò�������������������
					vs->color->alpha = material->diffuse->alpha;
#endif

					vs->transformed = 2;
				}//���մ������

				//mipmap��uv����
				if ( face->texture && face->texture->mipmaps )
				{
					//tmiplevels = logbase2ofx[face->texture->mipmaps[0]->width] + 1;
					tmiplevels = (BYTE)(face->texture->numMipLevels);

					//����uv
					if ( ! face->uvState )
					{
						//����һ�����mipmap�ȼ���uv��
						//triangle_createMipUVChain( face, tmiplevels );

						//triangle_setUV( face );

						triangle_transformUV( face, mesh->texTransform, mesh->addressMode );

						face->uvState = TRUE;
					}
					
					face->c_uv[0]->tu = (int)(face->uvwh[0]->u + 0.5f);
					face->c_uv[0]->tv = (int)(face->uvwh[0]->v + 0.5f);
					face->c_uv[1]->tu = (int)(face->uvwh[1]->u + 0.5f);
					face->c_uv[1]->tv = (int)(face->uvwh[1]->v + 0.5f);
					face->c_uv[2]->tu = (int)(face->uvwh[2]->u + 0.5f);
					face->c_uv[2]->tv = (int)(face->uvwh[2]->v + 0.5f);

					//���ʹ��mipmap
					if ( mesh->useMipmap && mesh->mip_dist )
					{
						tmiplevels --;

						miplevel = ( int )( face->depth / mesh->mip_dist );
						miplevel = MIN( miplevel, tmiplevels );

						if ( miplevel > 0 )
						{
							face->c_uv[0]->tu *= dot5miplevel_table[miplevel];
							face->c_uv[0]->tv *= dot5miplevel_table[miplevel];
							face->c_uv[1]->tu *= dot5miplevel_table[miplevel];
							face->c_uv[1]->tv *= dot5miplevel_table[miplevel];
							face->c_uv[2]->tu *= dot5miplevel_table[miplevel];
							face->c_uv[2]->tv *= dot5miplevel_table[miplevel];
						}

						face->miplevel = miplevel;
					}
				}

				//ƽ�����
				face->depth *= 0.3333333333f;

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
	Entity * entity, *parent;
	Mesh * mesh;
	RenderList * rl, * cl, * vl, * tl;

	scene = viewport->scene;
	camera = viewport->camera;
	
	//�������������
	camera_updateTransform( camera );

	//����
	sceneNode = scene->nodes;

	//animation_updateScene(sceneNode, time);

	while( sceneNode )
	{
		entity = sceneNode->entity;

		if(!entity->visible)
		{
			sceneNode = entity_getTreeTail(sceneNode);

			sceneNode = sceneNode->next;

			continue;
		}

		/*if( entity->animation ) 
			animation_update( entity->animation, time );

		if(entity->skeletalChannel)
			skeletalChannel_update(entity->skeletalChannel, entity);

		if(entity->morphChannel)
			morphChannel_update(entity->morphChannel, entity->mesh);*/

		if(entity->animation)
		{
			animation_update(entity->animation, time);
			entity->parentAnimation = entity->animation;

			if(entity->skeletalChannel)
				skeletalChannel_update(entity->skeletalChannel, entity, entity->parentAnimation);
		}
		else if(entity->parent && entity->parent->parentAnimation)
		{
			entity->parentAnimation = entity->parent->parentAnimation;

			if(entity->skeletalChannel)
				skeletalChannel_update(entity->skeletalChannel, entity, entity->parentAnimation);
		}
		else
			entity->parentAnimation = NULL;

		if ( entity->transformDirty ||
			( entity->parent && entity->parent->transformDirty && (!entity->isBone || entity->parent->isBone) ) )//||
			//camera->eye->transformDirty )
			entity_updateTransform( entity );

		//entity->hit = FALSE;

		sceneNode = sceneNode->next;
	}

	aabb_setToTransformedBox(camera->eye->worldAABB, camera->eye->aabb, camera->eye->transform);

	sceneNode = scene->nodes;

	while( sceneNode )
	{
		entity = sceneNode->entity;

		if(!entity->visible)
		{
			sceneNode = entity_getTreeTail(sceneNode);

			sceneNode = sceneNode->next;

			continue;
		}

		if(entity->mesh)
		{
			aabb_union(entity->aabb, entity->aabb, entity->mesh->octree->data->aabb);

			aabb_setToTransformedBox(entity->worldAABB, entity->aabb, entity->world);
			//aabb_copy(entity->aabb, entity->mesh->octree->data->worldAABB);

			parent = entity->parent;

			while(parent)
			{
				aabb_union(parent->worldAABB, parent->worldAABB, entity->worldAABB);

				entity = parent;
				parent = entity->parent;
			}
		}
		else
			aabb_setToTransformedBox(entity->worldAABB, entity->aabb, entity->world);

		sceneNode = sceneNode->next;
	}

	if(scene->terrain && camera->eye->terrainTrace)
		terrain_traceCamera(scene->terrain, camera);

	if(scene->terrain)
		terrain_trace(scene->terrain, scene->nodes);

	
	//�����������ӿڵ����Ըı�
	//�����¼���ͶӰ����
	if ( TRUE == camera->fnfDirty || TRUE == viewport->dirty )
	{
		getPerspectiveFovLH( camera->projectionMatrix, camera, viewport->aspect );

		matrix4x4_append4x4(camera->viewProjectionMatrix, camera->eye->world, camera->projectionMatrix);

		matrix4x4_copy(camera->invertProjectionMatrix, camera->projectionMatrix);

		matrix4x4_fastInvert(camera->invertProjectionMatrix);

		matrix4x4_copy(camera->invertViewProjectionMatrix, camera->viewProjectionMatrix);

		matrix4x4_fastInvert(camera->invertViewProjectionMatrix);
	}
	else if(camera->eye->transformDirty || camera->isUVN)
	{
		matrix4x4_append4x4(camera->viewProjectionMatrix, camera->eye->world, camera->projectionMatrix);

		matrix4x4_copy(camera->invertProjectionMatrix, camera->projectionMatrix);

		matrix4x4_fastInvert(camera->invertProjectionMatrix);

		matrix4x4_copy(camera->invertViewProjectionMatrix, camera->viewProjectionMatrix);

		matrix4x4_fastInvert(camera->invertViewProjectionMatrix);
	}


	vl = viewport->renderList->next;
	tl = viewport->transList->next;
	
	sceneNode = scene->nodes;

	while( sceneNode )
	{
		entity = sceneNode->entity;

		if(!entity->visible)
		{
			sceneNode = entity_getTreeTail(sceneNode);

			sceneNode = sceneNode->next;

			continue;
		}

		mesh = entity->mesh;

		if ( mesh && mesh->nFaces && mesh->nVertices )
		{
			mesh->hit = FALSE;
			//���¼��㷨�����Լ�����˲�������������Ǿ�̬��ֻ��ִ��һ�Σ�
			if ( mesh->v_dirty ) mesh_updateMesh( mesh );

			matrix4x4_append( entity->view, entity->world, camera->eye->world );

			//����͸��ͶӰ����
			matrix4x4_append4x4( entity->projection, entity->view, camera->projectionMatrix );

			//�����ӵ���ʵ��ľֲ�����
			matrix4x4_transformVector( entity->viewerToLocal, entity->worldInvert, camera->eye->position );

			rl = mesh->renderList->next;
			cl = mesh->clippedList->next;

			//�˲�
			if ( ! octree_culling( camera, sceneNode, mesh->octree, & rl, & cl, & vl, &tl ) )
			{
				sceneNode = sceneNode->next;

				continue;
			}

			//if(entity->skinMeshController)
			//	skinMeshController_updateSkinnedMesh(entity->skinMeshController, entity->mesh);
		}

		//viewport->tailList = tl->pre;

		//��������任����
		if ( mesh && mesh->texTransformDirty ) mesh_updateTexTransform( mesh );

		sceneNode = sceneNode->next;
	}

	viewport->tailList = tl->pre;

	viewport_lightting( viewport );
}

#include "RenderFGTINVZB.h"
#include "RenderWF.h"

void viewport_render( Viewport * viewport )
{
	RenderList * rl;
	Triangle * face;
	DWORD begin, end;

	//viewport->hitObject = NULL;

	aabb_empty(viewport->aabb);

	rl = viewport->renderList->next;

	while( rl && rl->polygon )
	{
		face = rl->polygon;

		aabb_add(viewport->aabb, face->vertex[0]->s_pos);
		aabb_add(viewport->aabb, face->vertex[1]->s_pos);
		aabb_add(viewport->aabb, face->vertex[2]->s_pos);

		if ( face->texture && face->texture->mipmaps )
		{
			switch ( face->render_mode )
			{
			case RENDER_WIREFRAME_TRIANGLE_32:

				Draw_Wireframe_Triangle_32( face, viewport );

				break;

			case RENDER_TEXTRUED_BILERP_TRIANGLE_INVZB_32:

				Draw_Textured_Bilerp_Triangle_INVZB_32( rl, viewport );

				break;

			case RENDER_TEXTRUED_TRIANGLE_INVZB_32:

				if ( face->depth < face->texture->perspective_dist )
					Draw_Textured_Perspective_Triangle_INVZB_32( rl, viewport );
				else if ( face->depth < face->texture->perspectiveLP_dist )
					Draw_Textured_PerspectiveLP_Triangle_INVZB_32( rl, viewport );
				else
					Draw_Textured_Triangle_INVZB_32( rl, viewport );

				break;

			case RENDER_TEXTRUED_TRIANGLE_FSINVZB_32:

				if ( face->depth < face->texture->perspective_dist )
					Draw_Textured_Perspective_Triangle_FSINVZB_32( rl, viewport );
				else if ( face->depth < face->texture->perspectiveLP_dist )
					Draw_Textured_PerspectiveLP_Triangle_FSINVZB_32( rl, viewport );
				else
					Draw_Textured_Triangle_FSINVZB_32( rl, viewport );

				break;

			case RENDER_TEXTRUED_TRIANGLE_GSINVZB_32:

				if ( face->depth < face->texture->perspective_dist )
					Draw_Textured_Perspective_Triangle_GSINVZB_32( rl, viewport );
				else if ( face->depth < face->texture->perspectiveLP_dist )
					Draw_Textured_PerspectiveLP_Triangle_GSINVZB_32( rl, viewport );
				else
					Draw_Textured_Triangle_GSINVZB_32( rl, viewport );

				break;

			case RENDER_FLAT_TRIANGLE_32 :

				Draw_Flat_Triangle_32( face, viewport );

				break;

			case RENDER_FLAT_TRIANGLEFP_32 :

				Draw_Flat_TriangleFP_32( face, viewport );

				break;

			case RENDER_FLAT_TRIANGLE_INVZB_32:

				Draw_Flat_Triangle_INVZB_32( rl, viewport );

				break;

			case RENDER_GOURAUD_TRIANGLE_INVZB_32:

				Draw_Gouraud_Triangle_INVZB_32( rl, viewport );

				break;

			case 0x10000000:

				Draw_Textured_Triangle_Test_32( face, viewport );

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
			case RENDER_TEXTRUED_BILERP_TRIANGLE_INVZB_32:
			case RENDER_TEXTRUED_TRIANGLE_INVZB_32:
			case RENDER_TEXTRUED_TRIANGLE_FSINVZB_32:

				Draw_Flat_Triangle_INVZB_32( rl, viewport );

				break;

			case RENDER_GOURAUD_TRIANGLE_INVZB_32:
			case RENDER_TEXTRUED_TRIANGLE_GSINVZB_32:

				Draw_Gouraud_Triangle_INVZB_32( rl, viewport );

				break;
			}
		}

		face->vertex[0]->transformed = FALSE;
		face->vertex[1]->transformed = FALSE;
		face->vertex[2]->transformed = FALSE;

		rl->polygon = NULL;

		rl = rl->next;
	}

	if(viewport->scene->fog)
	{
		if( aabb_isEmpty(viewport->aabb) )
		{
			begin = 0;
			end   = viewport->wh;
		}
		else
		{
			begin = MAX(0, viewport->aabb->min->x) + MAX(0, viewport->aabb->min->y) * viewport->width;
			end   = MIN(viewport->width, viewport->aabb->max->x) + MIN(viewport->height, viewport->aabb->max->y) * viewport->width;
		}

		fog_render(viewport->scene->fog, viewport->videoBuffer + begin * sizeof(DWORD), viewport->zBuffer + begin * sizeof(DWORD), end - begin, viewport->numFrames);
	}

	rl = viewport->transList->next;

	if(rl && rl->polygon && !viewport->useAlphaTable)
	{
		buildAlphaTable(&alphaTable);

		viewport->useAlphaTable = TRUE;
	}

	if(rl && rl->polygon && viewport->sortTransList)
		renderList_quickSort(rl, viewport->tailList, REVERSE_SORT);

	while( rl && rl->polygon )
	{
		face = rl->polygon;

		if ( face->texture && face->texture->mipmaps )
		{
			switch ( face->render_mode )
			{
				case RENDER_TRIANGLE_FSINVZB_ALPHA_32:

					Draw_Flat_Triangle_INVZB_Alpha_32( rl, viewport );

					break;

				case RENDER_TRIANGLE_GSINVZB_ALPHA_32:

					Draw_Gouraud_Triangle_INVZB_Alpha_32( rl, viewport );

					break;

				case RENDER_TEXTRUED_TRIANGLE_INVZB_ALPHA_32:

					if ( face->depth < face->texture->perspective_dist )
						Draw_Textured_Perspective_Triangle_INVZB_Alpha_32( rl, viewport );
					else if ( face->depth < face->texture->perspectiveLP_dist )
						Draw_Textured_PerspectiveLP_Triangle_INVZB_Alpha_32( rl, viewport );
					else
						Draw_Textured_Triangle_INVZB_Alpha_32( rl, viewport );

					break;

				case RENDER_TEXTRUED_TRIANGLE_FOG_INVZB_ALPHA_32:

					if ( viewport->scene->fog && face->fogEnable )
					{
						if(face->depth < face->texture->perspective_dist)
							Draw_Textured_Perspective_Triangle_FOG_INVZB_Alpha_32(rl, viewport);
						else if(face->depth < face->texture->perspectiveLP_dist)
							Draw_Textured_PerspectiveLP_Triangle_FOG_INVZB_Alpha_32(rl, viewport);
						else
							Draw_Textured_Triangle_FOG_INVZB_Alpha_32(rl, viewport);
					}
					else
					{
						if ( face->depth < face->texture->perspective_dist )
							Draw_Textured_Perspective_Triangle_INVZB_Alpha_32( rl, viewport );
						else if ( face->depth < face->texture->perspectiveLP_dist )
							Draw_Textured_PerspectiveLP_Triangle_INVZB_Alpha_32( rl, viewport );
						else
							Draw_Textured_Triangle_INVZB_Alpha_32( rl, viewport );
					}

					break;

				case RENDER_TRIANGLE_FOG_FSINVZB_ALPHA_32:

					if ( viewport->scene->fog && face->fogEnable )
						Draw_Triangle_FOG_FSINVZB_Alpha_32(rl, viewport);
					else
						Draw_Flat_Triangle_INVZB_Alpha_32(rl, viewport);

					break;

				case RENDER_TEXTRUED_TRIANGLE_FSINVZB_ALPHA_32:

					if ( face->depth < face->texture->perspective_dist )
						Draw_Textured_Perspective_Triangle_FSINVZB_Alpha_32( rl, viewport );
					else if ( face->depth < face->texture->perspectiveLP_dist )
						Draw_Textured_PerspectiveLP_Triangle_FSINVZB_Alpha_32( rl, viewport );
					else
						Draw_Textured_Triangle_FSINVZB_Alpha_32( rl, viewport );
						
					break;

				case RENDER_TEXTRUED_TRIANGLE_FOG_FSINVZB_ALPHA_32:

					if ( viewport->scene->fog && face->fogEnable )
					{
						if(face->depth < face->texture->perspective_dist)
							Draw_Textured_Perspective_Triangle_FOG_FSINVZB_Alpha_32(rl, viewport);
						else if(face->depth < face->texture->perspective_dist)
							Draw_Textured_PerspectiveLP_Triangle_FOG_FSINVZB_Alpha_32(rl, viewport);
						else
							Draw_Textured_Triangle_FOG_FSINVZB_Alpha_32(rl, viewport);
					}
					else
					{
						if ( face->depth < face->texture->perspective_dist )
							Draw_Textured_Perspective_Triangle_FSINVZB_Alpha_32( rl, viewport );
						else if ( face->depth < face->texture->perspectiveLP_dist )
							Draw_Textured_PerspectiveLP_Triangle_FSINVZB_Alpha_32( rl, viewport );
						else
							Draw_Textured_Triangle_FSINVZB_Alpha_32( rl, viewport );
					}

					break;

				case RENDER_TRIANGLE_FOG_GSINVZB_ALPHA_32:

					if ( viewport->scene->fog && face->fogEnable )
						Draw_Triangle_FOG_GSINVZB_Alpha_32(rl, viewport);
					else
						Draw_Gouraud_Triangle_INVZB_Alpha_32(rl, viewport);

					break;

				case RENDER_TEXTRUED_TRIANGLE_GSINVZB_ALPHA_32:

					if ( face->depth < face->texture->perspective_dist )
						Draw_Textured_Perspective_Triangle_GSINVZB_Alpha_32( rl, viewport );
					else if ( face->depth < face->texture->perspectiveLP_dist )
						Draw_Textured_PerspectiveLP_Triangle_GSINVZB_Alpha_32( rl, viewport );
					else
						Draw_Textured_Triangle_GSINVZB_Alpha_32( rl, viewport );

					break;

				case RENDER_TEXTRUED_TRIANGLE_FOG_GSINVZB_ALPHA_32:

					if ( viewport->scene->fog && face->fogEnable )
					{
						if( face->depth < face->texture->perspective_dist )
							Draw_Textured_Perspective_Triangle_FOG_GSINVZB_Alpha_32( rl, viewport );
						else if( face->depth < face->texture->perspectiveLP_dist )
							Draw_Textured_PerspectiveLP_Triangle_FOG_GSINVZB_Alpha_32( rl, viewport );
						else
							Draw_Textured_Triangle_FOG_GSINVZB_Alpha_32( rl, viewport );
					}
					else
					{
						if ( face->depth < face->texture->perspective_dist )
							Draw_Textured_Perspective_Triangle_GSINVZB_Alpha_32( rl, viewport );
						else if ( face->depth < face->texture->perspectiveLP_dist )
							Draw_Textured_PerspectiveLP_Triangle_GSINVZB_Alpha_32( rl, viewport );
						else
							Draw_Textured_Triangle_GSINVZB_Alpha_32( rl, viewport );
					}

					break;
			}
		}
		else
		{
			switch ( face->render_mode )
			{
				case RENDER_TEXTRUED_TRIANGLE_INVZB_ALPHA_32:
				case RENDER_TRIANGLE_FSINVZB_ALPHA_32:
				case RENDER_TEXTRUED_TRIANGLE_FSINVZB_ALPHA_32:

					Draw_Flat_Triangle_INVZB_Alpha_32( rl, viewport );

					break;

				case RENDER_TRIANGLE_GSINVZB_ALPHA_32:
				case RENDER_TEXTRUED_TRIANGLE_GSINVZB_ALPHA_32:

					Draw_Gouraud_Triangle_INVZB_Alpha_32( rl, viewport );

					break;

			case RENDER_TRIANGLE_FOG_FSINVZB_ALPHA_32:
			case RENDER_TEXTRUED_TRIANGLE_FOG_INVZB_ALPHA_32:
			case RENDER_TEXTRUED_TRIANGLE_FOG_FSINVZB_ALPHA_32:

				if ( viewport->scene->fog && face->fogEnable )
					Draw_Triangle_FOG_FSINVZB_Alpha_32(rl, viewport);
				else
					Draw_Flat_Triangle_INVZB_Alpha_32(rl, viewport);

				break;

			case RENDER_TEXTRUED_TRIANGLE_FOG_GSINVZB_ALPHA_32:
			case RENDER_TRIANGLE_FOG_GSINVZB_ALPHA_32:

				if (viewport->scene->fog && face->fogEnable)
					Draw_Triangle_FOG_GSINVZB_Alpha_32(rl, viewport);
				else
					Draw_Gouraud_Triangle_INVZB_Alpha_32(rl, viewport);

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

int viewport_mouseOn( Viewport * viewport, int mouseX, int mouseY )
{
	/*int hitEntity = OFF;

	RenderList * rl;

	rl = viewport->renderList->next;

	while( rl && rl->polygon )
	{
		if( triangle_hitTestPoint2D( rl -> polygon, mouseX, mouseY ) )
		{
			hitEntity = rl -> parent -> ID;
		}

		rl = rl->next;
	}

	rl = viewport->transList->next;

	while( rl && rl->polygon )
	{
		if( triangle_hitTestPoint2D( rl -> polygon, mouseX, mouseY ) )
		{
			hitEntity = rl -> parent -> ID;
		}

		rl = rl->next;
	}

	return hitEntity;*/

	RenderList* renderObject;
	DWORD z, numFrames = viewport->numFrames;

	if(mouseX < 0 || mouseY < 0 || mouseX >= viewport->width || mouseY >= viewport->height)
		return FALSE;
	
	z = ( (DWORD*)(viewport->zBuffer) )[mouseY * (viewport->zpitch >> 2) + mouseX];

	if( z > ( (numFrames & 7) << FIXP28_SHIFT ) )
	{
		renderObject = ( (RenderList**)(viewport->stencilBuffer) )[mouseY * (viewport->stencilpitch >> 2) + mouseX];

		renderObject->parent->entity->mesh->hit = TRUE;

		return TRUE;
	}

	return FALSE;
}

Vector3D* viewport_getWorldPositionFromView(Vector3D* output, Viewport* viewport, float x, float y, float z, int autoDepth)
{
	Camera* camera;
	Vector3D pointA, pointB;
	DWORD depth;

	if(viewport)
		camera = viewport->camera;
	else
		return NULL;

	if(!camera || x < 0 || y < 0 || x >= viewport->width || y >= viewport->height)
		return NULL;

	if(autoDepth)
	{
		depth = ( (DWORD*)(viewport->zBuffer) )[(int)(y * (viewport->zpitch >> 2) + x)];

		if( depth > ( ( (DWORD)(viewport->numFrames) & 7 ) << FIXP28_SHIFT ) )
			z = (float)( 0x10000000 * 1.0 / (depth & 0xfffffff) );
		else
		{
			//AS3_Trace(AS3_Number((double)depth));
			return NULL;
		}
	}

	if(!output)
		output = newVector3D(0, 0, 0, 1.0f);

	output->z = z;

	x = x / (viewport->width  * 0.5f) - 1.0f;
	y = y / (viewport->height * 0.5f) - 1.0f;

	pointA.x = x * camera->near;
	pointA.y = y * camera->near;
	pointA.z = 0;
	pointA.w = camera->near;

	pointB.x = x * camera->far;
	pointB.y = y * camera->far;
	pointB.z = 1;
	pointB.w = camera->far;

	matrix4x4_transformVector_self(camera->invertProjectionMatrix, &pointA);
	matrix4x4_transformVector_self(camera->invertProjectionMatrix, &pointB);

	z /= camera->far - camera->near;

	output->x = z * (pointB.x - pointA.x);
	output->y = z * (pointB.y - pointA.y);

	matrix4x4_transformVector_self(camera->eye->transform, output);

	return output;
}

#endif
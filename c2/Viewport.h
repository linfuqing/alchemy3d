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

	//��Ȼ�����
	float * zBuffer;

	//λͼ������
	uint32 * gfxBuffer;

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

	if ( NULL != viewport->gfxBuffer )
	{
		free( viewport->gfxBuffer );
	}

	if ( NULL != viewport->zBuffer )
	{
		free( viewport->zBuffer );
	}

	//��ʼ��������
	if( ( viewport->gfxBuffer = ( uint32 * )malloc( sizeof( uint32 ) * wh ) ) == NULL )
	{
		exit( TRUE );
	}

	if( ( viewport->zBuffer = ( float * )malloc( sizeof( float ) * wh ) ) == NULL )
	{
		exit( TRUE );
	}
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

	viewport->gfxBuffer = NULL;
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
	uint32 * gfxBuf = viewport->gfxBuffer;

	int wh = viewport->wh;

	int m = 0;

	for ( ; m < wh; m ++ )
	{
		zBuf[m] = FLT_MAX;
		gfxBuf[m] = 0;
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

//����AABB��Χ�е��ӿռ�ü�
//�����Ϊ�������������
INLINE int clip_viewCulling(Matrix3D * proj_matrix, AABB aabb)
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
	Vector3D viewPosition, eyeToLocal, d;
	VertexNode * vNode;
	Vertex * vtx;
	RenderList * renderList;
	RenderVertex * screenVertices;

	int i = 0, n = 0, m = 0;

	scene = viewport->scene;
	camera = viewport->camera;

	//�������������ӿڵ����Ըı�
	//�����¼���ͶӰ����
	if ( TRUE == camera->fnfDirty || TRUE == viewport->dirty)
	{
		getPerspectiveFovLH( camera->projectionMatrix, camera, viewport->aspect );

		camera->fnfDirty = viewport->dirty = FALSE;
	}
	
	proj_matrixPtr = camera->projectionMatrix;

	renderList = viewport->renderList;

	//��������
	sceneNode = scene->nodes;

	while( NULL != sceneNode && NULL != sceneNode->entity->mesh )
	{
		entity = sceneNode->entity;
		//�������������
		matrix3D_append( & view, entity->world, camera->eye->world );
		//����ͶӰ����
		matrix3D_append4x4( & view, proj_matrixPtr );

		//�ӿռ�ü�
		if ( clip_viewCulling( & view, * entity->mesh->aabb ) > 0 )
		{
			sceneNode = sceneNode->next;

			continue;
		}

		//========================�������û��������������²���======================

		//������Ⱦ�б�����Ƚ�������
		//����ʵ����ӿռ�����
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

		//�����޳�
		if ( BACKFACE_CULLING_MODE == 1 )
		{
			//�ܹ���9�˷�+1����+6�Ӽ�+1����
			//�Ƚ���Ļ���Ա����޳�
			//���㵽�ü��ռ�Ĵ��ۣ�12�˷�+12�ӷ�
			//���㵽��Ļ�ռ�Ĵ��ۣ�2�˷�+2�ӷ�
			//�ܹ���14�˷�+14�ӷ�
			FaceNode * sceneNode;

			matrix3D_transformVector( & eyeToLocal, entity->worldInvert, camera->eye->position );

			sceneNode = entity->mesh->faces->nodes;

			//������
			while ( NULL != sceneNode )
			{
				vector3D_subtract( & d, & eyeToLocal, sceneNode->face->center );	//3����

				vector3D_normalize( & d );	//3�˷�+3�˷�+1����+1����

				//����нǴ���90��С��-90ʱ
				if ( vector3D_dotProduct( & d, sceneNode->face->normal ) <= 0.0f )	//3�˷�+3�ӷ�
				{
					polygon_setBackFace( sceneNode->face );
				}

				sceneNode = sceneNode->next;
			}
		}//�����޳����

		//��������
		vNode = entity->mesh->vertices->nodes;

		while( NULL != vNode )
		{
			vtx = vNode->vertex;

			//������й����涼�Ǳ��������
			if ( vtx->nContectedFaces == vtx->bFlag )
			{
				vNode = vNode->next;

				continue;
			}

			vtx->index = i;

			screenVertices = viewport->screenVertices;

			matrix3D_transformVector( & viewPosition, & view, vtx->position );

			screenVertices[i].x = (viewPosition.x + 0.5f) * viewport->width;
			screenVertices[i].y = (viewPosition.y + 0.5f) * viewport->height;
			screenVertices[i].z = viewPosition.z;
			screenVertices[i].u = vtx->uv->x;
			screenVertices[i].v = vtx->uv->y;
			screenVertices[i].r = vtx->color->red * 255.0f;
			screenVertices[i].g = vtx->color->green * 255.0f;
			screenVertices[i].b = vtx->color->blue * 255.0f;
			screenVertices[i].a = vtx->color->alpha * 255.0f;

			i ++;

			viewport->nRenderVertex  = i;

			vNode = vNode->next;
		}//������������

		sceneNode = sceneNode->next;

		n ++;
	}
}

void triangle_rasterize( Viewport * view, RenderVertex * ver0, RenderVertex * ver1, RenderVertex * ver2 );
void triangle_rasterize_texture( Viewport * view, RenderVertex * ver0, RenderVertex * ver1, RenderVertex * ver2, Texture * texture );
void triangle_rasterize_light( Viewport * view, RenderVertex * ver0, RenderVertex * ver1, RenderVertex * ver2 );
void triangle_rasterize_light_texture( Viewport * view, RenderVertex * ver0, RenderVertex * ver1, RenderVertex * ver2, Texture * texture );

void viewport_render(Viewport * view)
{
	Scene * scene;
	FaceNode * faceNode;
	Entity * entity;
	Vertex * (* vertex)[3];
	int i = 0;

	scene = view->scene;

	//����ʵ��
	//while( sceneNode != NULL )
	for ( ; i < view->nRenderList; i ++ )
	{
		entity = view->renderList[i].entity;
		faceNode =entity->mesh->faces->nodes;

		//������
		while( faceNode != NULL )
		{
			//����汳������������ù�դ��
			if ( TRUE == faceNode->face->isBackFace )
			{
				polygon_resetBackFace( faceNode->face );

				faceNode = faceNode->next;

				continue;
			}

			vertex = & faceNode->face->vertex;
			
			if ( scene->lightOn == FALSE )
			{
				if ( NULL == entity->texture )
				{
					triangle_rasterize( view,
										& view->screenVertices[( * vertex)[0]->index],
										& view->screenVertices[( * vertex)[1]->index],
										& view->screenVertices[( * vertex)[2]->index] );
				}
				else
				{
					triangle_rasterize_texture( view,
											& view->screenVertices[( * vertex)[0]->index],
											& view->screenVertices[( * vertex)[1]->index],
											& view->screenVertices[( * vertex)[2]->index],
											entity->texture );
				}
			}
			else
			{
				if ( NULL == entity->texture )
				{
					triangle_rasterize_light( view,
											& view->screenVertices[( * vertex)[0]->index],
											& view->screenVertices[( * vertex)[1]->index],
											& view->screenVertices[( * vertex)[2]->index] );
				}
				else
				{
					triangle_rasterize_light_texture( view, 
													& view->screenVertices[( * vertex)[0]->index],
													& view->screenVertices[( * vertex)[1]->index],
													& view->screenVertices[( * vertex)[2]->index],
													entity->texture );
				}
			}

			polygon_resetBackFace( faceNode->face );

			faceNode = faceNode->next;
		}
	}
}

#endif
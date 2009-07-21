#ifndef __VIEWPORT_H
#define __VIEWPORT_H

#include <malloc.h>
#include <float.h>

#include "Entity.h"
#include "Scene.h"
#include "Camera.h"
#include "Matrix3D.h"
#include "Vector3D.h"

typedef struct Viewport
{
	int wh, dirty, nRenderVertex;

	float width, height, offsetX, offsetY, aspect;

	//��Ȼ�����
	float * zBuffer;

	//λͼ������
	uint32 * gfxBuffer;

	struct Scene * scene;

	struct Camera * camera;

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

	viewport->nRenderVertex = 0;

	if( ( viewport->screenVertices = ( RenderVertex * )malloc( sizeof( RenderVertex ) * scene->nVertices ) ) == NULL )
	{
		exit( TRUE );
	}

	return viewport;
}

static Vector3D T_L_F_V;
static Vector3D T_R_F_V;
static Vector3D T_L_B_V;
static Vector3D T_R_B_V;
static Vector3D B_L_F_V;
static Vector3D B_R_F_V;
static Vector3D B_L_B_V;
static Vector3D B_R_B_V;
static AABB * tmpAABB;

//����AABB��Χ�е��ӿռ�ü�
//�����Ϊ�������������
int clip_viewCulling(Matrix3D * proj_matrix, AABB aabb)
{
	Vector3D * min, * max;

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

	matrix3D_transformVector( proj_matrix, & T_L_F_V );
	matrix3D_transformVector( proj_matrix, & T_R_F_V );
	matrix3D_transformVector( proj_matrix, & T_L_B_V );
	matrix3D_transformVector( proj_matrix, & T_R_B_V );
	matrix3D_transformVector( proj_matrix, & B_L_F_V );
	matrix3D_transformVector( proj_matrix, & B_R_F_V );
	matrix3D_transformVector( proj_matrix, & B_L_B_V );
	matrix3D_transformVector( proj_matrix, & B_R_B_V );

	if ( NULL == tmpAABB )
		tmpAABB = newAABB();
	else
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
	Matrix3D view, proj_matrix, * proj_matrixPtr, * cam_world;
	Vector3D viewPosition, eyeToLocal, d, * test;
	VertexNode * vNode;
	Vertex * vtx;
	RenderVertex * screenVertices;

	int i = 0;

	scene = viewport->scene;
	camera = viewport->camera;
	cam_world = camera->eye->world;

	//���ͶӰ����
	if ( TRUE == camera->fnfDirty || TRUE == viewport->dirty)
	{
		proj_matrixPtr = getPerspectiveFovLH( & proj_matrix, camera, viewport->aspect );

		matrix3D_copy(camera->projectionMatrix, proj_matrixPtr);

		camera->fnfDirty = viewport->dirty = FALSE;
	}
	else
	{
		proj_matrixPtr = camera->projectionMatrix;
	}

	//��������
	sceneNode = scene->nodes;

	do
	{
		entity = sceneNode->entity;

		matrix3D_copy( & view, entity->world );
		matrix3D_append( & view, cam_world );			//�������������
		matrix3D_append4x4( & view, proj_matrixPtr );	//����view�����ͶӰ����

		/*test = newVector3D(0.0f, 150.0f, 0.0f, 1.0f );
		matrix3D_transformVector( & view, test );*/

		//�ӿռ�ü�
		if ( clip_viewCulling( & view, * entity->mesh->aabb ) > 0 )
		{
			sceneNode = sceneNode->next;

			entity->offScreen = TRUE;

			continue;
		}

		entity->offScreen = FALSE;

		//�����޳�
		if ( BACKFACE_CULLING_MODE == 1 )
		{
			FaceNode * sceneNode;

			vector3D_copy( & eyeToLocal, camera->eye->position );

			matrix3D_transformVector( entity->worldInvert, & eyeToLocal );

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
			//�ܹ���9�˷�+1����+6�Ӽ�+1����
			//�Ƚ���Ļ���Ա����޳�
			//���㵽�ü��ռ�Ĵ��ۣ�12�˷�+12�ӷ�
			//���㵽��Ļ�ռ�Ĵ��ۣ�2�˷�+2�ӷ�
			//�ܹ���14�˷�+14�ӷ�
		}

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

			vector3D_copy( & viewPosition, vtx->position );

			matrix3D_transformVector( & view, & viewPosition );

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
		}

		sceneNode = sceneNode->next;
	}
	while( NULL != sceneNode && NULL != sceneNode->entity->mesh );
}

void resetBuffer(Viewport * view)
{
	float * zBuf = view->zBuffer;
	uint32 * gfxBuf = view->gfxBuffer;

	int wh = view->wh;

	int m = 0;

	for ( ; m < wh; m ++ )
	{
		zBuf[m] = FLT_MAX;
		gfxBuf[m] = 0;
	}
}

#endif
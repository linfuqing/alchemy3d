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
	int wh, dirty, nRenderObjects;

	float width, height, offsetX, offsetY, aspect;

	//��Ȼ�����
	float * zBuffer;

	//λͼ������
	uint32 * gfxBuffer;

	struct Scene * scene;

	struct Camera * camera;

	Entity * * renderObjects;

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

	viewport->nRenderObjects = 0;

	if( ( viewport->renderObjects = malloc( sizeof( int ) * scene->nNodes ) ) == NULL )
	{
		exit( TRUE );
	}

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

	if ( tmpAABB->min->x < -1 )	code |= 0x01;
	if ( tmpAABB->max->x > 1 )	code |= 0x02;
	if ( tmpAABB->min->y < -1 )	code |= 0x04;
	if ( tmpAABB->max->y > 1 )	code |= 0x08;
	if ( tmpAABB->min->z < 0 )	code |= 0x10;
	if ( tmpAABB->max->z > 1 )	code |= 0x20;

	return code;
}

void backFaceCulling( Vector3D * pEye, Entity * entity )
{
	FaceNode * p;
	Vector3D eyeToLocal, d;

	vector3D_copy( & eyeToLocal, pEye );

	matrix3D_transformVector( entity->worldInvert, & eyeToLocal );

	p = entity->mesh->faces->nodes;

	//��ѭ��
	while ( NULL != p )
	{
		vector3D_subtract( & d, & eyeToLocal, p->face->center );	//3����

		vector3D_normalize( & d );	//3�˷�+3�˷�+1����+1����

		//����нǴ���90��С��-90ʱ
		if ( vector3D_dotProduct( & d, p->face->normal ) <= 0.0f )	//3�˷�+3�ӷ�
		{
			polygon_setBackFace( p->face );
		}

		p = p->next;
	}
	//�ܹ���9�˷�+1����+6�Ӽ�+1����
	//�Ƚ���Ļ���Ա����޳�
	//���㵽�ü��ռ�Ĵ��ۣ�12�˷�+12�ӷ�
	//���㵽��Ļ�ռ�Ĵ��ۣ�2�˷�+2�ӷ�
	//�ܹ���14�˷�+14�ӷ�
}

static Matrix3D * proj_view_matrixPtr, * tmpMtr;
static Vector3D * tmpViewPos;

void viewport_project( Viewport * viewport )
{
	Scene * scene;
	SceneNode * p;
	Camera * camera;
	Entity * entity;
	Matrix3D proj_matrix, * proj_matrixPtr, * camWorld;
	VertexNode * vNode;
	Vertex * vtx;
	RenderVertex * screenVertices;

	int i = 0;

	scene = viewport->scene;
	camera = viewport->camera;
	camWorld = camera->eye->world;

	//���ͶӰ����
	if ( TRUE == camera->fnfDirty || TRUE == viewport->dirty)
	{
		proj_matrix = getPerspectiveFovLH( camera, viewport->aspect );

		proj_matrixPtr = & proj_matrix;

		matrix3D_copy(camera->projectionMatrix, proj_matrixPtr);

		camera->fnfDirty = viewport->dirty = FALSE;
	}
	else
	{
		proj_matrixPtr = camera->projectionMatrix;
	}

	if ( NULL == tmpMtr)
		tmpMtr = newMatrix3D(NULL);

	if ( NULL == proj_view_matrixPtr)
		proj_view_matrixPtr = newMatrix3D(NULL);

	if ( NULL == tmpViewPos )
		tmpViewPos = newVector3D( 0, 0, 0, 1 );

	//��������
	p = scene->nodes;

	do
	{
		entity = p->entity;

		matrix3D_copy(tmpMtr, entity->world);
		matrix3D_append(tmpMtr, camWorld);	//�������������

		//���ʵ�����������Ϣ������ж���任
		if ( NULL != entity->mesh )
		{
			//����view�����ͶӰ����
			matrix3D_copy( proj_view_matrixPtr, tmpMtr );
			matrix3D_append4x4( proj_view_matrixPtr, proj_matrixPtr );

			//�ӿռ�ü�
			if ( FALSE == clip_viewCulling( proj_view_matrixPtr, * entity->mesh->aabb ) )
			{
				entity->offScreen = FALSE;

				//�����޳�
				if ( BACKFACE_CULLING_MODE == 1 )
					backFaceCulling( camera->eye->position, entity );

				//��������
				vNode = entity->mesh->vertices->nodes;

				while( NULL != vNode )
				{
					vtx = vNode->vertex;

					if ( vtx->nContectedFaces == vtx->bFlag )
					{
						vNode = vNode->next;

						continue;
					}

					vtx->index = i;

					screenVertices = viewport->screenVertices;

					vector3D_copy( tmpViewPos, vtx->position );

					matrix3D_transformVector( proj_view_matrixPtr, tmpViewPos );

					screenVertices[i].x = (tmpViewPos->x + 0.5f) * viewport->width;
					screenVertices[i].y = (tmpViewPos->y + 0.5f) * viewport->height;
					screenVertices[i].z = tmpViewPos->z;
					screenVertices[i].u = vtx->uv->x;
					screenVertices[i].v = vtx->uv->y;
					screenVertices[i].color = vtx->color;

					/*AS3_Trace(AS3_String("screen..."));
					AS3_Trace(AS3_Number(vtx->position->x));
					AS3_Trace(AS3_Number(vtx->position->y));
					AS3_Trace(AS3_Number(vtx->position->z));*/

					i ++;

					vNode = vNode->next;
				}
			}
		}

		p = p->next;
	}
	while( NULL != p );
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
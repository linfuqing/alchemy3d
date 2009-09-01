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
	//�߿�����Ƿ�������ԣ���Ⱦ�����ü������޳���
	int wh, dirty, nRenderList, nClippList, nCullList;

	//�ӿڿ�ߺͿ�߱�
	int width, height;

	float aspect;

	//��Ȼ�����
	LPBYTE zBuffer;

	//��Ƶ������
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

	//��ʼ��������
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

	//������Ⱦ�б�
	viewport->renderList = initializeRenderList( scene->nFaces + 2 );

	//����ü��б�
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

	//��ʼ��������
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

	//��������иı�
	if ( TRUE == viewport->scene->dirty )
	{
		RenderList * lastRenderList = viewport->renderList;

		//�ͷ���Ⱦ�б�
		while ( NULL != lastRenderList )
		{
			renderList = lastRenderList;

			lastRenderList = lastRenderList->next;

			free( renderList );
		}

		//���¹�����Ⱦ�б�
		viewport->renderList = initializeRenderList( viewport->scene->nFaces + 2 );
	}
	else
	{
		renderList = viewport->renderList->next;

		//������Ⱦ�б�
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
	//���������Ⱦ�б�β���������NUM_PER_RL_INIT���µĽ��(ע�Ᵽ����β)
	if ( NULL == (* rl_ptr)->next )
		extendRenderList( rl_ptr, NUM_PER_RL_INIT );

	( * rl_ptr )->polygon = face;
	( * rl_ptr ) = ( * rl_ptr )->next;
}

//����AABB��Χ�е��ӿռ�ü�
//viewport		�ӿ�
//camera		�����
//entity		��ǰҪ�����ʵ��
//rl_ptr		��ǰ��Ⱦ�б��ָ��
//cl_ptr		��ǰ�ü��б��ָ��
void frustumClipping( Viewport * viewport, Entity * entity, float near, RenderList ** rl_ptr, RenderList ** cl_ptr )
{
	int j, zCode0, zCode1, zCode2, verts_out, crossNear = FALSE;

	Vector3D viewerToLocal;

	Triangle * face, * face1, * face2;

	int v0, v1, v2;

	Vertex * tmpVert1, * tmpVert2;

	float t1, t2,  xi, yi, ui, vi, x01i, x02i, y01i, y02i, u01i, u02i, v01i, v02i, dx, dy, dz;

	Vertex * ver1_0, * ver1_1, * ver1_2, * ver2_0, * ver2_1, * ver2_2;

	//���԰�Χ���Ƿ�Խ������
	if ( entity->mesh->CVVAABB->max->w > near && entity->mesh->CVVAABB->min->w < near ) crossNear = TRUE;

	//�����������
	//������
	for( j = 0; j < entity->mesh->nFaces; j ++)
	{
		zCode0 = zCode1 = zCode2 = 0;

		//Խ������0
		verts_out = FALSE;

		face = & entity->mesh->faces[j];

		face1 = face2 = NULL;

		tmpVert1 = tmpVert2 = NULL;

		//�����޳�
		//�߿�ģʽ��������б����޳�
		if ( entity->mesh->render_mode != RENDER_WIREFRAME_TRIANGLE_32 )
		{
			vector3D_subtract( & viewerToLocal, entity->viewerToLocal, face->center );

			vector3D_normalize( & viewerToLocal );

			//����нǴ���90��С��-90ʱ�������������
			if ( vector3D_dotProduct( & viewerToLocal, face->normal ) < 0.0f )
			{
				viewport->nCullList ++;

				continue;
			}
		}

		//�����Χ�д�Խ�����棬���һ��������Ƿ�Խ������
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

			//�����Ϊ�������洩Խ������
			if ( zCode0 | zCode1 | zCode2 )
			{
				//��ʼ�ü�
				//���������
				//1��ֻ��һ�������ڽ������ڲ�
				if ( verts_out == 2 )
				{
					//�ۼƲü������
					viewport->nClippList ++;

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
						insertFaceToList( cl_ptr, face1 );
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
					dx = ver1_1->w_pos->x - ver1_0->w_pos->x;
					dy = ver1_1->w_pos->y - ver1_0->w_pos->y;
					dz = ver1_1->w_pos->z - ver1_0->w_pos->z;

					t1 = ( ( near - ver1_0->w_pos->z ) / dz );

					//���㽻��x��y����
					xi = ver1_0->w_pos->x + dx * t1;
					yi = ver1_0->w_pos->y + dy * t1;

					//�ý��㸲��ԭ���Ķ���
					ver1_1->w_pos->x = xi;
					ver1_1->w_pos->y = yi;
					ver1_1->w_pos->z = near;

					//=======================��v0->v2�߽��вü�=======================
					dx = ver1_2->w_pos->x - ver1_0->w_pos->x;
					dy = ver1_2->w_pos->y - ver1_0->w_pos->y;
					dz = ver1_2->w_pos->z - ver1_0->w_pos->z;

					t2 = ( ( near - ver1_0->w_pos->z ) / dz );

					//���㽻��x��y����
					xi = ver1_0->w_pos->x + dx * t2;
					yi = ver1_0->w_pos->y + dy * t2;

					//�ý��㸲��ԭ���Ķ���
					ver1_2->w_pos->x = xi;
					ver1_2->w_pos->y = yi;
					ver1_2->w_pos->z = near;

					//��������Ƿ������
					//����У��������������вü�
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
					//�����α��ü���Ϊ�ı��Σ���Ҫ�ָ�Ϊ����������

					//�ۼƲü������
					viewport->nClippList += 2;

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
						insertFaceToList( cl_ptr, face1 );
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
						insertFaceToList( cl_ptr, face2 );
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
					dx = ver1_1->w_pos->x - ver1_0->w_pos->x;
					dy = ver1_1->w_pos->y - ver1_0->w_pos->y;
					dz = ver1_1->w_pos->z - ver1_0->w_pos->z;

					t1 = ( ( near - ver1_0->w_pos->z ) / dz );

					x01i = ver1_0->w_pos->x + dx * t1;
					y01i = ver1_0->w_pos->y + dy * t1;

					//=======================��v0->v2�߽��вü�=======================
					dx = ver1_2->w_pos->x - ver1_0->w_pos->x;
					dy = ver1_2->w_pos->y - ver1_0->w_pos->y;
					dz = ver1_2->w_pos->z - ver1_0->w_pos->z;

					t2 = ( ( near - ver1_0->w_pos->z ) / dz );

					x02i = ver1_0->w_pos->x + dx * t2;
					y02i = ver1_0->w_pos->y + dy * t2;

					//������������Ҫ�ý���1����ԭ���������ζ���
					//�ָ��ĵ�һ��������

					//�ý���1����ԭ�������εĶ���0
					ver1_0->w_pos->x = x01i;
					ver1_0->w_pos->y = y01i;
					ver1_0->w_pos->z = near;

					//�ý���1�����������εĶ���1������2���Ƕ���0
					ver2_1->w_pos->x = x01i;
					ver2_1->w_pos->y = y01i;
					ver2_1->w_pos->z = near;

					ver2_0->w_pos->x = x02i;
					ver2_0->w_pos->y = y02i;
					ver2_0->w_pos->z = near;

					//��������Ƿ������
					//����У��������������вü�
					if ( NULL != face->texture )
					{
						u01i = ver1_0->uv->u + ( ver1_1->uv->u - ver1_0->uv->u ) * t1;
						v01i = ver1_0->uv->v + ( ver1_1->uv->v - ver1_0->uv->v ) * t1;

						u02i = ver1_0->uv->u + ( ver1_2->uv->u - ver1_0->uv->u ) * t2;
						v02i = ver1_0->uv->v + ( ver1_2->uv->v - ver1_0->uv->v ) * t2;

						//����ԭ������������
						ver1_0->uv->u = u01i;
						ver1_0->uv->v = v01i;

						ver2_1->uv->u = u01i;
						ver2_1->uv->v = v01i;

						ver2_0->uv->u = u02i;
						ver2_0->uv->v = v02i;
					}

					//���㶥�㷨��
				}
				//���ж����ڽ���������
				else
				{
					//�ۼ��޳������
					viewport->nCullList ++;

					continue;
				}
			}
		}

		//�����½�����
		//���û�����棬�����ԭ������
		if ( NULL == face1 && NULL == face2 )
		{
			insertFaceToList( rl_ptr, face );

			viewport->nRenderList ++;
		}
		//�����������
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

	//���ձ���
	Vertex * vs;
	Material * material;
	Lights * lights;
	Light * light;
	Vector3D vFDist, vLightToVertex, vVertexToLight, vVertexToCamera;
	FloatColor fColor, lastColor, mColor, outPutColor;
	float dot, fAttenuCoef, fc1, fc2, fDist, fSpotFactor, fShine, fShineFactor;

	float invw;

	int l = 0, j = 0, code = 0;

	//����й�Դ
	//���������ڼ�¼�Ա�����Ϊ�ο���Ĺ�Դ����
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

	//����Ⱦ�б��ָ��ָ���ͷ����һ�����
	rl_ptr = viewport->renderList->next;
	cl_ptr = viewport->clippedList->next;

	//�����������ӿڵ����Ըı�
	//�����¼���ͶӰ����
	if ( TRUE == camera->fnfDirty || TRUE == viewport->dirty)
	{
		getPerspectiveFovLH( camera->projectionMatrix, camera, viewport->aspect );

		camera->fnfDirty = viewport->dirty = FALSE;
	}

	//matrix3D_append4x4( & projection, camera->eye->world, camera->projectionMatrix );

	//����ʵ��
	sceneNode = scene->nodes;
	while( NULL != sceneNode )
	{
		entity = sceneNode->entity;

		//����ʵ��ľֲ�����������
		entity_updateTransform(entity);

		if ( NULL != entity->mesh )
		{
			code = 0;

			matrix3D_append( entity->view, entity->world, camera->eye->world );

			//����͸��ͶӰ����
			matrix3D_append4x4( entity->projection, entity->view, camera->projectionMatrix );

			//�������CVV��AABB
			mesh_transformNewAABB( entity->mesh->CVVAABB, entity->projection, entity->mesh->aabb );

			//���ڰ�Χ�е���׶���޳�
			if ( entity->mesh->CVVAABB->max->x < -1 )				code |= 0x01;
			if ( entity->mesh->CVVAABB->min->x > 1 )				code |= 0x02;
			if ( entity->mesh->CVVAABB->max->y < -1 )				code |= 0x04;
			if ( entity->mesh->CVVAABB->min->y > 1 )				code |= 0x08;
			if ( entity->mesh->CVVAABB->max->w <= camera->near )	code |= 0x10;
			if ( entity->mesh->CVVAABB->min->w >= camera->far )		code |= 0x20;

			//�����Ϊ�����ʵ������
			if ( code > 0 )
			{
				//�ۼ����������
				viewport->nCullList += entity->mesh->nFaces;

				sceneNode = sceneNode->next;

				continue;
			}

			//�����ӵ���ʵ��ľֲ�����
			matrix3D_transformVector( entity->viewerToLocal, entity->worldInvert, camera->eye->position );

			//��������
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
				//�������ֲ����귢���ı�
				if ( TRUE == entity->mesh->dirty )
				{
					vs->position->x = meshBuffer[k];
					vs->position->y = meshBuffer[k + 1];
					vs->position->z = meshBuffer[k + 2];
				}
#endif

				//�Ѷ���任���ӿռ䣬����fix_inv_z���1/z������4.28��������ʾ
				matrix3D_transformVector( vs->w_pos, entity->view, vs->position );
			}

			//��¼��ǰ����Ⱦ�б�ָ��
			curr_rl_ptr	= rl_ptr;

			//����βü�
			frustumClipping( viewport, entity, camera->near, & rl_ptr, & cl_ptr );

			//========================���ռ���======================

			//��Դ����Ϊ��
			l = 0;

			material = entity->mesh->material;

			//���¹�Դλ��
			lights = scene->lights;

			//������Դ
			while ( NULL != lights && entity->lightEnable )
			{
				if ( TRUE == lights->light->bOnOff )
				{
					//��ʵ������������任��Դ��λ�õõ���ʵ��Ϊ�ο���Ĺ�Դ��λ�ã�����������
					matrix3D_transformVector( & vLightsToObject[l], entity->worldInvert, lights->light->source->w_pos );
				}

				lights = lights->next;

				l ++;
			}

			//������Ч�ʺ���ʵ��֮��ȡ��һ��ƽ�⣬������ȫ�ֻ�����Ĺ��ף�ֱ��ʹ�ò��ʵĻ����ⷴ��ϵ����Ϊ������ɫ
			floatColor_zero( & mColor );
			floatColor_add_self( & mColor, material->ambient );

			//������Ⱦ�б�
			renderList = curr_rl_ptr;

			while ( NULL != renderList && NULL != renderList->polygon )
			{
				//Ϊ����α����Ⱦģʽ
				renderList->polygon->render_mode = entity->mesh->render_mode;

				//��������
				for ( j = 0; j < 3; j ++)
				{
					vs = renderList->polygon->vertex[j];

					//��������Ѿ��任����㣬��ֱ�ӽ�����һ������
					if ( TRUE == vs->transformed ) continue;

					//===================���պͶ�����ɫֵ===================

					//��Դ����Ϊ��
					l = 0;

					floatColor_identity( & lastColor);

					if ( NULL != material )
					{
						//���Ʋ�����ɫ��������ɫ
						floatColor_copy( & lastColor, & mColor );

						//��ʱ��ɫ����
						floatColor_zero( & fColor );

						lights = scene->lights;

						//�����ڹ�Դ�Ĺ���
						//����й�Դ
						while ( NULL != lights && entity->lightEnable )
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
							if ( light->mode == HIGH_MODE || light->mode == MID_MODE)
							{
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
										vector3D_subtract( & vFDist, & vLightsToObject[l], vs->position );
										fDist = vector3D_lengthSquared( & vFDist );

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
									vector3D_subtract( & vLightToVertex, vs->position, & vLightsToObject[l] );

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
								floatColor_append( & fColor, material->ambient, light->ambient );

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
									floatColor_add_self( & fColor, floatColor_scaleBy_self( floatColor_append( & outPutColor, material->diffuse, light->diffuse ), dot, dot, dot, 1 ) );

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
											floatColor_add_self( & fColor, floatColor_scaleBy_self( floatColor_append( & outPutColor, material->specular, light->specular ), fShineFactor, fShineFactor, fShineFactor, 1 ) );
										}
									}
								}

								//������˥���;۹����ӣ��� j ���������ĵڸ����������:
								fSpotFactor *= fAttenuCoef;

								floatColor_scaleBy_self( & fColor, fSpotFactor, fSpotFactor, fSpotFactor, 1.0f );

								//�ۼ��������ɫ:
								floatColor_add_self( & lastColor, & fColor );
							}
							else
							{
								vector3D_normalize( & vLightsToObject[l] );

								//��Դ�Ͷ���ļн�
								dot = vector3D_dotProduct( & vLightsToObject[l], vs->normalLength );

								//���뻷�����䲿��
								floatColor_append( & fColor, material->ambient, light->ambient );


								//����нǴ���0�����нǷ�Χ��(-90, 90)֮��
								if ( dot > 0.0f )
								{
									//�����䲿�ֵĹ���
									floatColor_add_self( & fColor, floatColor_scaleBy_self( floatColor_append( & outPutColor, material->diffuse, light->diffuse ), dot, dot, dot, 1 ) );
								}

								//�ۼ��������ɫ:
								floatColor_add_self( & lastColor, & fColor );
							}

							lights = lights->next;

							l ++;
						}//end light

						//����ɫ��һ������
						floatColor_normalize( & lastColor );

						//����alpha, ����򵥵��ò�������������������
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

		//ѡ����Ⱦ��
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
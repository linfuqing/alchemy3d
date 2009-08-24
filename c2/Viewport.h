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

	//��ʼ��������
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

	//������Ⱦ�б�
	viewport->renderList = initializeRenderList( scene->nFaces + 2 );

	//����ü��б�
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

	//��ʼ��������
	for ( ; m < wh; m ++ )
	{
		zBuf[m] = FLT_MAX;
		mixedChannel[m] = 0;
	}

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

	//���ձ���
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

	//�����������ӿڵ����Ըı�
	//�����¼���ͶӰ����
	if ( TRUE == camera->fnfDirty || TRUE == viewport->dirty)
	{
		getPerspectiveFovLH( camera->projectionMatrix, camera, viewport->aspect );

		camera->fnfDirty = viewport->dirty = FALSE;
	}

	//����й�Դ
	//���������ڼ�¼�Ա�����Ϊ�ο���Ĺ�Դ����
	if ( NULL != scene->lights )
		if( ( vLightsToObject = ( Vector3D * )calloc( scene->nLights, sizeof( Vector3D ) ) ) == NULL ) exit( TRUE );

	matrix3D_append4x4( & view_projection, camera->eye->world, camera->projectionMatrix );

	//����Ⱦ�б��ָ��ָ���ͷ����һ�����
	rl_ptr = viewport->renderList->next;
	cl_ptr = viewport->clippedList->next;

	sceneNode = scene->nodes;

	//����ʵ��
	while( NULL != sceneNode && NULL != sceneNode->entity->mesh )
	{
		entity = sceneNode->entity;

		//�������������
		//matrix3D_append( entity->view, entity->world, camera->eye->world );
		//matrix3D_getPosition( entity->viewPosition, entity->view );

		//����͸��ͶӰ����
		matrix3D_append4x4( entity->projection, entity->world, & view_projection );
		matrix3D_getPosition( entity->CVVPosition, entity->projection );
		vector3D_project( entity->CVVPosition );

		mesh_transformNewAABB( entity->mesh->worldAABB, entity->world, entity->mesh->aabb );

		//����͸��ͶӰʹAABB��Z������Ť��������޷���ͶӰ�����viewAABB���б任���������¹������CVV��AABB
		mesh_transformNewAABB( entity->mesh->CVVAABB, entity->projection, entity->mesh->aabb );

		matrix3D_transformVector( entity->viewerToLocal, entity->worldInvert, camera->eye->position );

		//���ڰ�Χ�е���׶���޳�
		if ( frustumCulling( entity, worldZNear, worldZFar, & rl_ptr, & cl_ptr ) > 0 )
		{
			sceneNode = sceneNode->next;

			continue;
		}

		//=======================ʵ��û������=======================

		//========================���ռ���======================

		//��Դ����Ϊ��
		l = 0;

		material = entity->material;

		//���¹�Դλ��
		lights = scene->lights;

		//������Դ
		while ( NULL != lights )
		{
			if ( TRUE == lights->light->bOnOff )
			{
				//��ʵ������������任��Դ��λ�õõ���ʵ��Ϊ�ο���Ĺ�Դ��λ�ã�����������
				matrix3D_transformVector( & vLightsToObject[l], entity->worldInvert, lights->light->source->worldPosition );
			}

			lights = lights->next;

			l ++;
		}

		//������Ч�ʺ���ʵ��֮��ȡ��һ��ƽ�⣬������ȫ�ֻ�����Ĺ��ף�ֱ��ʹ�ò��ʵĻ����ⷴ��ϵ����Ϊ������ɫ
		floatColor_zero( & mColor );
		floatColor_add_self( & mColor, material->ambient );

		//��������
		renderList = viewport->renderList->next;

		while ( NULL != renderList && NULL != renderList->polygon )
		{
			for ( j = 0; j < 3; j ++)
			{
				vs = renderList->polygon->vertex[j];

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
					while ( NULL != lights)
					{
						//�����Դ�ǹرյ�
						if ( FALSE == lights->light->bOnOff )
						{
							lights = lights->next;

							l ++;

							continue;
						}

						scene->lightOn = TRUE;

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

				//�Ѷ���任��CVV
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
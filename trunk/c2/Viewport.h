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

	//��ɫ������
	DWORD * colorBuffer;

	//��������
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

	//��ʼ��������
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

//����AABB��Χ�е��ӿռ�ü�
//�����Ϊ�������������
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

	//�����������ӿڵ����Ըı�
	//�����¼���ͶӰ����
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

	//����й�Դ
	if ( NULL != scene->lights )
	{
		//���������ڼ�¼�Ա�����Ϊ�ο���Ĺ�Դ����
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

	//����ʵ��
	while( NULL != sceneNode && NULL != sceneNode->entity->mesh )
	{
		entity = sceneNode->entity;
		//�������������
		matrix3D_append( & view, entity->world, camera->eye->world );
		//����ͶӰ����
		matrix3D_append4x4( & view, proj_matrixPtr );

		//�ӿռ�ü�
		if ( viewport_viewCulling( & view, * entity->mesh->aabb ) > 0 )
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
		//�ܹ���9�˷�+1����+6�Ӽ�+1����
		//�Ƚ���Ļ���Ա����޳�
		//���㵽�ü��ռ�Ĵ��ۣ�12�˷�+12�ӷ�
		//���㵽��Ļ�ռ�Ĵ��ۣ�2�˷�+2�ӷ�
		//�ܹ���14�˷�+14�ӷ�
		if ( BACKFACE_CULLING_MODE == 1 )
		{
			Vector3D eyeToLocal, d;

			matrix3D_transformVector( & eyeToLocal, entity->worldInvert, camera->eye->position );

			//������
			faces = entity->mesh->faces;

			for( j = 0; j < entity->mesh->nFaces; j ++)
			{
				vector3D_subtract( & d, & eyeToLocal, faces[j].center );	//3����

				vector3D_normalize( & d );	//3�˷�+3�˷�+1����+1����

				//����нǴ���90��С��-90ʱ
				if ( vector3D_dotProduct( & d, faces[j].normal ) <= 0.0f )	//3�˷�+3�ӷ�
				{
					polygon_setBackFace( & faces[j] );
				}
			}
		}//�����޳����

		//��Դ����Ϊ��
		l = 0;

		material = entity->material;

		if ( NULL != material )
		{
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
			color_zero( & mColor );
			color_add_self( & mColor, material->ambient );
		}

		//��������
		vs = entity->mesh->vertices;

		for( j = 0; j < entity->mesh->nVertices; j ++)
		{
			//������й����涼�Ǳ��������
			if ( vs[j].nContectedFaces == vs[j].bFlag )
			{
				continue;
			}

			//===================���պͶ�����ɫֵ===================

			//��Դ����Ϊ��
			l = 0;

			color_identity( & lastColor);

			if ( NULL != material )
			{
				//���Ʋ�����ɫ��������ɫ
				color_copy( & lastColor, & mColor );
				//��ʱ��ɫ����
				color_zero( & fColor );

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
								vector3D_subtract( & vFDist, & vLightsToObject[l], vs[j].position );
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
							vector3D_subtract( & vLightToVertex, vs[j].position, & vLightsToObject[l] );

							//��λ��
							vector3D_normalize( & vLightToVertex );

							//�۹����䷽��(�Ѿ���һ����λ����) �� ���� vLightToVertex �нǵ�����
							dot = vector3D_dotProduct( & vLightToVertex, light->source->direction );

							//�������λ�ڹ�׶֮��, �򲻻��о۹�������䵽������
							if( dot < light->spotCutoff )
								fSpotFactor = 0.0f;
							else
							{
								//���þ۹�ָ�����м���
								fSpotFactor = powf( dot, light->spotExp );
							}
						}
						// �������Թ�Դ�Ĺ���(�����Ѿ����㹻��������)

						//���뻷�����䲿��:
						color_append( & fColor, material->ambient, light->ambient );

						//���, ���������䲿��

						//����ָ���Դ������
						vector3D_subtract( & vVertexToLight, & vLightsToObject[l], vs[j].position );

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
						dot = vector3D_dotProduct( & vVertexToLight, vs[j].normal );

						if ( dot > 0.0f )
						{
							//���������䲿�ֵĹ���
							color_add_self( & fColor, color_scaleBy_self( color_append( & outPutColor, material->diffuse, light->diffuse ), dot, dot, dot, 1 ) );
						}

						//����߹ⲿ��
						if ( light->mode == HIGH_MODE )
						{
							if ( dot > 0.0f )
							{
								vector3D_add_self( & vVertexToCamera, & vVertexToLight );
								vector3D_normalize( & vVertexToCamera );

								//������ӻ���:�붥�㷨�������
								fShine = vector3D_dotProduct( & vVertexToCamera, vs[j].normal );

								if ( fShine > 0.0f )
								{
									fShineFactor = powf(fShine, material->power);

									//����߹ⲿ�ֵĹ���
									color_add_self( & fColor, color_scaleBy_self( color_append( & outPutColor, material->specular, light->specular ), fShineFactor, fShineFactor, fShineFactor, 1 ) );
								}
							}
						}

						//������˥���;۹����ӣ��� j ���������ĵڸ����������:
						fSpotFactor *= fAttenuCoef;

						color_scaleBy_self( & fColor, fSpotFactor, fSpotFactor, fSpotFactor, 1.0f );

						//�ۼ��������ɫ:
						color_add_self( & lastColor, & fColor );
					}
					else
					{
						vector3D_normalize( & vLightsToObject[l] );

						//��Դ�Ͷ���ļн�
						dot = vector3D_dotProduct( & vLightsToObject[l], vs[j].normal2 );

						//���뻷�����䲿��
						color_append( & fColor, material->ambient, light->ambient );


						//����нǴ���0�����нǷ�Χ��(-90, 90)֮��
						if ( dot > 0.0f )
						{
							//�����䲿�ֵĹ���
							color_add_self( & fColor, color_scaleBy_self( color_append( & outPutColor, material->diffuse, light->diffuse ), dot, dot, dot, 1 ) );
						}

						//�ۼ��������ɫ:
						color_add_self( & lastColor, & fColor );
					}

					lights = lights->next;

					l ++;
				}//end light

				//����ɫ��һ������
				color_normalize( & lastColor );

				//����alpha, ����򵥵��ò�������������������
				lastColor.alpha = material->diffuse->alpha;
			}

			vs[j].index = i;

			screenVertices = viewport->screenVertices;

			//�Ѷ���任����Ļ����ϵ
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
		}//������������

		sceneNode = sceneNode->next;

		n ++;
	}//��������ʵ��

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

	//����ʵ��
	for ( i = 0; i < view->nRenderList; i ++ )
	{
		entity = view->renderList[i].entity;

		//������
		faces = entity->mesh->faces;

		for( j = 0; j < entity->mesh->nFaces; j ++)
		{
			//����汳������������ù�դ��
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
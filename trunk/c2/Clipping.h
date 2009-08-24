#ifndef __CLIPPING_H
#define __CLIPPING_H

#include "AABB.h"

#define NUM_PER_RL_INIT 50

typedef struct RenderList
{
	Triangle * polygon;

	struct RenderList * next;
	struct RenderList * pre;
}RenderList;

typedef struct RenderVList
{
	Vertex * vertex;

	struct RenderVList * next;
	struct RenderVList * pre;
}RenderVList;

typedef struct Viewport
{
	int wh, dirty, nOrderList, nRenderList;

	float width, height, offsetX, offsetY, aspect;

	//��Ȼ�����
	float * zBuffer;

	LPDWORD mixedChannel;

	struct Scene * scene;

	struct Camera * camera;

	struct RenderList * renderList, * clippedList;
}Viewport;

RenderList * initializeRenderList( int number );
RenderVList * initializeRenderVList( int number );

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
//entity		��ǰҪ�����ʵ��
//worldZNear	���������������
//worldZFar		Զ�������������
//rl_ptr		��ǰ��Ⱦ�б��ָ��
//cl_ptr		��ǰ�ü��б��ָ��
INLINE int frustumCulling( Entity * entity, float worldZNear, float worldZFar, RenderList ** rl_ptr, RenderList ** cl_ptr )
{
	int code = 0;

	AABB * worldAABB = entity->mesh->worldAABB;
	AABB * CVVAABB = entity->mesh->CVVAABB;

	//�����Ϊ�����ʵ������
	if ( CVVAABB->max->x < -1 )	code |= 0x01;
	if ( CVVAABB->min->x > 1 )	code |= 0x02;
	if ( CVVAABB->max->y < -1 )	code |= 0x04;
	if ( CVVAABB->min->y > 1 )	code |= 0x08;
	if ( worldAABB->max->z <= worldZNear )	code |= 0x10;
	if ( worldAABB->min->z >= worldZFar )	code |= 0x20;

	//���ʵ��û������
	if ( code == 0 )
	{
		int j, zCode0, zCode1, zCode2, verts_out, crossNear = FALSE;

		Vector3D viewerToLocal;

		Triangle * face, * face1, * face2;

		AABB * worldAABB = entity->mesh->worldAABB;

		int v0, v1, v2;

		Vertex * tmpVert1, * tmpVert2;

		float t1, t2,  xi, yi, ui, vi, x01i, x02i, y01i, y02i, u01i, u02i, v01i, v02i;

		Vector3D v;

		Vertex * ver1_0, * ver1_1, * ver1_2, * ver2_0, * ver2_1, * ver2_2;

		//���԰�Χ���Ƿ�Խ������
		if ( worldAABB->max->z > worldZNear && worldAABB->min->z < worldZNear ) crossNear = TRUE;

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
			vector3D_subtract( & viewerToLocal, entity->viewerToLocal, face->center );

			vector3D_normalize( & viewerToLocal );

			//����нǴ���90��С��-90ʱ�������������
			if ( vector3D_dotProduct( & viewerToLocal, face->normal ) < 0.0f ) continue;

			//�����Χ�д�Խ�����棬���һ��������Ƿ�Խ������
			if ( TRUE == crossNear )
			{
				if ( face->vertex[0]->worldPosition->z <= worldZNear )
				{
					verts_out ++;
					zCode0 = 0x01;
				}

				if ( face->vertex[1]->worldPosition->z <= worldZNear )
				{
					verts_out ++;
					zCode1 = 0x02;
				}

				if ( face->vertex[2]->worldPosition->z <= worldZNear )
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
							////���ﲻ��clone��������Ϊ��Ҫ�͵�һ�������ι��ö���
							//face1 = newTriangle( face->vertex[0], face->vertex[1], face->vertex[2], face->vertex[0]->uv, face->vertex[1]->uv,  face->vertex[2]->uv, face->texture );
							//if( ( face1->normal = ( Vector3D * )malloc( sizeof( Vector3D ) ) ) == NULL ) exit( TRUE );
							//memcpy( face1->normal, face->normal, sizeof( Vector3D ) );
							face1 = triangle_clone( face );

							insertFaceToList( cl_ptr, face1 );
						}

						////��鵱ǰ�ü������б�ָ��Ķ���ָ���Ƿ�Ϊ��
						////�������ָ�Ϊԭʼ��Ϣ
						//if ( ( * cv_ptr )->vertex )
						//{
						//	tmpVert1 = ( * cv_ptr )->vertex;
						//}
						//else
						//{
						//	//�½�һ��������Ϊ���ӵĶ��㣬����Ҫ���ı�������͹����棬��ֻ��һ����ʱ���㣬����Ⱦ����ͷŵ�
						//	tmpVert1 = newVertex( 0.0f, 0.0f, 0.0f );
						//	tmpVert2 = newVertex( 0.0f, 0.0f, 0.0f );
						//}

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
						vector3D_subtract( & v, ver1_1->worldPosition, ver1_0->worldPosition );

						t1 = ( ( worldZNear - ver1_0->worldPosition->z ) / v.z );

						//���㽻��x��y����
						xi = ver1_0->worldPosition->x + v.x * t1;
						yi = ver1_0->worldPosition->y + v.y * t1;

						//�ý��㸲��ԭ���Ķ���
						ver1_1->worldPosition->x = xi;
						ver1_1->worldPosition->y = yi;
						ver1_1->worldPosition->z = worldZNear;

						//=======================��v0->v2�߽��вü�=======================
						vector3D_subtract( & v, ver1_2->worldPosition, ver1_0->worldPosition );

						t2 = ( ( worldZNear - ver1_0->worldPosition->z ) / v.z );

						//���㽻��x��y����
						xi = ver1_0->worldPosition->x + v.x * t2;
						yi = ver1_0->worldPosition->y + v.y * t2;

						//�ý��㸲��ԭ���Ķ���
						ver1_2->worldPosition->x = xi;
						ver1_2->worldPosition->y = yi;
						ver1_2->worldPosition->z = worldZNear;

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
							face1 = triangle_clone( face );

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
							face2 = triangle_clone( face );

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
						vector3D_subtract( &v, ver1_1->worldPosition, ver1_0->worldPosition );

						t1 = ( ( worldZNear - ver1_0->worldPosition->z ) / v.z );

						x01i = ver1_0->worldPosition->x + v.x * t1;
						y01i = ver1_0->worldPosition->y + v.y * t1;

						//=======================��v0->v2�߽��вü�=======================
						vector3D_subtract( &v, ver1_2->worldPosition, ver1_0->worldPosition );

						t2 = ( ( worldZNear - ver1_0->worldPosition->z ) / v.z );

						x02i = ver1_0->worldPosition->x + v.x * t2;
						y02i = ver1_0->worldPosition->y + v.y * t2;

						//������������Ҫ�ý���1����ԭ���������ζ���
						//�ָ��ĵ�һ��������

						//�ý���1����ԭ�������εĶ���0
						ver1_0->worldPosition->x = x01i;
						ver1_0->worldPosition->y = y01i;
						ver1_0->worldPosition->z = worldZNear;

						//�ý���1�����������εĶ���1������2���Ƕ���0
						ver2_1->worldPosition->x = x01i;
						ver2_1->worldPosition->y = y01i;
						ver2_1->worldPosition->z = worldZNear;

						ver2_0->worldPosition->x = x02i;
						ver2_0->worldPosition->y = y02i;
						ver2_0->worldPosition->z = worldZNear;

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
				}
			}

			//�����½�����
			//���û�����棬�����ԭ������
			if ( NULL == face1 && NULL == face2 )
			{
				insertFaceToList( rl_ptr, face );
			}
			//�����������
			else
			{
				if ( NULL != face1 )
				{
					insertFaceToList( rl_ptr, face1 );
				}

				if ( NULL != face2 )
				{
					insertFaceToList( rl_ptr, face2 );
				}
			}

			/*fz = face->vertex[0]->worldPosition->z;

			renderList = viewport->renderList;

			nextRenderList = viewport->rl_ptr;
			viewport->rl_ptr = viewport->rl_ptr->next;

			nextRenderList->polygon = face;
			nextRenderList->depth = fz;
			nextRenderList->pre->next = nextRenderList->next;
			if ( NULL != nextRenderList->next ) nextRenderList->next->pre = nextRenderList->pre;

			while ( NULL != renderList->next->polygon )
			{
				if ( fz < renderList->next->depth )
				{
					nextRenderList->next = renderList->next;
					renderList->next->pre = nextRenderList->pre;

					nextRenderList->pre = renderList;
					renderList->next = nextRenderList;

					linked = TRUE;

					break;
				}

				renderList = renderList->next;
			}

			if ( FALSE == linked )
			{
				nextRenderList->next = renderList->next;
				renderList->next->pre = nextRenderList;

				nextRenderList->pre = renderList;
				renderList->next = nextRenderList;
			}*/
		}
	}

	return code;
}

# endif
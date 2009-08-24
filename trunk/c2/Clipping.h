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

	//深度缓冲区
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
	//如果到达渲染列表尾部，则插入NUM_PER_RL_INIT个新的结点(注意保留表尾)
	if ( NULL == (* rl_ptr)->next )
		extendRenderList( rl_ptr, NUM_PER_RL_INIT );

	( * rl_ptr )->polygon = face;
	( * rl_ptr ) = ( * rl_ptr )->next;
}

//基于AABB包围盒的视空间裁剪
//entity		当前要处理的实体
//worldZNear	近截面的世界坐标
//worldZFar		远截面的世界坐标
//rl_ptr		当前渲染列表的指针
//cl_ptr		当前裁剪列表的指针
INLINE int frustumCulling( Entity * entity, float worldZNear, float worldZFar, RenderList ** rl_ptr, RenderList ** cl_ptr )
{
	int code = 0;

	AABB * worldAABB = entity->mesh->worldAABB;
	AABB * CVVAABB = entity->mesh->CVVAABB;

	//输出码为非零侧实体离屏
	if ( CVVAABB->max->x < -1 )	code |= 0x01;
	if ( CVVAABB->min->x > 1 )	code |= 0x02;
	if ( CVVAABB->max->y < -1 )	code |= 0x04;
	if ( CVVAABB->min->y > 1 )	code |= 0x08;
	if ( worldAABB->max->z <= worldZNear )	code |= 0x10;
	if ( worldAABB->min->z >= worldZFar )	code |= 0x20;

	//如果实体没有离屏
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

		//测试包围盒是否穿越近截面
		if ( worldAABB->max->z > worldZNear && worldAABB->min->z < worldZNear ) crossNear = TRUE;

		//不离屏则继续
		//遍历面
		for( j = 0; j < entity->mesh->nFaces; j ++)
		{
			zCode0 = zCode1 = zCode2 = 0;

			//越界标记置0
			verts_out = FALSE;

			face = & entity->mesh->faces[j];

			face1 = face2 = NULL;

			tmpVert1 = tmpVert2 = NULL;

			//背面剔除
			vector3D_subtract( & viewerToLocal, entity->viewerToLocal, face->center );

			vector3D_normalize( & viewerToLocal );

			//如果夹角大于90或小于-90时，即背向摄像机
			if ( vector3D_dotProduct( & viewerToLocal, face->normal ) < 0.0f ) continue;

			//如果包围盒穿越近截面，则进一步检测面是否穿越近截面
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

				//输出码为非零则面穿越近截面
				if ( zCode0 | zCode1 | zCode2 )
				{
					//开始裁剪
					//分两种情况
					//1、只有一个顶点在近截面内侧
					if ( verts_out == 2 )
					{
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
							////这里不用clone函数，因为需要和第一个三角形共用顶点
							//face1 = newTriangle( face->vertex[0], face->vertex[1], face->vertex[2], face->vertex[0]->uv, face->vertex[1]->uv,  face->vertex[2]->uv, face->texture );
							//if( ( face1->normal = ( Vector3D * )malloc( sizeof( Vector3D ) ) ) == NULL ) exit( TRUE );
							//memcpy( face1->normal, face->normal, sizeof( Vector3D ) );
							face1 = triangle_clone( face );

							insertFaceToList( cl_ptr, face1 );
						}

						////检查当前裁剪顶点列表指针的顶点指针是否为空
						////如果是则恢复为原始信息
						//if ( ( * cv_ptr )->vertex )
						//{
						//	tmpVert1 = ( * cv_ptr )->vertex;
						//}
						//else
						//{
						//	//新建一个顶点作为增加的顶点，不需要关心本地坐标和关联面，这只是一个临时顶点，在渲染后会释放掉
						//	tmpVert1 = newVertex( 0.0f, 0.0f, 0.0f );
						//	tmpVert2 = newVertex( 0.0f, 0.0f, 0.0f );
						//}

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
						vector3D_subtract( & v, ver1_1->worldPosition, ver1_0->worldPosition );

						t1 = ( ( worldZNear - ver1_0->worldPosition->z ) / v.z );

						//计算交点x、y坐标
						xi = ver1_0->worldPosition->x + v.x * t1;
						yi = ver1_0->worldPosition->y + v.y * t1;

						//用交点覆盖原来的顶点
						ver1_1->worldPosition->x = xi;
						ver1_1->worldPosition->y = yi;
						ver1_1->worldPosition->z = worldZNear;

						//=======================对v0->v2边进行裁剪=======================
						vector3D_subtract( & v, ver1_2->worldPosition, ver1_0->worldPosition );

						t2 = ( ( worldZNear - ver1_0->worldPosition->z ) / v.z );

						//计算交点x、y坐标
						xi = ver1_0->worldPosition->x + v.x * t2;
						yi = ver1_0->worldPosition->y + v.y * t2;

						//用交点覆盖原来的顶点
						ver1_2->worldPosition->x = xi;
						ver1_2->worldPosition->y = yi;
						ver1_2->worldPosition->z = worldZNear;

						//检查多边形是否带纹理
						//如果有，则对纹理坐标进行裁剪
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
						//三角形被裁剪后为四边形，需要分割为两个三角形

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
							face1 = triangle_clone( face );

							insertFaceToList( cl_ptr, face1 );
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
							face2 = triangle_clone( face );

							insertFaceToList( cl_ptr, face2 );
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
						vector3D_subtract( &v, ver1_1->worldPosition, ver1_0->worldPosition );

						t1 = ( ( worldZNear - ver1_0->worldPosition->z ) / v.z );

						x01i = ver1_0->worldPosition->x + v.x * t1;
						y01i = ver1_0->worldPosition->y + v.y * t1;

						//=======================对v0->v2边进行裁剪=======================
						vector3D_subtract( &v, ver1_2->worldPosition, ver1_0->worldPosition );

						t2 = ( ( worldZNear - ver1_0->worldPosition->z ) / v.z );

						x02i = ver1_0->worldPosition->x + v.x * t2;
						y02i = ver1_0->worldPosition->y + v.y * t2;

						//计算出交点后需要用交点1覆盖原来的三角形顶点
						//分割后的第一个三角形

						//用交点1覆盖原来三角形的顶点0
						ver1_0->worldPosition->x = x01i;
						ver1_0->worldPosition->y = y01i;
						ver1_0->worldPosition->z = worldZNear;

						//用交点1覆盖新三角形的顶点1，交点2覆盖顶点0
						ver2_1->worldPosition->x = x01i;
						ver2_1->worldPosition->y = y01i;
						ver2_1->worldPosition->z = worldZNear;

						ver2_0->worldPosition->x = x02i;
						ver2_0->worldPosition->y = y02i;
						ver2_0->worldPosition->z = worldZNear;

						//检查多边形是否带纹理
						//如果有，则对纹理坐标进行裁剪
						if ( NULL != face->texture )
						{
							u01i = ver1_0->uv->u + ( ver1_1->uv->u - ver1_0->uv->u ) * t1;
							v01i = ver1_0->uv->v + ( ver1_1->uv->v - ver1_0->uv->v ) * t1;

							u02i = ver1_0->uv->u + ( ver1_2->uv->u - ver1_0->uv->u ) * t2;
							v02i = ver1_0->uv->v + ( ver1_2->uv->v - ver1_0->uv->v ) * t2;

							//覆盖原来的纹理坐标
							ver1_0->uv->u = u01i;
							ver1_0->uv->v = v01i;

							ver2_1->uv->u = u01i;
							ver2_1->uv->v = v01i;

							ver2_0->uv->u = u02i;
							ver2_0->uv->v = v02i;
						}

						//计算顶点法线
					}
				}
			}

			//插入新建的面
			//如果没有新面，则插入原来的面
			if ( NULL == face1 && NULL == face2 )
			{
				insertFaceToList( rl_ptr, face );
			}
			//否则插入新面
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
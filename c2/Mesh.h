#ifndef __MESH_H
#define __MESH_H

#include "Triangle.h"
#include "Vertex.h"
#include "Vector.h"
#include "AABB.h"
#include "Material.h"
#include "Texture.h"
#include "RenderList.h"


#define OCTREE_NOT_READY	0
#define OCTREE_READY		1

typedef struct Mesh
{
	DWORD nFaces, nVertices, nRenderList, nClippList, nCullList;

	int type, octree_depth, octreeState, v_dirty, lightEnable, fogEnable, useMipmap;
	
	float mip_dist;

	//如果不是地形,那么将存在高度
	union
	{
		float halfHeight;

		struct
		{
			int widthSegment, heightSegment;
		};
	};

	Triangle ** faces;

	Vertex ** vertices;

	struct Octree * octree;

	//struct Animation * animation;

	struct RenderList * renderList, * clippedList;

}Mesh;

#include "Octree.h"

void mesh_build( Mesh * m, int nVertices, int nFaces )
{
	int i;
	Triangle * faces;
	Vertex * vertices;
	Vector   * uvp;
	Vector3D * vp;
	ARGBColor * ap;

	
	if( ( faces		  = ( Triangle  * )malloc( sizeof( Triangle   ) * nFaces                     ) ) == NULL
	 || ( vertices	  = ( Vertex    * )malloc( sizeof( Vertex     ) * nVertices                  ) ) == NULL
	 || ( m->faces	  = ( Triangle ** )malloc( sizeof( Triangle * ) * nFaces                     ) ) == NULL
	 || ( m->vertices = ( Vertex   ** )malloc( sizeof( Vertex   * ) * nVertices                  ) ) == NULL
	 || ( vp	      = ( Vector3D  * )malloc( sizeof( Vector3D   ) * ( nVertices * 5 + nFaces ) ) ) == NULL
	 || ( ap	      = ( ARGBColor * )malloc( sizeof( ARGBColor  ) * nVertices                  ) ) == NULL
	 || ( uvp	      = ( Vector    * )malloc( sizeof( Vector     ) * nFaces    * 3              ) ) == NULL ) 
	{
		exit( TRUE );
	}

	for ( i = 0; i < nFaces; i ++ )
	{
		faces[i].normal = vp ++;

		faces[i].t_uv[0]  = uvp ++;
		faces[i].t_uv[1]  = uvp ++;
		faces[i].t_uv[2]  = uvp ++;

		m->faces[i] = faces + i;
	}

	for ( i = 0; i < nVertices; i ++ )
	{
		vertices[i].position = vp ++;
		vertices[i].w_pos    = vp ++;
		vertices[i].v_pos    = vp ++;
		vertices[i].s_pos    = vp ++;
		vertices[i].normal   = vp ++;

		vertices[i].color    = ap ++;

		m->vertices[i] = vertices + i;
	}
	
	//构造渲染列表和裁剪列表
	m->renderList = initializeRenderList( nFaces + 2 );
	m->clippedList = initializeRenderList( nFaces + 2 );
}

Mesh * newMesh( int nVertices, int nFaces )
{
	Mesh * m;

	if( ( m	= ( Mesh * )malloc( sizeof( Mesh ) ) ) == NULL) exit( TRUE );

	m->nFaces			= 0;
	m->nVertices		= 0;

	if( nVertices && nFaces )
	{
		mesh_build( m, nVertices, nFaces );
	}

	m->octree			= newOctree();
	m->octreeState		= OCTREE_NOT_READY;
	m->v_dirty			= FALSE;
	m->lightEnable		= FALSE;
	m->fogEnable		= FALSE;
	m->useMipmap		= FALSE;
	m->mip_dist			= 0;
	//m->animation        = NULL;
	m->type				= 0;
	m->octree_depth		= 0;
	m->halfHeight		= 0;

	m->nRenderList = m->nCullList = m->nClippList = 0;

	return m;
}

Vertex * mesh_push_vertex( Mesh * m, float x, float y, float z )
{
	Vertex * v = m->vertices[m->nVertices];

	vector3D_set( v->position, x, y, z, 1.0f );// = newVector3D(x, y, z, 1.0f);
	vector3D_set( v->w_pos   , x, y, z, 1.0f );// = newVector3D(x, y, z, 1.0f);
	vector3D_set( v->v_pos   , x, y, z, 1.0f );// = newVector3D(x, y, z, 1.0f);
	vector3D_set( v->s_pos   , x, y, z, 1.0f );// = newVector3D(x, y, z, 1.0f);

	argbColor_identity( v->color );// = newARGBColor( 255, 255, 255, 255 );

	vector3D_set( v->normal, x, y, z, 1.0f );// = newVector3D( 0.0f, 0.0f, 0.0f, 1.0f );

	v->contectedFaces = NULL;
	v->nContectedFaces = 0;

	v->transformed = FALSE;
	v->fix_inv_z = 0;

	m->v_dirty = TRUE;
	m->nVertices ++;

	return v;
}

Triangle * mesh_push_triangle( Mesh * m, Vertex * va, Vertex * vb, Vertex * vc, Vector * uva, Vector * uvb, Vector * uvc, Material * material, Texture * texture, int render_mode )
{
	Triangle * p = m->faces[m->nFaces];

	p->vertex[0] = va;
	p->vertex[1] = vb;
	p->vertex[2] = vc;

	p->uv[0] = uva;
	p->uv[1] = uvb;
	p->uv[2] = uvc;

	vector_copy( p->t_uv[0], uva );// = vector_clone( uva );
	vector_copy( p->t_uv[1], uvb );// = vector_clone( uvb );
	vector_copy( p->t_uv[2], uvc );// = vector_clone( uvc );

	vector3D_set ( p->normal, 0.0f, 0.0f, 0.0f, 1.0f );// = newVector3D( 0.0f, 0.0f, 0.0f, 1.0f );

	p->texture = texture;
	p->material = material;

	p->miplevel = 0;
	p->render_mode = render_mode;
	p->uvTransformed = FALSE;

	vertex_addContectedFaces( p, va );
	vertex_addContectedFaces( p, vb );
	vertex_addContectedFaces( p, vc );

	m->nFaces ++;

	m->octree->data->nFaces = m->nFaces;
	m->octree->data->faces	= m->faces;

	return p;
}

//效率可改进函数,将IF提出循环
void mesh_updateFaces( Mesh * m )
{
	DWORD i = 0;
	Triangle * face;
	Vertex * v0, * v1, * v2;

	if ( ! m->faces || m->nFaces == 0) exit( TRUE );

	for( ; i < m->nFaces; i ++ )
	{
		face = m->faces[i];

		v0 = face->vertex[0];
		v1 = face->vertex[1];
		v2 = face->vertex[2];

		face->normal->x = 0.0f;
		face->normal->y = 0.0f;
		face->normal->z = 0.0f;
		face->normal->w = 1.0f;

		triangle_normal( face->normal, v0, v1, v2 );
	}
}

void mesh_updateVertices( Mesh * m )
{
	DWORD i = 0;
	float len, oneOverMag;
	ContectedFaces * cf;
	Vertex * vert;

	if ( ! m->vertices || m->nVertices == 0) exit( TRUE );

	aabb_empty( m->octree->data->aabb );

	//不使用任何内联函数
	for( ; i < m->nVertices; i ++ )
	{
		vert = m->vertices[i];

		//重新计算AABB
		aabb_add_3D( m->octree->data->aabb, vert->position );

		if ( vert->nContectedFaces == 0 ) continue;

		vert->normal->x = 0.0f;
		vert->normal->y = 0.0f;
		vert->normal->z = 0.0f;
		vert->normal->w = 1.0f;

		//遍历关联面
		cf = m->vertices[i]->contectedFaces;

		while ( NULL != cf )
		{
			vector3D_add_self( vert->normal, cf->face->normal );

			cf = cf->next;
		}

		len = sqrtf( vert->normal->x * vert->normal->x + vert->normal->y * vert->normal->y + vert->normal->z * vert->normal->z );

		oneOverMag = 1.0f / len;

		if ( !len ) vert->normal->x = vert->normal->y = vert->normal->z = 0.0f;
		else
		{
			vert->normal->x *= oneOverMag;
			vert->normal->y *= oneOverMag;
			vert->normal->z *= oneOverMag;
		}
	}
}

void mesh_setRenderMode(  Mesh * m, DWORD renderMode )
{
	DWORD i = 0;

	for( ; i < m->nFaces; i ++ )
	{
		m->faces[i]->render_mode = renderMode;
	}
}

void mesh_setMaterial( Mesh * m, Material * mat )
{
	DWORD i = 0;

	for( ; i < m->nFaces; i ++ )
	{
		m->faces[i]->material = mat;
	}
}

void mesh_setTexture( Mesh * m, Texture * t )
{
	DWORD i = 0;

	for( ; i < m->nFaces; i ++ )
	{
		m->faces[i]->texture = t;
	}
}

void mesh_setAttribute( Mesh * mesh, Material * material, Texture * texture, DWORD renderMode )
{
	DWORD i = 0;

	for( ; i < mesh->nFaces; i ++ )
	{
		mesh->faces[i]->material = material;
		mesh->faces[i]->texture = texture;
		mesh->faces[i]->render_mode = renderMode;
	}
}

INLINE void mesh_updateMesh( Mesh * m )
{
	if ( ! m->v_dirty ) return;

	//重新计算法向量
	mesh_updateFaces( m );
	mesh_updateVertices( m );

	if ( m->octreeState == OCTREE_NOT_READY )
	{
		buildOctree( m->octree, m->octree_depth );

		m->octreeState = OCTREE_READY;
	}
}

void mesh_clear( Mesh * mesh )
{
	DWORD i = 0;

	for ( ; i < mesh->nVertices; i ++ )
	{
		vertex_dispose( mesh->vertices[i] );
	}

	for ( i = 0; i < mesh->nFaces; i ++ )
	{
		triangle_dispose( mesh->faces[i] );
	}

	if ( mesh->renderList )
	{
		free( mesh->renderList );

		mesh->nRenderList = mesh->nCullList = mesh->nClippList = 0;
	}

	free( mesh->vertices );
	free( mesh->faces );
}

Mesh * mesh_reBuild(  Mesh * m, int nVertices, int nFaces )
{
	if( m == NULL )
	{
		m = newMesh( nVertices, nFaces );
	}
	else if( ! ( m -> nVertices ) || ! ( m -> nFaces ) )
	{
		mesh_build( m, nVertices, nFaces );
	}
	else
	{
		mesh_clear( m );
		mesh_build( m, nVertices, nFaces );
	}

	return m;
}

INLINE float mesh_minY( Mesh * mesh )
{
	DWORD i;
	float min = mesh -> vertices[0] -> position -> y;

	for( i = 1; i < mesh -> nVertices; i ++ )
	{
		if( mesh -> vertices[i] -> position -> y < min )
		{
			min = mesh -> vertices[i] -> position -> y;
		}
	}

	return min;
}

INLINE int mesh_intersectMesh( Mesh * m1, Mesh * m2 )
{
	return aabb_intersectAABBs( m1->octree->data->worldAABB, m2->octree->data->worldAABB, NULL );
}

void mesh_dispose( Mesh * mesh )
{
	//替换为树的销毁函数
	//aabb_dispose( mesh->aabb );
	//aabb_dispose( mesh->worldAABB );
	//aabb_dispose( mesh->CVVAABB );

	mesh_clear( mesh );

	free( mesh );
}

#endif


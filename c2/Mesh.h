#ifndef __MESH_H
#define __MESH_H

#define MATERIAL_KEY			0x01
#define TEXTURE_KEY				0x02
#define FOG_KEY					0x04
#define LIGHT_KEY				0x08
#define RENDERMODE_KEY			0x10
#define ALPHA_KEY				0x20

#define ADDRESS_MODE_NONE		0x01
#define ADDRESS_MODE_WRAP		0x02
#define ADDRESS_MODE_BORDER		0x04
#define ADDRESS_MODE_CLAMP		0x08
#define ADDRESS_MODE_MIRROR		0x10

#include "Triangle.h"
#include "Vertex.h"
#include "Vector.h"
#include "AABB.h"
#include "Material.h"
#include "Texture.h"
#include "RenderList.h"

typedef struct Mesh
{
	DWORD nFaces, nVertices, nRenderList, nClippList, nCullList;

	int type, octree_depth, octreeState, v_dirty, lightEnable, fogEnable, useMipmap, terrainTrace, addressMode, texTransformDirty;

	int widthSegment, heightSegment;
	
	float mip_dist;

	Triangle ** faces;

	Vertex ** vertices;

	RenderList * renderList, * clippedList;

	struct TexTransform * texTransform;

	struct Octree * octree;

}Mesh;

#include "Octree.h"

void mesh_build( Mesh * m, int nVertices, int nFaces )
{
	int i;
	Triangle * faces;
	Vertex * vertices;
	Vector   * uvp, * s_uvp;
	Vector3D * vp, * s_vp;

#ifdef RGB565
	Color565 * ap;
#else
	Color888 * ap;
#endif

	if( ( faces		  = ( Triangle  * )malloc( sizeof( Triangle   ) * nFaces                     ) ) == NULL
	 || ( vertices	  = ( Vertex    * )malloc( sizeof( Vertex     ) * nVertices                  ) ) == NULL
	 || ( m->faces	  = ( Triangle ** )malloc( sizeof( Triangle * ) * nFaces                     ) ) == NULL
	 || ( m->vertices = ( Vertex   ** )malloc( sizeof( Vertex   * ) * nVertices                  ) ) == NULL
	 || ( vp	      = ( Vector3D  * )malloc( sizeof( Vector3D   ) * ( nVertices * 5 + nFaces ) ) ) == NULL
	 || ( uvp	      = ( Vector    * )malloc( sizeof( Vector     ) * nFaces    * 3              ) ) == NULL
#ifdef RGB565
	 || ( ap	      = ( Color565 *  )malloc( sizeof( Color565   ) * nVertices                  ) ) == NULL
#else
	 || ( ap	      = ( Color888 *  )malloc( sizeof( Color888   ) * nVertices                  ) ) == NULL
#endif
	 ) 
	{
		exit( TRUE );
	}

	s_uvp = uvp;
	s_vp = vp;

	for ( i = 0; i < nFaces; i ++ )
	{
		faces[i].normal = vp ++;

		faces[i].c_uv[0]  = uvp ++;
		faces[i].c_uv[1]  = uvp ++;
		faces[i].c_uv[2]  = uvp ++;

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

	uvp = s_uvp;
	vp = s_vp;
	
	//构造渲染列表和裁剪列表
	m->renderList = initializeRenderList( nFaces + 2 );
	m->clippedList = initializeRenderList( nFaces + 2 );

	m->octree = newOctree();

	m->nRenderList = m->nCullList = m->nClippList = 0;
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

	m->octreeState			= OCTREE_NOT_READY;
	m->v_dirty				= FALSE;
	m->lightEnable			= FALSE;
	m->fogEnable			= FALSE;
	m->useMipmap			= FALSE;
	m->mip_dist				= 0;
	m->terrainTrace			= FALSE;
	m->type					= 0;
	m->octree_depth			= 0;
	m->addressMode			= ADDRESS_MODE_NONE;
	m->texTransform			= newTexTransform();
	m->texTransformDirty	= FALSE;

	return m;
}

Vertex * mesh_push_vertex( Mesh * m, float x, float y, float z )
{
	Vertex * v = m->vertices[m->nVertices];

	vector3D_set( v->position, x, y, z, 1.0f );
#ifdef RGB565
	color565_identity( v->color );
#else
	color888_identity( v->color );// = newColor888( 255, 255, 255, 255 );
#endif

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

	vector_copy( p->c_uv[0], uva );
	vector_copy( p->c_uv[1], uvb );
	vector_copy( p->c_uv[2], uvc );

	vector3D_set ( p->normal, 0.0f, 0.0f, 0.0f, 1.0f );

	p->texture = texture;
	p->material = material;

	p->miplevel = 0;
	p->render_mode = render_mode;
	p->uvTransformed = FALSE;	
	p->fogEnable = FALSE;
	p->lightEnable = FALSE;
	p->depth = 0;

	vertex_addContectedFaces( p, va );
	vertex_addContectedFaces( p, vb );
	vertex_addContectedFaces( p, vc );

	m->nFaces ++;

	m->octree->data->nFaces = m->nFaces;
	m->octree->data->faces	= m->faces;

	return p;
}

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
		aabb_add( m->octree->data->aabb, vert->position );

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

void mesh_setAlpha( Mesh * m, int alpha )
{
	DWORD i = 0;

	for( ; i < m->nFaces; i ++ )
	{
		m->faces[i]->vertex[0]->color->alpha = m->faces[i]->vertex[1]->color->alpha = m->faces[i]->vertex[2]->color->alpha = alpha;
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
	free( memset( mesh->vertices[0], 0, sizeof( Vertex ) * mesh->nVertices ) );
	free( memset( mesh->faces[0], 0, sizeof( Triangle ) * mesh->nFaces ) );

	if ( mesh->renderList )
	{		
		memset( mesh->renderList, 0, sizeof( RenderList ) );
		
		free( mesh->renderList );

		mesh->nRenderList = mesh->nCullList = mesh->nClippList = 0;
	}
	
	memset( mesh->vertices, 0, sizeof( Vertex * ) );
	memset( mesh->faces, 0, sizeof( Triangle * ) );

	free( mesh->vertices );
	free( mesh->faces );

	destoryOctree( mesh->octree );

	memset( mesh, 0, sizeof( Mesh ) );
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

float mesh_minY( Mesh * mesh )
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

float mesh_maxY( Mesh * mesh )
{
	DWORD i;
	float max = mesh -> vertices[0] -> position -> y;

	for( i = 1; i < mesh -> nVertices; i ++ )
	{
		if( mesh -> vertices[i] -> position -> y > max )
		{
			max = mesh -> vertices[i] -> position -> y;
		}
	}

	return max;
}

INLINE int mesh_intersectMesh( Mesh * m1, Mesh * m2 )
{
	return aabb_intersectAABBs( m1->octree->data->worldAABB, m2->octree->data->worldAABB, NULL );
}

void mesh_dispose( Mesh * mesh )
{
	mesh_clear( mesh );

	free( mesh );
}

void mesh_setTexOffset( Mesh * m, float x, float y )
{
	m->texTransform->offset->x = x;
	m->texTransform->offset->y = y;
}

Vector * mesh_getTexOffset( Mesh * m, Vector * output )
{
	vector_copy( output, m->texTransform->offset );

	return output;
}

void mesh_setTexScale( Mesh * m, float x, float y )
{
	m->texTransform->scale->x = 1.0f / x;
	m->texTransform->scale->y = 1.0f / y;
}

Vector * mesh_getTexScale( Mesh * m, Vector * output )
{
	vector_copy( output, m->texTransform->scale );

	return output;
}

void mesh_setTexRotation( Mesh * m, float angle )
{
	m->texTransform->rotation = angle;
}

float mesh_getTexRotation( Mesh * m )
{
	return m->texTransform->rotation;
}

#endif


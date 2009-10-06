#ifndef __MESH_H
#define __MESH_H

#include "Triangle.h"
#include "Vertex.h"
#include "Vector.h"
#include "AABB.h"
#include "Material.h"
#include "Texture.h"

#define NO_TEXTURE			0
#define TEX_NOT_READY		1
#define TEX_READY			2

#define OCTREE_NOT_READY	0
#define OCTREE_READY		1

typedef struct Mesh
{
	int nFaces, nVertices, v_dirty, textureState, lightEnable, octreeState;

	int type, octree_depth;

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

	struct Animation * animation;

}Mesh;

#include "Octree.h"

void mesh_build( Mesh * m, int nVertices, int nFaces )
{
	int i = 0;
	Triangle * faces;
	Vertex * vertices;
	
	if( ( faces		= ( Triangle * )malloc( sizeof( Triangle ) * nFaces ) ) == NULL ) exit( TRUE );
	if( ( vertices	= ( Vertex * )malloc( sizeof( Vertex ) * nVertices ) ) == NULL ) exit( TRUE );

	if( ( m->faces		= ( Triangle ** )malloc( sizeof( Triangle * ) * nFaces ) ) == NULL ) exit( TRUE );
	if( ( m->vertices	= ( Vertex ** )malloc( sizeof( Vertex * ) * nVertices ) ) == NULL ) exit( TRUE );

	for ( ; i < nFaces; i ++ )
	{
		m->faces[i] = & faces[i];
	}

	for ( i = 0; i < nVertices; i ++ )
	{
		m->vertices[i] = & vertices[i];
	}
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
	m->textureState		= NO_TEXTURE;
	m->lightEnable		= FALSE;

	m->animation        = NULL;

	m->type				= 0;
	m->octree_depth		= 0;
	m->halfHeight		= 0;

	return m;
}

Vertex * mesh_push_vertex( Mesh * m, float x, float y, float z )
{
	Vertex * v = m->vertices[m->nVertices];

	v->position = newVector3D(x, y, z, 1.0f);
	v->w_pos = newVector3D(x, y, z, 1.0f);
	v->s_pos = newVector3D(x, y, z, 1.0f);

	v->color = newARGBColor( 255, 255, 255, 255 );

	v->normal = newVector3D( 0.0f, 0.0f, 0.0f, 1.0f );

	v->contectedFaces = NULL;
	v->nContectedFaces = 0;

	v->transformed = FALSE;
	v->fix_inv_z = 0;

	//aabb_add_3D( (( OctreeData * )( m->octree->data ))->aabb, v->position );

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

	p->uv[0] = vector_clone( uva );
	p->uv[1] = vector_clone( uvb );
	p->uv[2] = vector_clone( uvc );

	p->normal = newVector3D( 0.0f, 0.0f, 0.0f, 1.0f );

	if ( texture ) m->textureState = TEX_NOT_READY;

	p->texture = texture;
	p->material = material;

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
	int i = 0, j = 0;
	Texture * t;
	Triangle * face;
	Vertex * v0, * v1, * v2;

	if ( ! m->v_dirty && m->textureState == NO_TEXTURE ) return;

	if ( ! m->faces || m->nFaces == 0) exit( TRUE );

	for( ; i < m->nFaces; i ++ )
	{
		face = m->faces[i];

		if ( m->v_dirty )
		{
			v0 = face->vertex[0];
			v1 = face->vertex[1];
			v2 = face->vertex[2];

			face->normal->x = 0.0f;
			face->normal->y = 0.0f;
			face->normal->z = 0.0f;
			face->normal->w = 1.0f;

			triangle_normal( face->normal, v0, v1, v2 );
		}

		if ( m->textureState == TEX_NOT_READY && face->texture )
		{
			t = face->texture;

			if ( ! face->uvTransformed && t->pRGBABuffer && t->width != 0 && t->height != 0 )
			{
				face->uv[0]->u *= t->width - 1;
				face->uv[0]->v *= t->height - 1;

				face->uv[1]->u *= t->width - 1;
				face->uv[1]->v *= t->height - 1;

				face->uv[2]->u *= t->width - 1;
				face->uv[2]->v *= t->height - 1;

				face->uvTransformed = TRUE;
			}

			if ( face->uvTransformed ) j ++;
		}
	}

	if ( i == j )
	{
		m->textureState = TEX_READY;
	}
}

void mesh_updateVertices( Mesh * m )
{
	int i = 0;
	float len, oneOverMag;
	ContectedFaces * cf;
	Vertex * vert;

	if ( ! m->v_dirty ) return;

	if ( ! m->vertices || m->nVertices == 0) exit( TRUE );

	aabb_empty( m->octree->data->aabb );

	//为提高效率，这么不使用任何内联函数，以降低函数调用开销
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
	int i = 0;

	for( ; i < m->nFaces; i ++ )
	{
		m->faces[i]->render_mode = renderMode;
	}
}

void mesh_setMaterial( Mesh * m, Material * mat )
{
	int i = 0;

	for( ; i < m->nFaces; i ++ )
	{
		m->faces[i]->material = mat;
	}
}

void mesh_setTexture( Mesh * m, Texture * t )
{
	int i = 0;

	for( ; i < m->nFaces; i ++ )
	{
		m->faces[i]->texture = t;
	}

	m->textureState = TEX_NOT_READY;
}

void mesh_updateMesh( Mesh * mesh )
{
	//重新计算法向量
	mesh_updateFaces( mesh );
	mesh_updateVertices( mesh );

	if ( mesh->octreeState == OCTREE_NOT_READY )
	{
		buildOctree( mesh->octree, mesh->octree_depth );

		mesh->octreeState = OCTREE_READY;
	}
}

INLINE AABB * mesh_transformNewAABB( AABB * output, AABB * aabb, Matrix3D * m )
{
	float invw;

	Vector3D T_L_F_V;
	Vector3D T_R_F_V;
	Vector3D T_L_B_V;
	Vector3D T_R_B_V;
	Vector3D B_L_F_V;
	Vector3D B_R_F_V;
	Vector3D B_L_B_V;
	Vector3D B_R_B_V;

	Vector3D * min = aabb->min, * max = aabb->max;

	T_L_F_V.x = min->x;	T_L_F_V.y = min->y;	T_L_F_V.z = min->z;	T_L_F_V.w = 1.0f;
	T_R_F_V.x = max->x;	T_R_F_V.y = min->y;	T_R_F_V.z = min->z;	T_R_F_V.w = 1.0f;
	T_L_B_V.x = min->x;	T_L_B_V.y = min->y;	T_L_B_V.z = max->z;	T_L_B_V.w = 1.0f;
	T_R_B_V.x = max->x;	T_R_B_V.y = min->y;	T_R_B_V.z = max->z;	T_R_B_V.w = 1.0f;
	B_L_F_V.x = min->x;	B_L_F_V.y = max->y;	B_L_F_V.z = min->z;	B_L_F_V.w = 1.0f;
	B_R_F_V.x = max->x;	B_R_F_V.y = max->y;	B_R_F_V.z = min->z;	B_R_F_V.w = 1.0f;
	B_L_B_V.x = min->x;	B_L_B_V.y = max->y;	B_L_B_V.z = max->z;	B_L_B_V.w = 1.0f;
	B_R_B_V.x = max->x;	B_R_B_V.y = max->y;	B_R_B_V.z = max->z;	B_R_B_V.w = 1.0f;

	matrix3D_transformVector_self( m, & T_L_F_V );
	invw = 1.0f / T_L_F_V.w;
	T_L_F_V.x *= invw;
	T_L_F_V.y *= invw;

	matrix3D_transformVector_self( m, & T_R_F_V );
	invw = 1.0f / T_R_F_V.w;
	T_R_F_V.x *= invw;
	T_R_F_V.y *= invw;

	matrix3D_transformVector_self( m, & T_L_B_V );
	invw = 1.0f / T_L_B_V.w;
	T_L_B_V.x *= invw;
	T_L_B_V.y *= invw;

	matrix3D_transformVector_self( m, & T_R_B_V );
	invw = 1.0f / T_R_B_V.w;
	T_R_B_V.x *= invw;
	T_R_B_V.y *= invw;

	matrix3D_transformVector_self( m, & B_L_F_V );
	invw = 1.0f / B_L_F_V.w;
	B_L_F_V.x *= invw;
	B_L_F_V.y *= invw;

	matrix3D_transformVector_self( m, & B_R_F_V );
	invw = 1.0f / B_R_F_V.w;
	B_R_F_V.x *= invw;
	B_R_F_V.y *= invw;

	matrix3D_transformVector_self( m, & B_L_B_V );
	invw = 1.0f / B_L_B_V.w;
	B_L_B_V.x *= invw;
	B_L_B_V.y *= invw;

	matrix3D_transformVector_self( m, & B_R_B_V );
	invw = 1.0f / B_R_B_V.w;
	B_R_B_V.x *= invw;
	B_R_B_V.y *= invw;

	aabb_empty( output );
	aabb_add_4D( output, & T_L_F_V );
	aabb_add_4D( output, & T_R_F_V );
	aabb_add_4D( output, & T_L_B_V );
	aabb_add_4D( output, & T_R_B_V );
	aabb_add_4D( output, & B_L_F_V );
	aabb_add_4D( output, & B_R_F_V );
	aabb_add_4D( output, & B_L_B_V );
	aabb_add_4D( output, & B_R_B_V );

	return output;
}

void mesh_clear( Mesh * mesh )
{
	int i = 0;

	for ( ; i < mesh->nVertices; i ++ )
	{
		vertex_dispose( mesh->vertices[i] );
	}
	for ( i = 0; i < mesh->nFaces; i ++ )
	{
		triangle_dispose( mesh->faces[i] );
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
	unsigned int i;
	float min = mesh -> vertices[0] -> w_pos -> y;

	for( i = 1; i < mesh -> nVertices; i ++ )
	{
		if( mesh -> vertices[i] -> w_pos -> y < min )
		{
			min = mesh -> vertices[i] -> w_pos -> y;
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

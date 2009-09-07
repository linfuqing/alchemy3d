#ifndef __MESH_H
#define __MESH_H

#include "Triangle.h"
#include "Vertex.h"
#include "Vector.h"
#include "AABB.h"
#include "Material.h"
#include "Texture.h"

typedef struct Mesh
{
	int nFaces, nVertices, v_dirty, f_dirty;

	Triangle  * faces;

	Vertex * vertices;

	AABB * aabb, * worldAABB, * CVVAABB;

#ifdef __AS3__
	float * meshBuffer;
#endif

}Mesh;

Mesh * newMesh( int nVertices, int nFaces, float * meshBuffer )
{
	Mesh * m;

	if( ( m				= ( Mesh * )calloc( 1, sizeof( Mesh ) ) ) == NULL) exit( TRUE );
	if( ( m->faces		= ( Triangle * )calloc( nFaces, sizeof( Triangle ) ) ) == NULL ) exit( TRUE );
	if( ( m->vertices	= ( Vertex * )calloc( nVertices, sizeof( Vertex ) ) ) == NULL ) exit( TRUE );
	
	m->aabb				= newAABB();
	m->worldAABB		= newAABB();
	m->CVVAABB			= newAABB();

	m->nFaces			= 0;
	m->nVertices		= 0;
	m->v_dirty			= FALSE;
	m->f_dirty			= FALSE;

#ifdef __AS3__
	m->meshBuffer = meshBuffer;
#endif

	return m;
}

Vertex * mesh_push_vertex( Mesh * m, float x, float y, float z )
{
	Vertex * v = & m->vertices[m->nVertices];

	v->position = newVector3D(x, y, z, 1.0f);
	v->w_pos = newVector3D(x, y, z, 1.0f);
	v->s_pos = newVector3D(x, y, z, 1.0f);

	v->uv = newVector( 0.0f, 0.0f );

	v->color = newARGBColor( 255, 255, 255, 255 );

	v->normal = newVector3D( 0.0f, 0.0f, 0.0f, 0.0f );
	v->normalLength = newVector3D( 0.0f, 0.0f, 0.0f, 0.0f );

	v->contectedFaces = NULL;
	v->nContectedFaces = 0;

	v->transformed = FALSE;

	m->nVertices ++;

	return v;
}

Triangle * mesh_push_triangle( Mesh * m, Vertex * va, Vertex * vb, Vertex * vc, Vector * uva, Vector * uvb, Vector * uvc, Material * material, Texture * texture )
{
	Triangle * p = & m->faces[m->nFaces];

	p->vertex[0] = va;
	p->vertex[1] = vb;
	p->vertex[2] = vc;

	va->uv = uva;
	vb->uv = uvb;
	vc->uv = uvc;

	p->texture = texture;
	p->material = material;

	vertex_addContectedFaces( p, va );
	vertex_addContectedFaces( p, vb );
	vertex_addContectedFaces( p, vc );

	m->nFaces ++;

	return p;
}

void computeFaceNormal( Mesh * m )
{
	int i = 0;

	Triangle * face;
	Vertex * v0, * v1, * v2;

	if ( ! m->faces || m->nFaces == 0) exit( TRUE );

	for( ; i < m->nFaces; i ++ )
	{
		face = & m->faces[i];

		v0 = face->vertex[0];
		v1 = face->vertex[1];
		v2 = face->vertex[2];

		face->center = newVector3D( 0.0f, 0.0f, 0.0f, 1.0f );
		face->center->x = ( v0->position->x + v1->position->x + v2->position->x ) * 0.33333333f;
		face->center->y = ( v0->position->y + v1->position->y + v2->position->y ) * 0.33333333f;
		face->center->z = ( v0->position->z + v1->position->z + v2->position->z ) * 0.33333333f;

		face->normal = newVector3D( 0.0f, 0.0f, 0.0f, 1.0f );

		triangle_normal( face->normal, face->vertex[0], face->vertex[1], face->vertex[2] );
	}
}

void computeVerticesNormal( Mesh * m )
{
	ContectedFaces * cf;
	Vector3D * nv, posN;
	float d;
	int i = 0;

	if ( ! m->vertices || m->nVertices == 0) exit( TRUE );

	for( ; i < m->nVertices; i ++ )
	{
		//AABB
		aabb_add(m->aabb, m->vertices[i].position);

		if ( m->vertices[i].nContectedFaces == 0 )
			continue;

		nv = newVector3D( 0.0f, 0.0f, 0.0f, 1.0f );

		//计算顶点法向量
		cf = m->vertices[i].contectedFaces;

		while ( NULL != cf )
		{
			vector3D_add( nv, nv, cf->face->normal );

			cf = cf->next;
		}

		vector3D_normalize( nv );

		vector3D_copy( m->vertices[i].normal, nv );

		vector3D_copy( & posN, m->vertices[i].position );

		d = 1.0f / m->vertices[i].nContectedFaces;

		vector3D_scaleBy( & posN, d );

		vector3D_normalize( & posN );

		vector3D_add( nv, nv, & posN );

		vector3D_normalize( nv );

		m->vertices[i].normalLength = nv;
	}
}

void mesh_setRenderMode(  Mesh * m, DWORD renderMode )
{
	int i = 0;

	for( ; i < m->nFaces; i ++ )
	{
		m->faces[i].render_mode = renderMode;
	}
}

void mesh_setMaterial( Mesh * m, Material * mat )
{
	int i = 0;

	for( ; i < m->nFaces; i ++ )
	{
		m->faces[i].material = mat;
	}
}

void mesh_correctUV( Mesh * m, Texture * t )
{
	int i = 0;

	for( i = 0; i < m->nVertices; i ++ )
	{
		//纹理坐范围在[width-1, height-1]
		m->vertices[i].uv->u *= t->width - 1;
		m->vertices[i].uv->v *= t->height - 1;
	}
}

void mesh_setTexture( Mesh * m, Texture * t )
{
	int i = 0;

	for( ; i < m->nFaces; i ++ )
	{
		m->faces[i].texture = t;
	}

	if ( t->pRGBABuffer )
	{
		mesh_correctUV( m, t );
	}
}

INLINE AABB * mesh_transformNewAABB( AABB * output, Matrix3D * m, AABB * aabb )
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

	T_L_F_V.x = min->x;	T_L_F_V.y = min->y;	T_L_F_V.z = min->z;	T_L_F_V.w = 1;
	T_R_F_V.x = max->x;	T_R_F_V.y = min->y;	T_R_F_V.z = min->z;	T_R_F_V.w = 1;
	T_L_B_V.x = min->x;	T_L_B_V.y = min->y;	T_L_B_V.z = max->z;	T_L_B_V.w = 1;
	T_R_B_V.x = max->x;	T_R_B_V.y = min->y;	T_R_B_V.z = max->z;	T_R_B_V.w = 1;
	B_L_F_V.x = min->x;	B_L_F_V.y = max->y;	B_L_F_V.z = min->z;	B_L_F_V.w = 1;
	B_R_F_V.x = max->x;	B_R_F_V.y = max->y;	B_R_F_V.z = min->z;	B_R_F_V.w = 1;
	B_L_B_V.x = min->x;	B_L_B_V.y = max->y;	B_L_B_V.z = max->z;	B_L_B_V.w = 1;
	B_R_B_V.x = max->x;	B_R_B_V.y = max->y;	B_R_B_V.z = max->z;	B_R_B_V.w = 1;

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

void mesh_dispose( Mesh * mesh )
{
	int i = 0;

	aabb_dispose( mesh->aabb );
	aabb_dispose( mesh->worldAABB );
	aabb_dispose( mesh->CVVAABB );

	for ( ; i < mesh->nVertices; i ++ )
	{
		vertex_dispose( & mesh->vertices[i] );
	}
	for ( i = 0; i < mesh->nFaces; i ++ )
	{
		triangle_dispose( & mesh->faces[i] );
	}

	free( mesh->vertices );
	free( mesh->faces );
	free( mesh );
}

#endif
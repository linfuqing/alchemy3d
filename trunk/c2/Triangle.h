#ifndef __triangle_H
#define __triangle_H

#include "Vertex.h"
#include "Vector.h"
#include "Texture.h"

//R
typedef struct Triangle
{
	Texture * texture;

	Vector3D * normal, * center;

	Vertex * vertex[3];
}Triangle;

Vector3D * triangle_normal( Vector3D * normal, Vertex * v0, Vertex * v1, Vertex * v2 )
{	
	Vector3D * a, * b, * c, ca, bc;
	
	a = v0->position;
	b = v1->position;
	c = v2->position;

	vector3D_crossProduct(normal, vector3D_subtract( &ca, c, a ), vector3D_subtract( &bc, b, c ) );

	vector3D_normalize(normal);

	return normal;
}

Triangle * newTriangle( Vertex * va, Vertex * vb, Vertex * vc, Vector * uva, Vector * uvb, Vector * uvc, Texture * texture )
{
	Triangle * p;

	if( ( p = ( Triangle * )malloc( sizeof( Triangle ) ) ) == NULL ) exit( TRUE );

	p->vertex[0] = va;
	p->vertex[1] = vb;
	p->vertex[2] = vc;

	va->uv = uva;
	vb->uv = uvb;
	vc->uv = uvc;

	vertex_addContectedFaces( p, va );
	vertex_addContectedFaces( p, vb );
	vertex_addContectedFaces( p, vc );

	p->texture = texture;

	return p;
}

INLINE void triangle_copy( Triangle * dest, Triangle * src )
{
	vertex_copy( dest->vertex[0], src->vertex[0] );
	vertex_copy( dest->vertex[1], src->vertex[1] );
	vertex_copy( dest->vertex[2], src->vertex[2] );

	vector3D_copy( dest->center, src->center );
	vector3D_copy( dest->normal, src->normal );

	dest->texture = src->texture;
}

INLINE Triangle * triangle_clone( Triangle * src )
{
	Triangle * dest;

	if( ( dest = ( Triangle * )malloc( sizeof( Triangle ) ) ) == NULL ) exit( TRUE );

	dest->vertex[0] = vertex_clone( src->vertex[0] );
	dest->vertex[1] = vertex_clone( src->vertex[1] );
	dest->vertex[2] = vertex_clone( src->vertex[2] );

	dest->vertex[0]->uv = vector_clone( src->vertex[0]->uv );
	dest->vertex[1]->uv = vector_clone( src->vertex[1]->uv );
	dest->vertex[2]->uv = vector_clone( src->vertex[2]->uv );

	vertex_addContectedFaces( dest, dest->vertex[0] );
	vertex_addContectedFaces( dest, dest->vertex[1] );
	vertex_addContectedFaces( dest, dest->vertex[2] );

	dest->texture = src->texture;

	dest->normal = vector3D_clone( src->normal );
	dest->center = vector3D_clone( src->center );

	return dest;
}

INLINE void triangle_dispose( Triangle * p)
{
	free( p->center );
	free( p->normal );
	
	p->texture = NULL;
	p->center = NULL;
	p->normal = NULL;
	p->vertex[0] = NULL;
	p->vertex[1] = NULL;
	p->vertex[2] = NULL;
}

#endif
#ifndef __triangle_H
#define __triangle_H

#include "Vertex.h"
#include "Vector.h"
#include "Texture.h"
#include "Material.h"

//R
typedef struct Triangle
{
	DWORD render_mode;

	int uvTransformed;

	Texture * texture;

	Material * material;

	Vector3D * normal;

	Vertex * vertex[3];

	Vector * uv[3];
}Triangle;

INLINE Vector3D * triangle_normal( Vector3D * normal, Vertex * v0, Vertex * v1, Vertex * v2 )
{	
	Vector3D u, v;

	vector3D_crossProduct(normal, vector3D_subtract( &u, v1->position, v0->position ), vector3D_subtract( &v, v2->position, v0->position ) );

	vector3D_normalize(normal);

	return normal;
}

Triangle * newTriangle( Vertex * va, Vertex * vb, Vertex * vc, Vector * uva, Vector * uvb, Vector * uvc, Material * material, Texture * texture )
{
	Triangle * p;

	if( ( p = ( Triangle * )malloc( sizeof( Triangle ) ) ) == NULL ) exit( TRUE );

	p->vertex[0] = va;
	p->vertex[1] = vb;
	p->vertex[2] = vc;

	p->uv[0] = vector_clone( uva );
	p->uv[1] = vector_clone( uvb );
	p->uv[2] = vector_clone( uvc );

	vertex_addContectedFaces( p, va );
	vertex_addContectedFaces( p, vb );
	vertex_addContectedFaces( p, vc );

	p->texture = texture;
	p->material = material;

	p->render_mode = RENDER_NONE;
	p->uvTransformed = FALSE;

	return p;
}

INLINE void triangle_copy( Triangle * dest, Triangle * src )
{
	vertex_copy( dest->vertex[0], src->vertex[0] );
	vertex_copy( dest->vertex[1], src->vertex[1] );
	vertex_copy( dest->vertex[2], src->vertex[2] );

	vector_copy( dest->uv[0], src->uv[0] );
	vector_copy( dest->uv[1], src->uv[1] );
	vector_copy( dest->uv[2], src->uv[2] );

	vector3D_copy( dest->normal, src->normal );

	dest->texture = src->texture;
	dest->material = src->material;
	dest->render_mode = src->render_mode;
}

INLINE Triangle * triangle_clone( Triangle * src )
{
	Triangle * dest;

	if( ( dest = ( Triangle * )malloc( sizeof( Triangle ) ) ) == NULL ) exit( TRUE );

	dest->vertex[0] = vertex_clone( src->vertex[0] );
	dest->vertex[1] = vertex_clone( src->vertex[1] );
	dest->vertex[2] = vertex_clone( src->vertex[2] );

	dest->uv[0] = vector_clone( src->uv[0] );
	dest->uv[1] = vector_clone( src->uv[1] );
	dest->uv[2] = vector_clone( src->uv[2] );

	vertex_addContectedFaces( dest, dest->vertex[0] );
	vertex_addContectedFaces( dest, dest->vertex[1] );
	vertex_addContectedFaces( dest, dest->vertex[2] );

	dest->texture = src->texture;
	dest->material = src->material;
	dest->render_mode = src->render_mode;

	dest->normal = vector3D_clone( src->normal );

	return dest;
}

INLINE void triangle_dispose( Triangle * p)
{
	free( p->normal );
	
	p->texture = NULL;
	p->normal = NULL;
	p->vertex[0] = NULL;
	p->vertex[1] = NULL;
	p->vertex[2] = NULL;
	p->uv[0] = NULL;
	p->uv[1] = NULL;
	p->uv[2] = NULL;
}

#endif
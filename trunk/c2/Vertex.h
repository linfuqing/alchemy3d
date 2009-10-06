#ifndef __VERTEX_H
#define __VERTEX_H

#include <malloc.h>

#include "Vector.h"
#include "Vector3D.h"
#include "Triangle.h"
#include "ARGBColor.h"

typedef struct ContectedFaces
{
	struct Triangle * face;

	struct ContectedFaces * next;

}ContectedFaces;

//RW
typedef struct Vertex
{
	int nContectedFaces, transformed;

	int fix_inv_z;

	Vector3D * position, * w_pos, * s_pos, * normal;

	ARGBColor * color;

	struct ContectedFaces * contectedFaces;

}Vertex;

Vertex * newVertex( float x, float y, float z )
{
	Vertex * v;

	if( ( v = ( Vertex * )malloc( sizeof( Vertex ) ) ) == NULL ) exit( TRUE );

	v->position = newVector3D(x, y, z, 1.0f);
	v->w_pos = newVector3D(x, y, z, 1.0f);
	v->s_pos = newVector3D(x, y, z, 1.0f);
	v->normal = newVector3D( 0.0f, 0.0f, 0.0f, 0.0f );
	v->color = newARGBColor( 255, 255, 255, 255 );
	v->contectedFaces = NULL;
	v->nContectedFaces = 0;
	v->transformed = FALSE;
	v->fix_inv_z = 0;

	return v;
}

INLINE void vertex_copy( Vertex * dest, Vertex * src )
{
	vector3D_copy( dest->position, src->position );
	vector3D_copy( dest->w_pos, src->w_pos );
	vector3D_copy( dest->s_pos, src->s_pos );

	vector3D_copy( dest->normal, src->normal );
	argbColor_copy( dest->color, src->color );
	dest->fix_inv_z = src->fix_inv_z;
	dest->contectedFaces = NULL;
	dest->nContectedFaces = 0;
	dest->transformed = src->transformed;
}

INLINE Vertex * vertex_clone( Vertex * src )
{
	Vertex * dest;

	if( ( dest = ( Vertex * )malloc( sizeof( Vertex ) ) ) == NULL ) exit( TRUE );

	dest->position		= vector3D_clone( src->position );
	dest->w_pos	= vector3D_clone( src->w_pos );
	dest->s_pos	= vector3D_clone( src->s_pos );

	dest->normal		= vector3D_clone( src->normal );
	dest->color			= argbColor_clone( src->color );
	dest->fix_inv_z = src->fix_inv_z;
	dest->contectedFaces = NULL;
	dest->nContectedFaces = 0;
	dest->transformed = src->transformed;

	return dest;
}

INLINE void vertex_dispose( Vertex * v )
{
	ContectedFaces * cf, * cf2;

	cf = v->contectedFaces;

	while ( NULL != cf)
	{
		cf2 = cf;

		cf->face = NULL;

		cf = cf->next;

		free( cf2 );

		cf2 =NULL;
	}

	vector3D_dispose( v->normal );
	vector3D_dispose( v->position );
	vector3D_dispose( v->w_pos );
	vector3D_dispose( v->s_pos );
	argbColor_dispose( v->color );

	v->contectedFaces = NULL;
	v->normal = NULL;
	v->position = NULL;
	v->w_pos = NULL;
	v->s_pos = NULL;
	v->color = NULL;
}

void vertex_addContectedFaces( struct Triangle * p, Vertex * v )
{
	ContectedFaces * cf;

	if( ( cf = ( ContectedFaces * )malloc( sizeof( ContectedFaces ) ) ) == NULL )
	{
		exit( TRUE );
	}

	cf->face = p;
	cf->next = v->contectedFaces;

	v->contectedFaces = cf;

	v->nContectedFaces ++;
}

# endif
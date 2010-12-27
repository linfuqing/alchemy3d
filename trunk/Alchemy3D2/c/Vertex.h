#ifndef __VERTEX_H
#define __VERTEX_H

#include <malloc.h>

#include "Vector.h"
#include "Vector3D.h"
#include "Triangle.h"
#include "Color565.h"
#include "Color888.h"

typedef struct ContectedFaces
{
	struct Triangle * face;

	struct ContectedFaces * next;

}ContectedFaces;

//RW
typedef struct Vertex
{
	int nContectedFaces, transformed, index;

	int fix_inv_z;

	Vector3D * position, * w_pos, * v_pos, * s_pos, * normal;

#ifdef RGB565
	Color565 * color;
#else
	Color888 * color;
#endif

	struct ContectedFaces * contectedFaces;

}Vertex;

Vertex * newVertex( float x, float y, float z )
{
	Vertex * v;

	if( ( v = ( Vertex * )malloc( sizeof( Vertex ) ) ) == NULL ) exit( TRUE );

	v->position = newVector3D(x, y, z, 1.0f);
	v->w_pos = newVector3D(x, y, z, 1.0f);
	v->v_pos = newVector3D(x, y, z, 1.0f);
	v->s_pos = newVector3D(x, y, z, 1.0f);
	v->normal = newVector3D( 0.0f, 0.0f, 0.0f, 0.0f );
#ifdef RGB565
	v->color = newColor565( 31, 63, 31 );
#else
	v->color = newColor888( 255, 255, 255, 255 );
#endif
	v->contectedFaces = NULL;
	v->nContectedFaces = 0;
	v->transformed = FALSE;
	v->fix_inv_z = 0;
	v->index = 0;

	return v;
}

void vertex_destroy(Vertex **vertex)
{
	ContectedFaces *face = (*vertex)->contectedFaces, *data;

	while(face)
	{
		data = face;
		face = face->next;
		free(data);
	}

	vector3D_destroy(&(*vertex)->normal);
	vector3D_destroy(&(*vertex)->position);
	vector3D_destroy(&(*vertex)->w_pos);
	vector3D_destroy(&(*vertex)->v_pos);
	vector3D_destroy(&(*vertex)->s_pos);

	color888_destroy(&(*vertex)->color);

	free(*vertex);

	vertex = NULL;
}

INLINE void vertex_copy( Vertex * dest, Vertex * src )
{
	vector3D_copy( dest->position, src->position );
	vector3D_copy( dest->w_pos, src->w_pos );
	vector3D_copy( dest->v_pos, src->v_pos );
	vector3D_copy( dest->s_pos, src->s_pos );

	vector3D_copy( dest->normal, src->normal );
#ifdef RGB565
	color565_copy( dest->color, src->color );
#else
	color888_copy( dest->color, src->color );
#endif
	dest->fix_inv_z = src->fix_inv_z;
	//dest->contectedFaces = NULL;
	//dest->nContectedFaces = 0;
	dest->transformed = src->transformed;
}

INLINE Vertex * vertex_clone( Vertex * src )
{
	Vertex * dest;

	if( ( dest = ( Vertex * )malloc( sizeof( Vertex ) ) ) == NULL ) exit( TRUE );

	dest->position		= vector3D_clone( src->position );
	dest->w_pos	= vector3D_clone( src->w_pos );
	dest->v_pos	= vector3D_clone( src->v_pos );
	dest->s_pos	= vector3D_clone( src->s_pos );

	dest->normal		= vector3D_clone( src->normal );
#ifdef RGB565
	dest->color			= color565_clone( src->color );
#else
	dest->color			= color888_clone( src->color );
#endif
	dest->fix_inv_z = src->fix_inv_z;
	dest->contectedFaces = NULL;
	dest->nContectedFaces = 0;
	dest->transformed = src->transformed;

	return dest;
}

INLINE void vertex_dispose( Vertex * v )
{
	vector3D_dispose( v->normal );
	vector3D_dispose( v->position );
	vector3D_dispose( v->w_pos );
	vector3D_dispose( v->v_pos );
	vector3D_dispose( v->s_pos );
#ifdef RGB565
	color565_dispose( v->color );
#else
	color888_dispose( v->color );
#endif

	v->contectedFaces = NULL;
	v->normal = NULL;
	v->position = NULL;
	v->w_pos = NULL;
	v->v_pos = NULL;
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
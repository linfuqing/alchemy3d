#ifndef __VERTEX_H
#define __VERTEX_H

#include <malloc.h>

#include "Vector.h"
#include "Vector3D.h"
#include "Polygon.h"
#include "Color.h"

typedef struct ContectedFaces
{
	struct Polygon * face;

	struct ContectedFaces * next;
}ContectedFaces;

//RW
typedef struct Vertex
{
	int index, nContectedFaces, bFlag;

	Vector3D * position, * worldPosition, * normal, * normal2;

	Vector * uv;

	Color * color;

	struct ContectedFaces * contectedFaces;
}Vertex;

//RW
typedef struct RenderVertex
{
	float x, y, z, u, v, r, g, b, a;
}RenderVertex;

Vertex * newVertex( float x, float y, float z )
{
	Vertex * v;

	if( ( v = ( Vertex * )malloc( sizeof( Vertex ) ) ) == NULL )
	{
		exit( TRUE );
	}

	v->contectedFaces = NULL;
	v->normal = newVector3D( 0.0f, 0.0f, 0.0f, 0.0f );
	v->normal2 = newVector3D( 0.0f, 0.0f, 0.0f, 0.0f );

	v->uv = newVector( 0.0f, 0.0f );
	v->position = newVector3D(x, y, z, 1.0f);
	v->worldPosition = newVector3D(x, y, z, 1.0f);
	v->color = newColor( 1.0f, 1.0f, 1.0f, 1.0f );

	v->nContectedFaces = FALSE;
	v->bFlag = FALSE;

	return v;
}

void vertex_addContectedFaces( Vertex * v, struct Polygon * p )
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
#ifndef __POLYGON_H
#define __POLYGON_H

#include "Vertex.h"
#include "Vector.h"

//R
typedef struct Polygon
{
	int isBackFace;

	Vector3D * normal, * center;

	Vertex * * vertex;
}Polygon;

void triangle3D_dispose( Polygon * p )
{
	vector3D_dispose( p->normal );
	vector3D_dispose( p->center );
	free( p->vertex );
	free( p );
}

Vector3D * polygon_normal( Vector3D * normal, Vertex * v0, Vertex * v1, Vertex * v2 )
{	
	Vector3D * a, * b, * c, ca, bc;
	
	a = v0->position;
	b = v1->position;
	c = v2->position;

	vector3D_crossProduct(normal, vector3D_subtract( &ca, c, a ), vector3D_subtract( &bc, b, c ) );

	vector3D_normalize(normal);

	return normal;
}

Polygon * newTriangle3D( Polygon * p, Vertex * va, Vertex * vb, Vertex * vc, Vector *uva, Vector * uvb, Vector * uvc )
{
	p->isBackFace = FALSE;

	if( ( p->vertex = ( Vertex * * )malloc( sizeof( Vertex * ) * 3 ) ) == NULL )
	{
		exit( TRUE );
	}

	p->vertex[0] = va;
	p->vertex[1] = vb;
	p->vertex[2] = vc;

	va->uv = uva;
	vb->uv = uvb;
	vc->uv = uvc;

	p->normal = newVector3D(0.0f, 0.0f, 0.0f, 1.0f);
	polygon_normal( p->normal, va, vb, vc);

	p->center = newVector3D(0.0f, 0.0f, 0.0f, 1.0f);
	p->center->x = ( va->position->x + vb->position->x + vc->position->x ) * 0.33333333f;
	p->center->y = ( va->position->y + vb->position->y + vc->position->y ) * 0.33333333f;
	p->center->z = ( va->position->z + vb->position->z + vc->position->z ) * 0.33333333f;

	vertex_addContectedFaces( va, p );
	vertex_addContectedFaces( vb, p );
	vertex_addContectedFaces( vc, p );

	return p;
}

void polygon_dispose( Polygon * p)
{
	free( p->center );
	free( p->normal );
	free( p );
}

void polygon_setBackFace( Polygon * polygon )
{
	polygon->isBackFace = TRUE;

	polygon->vertex[0]->bFlag ++;
	polygon->vertex[1]->bFlag ++;
	polygon->vertex[2]->bFlag ++;
}

void polygon_resetBackFace( Polygon * polygon )
{
	polygon->isBackFace = FALSE;

	polygon->vertex[0]->bFlag  = 0;
	polygon->vertex[1]->bFlag  = 0;
	polygon->vertex[2]->bFlag  = 0;
}

#endif
#ifndef __POLYGON_H
#define __POLYGON_H

#include "Vertex.h"
#include "Vector.h"

//R
typedef struct Polygon
{
	int flag, isBackFace;

	Vector3D * normal, * center;

	Vertex  * vertex[3];
}Polygon;

Polygon * polygon_initiate()
{
	Polygon * p;

	if( ( p = ( Polygon * )malloc( sizeof( Polygon ) ) ) == NULL )
	{
		exit( TRUE );
	}

	p->flag = p->isBackFace = FALSE;

	p->center = newVector3D(0.0f, 0.0f, 0.0f, 1.0f);

	p->normal = newVector3D(0.0f, 0.0f, 0.0f, 1.0f);

	return p;
}

void triangle3D_dispose( Polygon * p )
{
	vector3D_dispose( p->normal );
	vector3D_dispose( p->center );
	free ( p->vertex );
	free( p );
}

void polygon_normal( Vector3D * normal, Vertex * v1, Vertex * v2, Vertex * v3 )
{	
	Vector3D * a, * b, * c, ca, bc;
	
	a = v1->worldPosition;
	b = v2->worldPosition;
	c = v3->worldPosition;

	vector3D_crossProduct(normal, vector3D_subtract( &ca, c, a ), vector3D_subtract( &bc, b, c ) );

	vector3D_normalize(normal);
}

Polygon * newTriangle3D( Vertex * va, Vertex * vb, Vertex * vc, Vector *uva, Vector * uvb, Vector * uvc )
{
	Polygon * p = polygon_initiate();

	va->uv = uva;
	vb->uv = uvb;
	vc->uv = uvc;

	p->vertex[0] = va;
	p->vertex[1] = vb;
	p->vertex[2] = vc;

	polygon_normal(p->normal, va, vb, vc);

	p->center->x = ( va->position->x + vb->position->x + vc->position->x ) * 0.33333f;
	p->center->y = ( va->position->y + vb->position->y + vc->position->y ) * 0.33333f;
	p->center->z = ( va->position->z + vb->position->z + vc->position->z ) * 0.33333f;

	vertex_addContectedFaces( va, p );
	vertex_addContectedFaces( vb, p );
	vertex_addContectedFaces( vc, p );

	return p;
}

//void polygon_setCenterPoint( Polygon * polygon )
//{
//	Vector3D * v0, * v1, * v2;
//
//	v0 = polygon->vertex[0]->worldPosition;
//	v1 = polygon->vertex[1]->worldPosition;
//	v2 = polygon->vertex[2]->worldPosition;
//
//	polygon->center->x = ( v0->x + v1->x + v2->x ) * 0.333f;
//	polygon->center->y = ( v0->y + v1->y + v2->y ) * 0.333f;
//	polygon->center->z = ( v0->z + v1->z + v2->z ) * 0.333f;
//}

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
#ifndef __PLANE3D_H
#define __PLANE3D_H

#include "Vector3D.h"

typedef struct Plane3D
{
	float a, b, c, d;
}Plane3D;

Plane3D * newPlane3D( float a, float b, float c, float d )
{
	Plane3D * p;

	if( ( p	= ( Plane3D * )malloc( sizeof( Plane3D ) ) ) == NULL ) exit( TRUE );

	p->a = a;
	p->b = b;
	p->c = c;
	p->d = d;

	return p;
}

Plane3D * newPlane3Dfrom3Point( Vector3D * v0, Vector3D * v1, Vector3D * v2 )
{
	float d1x = v1->x - v0->x;
	float d1y = v1->y - v0->y;
	float d1z = v1->z - v0->z;

	float d2x = v2->x - v0->x;
	float d2y = v2->y - v0->y;
	float d2z = v2->z - v0->z;

	float a = d1y * d2z - d1z * d2y;
	float b = d1z * d2x - d1x * d2z;
	float c = d1x * d2y - d1y * d2x;

	float d = - (a * v0->x + b * v0->y + c * v0->z);

	return newPlane3D( a, b, c, d );
}

float plane3D_side( Plane3D * p, Vector3D * v )
{
	float result = p->a * v->x + p->b * v->y + p->c * v->z + p->d;

	if ( ( result > -0.001f ) && ( result < 0.001f ) ) return 0;

	return p->a * v->x + p->b * v->y + p->c * v->z + p->d;
}

float plane3D_distance( Plane3D * p, Vector3D * v )
{
	return plane3D_side( p, v ) / sqrtf( p->a * p->a + p->b * p->b + p->c * p->c );
}

# endif
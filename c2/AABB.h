#ifndef __AABB_H
#define __AABB_H

//---------------------------------------------------------------------------    
// aabb_corner    
//    
// Return one of the 8 corner points.  The points are numbered as follows:    
//    
//            6                                7    
//              ------------------------------    
//             /|                           /|    
//            / |                          / |    
//           /  |                         /  |    
//          /   |                        /   |    
//         /    |                       /    |    
//        /     |                      /     |    
//       /      |                     /      |    
//      /       |                    /       |    
//     /        |                   /        |    
//  2 /         |                3 /         |    
//   /----------------------------/          |    
//   |          |                 |          |    
//   |          |                 |          |      +Y    
//   |        4 |                 |          |     
//   |          |-----------------|----------|      |    
//   |         /                  |         /  5    |    
//   |        /                   |        /        |       +Z    
//   |       /                    |       /         |    
//   |      /                     |      /          |     /    
//   |     /                      |     /           |    /    
//   |    /                       |    /            |   /    
//   |   /                        |   /             |  /    
//   |  /                         |  /              | /    
//   | /                          | /               |/    
//   |/                           |/                ----------------- +X    
//   ------------------------------    
//  0                              1    
//    
// Bit 0 selects aabb->min->x vs. aabb->max->x    
// Bit 1 selects aabb->min->y vs. aabb->max->y    
// Bit 2 selects aabb->min->z vs. aabb->max->z    

#include <float.h>
#include <math.h>

#include "Vector3D.h"
#include "Matrix3D.h"

typedef struct AABB
{
	Vector3D * min;
	Vector3D * max;
}AABB;

//---------------------------------------------------------------------------
// aabb_empty
//
// "Empty" the box, by setting the values to really
// large/small numbers

INLINE void aabb_empty(AABB * aabb)
{
	aabb->min->x = aabb->min->y = aabb->min->z = FLT_MAX;
	aabb->max->x = aabb->max->y = aabb->max->z = -FLT_MAX;
}

AABB * newAABB()
{
	AABB * aabb;

	if( ( aabb = ( AABB * )malloc( sizeof( AABB ) ) ) == NULL )
	{
		exit( TRUE );
	}

	aabb->min = newVector3D(FLT_MAX, FLT_MAX, FLT_MAX, 1);
	aabb->max = newVector3D(- FLT_MAX, - FLT_MAX, - FLT_MAX, 1);

	return aabb;
}

INLINE Vector3D * aabb_corner(Vector3D * output, AABB * aabb, int i)
{
	if (i < 0 || i > 7) return NULL;

	output->x = (i & 1) ? aabb->max->x : aabb->min->x;
	output->y = (i & 1) ? aabb->max->x : aabb->min->x;
	output->z = (i & 1) ? aabb->max->x : aabb->min->x;
	output->z = 1;

	return output;
}

//---------------------------------------------------------------------------
// aabb_add
//
// Add a point to the box

INLINE void aabb_add(AABB * aabb, Vector3D * p)
{
	if (p->x < aabb->min->x) aabb->min->x = p->x;
	if (p->x > aabb->max->x) aabb->max->x = p->x;
	if (p->y < aabb->min->y) aabb->min->y = p->y;
	if (p->y > aabb->max->y) aabb->max->y = p->y;
	if (p->z < aabb->min->z) aabb->min->z = p->z;
	if (p->z > aabb->max->z) aabb->max->z = p->z;
}

void aabb_dispose( AABB * aabb )
{
	free( aabb->max );
	free( aabb->min );
	aabb->max = NULL;
	aabb->min = NULL;
}

INLINE AABB * aabb_createFromAABB(AABB * box)
{
	AABB * aabb;
	Vector3D * p;

	aabb = newAABB();

	p = box->min;
	aabb_add( aabb, p );

	p = box->max;
	aabb_add( aabb, p );

	return aabb;
}

INLINE void aabb_createFromSelf(AABB * * aabb)
{
	Vector3D min;
	Vector3D max;

	vector3D_copy( & min, ( * aabb )->min );
	vector3D_copy( & max, ( * aabb )->max );

	free( * aabb );

	( * aabb ) = newAABB();

	aabb_add( * aabb, & min );
	aabb_add( * aabb, & max );
}

INLINE void aabb_copy(AABB * aabb, AABB * src)
{
	vector3D_copy(aabb->min, src->min);
	vector3D_copy(aabb->max, src->max);
}

//---------------------------------------------------------------------------
// aabb_isEmpty
//
// Return true if the box is enmpty

INLINE int aabb_isEmpty(AABB * aabb)
{
	return (aabb->min->x > aabb->max->x) || (aabb->min->y > aabb->max->y) || (aabb->min->z > aabb->max->z);
}

//---------------------------------------------------------------------------
// aabb_setToTransformedBox
//
// Transform the box and compute the new AABB. Remember, this always
// results in an AABB that is at least as big as the origin, and may be
// considerably bigger.
//
// See 12.4.4

AABB * aabb_setToTransformedBox(AABB * aabb, AABB * box, Matrix3D * m)
{
	if (aabb_isEmpty(box))
	{
		aabb_empty(aabb);
		return aabb;
	}

	// Start with the translation portion
	aabb->min = aabb->max = matrix3D_getPosition(aabb->max, m);

	// Examine each of the 9 matrix elements
	// and compute the new AABB
	if (m->m11 > 0.0f)
	{
		aabb->min->x += m->m11 * box->min->x;
		aabb->max->x += m->m11 * box->max->x;
	}
	else
	{
		aabb->min->x += m->m11 * box->max->x;
		aabb->max->x += m->m11 * box->min->x;
	}

	if (m->m12 > 0.0f)
	{
		aabb->min->y += m->m12 * box->min->x;
		aabb->max->y += m->m12 * box->max->x;
	}
	else
	{
		aabb->min->y += m->m12 * box->max->x;
		aabb->max->y += m->m12 * box->min->x;
	}

	if (m->m13 > 0.0f)
	{
		aabb->min->z += m->m13 * box->min->x;
		aabb->max->z += m->m13 * box->max->x;
	}
	else
	{
		aabb->min->z += m->m13 * box->max->x;
		aabb->max->z += m->m13 * box->min->x;
	}

	if (m->m21 > 0.0f)
	{
		aabb->min->x += m->m21 * box->min->y;
		aabb->max->x += m->m21 * box->max->y;
	}
	else
	{
		aabb->min->x += m->m21 * box->max->y;
		aabb->max->x += m->m21 * box->min->y;
	}

	if (m->m22 > 0.0f)
	{
		aabb->min->y += m->m22 * box->min->y;
		aabb->max->y += m->m22 * box->max->y;
	}
	else
	{
		aabb->min->y += m->m22 * box->max->y;
		aabb->max->y += m->m22 * box->min->y;
	}

	if (m->m23 > 0.0f)
	{
		aabb->min->z += m->m23 * box->min->y;
		aabb->max->z += m->m23 * box->max->y;
	}
	else
	{
		aabb->min->z += m->m23 * box->max->y;
		aabb->max->z += m->m23 * box->min->y;
	}

	if (m->m31 > 0.0f)
	{
		aabb->min->x += m->m31 * box->min->z;
		aabb->max->x += m->m31 * box->max->z;
	}
	else
	{
		aabb->min->x += m->m31 * box->max->z;
		aabb->max->x += m->m31 * box->min->z;
	}

	if (m->m32 > 0.0f)
	{
		aabb->min->y += m->m32 * box->min->z;
		aabb->max->y += m->m32 * box->max->z;
	}
	else
	{
		aabb->min->y += m->m32 * box->max->z;
		aabb->max->y += m->m32 * box->min->z;
	}

	if (m->m33 > 0.0f)
	{
		aabb->min->z += m->m33 * box->min->z;
		aabb->max->z += m->m33 * box->max->z;
	}
	else
	{
		aabb->min->z += m->m33 * box->max->z;
		aabb->max->z += m->m33 * box->min->z;
	}

	return aabb;
}

//---------------------------------------------------------------------------
// aabb_contains
//
// Return true if the box contains a point

int aabb_contains(AABB * aabb, Vector3D * p)
{

	return
		(p->x >= aabb->min->x) && (p->x <= aabb->max->x) &&
		(p->y >= aabb->min->y) && (p->y <= aabb->max->y) &&
		(p->z >= aabb->min->z) && (p->z <= aabb->max->z);
}

//---------------------------------------------------------------------------
// aabb_closestPointTo
//
// Return the closest point on this box to another point

Vector3D * aabb_closestPointTo(Vector3D * output, AABB * aabb, Vector3D * p)
{
	if (p->x < aabb->min->x)
	{
		output->x = aabb->min->x;
	}
	else if (p->x > aabb->max->x)
	{
		output->x = aabb->max->x;
	}
	else
	{
		output->x = p->x;
	}

	if (p->y < aabb->min->y)
	{
		output->y = aabb->min->y;
	}
	else if (p->y > aabb->max->y)
	{
		output->y = aabb->max->y;
	}
	else
	{
		output->y = p->y;
	}

	if (p->z < aabb->min->z)
	{
		output->z = aabb->min->z;
	}
	else if (p->z > aabb->max->z)
	{
		output->z = aabb->max->z;
	}
	else
	{
		output->z = p->z;
	}

	return output;
}

//---------------------------------------------------------------------------
// aabb_intersectsSphere
//
// Return true if we intersect a sphere. Uses Arvo's algorithm.

int aabb_intersectsSphere(AABB * aabb, Vector3D * center, float radius)
{
	Vector3D output, dis, * d;

	aabb_closestPointTo(&output, aabb, center);

	d = vector3D_subtract(&dis, &output, center);

	return vector3D_length(d) < radius * radius;
}

//---------------------------------------------------------------------------
// aabb_rayIntersect
//
// Parametric intersection with a ray. Returns parametric point
// of intsersection in range 0...1 or a really big number (>1) if no
// intersection.
//
// From "Fast Ray-Box Intersection," by Woo in Graphics Gems I,
// page 395.
//
// See 12.9.11

float aabb_rayIntersect(AABB * aabb, Vector3D * rayOrg, Vector3D * rayDelta, Vector3D * returnNormal)
{
	int inside = TRUE;

	float xt, xn, yt, yn, zt, zn, x, y, z, t;
	int which;

	if (rayOrg->x < aabb->min->x)
	{
		xt = aabb->min->x - rayOrg->x;
		if (xt > rayDelta->x) return FLT_MAX;
		xt /= rayDelta->x;
		inside = FALSE;
		xn = -1.0f;
	}
	else if (rayOrg->x > aabb->max->x)
	{
		xt = aabb->max->x - rayOrg->x;
		if (xt < rayDelta->x) return FLT_MAX;
		xt /= rayDelta->x;
		inside = FALSE;
		xn = 1.0f;
	}
	else
	{
		xt = -1.0f;
	}

	if (rayOrg->y < aabb->min->y)
	{
		yt = aabb->min->y - rayOrg->y;
		if (yt > rayDelta->y) return FLT_MAX;
		yt /= rayDelta->y;
		inside = FALSE;
		yn = -1.0f;
	}
	else if (rayOrg->y > aabb->max->y)
	{
		yt = aabb->max->y - rayOrg->y;
		if (yt < rayDelta->y) return FLT_MAX;
		yt /= rayDelta->y;
		inside = FALSE;
		yn = 1.0f;
	}
	else
	{
		yt = -1.0f;
	}

	if (rayOrg->z < aabb->min->z)
	{
		zt = aabb->min->z - rayOrg->z;
		if (zt > rayDelta->z) return FLT_MAX;
		zt /= rayDelta->z;
		inside = FALSE;
		zn = -1.0f;
	}
	else if (rayOrg->z > aabb->max->z)
	{
		zt = aabb->max->z - rayOrg->z;
		if (zt < rayDelta->z) return FLT_MAX;
		zt /= rayDelta->z;
		inside = FALSE;
		zn = 1.0f;
	}
	else
	{
		zt = -1.0f;
	}

	// Inside box?

	if (inside)
	{
		if (returnNormal != NULL)
		{
			returnNormal->x = - rayDelta->x;
			returnNormal->y = - rayDelta->y;
			returnNormal->z = - rayDelta->z;
			returnNormal->w = - rayDelta->w;

			vector3D_normalize(returnNormal);
		}
		return 0.0f;
	}

	// Select farthest plane - this is
	// the plane of intersection.

	which = 0;
	t = xt;
	if (yt > t)
	{
		which = 1;
		t = yt;
	}
	if (zt > t)
	{
		which = 2;
		t = zt;
	}

	switch (which)
	{
	case 0:
		{
			y = rayOrg->y + rayDelta->y*t;
			if (y < aabb->min->y || y > aabb->max->y) return FLT_MAX;
			z = rayOrg->z + rayDelta->z*t;
			if (z < aabb->min->z || z > aabb->max->z) return FLT_MAX;

			if (returnNormal != NULL) {
				returnNormal->x = xn;
				returnNormal->y = 0.0f;
				returnNormal->z = 0.0f;
			}

		} break;

	case 1:
		{
			x = rayOrg->x + rayDelta->x*t;
			if (x < aabb->min->x || x > aabb->max->x) return FLT_MAX;
			z = rayOrg->z + rayDelta->z*t;
			if (z < aabb->min->z || z > aabb->max->z) return FLT_MAX;

			if (returnNormal != NULL) {
				returnNormal->x = 0.0f;
				returnNormal->y = yn;
				returnNormal->z = 0.0f;
			}

		} break;

	case 2:
		{
			x = rayOrg->x + rayDelta->x*t;
			if (x < aabb->min->x || x > aabb->max->x) return FLT_MAX;
			y = rayOrg->y + rayDelta->y*t;
			if (y < aabb->min->y || y > aabb->max->y) return FLT_MAX;

			if (returnNormal != NULL) {
				returnNormal->x = 0.0f;
				returnNormal->y = 0.0f;
				returnNormal->z = zn;
			}

		} break;
	}

	return t;

}

//---------------------------------------------------------------------------
// aabb_classifyPlane
//
// Perform static AABB-plane intersection test. Returns:
//
// <0 Box is completely on the BACK side of the plane
// >0 Box is completely on the FRONT side of the plane
// 0 Box intersects the plane

int aabb_classifyPlane(AABB * aabb, Vector3D * n, float d)
{
	float minD, maxD;

	if (n->x > 0.0f)
	{
		minD = n->x * aabb->min->x; maxD = n->x * aabb->max->x;
	}
	else
	{
		minD = n->x * aabb->max->x; maxD = n->x * aabb->min->x;
	}

	if (n->y > 0.0f)
	{
		minD += n->y * aabb->min->y; maxD += n->y * aabb->max->y;
	}
	else
	{
		minD += n->y * aabb->max->y; maxD += n->y * aabb->min->y;
	}

	if (n->z > 0.0f)
	{
		minD += n->z * aabb->min->z; maxD += n->z * aabb->max->z;
	}
	else
	{
		minD += n->z * aabb->max->z; maxD += n->z * aabb->min->z;
	}

	// Check if completely on the front side of the plane

	if (minD >= d)
	{
		return 1;
	}

	// Check if completely on the back side of the plane

	if (maxD <= d)
	{
		return -1;
	}

	// We straddle the plane

	return 0;
}

//---------------------------------------------------------------------------
// aabb_intersectPlane
//
// Perform dynamic AABB-plane intersection test.
//
// n is the plane normal (assumed to be normalized)
// planeD is the D value of the plane equation p.n = d
// dir dir is the direction of movement of the AABB.
//
// The plane is assumed to be stationary.
//
// Returns the parametric point of intersection - the distance traveled
// before an intersection occurs. If no intersection, a REALLY big
// number is returned. You must check against the length of the
// displacement.
//
// Only intersections with the front side of the plane are detected

float aabb_intersectPlane(AABB * aabb, Vector3D * n, float planeD, Vector3D * dir)
{
	float dot, minD, maxD, t;
	if (fabs(vector3D_dotProduct(n, n) - 1.0f) < 0.1 || fabs(vector3D_dotProduct(dir, dir) - 1.0f) < 0.1)
		return 0.0f;

	// Compute glancing angle, make sure we are moving towards
	// the front of the plane

	dot = vector3D_dotProduct(n, dir);

	if (dot >= 0.0f)
	{
		return FLT_MAX;
	}

	// Inspect the normal and compute the minimum and maximum
	// D values. minD is the D value of the "frontmost" corner point

	if (n->x > 0.0f)
	{
		minD = n->x * aabb->min->x; maxD = n->x * aabb->max->x;
	}
	else
	{
		minD = n->x * aabb->max->x; maxD = n->x * aabb->min->x;
	}

	if (n->y > 0.0f)
	{
		minD += n->y * aabb->min->y; maxD += n->y * aabb->max->y;
	}
	else
	{
		minD += n->y * aabb->max->y; maxD += n->y * aabb->min->y;
	}

	if (n->z > 0.0f)
	{
		minD += n->z * aabb->min->z; maxD += n->z * aabb->max->z;
	}
	else
	{
		minD += n->z * aabb->max->z; maxD += n->z * aabb->min->z;
	}

	// Check if we're already completely on the other
	// side of the plane

	if (maxD <= planeD)
	{
		return FLT_MAX;
	}

	// Perform standard raytrace equation using the
	// frontmost corner point

	t = (planeD - minD) / dot;

	// Were we already penetrating?

	if (t < 0.0f)
	{
		return 0.0f;
	}

	// Return it. If > l, then we didn't hit in time. That's
	// the condition that the caller should be checking for.

	return t;
}

/////////////////////////////////////////////////////////////////////////////
//
// Global nonmember code
//
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
// intersectAABBs
//
// Check if two AABBs intersect, and return true if so. Optionally return
// the AABB of their intersection if an intersection is detected

int intersectAABBs(AABB * box1, AABB * box2, AABB * boxIntersect)
{
	// Check for no overlap

	if (box1->min->x > box2->max->x) return FALSE;
	if (box1->max->x < box2->min->x) return FALSE;
	if (box1->min->y > box2->max->y) return FALSE;
	if (box1->max->y < box2->min->y) return FALSE;
	if (box1->min->z > box2->max->z) return FALSE;
	if (box1->max->z < box2->min->z) return FALSE;

	// We have overlap. Compute AABB of intersection, if they want it

	if (boxIntersect != NULL) {
		boxIntersect->min->x = MAX(box1->min->x, box2->min->x);
		boxIntersect->max->x = MIN(box1->max->x, box2->max->x);
		boxIntersect->min->y = MAX(box1->min->y, box2->min->y);
		boxIntersect->max->y = MIN(box1->max->y, box2->max->y);
		boxIntersect->min->z = MAX(box1->min->z, box2->min->z);
		boxIntersect->max->z = MIN(box1->max->z, box2->max->z);
	}

	// They intersected

	return TRUE;
}

//---------------------------------------------------------------------------
// intersectMovingAABB
//
// Return parametric point in time when a moving AABB collides
// with a stationary AABB. Returns > 1 if no intersection

float intersectMovingAABB(AABB * stationaryBox, AABB * movingBox, Vector3D * d)
{
	// Initialize interval to contain all the time under consideration

	float tEnter = 0.0f;
	float tLeave = 1.0f;

	//
	// Compute interval of overlap on each dimension, and intersect
	// this interval with the interval accumulated so far. As soon as
	// an empty interval is detected, return a negative result
	// (no intersection.) In each case, we have to be careful for
	// an infinite of empty interval on each dimension
	//

	// Check x-axis

	if (d->x == 0.0f)
	{

		// Empty or infinite inverval on x

		if (
			(stationaryBox->min->x >= movingBox->max->x) ||
			(stationaryBox->max->x <= movingBox->min->x)
			)
		{

				// Empty time interval, so no intersection

				return FLT_MAX;
		}

		// Inifinite time interval - no update necessary

	}
	else
	{

		// Divide once

		float oneOverD = 1.0f / d->x;

		// Compute time value when they begin and end overlapping

		float xEnter = (stationaryBox->min->x - movingBox->max->x) * oneOverD;
		float xLeave = (stationaryBox->max->x - movingBox->min->x) * oneOverD;

		// Check for interval out of order

		if (xEnter > xLeave) {
			swapf(&xEnter, &xLeave);
		}

		// Update interval

		if (xEnter > tEnter) tEnter = xEnter;
		if (xLeave < tLeave) tLeave = xLeave;

		// Check if this resulted in empty interval

		if (tEnter > tLeave) {
			return FLT_MAX;
		}
	}

	// Check y-axis

	if (d->y == 0.0f) {

		// Empty or infinite inverval on y

		if (
			(stationaryBox->min->y >= movingBox->max->y) ||
			(stationaryBox->max->y <= movingBox->min->y)
			) {

				// Empty time interval, so no intersection

				return FLT_MAX;
		}

		// Inifinite time interval - no update necessary

	} else {

		// Divide once

		float oneOverD = 1.0f / d->y;

		// Compute time value when they begin and end overlapping

		float yEnter = (stationaryBox->min->y - movingBox->max->y) * oneOverD;
		float yLeave = (stationaryBox->max->y - movingBox->min->y) * oneOverD;

		// Check for interval out of order

		if (yEnter > yLeave) {
			swapf(&yEnter, &yLeave);
		}

		// Update interval

		if (yEnter > tEnter) tEnter = yEnter;
		if (yLeave < tLeave) tLeave = yLeave;

		// Check if this resulted in empty interval

		if (tEnter > tLeave) {
			return FLT_MAX;
		}
	}

	// Check z-axis

	if (d->z == 0.0f) {

		// Empty or infinite inverval on z

		if (
			(stationaryBox->min->z >= movingBox->max->z) ||
			(stationaryBox->max->z <= movingBox->min->z)
			) {

				// Empty time interval, so no intersection

				return FLT_MAX;
		}

		// Inifinite time interval - no update necessary

	}
	else
	{

		// Divide once

		float oneOverD = 1.0f / d->z;

		// Compute time value when they begin and end overlapping

		float zEnter = (stationaryBox->min->z - movingBox->max->z) * oneOverD;
		float zLeave = (stationaryBox->max->z - movingBox->min->z) * oneOverD;

		// Check for interval out of order

		if (zEnter > zLeave) {
			swapf(&zEnter, &zLeave);
		}

		// Update interval

		if (zEnter > tEnter) tEnter = zEnter;
		if (zLeave < tLeave) tLeave = zLeave;

		// Check if this resulted in empty interval

		if (tEnter > tLeave) {
			return FLT_MAX;
		}
	}

	// OK, we have an intersection. Return the parametric point in time
	// where the intersection occurs

	return tEnter;
}

#endif
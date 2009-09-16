#ifndef __VECTOR3D_H
#define __VECTOR3D_H


/**************************************************************************************
 **----------------------------------------------------------------------------------**
 **                                 |Vector3D verson1.5|                             **
 **__________________________________________________________________________________**
 **                          include< math.h, malloc.h, Base.h >                     **
 **__________________________________________________________________________________**
 **************************************************************************************/

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Base.h"
#include "Math3D.h"

//RW
typedef struct Vector3D
{
	float x;
	float y;
	float z;
	float w;
}Vector3D;

/**************************************************************************************
 **----------------------------------------------------------------------------------**
 **                                    |vector3D|                                    **
 **__________________________________________________________________________________**
 **************************************************************************************/

INLINE float vector3D_lengthSquared( Vector3D * v )
{
	return v->x * v->x + v->y * v->y + v->z * v->z;
}

INLINE float vector3D_fast_lengthSquared( float fx, float fy, float fz )
{
	int temp;
	int x, y, z;
	int dist;

	x = (int)(fabs(fx) * 1024);
	y = (int)(fabs(fy) * 1024);
	z = (int)(fabs(fz) * 1024);

	if (y < x) SWAP(x, y, temp);

	if (z < y) SWAP(y ,z, temp);

	if (y < x) SWAP(x, y, temp);

	dist = (z + 11 * (y >> 5) + (x >> 2) );

	return((float)(dist >> 10));
}

INLINE float vector3D_length( Vector3D * v )
{
	//return vector3D_fast_lengthSquared( v->x, v->y, v->z );

	//printf("%f", vector3D_fast_lengthSquared( v->x, v->y, v->z ));
	return sqrtf( vector3D_lengthSquared( v ) );
}

INLINE void vector3D_set( Vector3D * v, float x, float y, float z, float w )
{
	v->x = x;
	v->y = y;
	v->z = z;
	v->w = w;
}

Vector3D * newVector3D( float x, float y, float z, float w )
{
	Vector3D * v;

	if( ( v = ( Vector3D * )malloc( sizeof( Vector3D ) ) ) == NULL ) exit( TRUE );

	vector3D_set( v, x, y, z, w );

	return v;
}

INLINE void vector3D_dispose( Vector3D * v )
{
	free( v );
}

INLINE void vector3D_copy( Vector3D * v, Vector3D * src )
{
	v->x = src->x;
	v->y = src->y;
	v->z = src->z;
	v->w = src->w;
}

INLINE Vector3D * vector3D_clone( Vector3D * src )
{
	Vector3D * v;

	if( ( v = ( Vector3D * )malloc( sizeof( Vector3D ) ) ) == NULL ) exit( TRUE );

	memcpy( v, src, sizeof( Vector3D ) );

	return v;
}

INLINE Vector3D * vector3D_add( Vector3D * output, Vector3D * v1, Vector3D * v2 )
{
	output->x = v1->x + v2->x;
	output->y = v1->y + v2->y;
	output->z = v1->z + v2->z;

	return output;
}

INLINE void vector3D_add_self( Vector3D * v1, Vector3D * v2 )
{
	v1->x += v2->x;
	v1->y += v2->y;
	v1->z += v2->z;
}

INLINE Vector3D * vector3D_subtract( Vector3D * output, Vector3D * v1, Vector3D * v2 )
{
	output->x = v1->x - v2->x;
	output->y = v1->y - v2->y;
	output->z = v1->z - v2->z;

	return output;
}

INLINE void vector3D_subtract_self( Vector3D * v1, Vector3D * v2 )
{
	v1->x -= v2->x;
	v1->y -= v2->y;
	v1->z -= v2->z;
}

INLINE Vector3D * vector3D_crossProduct( Vector3D * output, Vector3D * v1, Vector3D * v2 )
{
	output->x = v1->y * v2->z - v1->z * v2->y;
	output->y = v1->z * v2->x - v1->x * v2->z;
	output->z = v1->x * v2->y - v1->y * v2->x;

	return output;
}

INLINE float vector3D_distance( Vector3D * v1, Vector3D * v2 )
{
	Vector3D result;

	return vector3D_length( vector3D_subtract(&result, v2, v1 ) );
}

INLINE float vector3D_dotProduct( Vector3D * v1, Vector3D * v2 )
{
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

INLINE float vector3D_angleBetween( Vector3D * v1, Vector3D * v2 )
{
	return acosf( vector3D_dotProduct( v1, v2 ) );
}

INLINE int vector3D_equals( Vector3D * v1, Vector3D * v2, int allFour )
{
	return v1->x == v2->x && v1->y == v2->y && v1->z == v2->z && ( !allFour || v1->w == v2->w );
}

INLINE int vector3D_nearEquals( Vector3D * v1, Vector3D * v2, float lerance, int allFour )
{
	return ( fabs( v1->x - v2->x ) < lerance ) && ( fabs( v1->y - v2->y ) < lerance ) && ( fabs( v1->z - v2->z ) < lerance ) && ( !allFour || ( fabs( v1->w - v2->w ) < lerance ) );
}

INLINE void vector3D_negate( Vector3D * v )
{
	v->x *= -1;
	v->y *= -1;
	v->z *= -1;
}

INLINE float vector3D_normalize( Vector3D * v )
{
	float len, oneOverMag;

	//len = sqrtf( v->x * v->x + v->y * v->y + v->z * v->z );
	//oneOverMag = 1.0f / len;

	len = v->x * v->x + v->y * v->y + v->z * v->z;
	oneOverMag = invSqrt( len );

	if( !len )
	{
		v->x = v->y = v->z = 0.0f;
	}
	else
	{
		v->x *= oneOverMag;
		v->y *= oneOverMag;
		v->z *= oneOverMag;
	}

	return len;
}

INLINE void vector3D_project( Vector3D * v )
{
	float oneOverMag = 1.0f / v->w;

	v->x *= oneOverMag;
	v->y *= oneOverMag;
	v->z *= oneOverMag;
	v->w  = 1;
}

INLINE void vector3D_scaleBy( Vector3D * v, float value )
{
	v->x *= value;
	v->y *= value;
	v->z *= value;
}

/*char *toString( Vector3D v )
{
	return strcat( "Vector3D( ", strcat( toChar( v.x ), strcat( toChar( v.y ), toChar( v.z ) ) ) );
}*/

/**************************************************************************************
 **----------------------------------------------------------------------------------**
 **                                    |axisAngle|                                   **
 **__________________________________________________________________________________**
 **************************************************************************************/

Vector3D * g_X_AXIS;
Vector3D * g_Y_AXIS;
Vector3D * g_Z_AXIS;
Vector3D * g_INV_X_AXIS;
Vector3D * g_INV_Y_AXIS;
Vector3D * g_INV_Z_AXIS;

//void printfVector3D( Vector3D v )
//{
//	printf( "\nVector3D( x: %f, y: %f, z: %f, w: %f )\n", v.x, v.y, v.z, v.w );
//}

# endif
#ifndef VECTOR_H
#define VECTOR_H


/**************************************************************************************
 **----------------------------------------------------------------------------------**
 **                                 |Vector3D verson1.5|                             **
 **__________________________________________________________________________________**
 **                          include< math.h, malloc.h, Base.h >                     **
 **__________________________________________________________________________________**
 **************************************************************************************/

# include<malloc.h>
# include<stdio.h>
//# include<string.h>

# include "Base.h"

//RW
typedef struct
{
	Number x;
	Number y;
	Number z;
	Number w;
}Vector3D;

/**************************************************************************************
 **----------------------------------------------------------------------------------**
 **                                    |vector3D|                                    **
 **__________________________________________________________________________________**
 **************************************************************************************/

Number vector3D_lengthSquared( Vector3D * v )
{
	return v -> x * 2 + v -> y * 2 + v -> z * 2;
}

Number vector3D_length( Vector3D * v )
{
	return sqrt( vector3D_lengthSquared( v ) );
}

Vector3D * newVector3D( Number x, Number y, Number z, Number w )
{
	Vector3D * v;

	if( ( v = ( Vector3D * )malloc( sizeof( Vector3D ) ) ) == NULL )
	{
		exit( TRUE );
	}

	v -> x = x;
	v -> y = y;
	v -> z = z;
	v -> w = w;

	return v;
}

void vector3D_add( Vector3D * v1, Vector3D * v2 )
{
	v1 -> x = v1 -> x + v2 -> x;
	v1 -> y = v1 -> y + v2 -> y;
	v1 -> z = v1 -> z + v2 -> z;
}

void vector3D_subtract( Vector3D * v1, Vector3D * v2 )
{
	v1 -> x = v1 -> x - v2 -> x;
	v1 -> y = v1 -> y - v2 -> y;
	v1 -> z = v1 -> z - v2 -> z;
}

Vector3D * vector3D_clone( Vector3D * v )
{
	return newVector3D( v -> x, v -> y, v -> z, v -> w );
}

/*void vector3D_copy( Vector3D * v, Vector3D c )
{
	( * v ).x = c.x;
	( * v ).y = c.y;
	( * v ).z = c.z;
	( * v ).w = c.w;
}*/

Vector3D * vector3D_crossProduct( Vector3D * v1, Vector3D * v2 )
{
	return newVector3D( v1 -> y * v2 -> z - v1 -> z * v2 -> y, v1 -> z * v2 -> x - v1 -> x * v2 -> z, v1 -> x * v2 -> y - v1 -> y * v2 -> x, 1.0 );
}

Number vector3D_distance( Vector3D * v1, Vector3D * v2 )
{
	Vector3D v =  * v2;

	vector3D_subtract( & v, v1 );

	return vector3D_length( & v );
}

Number vector3D_dotMetrix( Vector3D * v1, Vector3D * v2 )
{
	return v1 -> x * v2 -> x + v1 -> y * v2 -> y + v1 -> z * v2 -> z;
}

Number vector3D_dotProduct( Vector3D * v1, Vector3D * v2 )
{
	return vector3D_dotMetrix( v1, v2 ) / ( vector3D_length( v1 ) * vector3D_length( v2 ) );
}

Number vector3D_angleBetween( Vector3D * v1, Vector3D * v2 )
{
	return acos( vector3D_dotProduct( v1, v2 ) );
}

int vector3D_equals( Vector3D * v1, Vector3D * v2, int allFour )
{
	return v1 -> x == v2 -> x && v1 -> y == v2 -> y && v1 -> z == v2 -> z && ( !allFour || v1 -> w == v2 -> w );
}


int vector3D_nearEquals( Vector3D * v1, Vector3D * v2, Number lerance, int allFour )
{
	return ( fabs( v1 -> x - v2 -> x ) < lerance ) && ( fabs( v1 -> y - v2 -> y ) < lerance ) && ( fabs( v1 -> z - v2 -> z ) < lerance ) && ( !allFour || ( fabs( v1 -> w - v2 -> w ) < lerance ) );
}

void vector3D_negate( Vector3D * v )
{
	v -> x = - v -> x;
	v -> y = - v -> y;
	v -> z = - v -> z;
}

Number vector3D_normalize( Vector3D * v )
{
	Number len = vector3D_length( v );

	if( len )
	{
		v -> x /= len;
		v -> y /= len;
		v -> z /= len;
	}

	return len;
}

void vector3D_project( Vector3D * v )
{
	v -> x /= v -> w;
	v -> y /= v -> w;
	v -> z /= v -> w;
}

void vector3D_scaleBy( Vector3D * v, Number value )
{
	v -> x *= value;
	v -> y *= value;
	v -> z *= value;
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

Vector3D * X_AXIS()
{
	return newVector3D( 1.0, 0, 0, 1.0 );
}

Vector3D * Y_AXIS()
{
	return newVector3D( 0, 1.0, 0, 1.0 );
}

Vector3D * Z_AXIS()
{
	return newVector3D( 0, 0, 1.0, 1.0 );
}

/**
后置乘法.
**/
void vector3D_apprend( Vector3D * thisVector, Vector3D * lhs )
{
	Vector3D q = * thisVector;

	thisVector -> x = q.w * lhs -> x + q.x * lhs -> w + q.y * lhs -> z - q.z * lhs -> y;
    thisVector -> y = q.w * lhs -> y - q.x * lhs -> z + q.y * lhs -> w + q.z * lhs -> x;
    thisVector -> z = q.w * lhs -> z + q.x * lhs -> y - q.y * lhs -> x + q.z * lhs -> w;
    thisVector -> w = q.w * lhs -> w - q.x * lhs -> x - q.y * lhs -> y - q.z * lhs -> z;
}

/**
前置乘法.
**/
void vector3D_prepend( Vector3D * thisVector, Vector3D * lhs )
{
	Vector3D q = * thisVector;

	thisVector -> x = lhs -> x * q.w + lhs -> w * q.x + lhs -> z * q.y - lhs -> y * q.z;
    thisVector -> y = lhs -> y * q.w - lhs -> z * q.x + lhs -> w * q.y + lhs -> x * q.z;
    thisVector -> z = lhs -> z * q.w + lhs -> y * q.x - lhs -> x * q.y + lhs -> w * q.z;
    thisVector -> w = lhs -> w * q.w - lhs -> x * q.x - lhs -> y * q.y - lhs -> z * q.z;
}

/**
后置旋转.
**/
void vector3D_apprendRotation( Vector3D * v, Number degrees, Vector3D * axis )
{
	Vector3D q;
	
	Number angle = degrees * TORADIANS * 0.5, s = sin( angle );

	q.x = axis -> x * s;
	q.y = axis -> y * s;
	q.z = axis -> z * s;
	q.w = cos( angle );

	vector3D_normalize( & q );

	vector3D_apprend( v, &q );
}

/**
前置旋转.
**/
void vector3D_prependRotation( Vector3D * v, Number degrees, Vector3D * axis )
{
	Vector3D q;
	
	Number angle = degrees * TORADIANS * 0.5, s = sin( angle );

	q.x = axis -> x * s;
	q.y = axis -> y * s;
	q.z = axis -> z * s;
	q.w = cos( angle );

	vector3D_normalize( & q );

	vector3D_prepend( v, &q );
}

/**
四元数插值。
**/
void vector3D_slerp( Vector3D * from, Vector3D * to, Vector3D * res, Number t )
{
	Vector3D * tol = vector3D_clone( from );
	Number cosom  = vector3D_dotProduct( from, to ), omega, sinom, scale0, scale1;

	if (cosom < 0.0)
	{
		cosom = - cosom;
		vector3D_negate( tol );
	}

	if ( ( 1.0 - cosom ) > 0 )
	{
		omega  = acos ( cosom );
		sinom  = sin ( omega );
		scale0 = sin ( ( 1.0 - t ) * omega ) / sinom;
		scale1 = sin ( t * omega ) / sinom;

	}
	else
	{
		scale0 = 1.0 - t;
		scale1 = t;
	}

	res -> x = scale0 * from -> x + scale1 * tol -> x;
	res -> y = scale0 * from -> y + scale1 * tol -> y;
	res -> z = scale0 * from -> z + scale1 * tol -> z;
	res -> w = scale0 * from -> w + scale1 * tol -> w;
}

void printfVector3D( Vector3D * v )
{
	printf( "\nVector3D( x: %f, y: %f, z: %f, w: %f )\n", v -> x, v -> y, v -> z, v -> w );
}


//RW
typedef struct
{
	Number x;
	Number y;
}Vector;

# endif
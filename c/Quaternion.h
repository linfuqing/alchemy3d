# ifndef Quaternion_H
# define Quaternion_H

# include "Vector3D.h"

typedef struct
{
	Number x;
	Number y;
	Number z;
	Number w;
}Quaternion;

Number quaternion_moduloSquared( Quaternion * q )
{
	return q -> x * q -> x + q -> y * q -> y + q -> z * q ->z + q -> w * q -> w;
}

Number quaternion_modulo( Quaternion * q )
{
	return sqrt( quaternion_moduloSquared( q ) );
}

Quaternion * newQuaternion( Number x, Number y, Number z, Number w )
{
	Quaternion * q;

	if( ( q = ( Quaternion * )malloc( sizeof( Quaternion ) ) ) == NULL )
	{
		exit( TRUE );
	}

	q -> x = x;
	q -> y = y;
	q -> z = z;
	q -> w = w;

	return q;
}

void quaternion_add( Quaternion * q1, Quaternion * q2 )
{
	q1 -> x = q1 -> x + q2 -> x;
	q1 -> y = q1 -> y + q2 -> y;
	q1 -> z = q1 -> z + q2 -> z;
	q1 -> w = q1 -> w + q2 -> w;
}

void quaternion_subtract( Quaternion * q1, Quaternion * q2 )
{
	q1 -> x = q1 -> x - q2 -> x;
	q1 -> y = q1 -> y - q2 -> y;
	q1 -> z = q1 -> z - q2 -> z;
	q1 -> w = q1 -> w - q2 -> w;
}

Quaternion * Quaternion_clone( Quaternion * q )
{
	return newQuaternion( q -> x, q -> y, q -> z, q -> w );
}

Number quaternion_dotMetrix( Quaternion * q1, Quaternion * q2 )
{
	return q1 -> x * q2 -> x + q1 -> y * q2 -> y + q1 -> z * q2 -> z + q1 -> w * q2 -> w;
}

Number quaternion_dotProduct( Quaternion * q1, Quaternion * q2 )
{
	return quaternion_dotMetrix( q1, q2 ) / ( quaternion_modulo( q1 ) * quaternion_modulo( q2 ) );
}

int quaternion_equals( Quaternion * q1, Quaternion * q2 )
{
	return q1 -> x == q2 -> x && q1 -> y == q2 -> y && q1 -> z == q2 -> z &&  q1 -> w == q2 -> w;
}


int quaternion_nearEquals( Quaternion * q1, Quaternion * q2, Number lerance )
{
	return ( fabs( q1 -> x - q2 -> x ) < lerance ) && ( fabs( q1 -> y - q2 -> y ) < lerance ) && ( fabs( q1 -> z - q2 -> z ) < lerance ) && ( fabs( q1 -> w - q2 -> w ) < lerance );
}

void quaternion_negate( Quaternion * q )
{
	q -> x = - q -> x;
	q -> y = - q -> y;
	q -> z = - q -> z;
	q -> w = - q -> w;
}

void quaternion_conjugate( Quaternion * q, Quaternion * target )
{
	target -> x = - q -> x;
	target -> y = - q -> y;
	target -> z = - q -> z;
	target -> w =   q -> w;
}

Number quaternion_normalize( Quaternion * q )
{
	Number modulo = quaternion_modulo( q );

	if( modulo )
	{
		q -> x /= modulo;
		q -> y /= modulo;
		q -> z /= modulo;
		q -> w /= modulo; 
	}

	return modulo;
}

/**
前置乘法.
**/
void quaternion_prepend( Quaternion * thisQuaternion, Quaternion * lhs )
{
	Quaternion q = * thisQuaternion;

	thisQuaternion -> x = q.w * lhs -> x + q.x * lhs -> w + q.y * lhs -> z - q.z * lhs -> y;
    thisQuaternion -> y = q.w * lhs -> y - q.x * lhs -> z + q.y * lhs -> w + q.z * lhs -> x;
    thisQuaternion -> z = q.w * lhs -> z + q.x * lhs -> y - q.y * lhs -> x + q.z * lhs -> w;
    thisQuaternion -> w = q.w * lhs -> w - q.x * lhs -> x - q.y * lhs -> y - q.z * lhs -> z;
}

/**
后置乘法.
**/
void quaternion_apprend( Quaternion * thisQuaternion, Quaternion * lhs )
{
	Quaternion q = * thisQuaternion;

	thisQuaternion -> x = lhs -> x * q.w + lhs -> w * q.x + lhs -> z * q.y - lhs -> y * q.z;
    thisQuaternion -> y = lhs -> y * q.w - lhs -> z * q.x + lhs -> w * q.y + lhs -> x * q.z;
    thisQuaternion -> z = lhs -> z * q.w + lhs -> y * q.x - lhs -> x * q.y + lhs -> w * q.z;
    thisQuaternion -> w = lhs -> w * q.w - lhs -> x * q.x - lhs -> y * q.y - lhs -> z * q.z;
}

/**
后置旋转.
**/
void quaternion_apprendRotation( Quaternion * q, Number degrees, Vector3D * axis )
{
	Quaternion rotation;
	
	Number angle = degrees * TORADIANS * 0.5, s = sin( angle );

	rotation.x = axis -> x * s;
	rotation.y = axis -> y * s;
	rotation.z = axis -> z * s;
	rotation.w = cos( angle );

	quaternion_normalize( & rotation );

	quaternion_apprend( q, & rotation );
}

/**
前置旋转.
**/
void quaternion_prependRotation( Quaternion * q, Number degrees, Vector3D * axis )
{
	Quaternion rotation;
	
	Number angle = degrees * TORADIANS * 0.5, s = sin( angle );

	rotation.x = axis -> x * s;
	rotation.y = axis -> y * s;
	rotation.z = axis -> z * s;
	rotation.w = cos( angle );

	quaternion_normalize( & rotation );

	quaternion_prepend( q, & rotation );
}

/**
四元数插值。
**/
void quaternion_slerp( Quaternion * from, Quaternion * to, Quaternion * res, Number t )
{
	Quaternion * tol = Quaternion_clone( from );
	Number cosom  = quaternion_dotProduct( from, to ), omega, sinom, scale0, scale1;

	if (cosom < 0.0)
	{
		cosom = - cosom;
		quaternion_negate( tol );
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


void quaternion_decompose( Quaternion * q, Vector3D * rotation )
{
	Number test = q -> x * q -> y + q -> z * q -> w, sqx, sqy, sqz;

	if ( test > 0.5 ) {
		rotation -> x = 2.0 * atan2( q -> x,q -> w );
		rotation -> y = PI / 2.0;
		rotation -> z = 0.0;
	}
	else if ( test < - 0.5 ) {
		rotation -> x = - 2.0 * atan2( q -> x,q -> w );
		rotation -> y = - PI / 2.0;
		rotation -> z = 0.0;
	}
	else
	{
		sqx = q -> x * q -> x;
		sqy = q -> y * q -> y;
		sqz = q -> z * q -> z;
		    
		rotation -> x = atan2( 2.0 * q -> y * q -> w - 2.0 * q -> x * q -> z, 1 - 2.0 * sqy - 2.0 * sqz );
		rotation -> y = asin(  2.0 * test );   
		rotation -> z = atan2( 2.0 * q -> x * q -> w - 2.0 * q -> y * q -> z, 1 - 2.0 * sqx - 2.0 * sqz );
		rotation -> w = 1.0;
	}
}

void quaternion_recompose( Quaternion * q, Vector3D * rotation )
{
	Number ax              = rotation -> x * TORADIANS;
	Number ay              = rotation -> y * TORADIANS;
	Number az              = rotation -> z * TORADIANS;

	Number fSinPitch       = sin( ax * 0.5 );
	Number fCosPitch       = cos( ax * 0.5 );
	Number fSinYaw         = sin( ay * 0.5 );
	Number fCosYaw         = cos( ay * 0.5 );
	Number fSinRoll        = sin( az * 0.5 );
	Number fCosRoll        = cos( az * 0.5 );
	Number fCosPitchCosYaw = fCosPitch * fCosYaw;
	Number fSinPitchSinYaw = fSinPitch * fSinYaw;

	q -> x = fSinRoll * fCosPitchCosYaw     - fCosRoll * fSinPitchSinYaw;
	q -> y = fCosRoll * fSinPitch * fCosYaw + fSinRoll * fCosPitch * fSinYaw;
	q -> z = fCosRoll * fCosPitch * fSinYaw - fSinRoll * fSinPitch * fCosYaw;
	q -> w = fCosRoll * fCosPitchCosYaw     + fSinRoll * fSinPitchSinYaw;
}

# endif
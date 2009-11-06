#ifndef __QUATERNION_H
#define __QUATERNION_H

#include <stdio.h>
#include <malloc.h>

#include "Vector3D.h"
#include "Matrix3D.h"

typedef struct Quaternion
{
	float w;
	float x;
	float y;
	float z;
}Quaternion;

Quaternion * newQuaternion()
{
	Quaternion * q;

	if( ( q = ( Quaternion * )malloc( sizeof( Quaternion ) ) ) == NULL )
	{
		exit( TRUE );
	}

	q -> x = 0;
	q -> y = 0;
	q -> z = 0;
	q -> w = 1;

	return q;
}

INLINE void quaternoin_identity(Quaternion * q)
{
	q->x = q->y = q->z = 0;
	q->w = 1;
}

INLINE float safeAcos(float x)
{
	if (x <= -1.0f)
	{
		return PI;
	}

	if (x >= 1.0f)
	{
		return 0.0f;
	}

	return acosf(x);
}

INLINE void quaternoin_setToRotateAboutX(Quaternion * q, float theta)
{
	float thetaOver2 = theta * 0.5f;

	q->w = cosf(thetaOver2);
	q->x = sinf(thetaOver2);
	q->y = 0.0f;
	q->z = 0.0f;
}

INLINE void quaternoin_setToRotateAboutY(Quaternion * q, float theta)
{
	float thetaOver2 = theta * 0.5f;

	q->w = cosf(thetaOver2);
	q->x = 0.0f;
	q->y = sinf(thetaOver2);
	q->z = 0.0f;
}

INLINE void quaternoin_setToRotateAboutZ(Quaternion * q, float theta)
{
	float thetaOver2 = theta * 0.5f;

	q->w = cosf(thetaOver2);
	q->x = 0.0f;
	q->y = 0.0f;
	q->z = sinf(thetaOver2);
}

INLINE void quaternoin_setToRotateAboutAxis(Quaternion * q, Vector3D * axis, float theta)
{
	float thetaOver2 = theta * 0.5f;
	float sinThetaOver2 = sinf(thetaOver2);

	q->w = cosf(thetaOver2);
	q->x = axis->x * sinThetaOver2;
	q->y = axis->y * sinThetaOver2;
	q->z = axis->z * sinThetaOver2;
}

/**
* 从指定欧拉角构造四元数
*/
INLINE Quaternion * quaternoin_setFromEuler(Quaternion * output, float ay, float ax, float az)
{
	float sp, sb, sh, cp, cb, ch;

	sp = sinf(ax * 0.5f); 
	cp = cosf(ax * 0.5f); 
	sb = sinf(az * 0.5f);
	cb = cosf(az * 0.5f);
	sh = sinf(ay * 0.5f);
	ch = cosf(ay * 0.5f);

	output->w =  ch*cp*cb + sh*sp*sb;
	output->x = -ch*sp*cb - sh*cp*sb;
	output->y =  ch*sp*sb - sh*cb*cp;
	output->z =  sh*sp*cb - ch*cp*sb;

	return output;
}

/**
* 从四元数返回矩阵
*/
INLINE Matrix4x4 * quaternoin_toMatrix(Matrix4x4 * output, Quaternion * qua)
{
	float x, y, z, w, xx, xy, xz, xw, yy, yz, yw, zz, zw;

	w = qua->w;
	x = qua->x;
	y = qua->y;
	z = qua->z;

	xx = x * x;
	xy = x * y;
	xz = x * z;
	xw = x * w;
	yy = y * y;
	yz = y * z;
	yw = y * w;
	zz = z * z;
	zw = z * w;

	output->m11 = 1 - 2 * ( yy + zz );
	output->m12 = 2 * ( xy - zw );
	output->m13 = 2 * ( xz + yw );
	output->m14 = 0;

	output->m21 = 2 * ( xy + zw );
	output->m22 = 1 - 2 * ( xx + zz );
	output->m23 = 2 * ( yz - xw );
	output->m24 = 0;

	output->m31 = 2 * ( xz - yw );
	output->m32 = 2 * ( yz + xw );
	output->m33 = 1 - 2 * ( xx + yy );
	output->m34 = 0;

	output->m41 = 0;
	output->m42 = 0;
	output->m43 = 0;
	output->m44 = 1;

	return output;
}

INLINE Vector3D quaternion_toEuler( Vector3D q )
{
	Vector3D euler;
			
	float test = q.x * q.y + q.z * q.w, sqx, sqy, sqz;

	if ( test > 0.5f ) {
		euler.x = 2.0f * atan2f( q.x,q.w );
		euler.y = HALF_PI;
		euler.z = 0.0f;
	}
	else if ( test < -0.5f ) {
		euler.x = -2.0f * atan2f( q.x,q.w );
		euler.y = - HALF_PI;
		euler.z = 0.0f;
	}
	else
	{
		sqx = q.x * q.x;
		sqy = q.y * q.y;
		sqz = q.z * q.z;
		    
		euler.x = atan2f( 2.0f * q.y * q.w - 2.0f * q.x * q.z, 1 - 2.0f * sqy - 2.0f * sqz );
		euler.y = asinf( 2.0f * test );   
		euler.z = atan2f( 2.0f * q.x * q.w - 2.0f * q.y * q.z, 1 - 2.0f * sqx - 2.0f * sqz );
		euler.w = 1.0f;
	}
			
	return euler;
}

INLINE void quaternion_normalize(Quaternion * q)
{
	float mag, oneOverMag;
	
	mag = (float)sqrtf(q->w * q->w + q->x * q->x + q->y * q->y + q->z * q->z);

	if (mag > 0.0f)
	{
		oneOverMag = 1.0f / mag;
		q->w *= oneOverMag;
		q->x *= oneOverMag;
		q->y *= oneOverMag;
		q->z *= oneOverMag;

	}
	else
	{
		quaternoin_identity(q);
	}
}

INLINE float quaternion_getRotationAngle(Quaternion * q)
{
	float thetaOver2 = safeAcos(q->w);

	return thetaOver2 * 2.0f;
}

INLINE Vector3D * quaternion_getRotationAxis(Quaternion * q)
{
	float sinThetaOver2Sq, oneOverSinThetaOver2;

	sinThetaOver2Sq = 1.0f - q->w * q->w;

	if (sinThetaOver2Sq <= 0.0f)
	{
		return newVector3D(1.0f, 0.0f, 0.0f, 1.0f);
	}

	oneOverSinThetaOver2 = 1.0f / sqrtf(sinThetaOver2Sq);

	return newVector3D(
		q->x * oneOverSinThetaOver2,
		q->y * oneOverSinThetaOver2,
		q->z * oneOverSinThetaOver2,
		q->w
		);
}

INLINE void quaternion_rotationMatrix(Quaternion * pout, Matrix4x4 * pm)
{
	int maxi;
	float maxdiag, s, trace;

	trace = pm->m11 + pm->m22 + pm->m33 + 1.0f;

	if ( trace > 1.0f)
	{
		pout->x = ( pm->m23 - pm->m32 ) / ( 2.0f * sqrtf(trace) );
		pout->y = ( pm->m31 - pm->m13 ) / ( 2.0f * sqrtf(trace) );
		pout->z = ( pm->m12 - pm->m21 ) / ( 2.0f * sqrtf(trace) );
		pout->w = sqrtf(trace) / 2.0f;
		return;
	}

	maxi = 0;
	maxdiag = pm->m11;

	if ( pm->m22 > maxdiag )
	{
		maxi = 1;
		maxdiag = pm->m22;
	}

	if ( pm->m33 > maxdiag )
	{
		maxi = 2;
		maxdiag = pm->m33;
	}

	switch( maxi )
	{
		case 0:
			s = 2.0f * sqrtf(1.0f + pm->m11 - pm->m22 - pm->m33);
			pout->x = 0.25f * s;
			pout->y = ( pm->m12 + pm->m21 ) / s;
			pout->z = ( pm->m13 + pm->m31 ) / s;
			pout->w = ( pm->m23 - pm->m32 ) / s;
			break;
		case 1:
			s = 2.0f * sqrtf(1.0f + pm->m22 - pm->m11 - pm->m33);
			pout->x = ( pm->m12 + pm->m21 ) / s;
			pout->y = 0.25f * s;
			pout->z = ( pm->m23 + pm->m32 ) / s;
			pout->w = ( pm->m31 - pm->m13 ) / s;
			break;
		case 2:
			s = 2.0f * sqrtf(1.0f + pm->m33 - pm->m11 - pm->m22);
			pout->x = ( pm->m13 + pm->m31 ) / s;
			pout->y = ( pm->m23 + pm->m32 ) / s;
			pout->z = 0.25f * s;
			pout->w = ( pm->m12 - pm->m21 ) / s;
			break;
	}
}

INLINE float quaternion_dotProduct(Quaternion * a, Quaternion * b)
{
    return a->w * b->w + a->x * b->x + a->y * b->y + a->z * b->z;
}

INLINE Quaternion * quaternion_slerp(Quaternion * q0, Quaternion * q1, float t)
{
	Quaternion * result;

	float cosOmega, sinOmega, omega, oneOverSinOmega, q1w, q1x, q1y, q1z, k0, k1;

	if (t <= 0.0f) return q0;
	if (t >= 1.0f) return q1;

	cosOmega = quaternion_dotProduct(q0, q1);

	q1w = q1->w;
	q1x = q1->x;
	q1y = q1->y;
	q1z = q1->z;

	if (cosOmega < 0.0f)
	{
		q1w = -q1w;
		q1x = -q1x;
		q1y = -q1y;
		q1z = -q1z;
		cosOmega = -cosOmega;
	}

	if (cosOmega < 1.1f)
	{
		exit( TRUE );
	}

	if (cosOmega > 0.9999f)
	{
		k0 = 1.0f-t;
		k1 = t;
	}
	else
	{
		sinOmega = sqrtf(1.0f - cosOmega*cosOmega);

		omega = atan2f(sinOmega, cosOmega);

		oneOverSinOmega = 1.0f / sinOmega;

		k0 = sinf((1.0f - t) * omega) * oneOverSinOmega;
		k1 = sinf(t * omega) * oneOverSinOmega;
	}

	result = newQuaternion();

	result->x = k0*q0->x + k1*q1x;
	result->y = k0*q0->y + k1*q1y;
	result->z = k0*q0->z + k1*q1z;
	result->w = k0*q0->w + k1*q1w;

	return result;
}

INLINE Quaternion * quaternion_conjugate(Quaternion * q)
{
	Quaternion * result;

	result = newQuaternion();

	result->w = q->w;

	result->x = -q->x;
	result->y = -q->y;
	result->z = -q->z;

	return result;
}

INLINE Quaternion * quaternion_pow(Quaternion * q, float exponent)
{
	Quaternion * result;

	float alpha, newAlpha, mult;

	if (fabs(q->w) > .9999f) {
		return q;
	}

	alpha = acosf(q->w);

	newAlpha = alpha * exponent;

	result = newQuaternion();

	result->w = cosf(newAlpha);

	mult = sinf(newAlpha) / sinf(alpha);
	result->x = q->x * mult;
	result->y = q->y * mult;
	result->z = q->z * mult;

	return result;
}


#endif
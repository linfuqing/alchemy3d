#ifndef __MATRIX3D_H
#define __MATRIX3D_H


/**************************************************************************************
 **----------------------------------------------------------------------------------**
 **                               |Matrix4x4 verson 1.5|                              **
 **----------------------------------------------------------------------------------**
 **                           include< stdlib.h, Vector3D.h>                         **
 **__________________________________________________________________________________**
 **************************************************************************************/


#include <string.h>
#include <math.h>

#include "Vector.h"
#include "Vector3D.h"
#include "Math3D.h"

/*
============
Matrix2x2
============
*/
typedef struct Matrix2x2
{
	union
	{
		struct
		{
			float m11, m12;
			float m21, m22;
		};

		float m[2][2];

		float _m[4];
	};
}Matrix2x2;

int matrix2x2_invert(Matrix2x2 * m)
{
	float det, invDet, a;

	det = m->m[0][0] * m->m[1][1] - m->m[0][1] * m->m[1][0];

	if ( fabs( det ) < MATRIX_INVERSE_EPSILON )
	{
		return FALSE;
	}

	invDet = 1.0f / det;

	a = m->m[0][0];
	m->m[0][0] =   m->m[1][1] * invDet;
	m->m[0][1] = - m->m[0][1] * invDet;
	m->m[1][0] = - m->m[1][0] * invDet;
	m->m[1][1] =   a * invDet;

	return TRUE;
}

/*
============
Matrix3x3
============
*/
typedef struct Matrix3x3
{
	union
	{
		struct
		{
			float m11, m12, m13;
			float m21, m22, m23;
			float m31, m32, m33;
		};

		float m[3][3];

		float _m[9];
	};
}Matrix3x3;

INLINE Matrix3x3 * matrix3x3_identity( Matrix3x3 * m )
{
	m->m11 = 1.0f; m->m12 = 0.0f; m->m13 = 0.0f;

	m->m21 = 0.0f; m->m22 = 1.0f; m->m23 = 0.0f;

	m->m31 = 0.0f; m->m32 = 0.0f; m->m33 = 1.0f;

	return m;
}

INLINE float matrix3x3_determinant( Matrix3x3 * m )
{
	return m->m11 * ( m->m22 * m->m33 - m->m23 * m->m32 ) + m->m12 * ( m->m23 * m->m31 - m->m21 * m->m33 ) + m->m13 * ( m->m21 * m->m32 - m->m22 * m->m31 );
}

INLINE float * matrix3x3_getRawData( Matrix3x3 * m )
{
	float * rawData;

	if( ( rawData = (float *)malloc( sizeof( float ) * 9 ) ) == NULL )
	{
		exit( TRUE );
	}

	rawData[0] = m->m11;
	rawData[1] = m->m21;
	rawData[2] = m->m31;
	
	rawData[3] = m->m12;
	rawData[4] = m->m22;
	rawData[5] = m->m32;

	rawData[6] = m->m13;
	rawData[7] = m->m23;
	rawData[8] = m->m33;

	return rawData;
}

INLINE void matrix3x3_setRawData( Matrix3x3 * m, float ( * rawData )[9] )
{
	m->m11 = ( * rawData )[0];
	m->m12 = ( * rawData )[1];
	m->m13 = ( * rawData )[2];

	m->m21 = ( * rawData )[3];
	m->m22 = ( * rawData )[4];
	m->m23 = ( * rawData )[5];

	m->m31 = ( * rawData )[6];
	m->m32 = ( * rawData )[7];
	m->m33 = ( * rawData )[8];

	if( !matrix3x3_determinant( m ) )
	{
		exit( TRUE );
	}
}


Matrix3x3 * newMatrix3x3( float ( * rawData )[9] )
{
	Matrix3x3 * m;

	if( ( m = ( Matrix3x3 * )malloc( sizeof( Matrix3x3 ) ) ) == NULL )
	{
		exit( TRUE );
	}

	if( rawData == NULL )
	{
		matrix3x3_identity( m );
	}
	else
	{
		matrix3x3_setRawData( m, rawData );
	}

	return m;
}

void matrix3x3_destroy(Matrix3x3 **matrix)
{
	free(*matrix);

	*matrix = NULL;
}

INLINE void * matrix3x3_copy( Matrix3x3 * m, Matrix3x3 * src )
{
	m->m11 = src->m11; m->m12 = src->m12; m->m13 = src->m13;

	m->m21 = src->m21; m->m22 = src->m22; m->m23 = src->m23;

	m->m31 = src->m31; m->m32 = src->m32; m->m33 = src->m33;

	return m;
}

INLINE Matrix3x3 * matrix3x3_clone( Matrix3x3 * src )
{
	Matrix3x3 * dest = newMatrix3x3( NULL );

	matrix3x3_copy( dest, src );

	return dest;
}

void Matrix3x3_dispose( Matrix3x3 * m )
{
	memset( m, 0, sizeof( Matrix3x3 ) );
	free( m );
}

INLINE Vector * matrix3x3_getPosition( Vector * output, Matrix3x3 * m )
{
	output->x = m->m31;
	output->y = m->m32;

	return output;
}

INLINE void matrix3x3_setPosition( Matrix3x3 * m, Vector * v )
{
	m->m31 = v->x;
	m->m32 = v->y;
}

INLINE Matrix3x3 * matrix3x3_append( Matrix3x3 * output, Matrix3x3 * m1, Matrix3x3 * m2 )
{
	output->m11 = m1->m11 * m2->m11 + m1->m12 * m2->m21;
	output->m12 = m1->m11 * m2->m12 + m1->m12 * m2->m22;

	output->m21 = m1->m21 * m2->m11 + m1->m22 * m2->m21;
	output->m22 = m1->m21 * m2->m12 + m1->m22 * m2->m22;

	output->m31 = m1->m31 * m2->m11 + m1->m32 * m2->m21 + m2->m31;
	output->m32 = m1->m31 * m2->m12 + m1->m32 * m2->m22 + m2->m32;

	return output;
}

INLINE Matrix3x3 * matrix3x3_append_self( Matrix3x3 * thisMatrix, Matrix3x3 * lhs )
{
	float m11, m12, m13, m21, m22, m23, m31, m32, m33, lhs_m11, lhs_m12, lhs_m13, lhs_m21, lhs_m22, lhs_m23;

	m11 = thisMatrix->m11;	m12 = thisMatrix->m12;	m13 = thisMatrix->m13;
	m21 = thisMatrix->m21;	m22 = thisMatrix->m22;	m23 = thisMatrix->m23;
	m31 = thisMatrix->m31;	m32 = thisMatrix->m32;	m33 = thisMatrix->m33;

	lhs_m11 = lhs->m11;	lhs_m12 = lhs->m12;	lhs_m13 = lhs->m13;
	lhs_m21 = lhs->m21;	lhs_m22 = lhs->m22;	lhs_m23 = lhs->m23;

	thisMatrix->m11 = m11 * lhs_m11 + m12 * lhs_m21;
	thisMatrix->m12 = m11 * lhs_m12 + m12 * lhs_m22;

	thisMatrix->m21 = m21 * lhs_m11 + m22 * lhs_m21;
	thisMatrix->m22 = m21 * lhs_m12 + m22 * lhs_m22;

	thisMatrix->m31 = m31 * lhs_m11 + m32 * lhs_m21 + lhs->m31;
	thisMatrix->m32 = m31 * lhs_m12 + m32 * lhs_m22 + lhs->m32;

	return thisMatrix;
}

INLINE Matrix3x3 * matrix3x3_translationMatrix( Matrix3x3 * output, float x, float y )
{
	output->m12 = output->m13 = output->m21 = output->m23 = output->m31 = output->m32 = 0;
	output->m11 = output->m22 = output->m33 = 1;

	output->m31 = x;
	output->m32 = y;

	return output;
}

INLINE void matrix3x3_appendTranslation( Matrix3x3 * m, float x, float y )
{
	Matrix3x3 input;

	matrix3x3_append_self( m, matrix3x3_translationMatrix( &input, x, y ) );
}

INLINE Matrix3x3 * matrix3x3_rotationMatrix( Matrix3x3 * output, float degrees )
{
	float angle = DEG2RAD(degrees);
	float s = sinf( angle );
	float c = cosf( angle );

	output->m11 = c;		output->m12 = s;
	output->m21 = -s;		output->m22 = c;

	output->m31 = output->m32 = output->m13 = output->m23 =0.0f;
	output->m33 = 1.0f;

	return output;
}

INLINE void matrix3x3_appendRotation( Matrix3x3 * m, float degrees )
{
	Matrix3x3 input;

	matrix3x3_append_self( m, matrix3x3_rotationMatrix( &input, degrees ) );
}

INLINE Matrix3x3 * matrix3x3_scaleMatrix( Matrix3x3 * output, float xScale, float yScale )
{
	output->m11 = xScale; output->m12 = 0.0f;
	output->m21 = 0.0f; output->m22 = yScale;

	output->m31 = output->m32 = output->m13 = output->m23 = 0.0f;
	output->m33 = 1.0f;

	return output;
}

INLINE void matrix3x3_appendScale( Matrix3x3 * m, float xScale, float yScale )
{
	Matrix3x3 input;

	matrix3x3_append_self( m, matrix3x3_scaleMatrix( &input, xScale, yScale ) );
}

INLINE Vector * matrix3x3_transformVector( Vector * output, Matrix3x3 * m, Vector * v )
{
	output->x = m->m11 * v->x + m->m21 * v->y + m->m31;
	output->y = m->m12 * v->x + m->m22 * v->y + m->m32;

	return output;
}

INLINE void matrix3x3_transformVector_self( Matrix3x3 * m, Vector * v )
{
	float x, y;

	x = m->m11 * v->x + m->m21 * v->y + m->m31;
	y = m->m12 * v->x + m->m22 * v->y + m->m32;

	v->x = x;
	v->y = y;
}

/*
============
Matrix4x4
============
*/
typedef struct Matrix4x4
{
	union
	{
		struct 
		{
			float m11, m12, m13, m14;
			float m21, m22, m23, m24;
			float m31, m32, m33, m34;
			float m41, m42, m43, m44;
		};

		float m[4][4];

		float _m[16];
	};
}Matrix4x4;

/**
����ǰ����ת��Ϊ��Ȼ�λ���󡣺�Ⱦ����е����Խ���λ���ϵ�Ԫ�ص�ֵΪһ������������Ԫ�ص�ֵΪ�㡣
���ɵĽ����һ���������У�rawData ֵΪ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1����ת����Ϊ Vector3D(0,0,0)��λ�û�ƽ������Ϊ Vector3D(0,0,0)����������Ϊ Vector3D(1,1,1)��
**/
INLINE Matrix4x4 * matrix4x4_identity( Matrix4x4 * m )
{
	m->m11 = 1.0f; m->m12 = 0.0f; m->m13 = 0.0f; m->m14 = 0.0f;

	m->m21 = 0.0f; m->m22 = 1.0f; m->m23 = 0.0f; m->m24 = 0.0f;

	m->m31 = 0.0f; m->m32 = 0.0f; m->m33 = 1.0f; m->m34 = 0.0f;

	m->m41 = 0.0f; m->m42 = 0.0f; m->m43 = 0.0f; m->m44 = 1.0f;

	return m;
}

/**
����ǰ Matrix4x4 ����ת��Ϊһ�����󣬲����������е��к��С�
���磬�����ǰ Matrix4x4 ����� rawData �������� 16 �����֣�1,2,3,4,11,12,13,14,21,22,23,24,31,32,33,34���� transpose() �����Ὣÿ�ĸ�Ԫ����Ϊһ���ж�ȡ������Щ��ת��Ϊ�С�
���ɵĽ����һ�������� rawData Ϊ��1,11,21,31,2,12,22,32,3,13,23,33,4,14,24,34�� 

transpose() �����Ὣ��ǰ�����滻Ϊת�þ���
���Ҫת�þ��󣬶������ĵ�ǰ��������ʹ�� clone() �������Ƶ�ǰ����Ȼ������ɵĸ���Ӧ�� transpose() ������ 

����������һ�������ξ��󣬸þ����ת�þ�����������ͬ��
**/
INLINE Matrix4x4* matrix4x4_transpose( Matrix4x4 * m )
{
	float m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44;

	m11 = m->m11;	m12 = m->m21;	m13 = m->m31;	m14 = m->m41;
	m21 = m->m12;	m22 = m->m22;	m23 = m->m32;	m24 = m->m42;
	m31 = m->m13;	m32 = m->m23;	m33 = m->m33;	m34 = m->m43;
	m41 = m->m14;	m42 = m->m24;	m43 = m->m34;	m44 = m->m44;

	m->m11 = m11;	m->m12 = m12;	m->m13 = m13;	m->m14 = m14;
	m->m21 = m21;	m->m22 = m22;	m->m23 = m23;	m->m24 = m24;
	m->m31 = m31;	m->m32 = m32;	m->m33 = m33;	m->m34 = m34;
	m->m41 = m41;	m->m42 = m42;	m->m43 = m43;	m->m44 = m44;

	return m;
}

/**
һ������ȷ�������Ƿ��������֡� 

Matrix4x4 ��������ǿ���ġ�����ʹ�� determinant ����ȷ�� Matrix4x4 �����ǿ���ġ�
�������ʽΪ�㣬�����û�������
���磬�������������л���Ϊ�㣬����������л�����ȣ�������ʽΪ�㡣
����ʽ�������ڶ�һϵ�з��̽�����⡣ 
**/
INLINE float matrix4x4_determinant( Matrix4x4 * m )
{
	return m->m11 * ( m->m22 * m->m33 - m->m23 * m->m32 ) + m->m12 * ( m->m23 * m->m31 - m->m21 * m->m33 ) + m->m13 * ( m->m21 * m->m32 - m->m22 * m->m31 );
}

INLINE float matrix4x4_determinant4x4( Matrix4x4 * m )
{
	float det2_01_01, det2_01_02, det2_01_03, det2_01_12, det2_01_13, det2_01_23, det3_201_012, det3_201_013, det3_201_023, det3_201_123;

	det2_01_01 = m->m11 * m->m22 - m->m12 * m->m21;
	det2_01_02 = m->m11 * m->m23 - m->m13 * m->m21;
	det2_01_03 = m->m11 * m->m24 - m->m14 * m->m21;
	det2_01_12 = m->m12 * m->m23 - m->m13 * m->m22;
	det2_01_13 = m->m12 * m->m24 - m->m14 * m->m22;
	det2_01_23 = m->m13 * m->m24 - m->m14 * m->m23;

	det3_201_012 = m->m31 * det2_01_12 - m->m32 * det2_01_02 + m->m33 * det2_01_01;
	det3_201_013 = m->m31 * det2_01_13 - m->m32 * det2_01_03 + m->m34 * det2_01_01;
	det3_201_023 = m->m31 * det2_01_23 - m->m33 * det2_01_03 + m->m34 * det2_01_02;
	det3_201_123 = m->m32 * det2_01_23 - m->m33 * det2_01_13 + m->m34 * det2_01_12;

	return ( - det3_201_123 * m->m41 + det3_201_023 * m->m42 - det3_201_013 * m->m43 + det3_201_012 * m->m44 );
}

INLINE int matrix4x4_invert4x4( Matrix4x4 * m )
{
	float det2_01_01, det2_01_02, det2_01_03, det2_01_12, det2_01_13, det2_01_23, det3_201_012, det3_201_013, det3_201_023, det3_201_123;
	float det2_03_01, det2_03_02, det2_03_03, det2_03_12, det2_03_13, det2_03_23, det2_13_01, det2_13_02, det2_13_03, det2_13_12, det2_13_13, det2_13_23, det3_203_012, det3_203_013, det3_203_023, det3_203_123, det3_213_012, det3_213_013, det3_213_023, det3_213_123, det3_301_012, det3_301_013, det3_301_023, det3_301_123;

	float det, invDet;

	det2_01_01 = m->m11 * m->m22 - m->m12 * m->m21;
	det2_01_02 = m->m11 * m->m23 - m->m13 * m->m21;
	det2_01_03 = m->m11 * m->m24 - m->m14 * m->m21;
	det2_01_12 = m->m12 * m->m23 - m->m13 * m->m22;
	det2_01_13 = m->m12 * m->m24 - m->m14 * m->m22;
	det2_01_23 = m->m13 * m->m24 - m->m14 * m->m23;

	det3_201_012 = m->m31 * det2_01_12 - m->m32 * det2_01_02 + m->m33 * det2_01_01;
	det3_201_013 = m->m31 * det2_01_13 - m->m32 * det2_01_03 + m->m34 * det2_01_01;
	det3_201_023 = m->m31 * det2_01_23 - m->m33 * det2_01_03 + m->m34 * det2_01_02;
	det3_201_123 = m->m32 * det2_01_23 - m->m33 * det2_01_13 + m->m34 * det2_01_12;
	
	det = - det3_201_123 * m->m41 + det3_201_023 * m->m42 - det3_201_013 * m->m43 + det3_201_012 * m->m44;

	if ( fabs( det ) < MATRIX_INVERSE_EPSILON )
	{
		return FALSE;
	}

	invDet = 1.0f / det;

	// remaining 2x2 sub-determinants
	det2_03_01 = m->m11 * m->m42 - m->m12 * m->m41;
	det2_03_02 = m->m11 * m->m43 - m->m13 * m->m41;
	det2_03_03 = m->m11 * m->m44 - m->m14 * m->m41;
	det2_03_12 = m->m12 * m->m43 - m->m13 * m->m42;
	det2_03_13 = m->m12 * m->m44 - m->m14 * m->m42;
	det2_03_23 = m->m13 * m->m44 - m->m14 * m->m43;

	det2_13_01 = m->m21 * m->m42 - m->m22 * m->m41;
	det2_13_02 = m->m21 * m->m43 - m->m23 * m->m41;
	det2_13_03 = m->m21 * m->m44 - m->m24 * m->m41;
	det2_13_12 = m->m22 * m->m43 - m->m23 * m->m42;
	det2_13_13 = m->m22 * m->m44 - m->m24 * m->m42;
	det2_13_23 = m->m23 * m->m44 - m->m24 * m->m43;

	// remaining 3x3 sub-determinants
	det3_203_012 = m->m31 * det2_03_12 - m->m32 * det2_03_02 + m->m33 * det2_03_01;
	det3_203_013 = m->m31 * det2_03_13 - m->m32 * det2_03_03 + m->m34 * det2_03_01;
	det3_203_023 = m->m31 * det2_03_23 - m->m33 * det2_03_03 + m->m34 * det2_03_02;
	det3_203_123 = m->m32 * det2_03_23 - m->m33 * det2_03_13 + m->m34 * det2_03_12;

	det3_213_012 = m->m31 * det2_13_12 - m->m32 * det2_13_02 + m->m33 * det2_13_01;
	det3_213_013 = m->m31 * det2_13_13 - m->m32 * det2_13_03 + m->m34 * det2_13_01;
	det3_213_023 = m->m31 * det2_13_23 - m->m33 * det2_13_03 + m->m34 * det2_13_02;
	det3_213_123 = m->m32 * det2_13_23 - m->m33 * det2_13_13 + m->m34 * det2_13_12;

	det3_301_012 = m->m41 * det2_01_12 - m->m42 * det2_01_02 + m->m43 * det2_01_01;
	det3_301_013 = m->m41 * det2_01_13 - m->m42 * det2_01_03 + m->m44 * det2_01_01;
	det3_301_023 = m->m41 * det2_01_23 - m->m43 * det2_01_03 + m->m44 * det2_01_02;
	det3_301_123 = m->m42 * det2_01_23 - m->m43 * det2_01_13 + m->m44 * det2_01_12;

	m->m11 = - det3_213_123 * invDet;
	m->m21 = + det3_213_023 * invDet;
	m->m31 = - det3_213_013 * invDet;
	m->m41 = + det3_213_012 * invDet;

	m->m12 = + det3_203_123 * invDet;
	m->m22 = - det3_203_023 * invDet;
	m->m32 = + det3_203_013 * invDet;
	m->m42 = - det3_203_012 * invDet;

	m->m13 = + det3_301_123 * invDet;
	m->m23 = - det3_301_023 * invDet;
	m->m33 = + det3_301_013 * invDet;
	m->m43 = - det3_301_012 * invDet;

	m->m14 = - det3_201_123 * invDet;
	m->m24 = + det3_201_023 * invDet;
	m->m34 = - det3_201_013 * invDet;
	m->m44 = + det3_201_012 * invDet;

	return TRUE;
}

INLINE int matrix4x4_fastInvert4x4( Matrix4x4 * m )
{
	Matrix2x2 r0, r1, r2, r3;
	float a, det, invDet;
	//float *mat = reinterpret_cast<float *>(this);

	// r0 = m0.Inverse();
	det = m->_m[0*4+0] * m->_m[1*4+1] - m->_m[0*4+1] * m->_m[1*4+0];

	if ( fabs( det ) < MATRIX_INVERSE_EPSILON )
	{
		return FALSE;
	}

	invDet = 1.0f / det;

	r0.m[0][0] =   m->_m[1*4+1] * invDet;
	r0.m[0][1] = - m->_m[0*4+1] * invDet;
	r0.m[1][0] = - m->_m[1*4+0] * invDet;
	r0.m[1][1] =   m->_m[0*4+0] * invDet;

	// r1 = r0 * m1;
	r1.m[0][0] = r0.m[0][0] * m->_m[0*4+2] + r0.m[0][1] * m->_m[1*4+2];
	r1.m[0][1] = r0.m[0][0] * m->_m[0*4+3] + r0.m[0][1] * m->_m[1*4+3];
	r1.m[1][0] = r0.m[1][0] * m->_m[0*4+2] + r0.m[1][1] * m->_m[1*4+2];
	r1.m[1][1] = r0.m[1][0] * m->_m[0*4+3] + r0.m[1][1] * m->_m[1*4+3];

	// r2 = m2 * r1;
	r2.m[0][0] = m->_m[2*4+0] * r1.m[0][0] + m->_m[2*4+1] * r1.m[1][0];
	r2.m[0][1] = m->_m[2*4+0] * r1.m[0][1] + m->_m[2*4+1] * r1.m[1][1];
	r2.m[1][0] = m->_m[3*4+0] * r1.m[0][0] + m->_m[3*4+1] * r1.m[1][0];
	r2.m[1][1] = m->_m[3*4+0] * r1.m[0][1] + m->_m[3*4+1] * r1.m[1][1];

	// r3 = r2 - m3;
	r3.m[0][0] = r2.m[0][0] - m->_m[2*4+2];
	r3.m[0][1] = r2.m[0][1] - m->_m[2*4+3];
	r3.m[1][0] = r2.m[1][0] - m->_m[3*4+2];
	r3.m[1][1] = r2.m[1][1] - m->_m[3*4+3];

	// r3.InverseSelf();
	det = r3.m[0][0] * r3.m[1][1] - r3.m[0][1] * r3.m[1][0];

	if ( fabs( det ) < MATRIX_INVERSE_EPSILON )
	{
		return FALSE;
	}

	invDet = 1.0f / det;

	a = r3.m[0][0];
	r3.m[0][0] =   r3.m[1][1] * invDet;
	r3.m[0][1] = - r3.m[0][1] * invDet;
	r3.m[1][0] = - r3.m[1][0] * invDet;
	r3.m[1][1] =   a * invDet;

	// r2 = m2 * r0;
	r2.m[0][0] = m->_m[2*4+0] * r0.m[0][0] + m->_m[2*4+1] * r0.m[1][0];
	r2.m[0][1] = m->_m[2*4+0] * r0.m[0][1] + m->_m[2*4+1] * r0.m[1][1];
	r2.m[1][0] = m->_m[3*4+0] * r0.m[0][0] + m->_m[3*4+1] * r0.m[1][0];
	r2.m[1][1] = m->_m[3*4+0] * r0.m[0][1] + m->_m[3*4+1] * r0.m[1][1];

	// m2 = r3 * r2;
	m->_m[2*4+0] = r3.m[0][0] * r2.m[0][0] + r3.m[0][1] * r2.m[1][0];
	m->_m[2*4+1] = r3.m[0][0] * r2.m[0][1] + r3.m[0][1] * r2.m[1][1];
	m->_m[3*4+0] = r3.m[1][0] * r2.m[0][0] + r3.m[1][1] * r2.m[1][0];
	m->_m[3*4+1] = r3.m[1][0] * r2.m[0][1] + r3.m[1][1] * r2.m[1][1];

	// m0 = r0 - r1 * m2;
	m->_m[0*4+0] = r0.m[0][0] - r1.m[0][0] * m->_m[2*4+0] - r1.m[0][1] * m->_m[3*4+0];
	m->_m[0*4+1] = r0.m[0][1] - r1.m[0][0] * m->_m[2*4+1] - r1.m[0][1] * m->_m[3*4+1];
	m->_m[1*4+0] = r0.m[1][0] - r1.m[1][0] * m->_m[2*4+0] - r1.m[1][1] * m->_m[3*4+0];
	m->_m[1*4+1] = r0.m[1][1] - r1.m[1][0] * m->_m[2*4+1] - r1.m[1][1] * m->_m[3*4+1];

	// m1 = r1 * r3;
	m->_m[0*4+2] = r1.m[0][0] * r3.m[0][0] + r1.m[0][1] * r3.m[1][0];
	m->_m[0*4+3] = r1.m[0][0] * r3.m[0][1] + r1.m[0][1] * r3.m[1][1];
	m->_m[1*4+2] = r1.m[1][0] * r3.m[0][0] + r1.m[1][1] * r3.m[1][0];
	m->_m[1*4+3] = r1.m[1][0] * r3.m[0][1] + r1.m[1][1] * r3.m[1][1];

	// m3 = -r3;
	m->_m[2*4+2] = -r3.m[0][0];
	m->_m[2*4+3] = -r3.m[0][1];
	m->_m[3*4+2] = -r3.m[1][0];
	m->_m[3*4+3] = -r3.m[1][1];

	return TRUE;
}

/**
��ת��ǰ����
����������ԭʼ������ͬ�Ĵ�С������ִ�е�ת����ԭʼ�����෴��
���磬�����ԭʼ�����У�һ������ĳ�������� x ����ת����ô��������У��ö��󽫰��˷���ķ������� x ����ת��
��һ������Ӧ�������ɳ���ԭʼ����ִ�е�ת���������һ�����������������ˣ��򽫲���һ����Ⱦ��� 

���þ��������󣬿��Խ�һ�����������һ������ͨ�������� A ����� B ���������ˣ����Խ����� A ���Ծ��� B��
����󻹿�����������ͷ�ռ䡣
������ͷ����ʵ����ռ����ƶ�ʱ����ʵ�����еĶ����谴������ͷ�෴�ķ����ƶ����������ܴ���ʵ�����ӵ�ת��������ͷ���ӵ�ռ䡣
���磬�������ͷ�ƶ���Խ�����������Խ��
���仰˵���������ͷ����ʵ����� z �����ƣ��������ش� z �����ơ�

invert() �����Ὣ��ǰ�����滻Ϊ�����
���Ҫ��ת���󣬶������ĵ�ǰ��������ʹ�� clone() �������Ƶ�ǰ����Ȼ������ɵĸ���Ӧ�� invert() ������ 

Matrix4x4 ��������ǿ���ġ�
**/
INLINE int matrix4x4_invert( Matrix4x4 * m )
{
	float m11, m12, m13, m21, m22, m23, m31, m32, m33, m41, m42, m43;

	float det = matrix4x4_determinant( m );

	if ( fabs( det ) < MATRIX_INVERSE_EPSILON )
	{
		return FALSE;
	}

	m11 = m->m11; m21 = m->m21; m31 = m->m31; m41 = m->m41;
	m12 = m->m12; m22 = m->m22; m32 = m->m32; m42 = m->m42;
	m13 = m->m13; m23 = m->m23; m33 = m->m33; m43 = m->m43;

	det = 1.0f / det;

	m->m11 = (m22 * m33 - m23 * m32) * det;
	m->m12 = (m13 * m32 - m12 * m33) * det;
	m->m13 = (m12 * m23 - m13 * m22) * det;

	m->m21 = (m23 * m31 - m21 * m33) * det;
	m->m22 = (m11 * m33 - m13 * m31) * det;
	m->m23 = (m13 * m21 - m11 * m23) * det;

	m->m31 = (m21 * m32 - m22 * m31) * det;
	m->m32 = (m12 * m31 - m11 * m32) * det;
	m->m33 = (m11 * m22 - m12 * m21) * det;

	m->m41 = -(m41 * m->m11 + m42 * m->m21 + m43 * m->m31);
	m->m42 = -(m41 * m->m12 + m42 * m->m22 + m43 * m->m32);
	m->m43 = -(m41 * m->m13 + m42 * m->m23 + m43 * m->m33);

	return TRUE;
}

INLINE int matrix4x4_fastInvert( Matrix4x4 * m )
{
	float det, invDet, det_00, det_10, det_20, det_01, det_02, det_11, det_12, det_21, det_22;
	float m11, m12, m13, m21, m22, m23, m31, m32, m33, m41, m42, m43;

	m11 = m->m11; m21 = m->m21; m31 = m->m31; m41 = m->m41;
	m12 = m->m12; m22 = m->m22; m32 = m->m32; m42 = m->m42;
	m13 = m->m13; m23 = m->m23; m33 = m->m33; m43 = m->m43;

	det_00 = m22 * m33 - m23 * m32;
	det_10 = m23 * m31 - m21 * m33;
	det_20 = m21 * m32 - m22 * m31;

	det = m11 * det_00 + m12 * det_10 + m13 * det_20;

	if ( fabs( det ) < MATRIX_INVERSE_EPSILON )
	{
		return FALSE;
	}

	invDet = 1.0f / det;

	det_01 = m13 * m32 - m12 * m33;
	det_02 = m12 * m23 - m13 * m22;
	det_11 = m11 * m33 - m13 * m31;
	det_12 = m13 * m21 - m11 * m23;
	det_21 = m12 * m31 - m11 * m32;
	det_22 = m11 * m22 - m12 * m21;

	m->m11 = det_00 * invDet;
	m->m12 = det_01 * invDet;
	m->m13 = det_02 * invDet;

	m->m21 = det_10 * invDet;
	m->m22 = det_11 * invDet;
	m->m23 = det_12 * invDet;

	m->m31 = det_20 * invDet;
	m->m32 = det_21 * invDet;
	m->m33 = det_22 * invDet;
	
	m->m41 = -(m41 * m->m11 + m42 * m->m21 + m43 * m->m31);
	m->m42 = -(m41 * m->m12 + m42 * m->m22 + m43 * m->m32);
	m->m43 = -(m41 * m->m13 + m42 * m->m23 + m43 * m->m33);

	return TRUE;
}

/**
һ���� 16 ��������ɵ�ʸ�������У�ÿ�ĸ�Ԫ�ؿ����� 4x4 �����һ�л�һ�С� 

��� rawData ��������Ϊһ��������ľ�����������쳣��Matrix3D ��������ǿ���ġ�
**/
INLINE float * matrix4x4_getRawData( Matrix4x4 * m )
{
	float * rawData;

	if( ( rawData = (float *)malloc( sizeof( float ) * 16 ) ) == NULL )
	{
		exit( TRUE );
	}

	rawData[0]  = m->m11;
	rawData[1]  = m->m21;
	rawData[2]  = m->m31;
	rawData[3]  = m->m41;
	
	rawData[4]  = m->m12;
	rawData[5]  = m->m22;
	rawData[6]  = m->m32;
	rawData[7]  = m->m42;

	rawData[8]  = m->m13;
	rawData[9]  = m->m23;
	rawData[10] = m->m33;
	rawData[11] = m->m43;

	rawData[12] = m->m14;
	rawData[13] = m->m24;
	rawData[14] = m->m34;
	rawData[15] = m->m44;

	return rawData;
}

/**
һ���� 16 ��������ɵ�ʸ�������У�ÿ�ĸ�Ԫ�ؿ����� 4x4 �����һ�л�һ�С� 

��� rawData ��������Ϊһ��������ľ�����������쳣��Matrix3D ��������ǿ���ġ�
**/
INLINE void matrix4x4_setRawData( Matrix4x4 * m, float ( * rawData )[16] )
{
	m->m11 = ( * rawData )[0];
	m->m12 = ( * rawData )[1];
	m->m13 = ( * rawData )[2];
	m->m14 = ( * rawData )[3];

	m->m21 = ( * rawData )[4];
	m->m22 = ( * rawData )[5];
	m->m23 = ( * rawData )[6];
	m->m24 = ( * rawData )[7];

	m->m31 = ( * rawData )[8];
	m->m32 = ( * rawData )[9];
	m->m33 = ( * rawData )[10];
	m->m34 = ( * rawData )[11];

	m->m41 = ( * rawData )[12];
	m->m42 = ( * rawData )[13];
	m->m43 = ( * rawData )[14];
	m->m44 = ( * rawData )[15];

	if( !matrix4x4_determinant( m ) )
	{
		exit( TRUE );
	}
}

/**
���� Matrix4x4 ���󡣿���ʹ��һ���� 16 ��������ɵ�ʸ������ʼ�� Matrix4x4 �������У�ÿ�ĸ�Ԫ�ؿ�����һ�л�һ�С�
���� Matrix4x4 ����֮�󣬿���ʹ�� rawData ���Է��ʸö���ľ���Ԫ�ء� 

���δ�����κβ��������캯��������һ����Ȼ�λ Matrix4x4 ����
�ھ����ʾ���У���Ⱦ����е����Խ���λ���ϵ�����Ԫ�ص�ֵ��Ϊһ������������Ԫ�ص�ֵ��Ϊ�㡣
��Ⱦ���� rawData ���Ե�ֵΪ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1��
��Ⱦ����λ�û�ƽ��ֵΪ Vector3D(0,0,0)����ת����Ϊ Vector3D(0,0,0)������ֵΪ Vector3D(1,1,1)��
**/
Matrix4x4 * newMatrix4x4( float ( * rawData )[16] )
{
	Matrix4x4 * m;

	if( ( m = ( Matrix4x4 * )malloc( sizeof( Matrix4x4 ) ) ) == NULL )
	{
		exit( TRUE );
	}

	if( rawData == NULL )
	{
		matrix4x4_identity( m );
	}
	else
	{
		matrix4x4_setRawData( m, rawData );
	}

	return m;
}

void matrix4x4_destroy(Matrix4x4 **matrix)
{
	free(*matrix);

	*matrix = NULL;
}

/**
���ƾ���
**/
INLINE void * matrix4x4_copy( Matrix4x4 * m, Matrix4x4 * src )
{
	m->m11 = src->m11; m->m12 = src->m12; m->m13 = src->m13; m->m14 = src->m14;

	m->m21 = src->m21; m->m22 = src->m22; m->m23 = src->m23; m->m24 = src->m24;

	m->m31 = src->m31; m->m32 = src->m32; m->m33 = src->m33; m->m34 = src->m34;

	m->m41 = src->m41; m->m42 = src->m42; m->m43 = src->m43; m->m44 = src->m44;

	//memcpy( m, src, sizeof( * src ) );

	return m;
}

INLINE Matrix4x4 * matrix4x4_clone( Matrix4x4 * src )
{
	Matrix4x4 * dest = newMatrix4x4( NULL );

	matrix4x4_copy( dest, src );

	return dest;
}

void matrix4x4_dispose( Matrix4x4 * m )
{
	memset( m, 0, sizeof( Matrix4x4 ) );
	free( m );
}


/**
һ��������ʾ������ת������֡�е� 3D ���� (x,y,z) λ�õ� Vector3D ����
���� position ���ԣ�������������ʾ��������ƽ��ʸ����������ֽ��������Ͼ��� 

���� position ���ԣ����Ի�ȡ������ת�������ƽ��Ԫ�ء�
**/
INLINE Vector3D * matrix4x4_getPosition( Vector3D * output, Matrix4x4 * m )
{
	output->x = m->m41;
	output->y = m->m42;
	output->z = m->m43;
	//output->w = m->m44;

	return output;
}

/**
һ��������ʾ������ת������֡�е� 3D ���� (x,y,z) λ�õ� Vector3D ����
���� position ���ԣ�������������ʾ��������ƽ��ʸ����������ֽ��������Ͼ��� 

���� position ���ԣ����Ի�ȡ������ת�������ƽ��Ԫ�ء�
**/
INLINE void matrix4x4_setPosition( Matrix4x4 * m, Vector3D * v )
{
	m->m41 = v->x;
	m->m42 = v->y;
	m->m43 = v->z;
	m->m44 = v->w;
}

/**
ͨ������һ�� Matrix4x4 �����뵱ǰ Matrix4x4 �������������һ������
�õ��Ľ�����ϲ���������ת�������Խ�һ�� Matrix4x4 ��������������ˡ����յ� Matrix4x4 ���󽫰�������ת���Ľ���� 

����˷����������ӷ����㲻ͬ������˷������ǲ��ɽ����ġ�
���仰˵��A ���� B �������� B ���� A��
��ʹ�� append() ����ʱ���˷����㽫����࿪ʼ������ζ�� lhs Matrix4x4 ����λ�ڳ˷����������ࡣ 

thisMatrix = lhs * thisMatrix; 
�״ε��� append() ����ʱ���˷�����Ը����ռ��������޸ġ�
������������õ� Matrix4x4 ����Ĳ���֡��ء� 

append() �����Ὣ��ǰ�����滻Ϊ���õľ���
���Ҫ�����������󣬶������ĵ�ǰ������ʹ�� clone() �������Ƶ�ǰ����Ȼ������ɵĸ���Ӧ�� append() ������ 
**/
INLINE Matrix4x4 * matrix4x4_append( Matrix4x4 * output, Matrix4x4 * m1, Matrix4x4 * m2 )
{
	output->m11 = m1->m11 * m2->m11 + m1->m12 * m2->m21 + m1->m13 * m2->m31;
	output->m12 = m1->m11 * m2->m12 + m1->m12 * m2->m22 + m1->m13 * m2->m32;
	output->m13 = m1->m11 * m2->m13 + m1->m12 * m2->m23 + m1->m13 * m2->m33;

	output->m21 = m1->m21 * m2->m11 + m1->m22 * m2->m21 + m1->m23 * m2->m31;
	output->m22 = m1->m21 * m2->m12 + m1->m22 * m2->m22 + m1->m23 * m2->m32;
	output->m23 = m1->m21 * m2->m13 + m1->m22 * m2->m23 + m1->m23 * m2->m33;

	output->m31 = m1->m31 * m2->m11 + m1->m32 * m2->m21 + m1->m33 * m2->m31;
	output->m32 = m1->m31 * m2->m12 + m1->m32 * m2->m22 + m1->m33 * m2->m32;
	output->m33 = m1->m31 * m2->m13 + m1->m32 * m2->m23 + m1->m33 * m2->m33;

	output->m41 = m1->m41 * m2->m11 + m1->m42 * m2->m21 + m1->m43 * m2->m31 + m2->m41;
	output->m42 = m1->m41 * m2->m12 + m1->m42 * m2->m22 + m1->m43 * m2->m32 + m2->m42;
	output->m43 = m1->m41 * m2->m13 + m1->m42 * m2->m23 + m1->m43 * m2->m33 + m2->m43;

	return output;
}

INLINE Matrix4x4 * matrix4x4_append_self( Matrix4x4 * thisMatrix, Matrix4x4 * lhs )
{
	float m11, m12, m13, m21, m22, m23, m31, m32, m33, m41, m42, m43, lhs_m11, lhs_m12, lhs_m13, lhs_m21, lhs_m22, lhs_m23, lhs_m31, lhs_m32, lhs_m33;

	m11 = thisMatrix->m11;	m12 = thisMatrix->m12;	m13 = thisMatrix->m13;
	m21 = thisMatrix->m21;	m22 = thisMatrix->m22;	m23 = thisMatrix->m23;
	m31 = thisMatrix->m31;	m32 = thisMatrix->m32;	m33 = thisMatrix->m33;
	m41 = thisMatrix->m41;	m42 = thisMatrix->m42;	m43 = thisMatrix->m43;

	lhs_m11 = lhs->m11;	lhs_m12 = lhs->m12;	lhs_m13 = lhs->m13;
	lhs_m21 = lhs->m21;	lhs_m22 = lhs->m22;	lhs_m23 = lhs->m23;
	lhs_m31 = lhs->m31;	lhs_m32 = lhs->m32;	lhs_m33 = lhs->m33;

	thisMatrix->m11 = m11 * lhs_m11 + m12 * lhs_m21 + m13 * lhs_m31;
	thisMatrix->m12 = m11 * lhs_m12 + m12 * lhs_m22 + m13 * lhs_m32;
	thisMatrix->m13 = m11 * lhs_m13 + m12 * lhs_m23 + m13 * lhs_m33;

	thisMatrix->m21 = m21 * lhs_m11 + m22 * lhs_m21 + m23 * lhs_m31;
	thisMatrix->m22 = m21 * lhs_m12 + m22 * lhs_m22 + m23 * lhs_m32;
	thisMatrix->m23 = m21 * lhs_m13 + m22 * lhs_m23 + m23 * lhs_m33;

	thisMatrix->m31 = m31 * lhs_m11 + m32 * lhs_m21 + m33 * lhs_m31;
	thisMatrix->m32 = m31 * lhs_m12 + m32 * lhs_m22 + m33 * lhs_m32;
	thisMatrix->m33 = m31 * lhs_m13 + m32 * lhs_m23 + m33 * lhs_m33;

	thisMatrix->m41 = m41 * lhs_m11 + m42 * lhs_m21 + m43 * lhs_m31 + lhs->m41;
	thisMatrix->m42 = m41 * lhs_m12 + m42 * lhs_m22 + m43 * lhs_m32 + lhs->m42;
	thisMatrix->m43 = m41 * lhs_m13 + m42 * lhs_m23 + m43 * lhs_m33 + lhs->m43;

	return thisMatrix;
}

INLINE Matrix4x4 * matrix4x4_append4x4( Matrix4x4 * output, Matrix4x4 * thisMatrix, Matrix4x4 * lhs )
{
	float m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44;
	float lhs_m11, lhs_m12, lhs_m13, lhs_m14, lhs_m21, lhs_m22, lhs_m23, lhs_m24, lhs_m31, lhs_m32, lhs_m33, lhs_m34, lhs_m41, lhs_m42, lhs_m43, lhs_m44;

	m11 = thisMatrix->m11;	m12 = thisMatrix->m12;	m13 = thisMatrix->m13;	m14 = thisMatrix->m14;
	m21 = thisMatrix->m21;	m22 = thisMatrix->m22;	m23 = thisMatrix->m23;	m24 = thisMatrix->m24;
	m31 = thisMatrix->m31;	m32 = thisMatrix->m32;	m33 = thisMatrix->m33;	m34 = thisMatrix->m34;
	m41 = thisMatrix->m41;	m42 = thisMatrix->m42;	m43 = thisMatrix->m43;	m44 = thisMatrix->m44;

	lhs_m11 = lhs->m11;	lhs_m12 = lhs->m12;	lhs_m13 = lhs->m13;	lhs_m14 = lhs->m14;
	lhs_m21 = lhs->m21;	lhs_m22 = lhs->m22;	lhs_m23 = lhs->m23;	lhs_m24 = lhs->m24;
	lhs_m31 = lhs->m31;	lhs_m32 = lhs->m32;	lhs_m33 = lhs->m33;	lhs_m34 = lhs->m34;
	lhs_m41 = lhs->m41;	lhs_m42 = lhs->m42;	lhs_m43 = lhs->m43;	lhs_m44 = lhs->m44;

	output->m11 = m11 * lhs_m11 + m12 * lhs_m21 + m13 * lhs_m31;
	output->m12 = m11 * lhs_m12 + m12 * lhs_m22 + m13 * lhs_m32;
	output->m13 = m11 * lhs_m13 + m12 * lhs_m23 + m13 * lhs_m33;
	output->m14 = m11 * lhs_m14 + m12 * lhs_m24 + m13 * lhs_m34;

	output->m21 = m21 * lhs_m11 + m22 * lhs_m21 + m23 * lhs_m31;
	output->m22 = m21 * lhs_m12 + m22 * lhs_m22 + m23 * lhs_m32;
	output->m23 = m21 * lhs_m13 + m22 * lhs_m23 + m23 * lhs_m33;
	output->m24 = m21 * lhs_m14 + m22 * lhs_m24 + m23 * lhs_m34;

	output->m31 = m31 * lhs_m11 + m32 * lhs_m21 + m33 * lhs_m31;
	output->m32 = m31 * lhs_m12 + m32 * lhs_m22 + m33 * lhs_m32;
	output->m33 = m31 * lhs_m13 + m32 * lhs_m23 + m33 * lhs_m33;
	output->m34 = m31 * lhs_m14 + m32 * lhs_m24 + m33 * lhs_m34;

	output->m41 = m41 * lhs_m11 + m42 * lhs_m21 + m43 * lhs_m31 + lhs->m41;
	output->m42 = m41 * lhs_m12 + m42 * lhs_m22 + m43 * lhs_m32 + lhs->m42;
	output->m43 = m41 * lhs_m13 + m42 * lhs_m23 + m43 * lhs_m33 + lhs->m43;
	output->m44 = m41 * lhs_m14 + m42 * lhs_m24 + m43 * lhs_m34 + lhs->m44;

	return output;
}

INLINE Matrix4x4 * matrix4x4_append4x4_self( Matrix4x4 * thisMatrix, Matrix4x4 * lhs )
{
	float m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44;
	float lhs_m11, lhs_m12, lhs_m13, lhs_m14, lhs_m21, lhs_m22, lhs_m23, lhs_m24, lhs_m31, lhs_m32, lhs_m33, lhs_m34, lhs_m41, lhs_m42, lhs_m43, lhs_m44;

	m11 = thisMatrix->m11;	m12 = thisMatrix->m12;	m13 = thisMatrix->m13;	m14 = thisMatrix->m14;
	m21 = thisMatrix->m21;	m22 = thisMatrix->m22;	m23 = thisMatrix->m23;	m24 = thisMatrix->m24;
	m31 = thisMatrix->m31;	m32 = thisMatrix->m32;	m33 = thisMatrix->m33;	m34 = thisMatrix->m34;
	m41 = thisMatrix->m41;	m42 = thisMatrix->m42;	m43 = thisMatrix->m43;	m44 = thisMatrix->m44;

	lhs_m11 = lhs->m11;	lhs_m12 = lhs->m12;	lhs_m13 = lhs->m13;	lhs_m14 = lhs->m14;
	lhs_m21 = lhs->m21;	lhs_m22 = lhs->m22;	lhs_m23 = lhs->m23;	lhs_m24 = lhs->m24;
	lhs_m31 = lhs->m31;	lhs_m32 = lhs->m32;	lhs_m33 = lhs->m33;	lhs_m34 = lhs->m34;
	lhs_m41 = lhs->m41;	lhs_m42 = lhs->m42;	lhs_m43 = lhs->m43;	lhs_m44 = lhs->m44;

	thisMatrix->m11 = m11 * lhs_m11 + m12 * lhs_m21 + m13 * lhs_m31;
	thisMatrix->m12 = m11 * lhs_m12 + m12 * lhs_m22 + m13 * lhs_m32;
	thisMatrix->m13 = m11 * lhs_m13 + m12 * lhs_m23 + m13 * lhs_m33;
	thisMatrix->m14 = m11 * lhs_m14 + m12 * lhs_m24 + m13 * lhs_m34;

	thisMatrix->m21 = m21 * lhs_m11 + m22 * lhs_m21 + m23 * lhs_m31;
	thisMatrix->m22 = m21 * lhs_m12 + m22 * lhs_m22 + m23 * lhs_m32;
	thisMatrix->m23 = m21 * lhs_m13 + m22 * lhs_m23 + m23 * lhs_m33;
	thisMatrix->m24 = m21 * lhs_m14 + m22 * lhs_m24 + m23 * lhs_m34;

	thisMatrix->m31 = m31 * lhs_m11 + m32 * lhs_m21 + m33 * lhs_m31;
	thisMatrix->m32 = m31 * lhs_m12 + m32 * lhs_m22 + m33 * lhs_m32;
	thisMatrix->m33 = m31 * lhs_m13 + m32 * lhs_m23 + m33 * lhs_m33;
	thisMatrix->m34 = m31 * lhs_m14 + m32 * lhs_m24 + m33 * lhs_m34;

	thisMatrix->m41 = m41 * lhs_m11 + m42 * lhs_m21 + m43 * lhs_m31 + lhs->m41;
	thisMatrix->m42 = m41 * lhs_m12 + m42 * lhs_m22 + m43 * lhs_m32 + lhs->m42;
	thisMatrix->m43 = m41 * lhs_m13 + m42 * lhs_m23 + m43 * lhs_m33 + lhs->m43;
	thisMatrix->m44 = m41 * lhs_m14 + m42 * lhs_m24 + m43 * lhs_m34 + lhs->m44;

	return thisMatrix;
}

/**
��ȡһ��λ�ƾ���
**/
INLINE Matrix4x4 * matrix4x4_translationMatrix( Matrix4x4 * output, float x, float y, float z )
{
	output->m12 = output->m13 = output->m14 = output->m21 = output->m23 = output->m24 = output->m31 = output->m32 = output->m34 = 0;
	output->m11= output->m22 = output->m33 = output->m44 =  1;

	output->m41 = x;
	output->m42 = y;
	output->m43 = z;

	return output;
}

/**
�� Matrix4x4 �����Ϻ���һ������ƽ�ƣ��� x��y �� z �����¶�λ��
�ڽ� Matrix4x4 ����Ӧ������ʾ����ʱ��������� Matrix4x4 ��������ִ������ת����Ȼ����ִ��ƽ�Ƹ��ġ� 

ƽ�ƽ���Ϊ�������ᣨx��y��z�����е������������ļ����ж��塣
�ڶ���ʾ����Ӧ��ת��ʱ����ʾ�����ӵ�ǰλ���� x��y �� z �ᰴ����ָ���������ƶ���
��Ҫȷ��ƽ��ֻӰ���ض����ᣬ�뽫������������Ϊ�㡣����Ϊ���ʾ�����ض���������κθ��ġ�

ƽ�Ƹ��Ĳ��Ǿ��Ը��ġ�
���������ĵ�ǰλ�úͷ�����ء�
��Ҫȷ����ת��������о��Ը��ģ���ʹ�� recompose() ������ת����˳��Ҳ����Ҫ��
��ƽ������ת��ת����������Ч��������ת��ƽ����������Ч����ͬ�� 
**/
INLINE void matrix4x4_appendTranslation( Matrix4x4 * m, float x, float y, float z )
{
	Matrix4x4 input;

	matrix4x4_append_self( m, matrix4x4_translationMatrix( &input, x, y, z ) );
}

INLINE Matrix4x4 * matrix4x4_rotationMatrix( Matrix4x4 * output,  float degrees, int axis )
{
	float angle = DEG2RAD(degrees);
	float s = sinf( angle );
	float c = cosf( angle );

	switch (axis)
	{
		case 1: // Rotate about the x-axis

			output->m11 = 1.0f;	output->m12 = 0.0f;		output->m13 = 0.0f;
			output->m21 = 0.0f;	output->m22 = c;		output->m23 = s;
			output->m31 = 0.0f;	output->m32 = -s;		output->m33 = c;
			break;

		case 2: // Rotate about the y-axis

			output->m11 = c;		output->m12 = 0.0f;	output->m13 = -s;
			output->m21 = 0.0f;		output->m22 = 1.0f;	output->m23 = 0.0f;
			output->m31 = s;		output->m32 = 0.0f;	output->m33 = c;
			break;

		case 3: // Rotate about the z-axis

			output->m11 = c;		output->m12 = s;		output->m13 = 0.0f;
			output->m21 = -s;		output->m22 = c;		output->m23 = 0.0f;
			output->m31 = 0.0f;		output->m32 = 0.0f;		output->m33 = 1.0f;
			break;
	}

	output->m41 = output->m42 = output->m43 = output->m14 = output->m24 = output->m34 = 0.0f;
	output->m44 = 1.0f;

	return output;
}

/**
�� Matrix4x4 ������ǰ��һ��������ת��
�ڽ� Matrix4x4 ����Ӧ������ʾ����ʱ��������� Matrix4x4 ��������ִ����ת��Ȼ����ִ������ת���� 

��ʾ�������ת������Ԫ�ض��壺һ���ᡢ�Ƹ�����ת�������ǶȺͶ�����ת���ĵĿ�ѡ��㡣
��������κγ��淽��
��������Ϊ XAXIS (Vector3D(1,0,0))��YAXIS (Vector3D(0,1,0)) �� ZAXIS (Vector3D(0,0,1))��
�ں��������У��й� y �����ת��Ϊƫ�����й� x �����ת��Ϊ�������й� z �����ת��Ϊ������ 

ת����˳�����Ҫ��
����ת��ƽ�Ƶ�ת����������Ч������ƽ������ת��������Ч����ͬ��

��תЧ�����Ǿ���Ч����
��Ч��������йأ�����ԭʼλ�úͷ���Ĳ���֡��ԡ���Ҫȷ����ת�����о��Ը��ģ���ʹ�� recompose() ������
**/
INLINE void matrix4x4_appendRotation( Matrix4x4 * m, float degrees, int axis )
{
	Matrix4x4 input;

	matrix4x4_append_self( m, matrix4x4_rotationMatrix( &input,  degrees, axis ) );
}

/**
��ȡһ����ת����.
**/
INLINE Matrix4x4 * matrix4x4_rotationMatrix3DByAxis( Matrix4x4 * output,  float degrees, Vector3D * axis )
{
	float angle = DEG2RAD(degrees);
	float s = sinf( angle );
	float c = cosf( angle );

	float a = 1.0f - c;

	float x = axis->x;
	float y = axis->y;
	float z = axis->z;

	float ax = a * x;
	float ay = a * y;
	float az = a * z;

	output->m11 = ax * x + c;
	output->m12 = ax * y + z * s;
	output->m13 = ax * z - y * s;

	output->m21 = ay * x - z * s;
	output->m22 = ay * y + c;
	output->m23 = ay * z + x * s;

	output->m31 = az * x + y * s;
	output->m32 = az * y - x * s;
	output->m33 = az * z + c;

	output->m41 = output->m42 = output->m43 = output->m14 = output->m24 = output->m34 = 0.0f;
	output->m44 = 1.0f;

	return output;
}

INLINE void matrix4x4_appendRotationByAxis( Matrix4x4 * m, float degrees, Vector3D * axis )
{
	Matrix4x4 input;

	matrix4x4_append_self( m, matrix4x4_rotationMatrix3DByAxis( &input, degrees, axis ) );
}

/**
����һ�����ž���.
**/
INLINE Matrix4x4 * matrix4x4_scaleMatrix( Matrix4x4 * output, float xScale, float yScale, float zScale )
{
	output->m11 = xScale; output->m12 = 0.0f; output->m13 = 0.0f;
	output->m21 = 0.0f; output->m22 = yScale; output->m23 = 0.0f;
	output->m31 = 0.0f; output->m32 = 0.0f; output->m33 = zScale;

	output->m41 = output->m42 = output->m43 = output->m14 = output->m24 = output->m34 = 0.0f;
	output->m44 = 1;

	return output;
}

/**
�� Matrix4x4 �����Ϻ���һ���������ţ��� x��y �� z ��ı�λ�á�
�ڽ� Matrix4x4 ����Ӧ������ʾ����ʱ��������� Matrix4x4 ��������ִ������ת����Ȼ����ִ�����Ÿ��ġ�
Ĭ�ϵ�����ϵ��Ϊ (1.0f, 1.0f, 1.0f)�� 

���Ž���Ϊ�������ᣨx��y��z�����е������������ļ����ж��塣���Խ�ÿ�����벻ͬ��������ˡ�
�ڽ����Ÿ���Ӧ������ʾ����ʱ���ö���Ĵ�С��������С��
���磬�� x��y �� z ������Ϊ 2 ��ʹ�����СΪԭ��������������������Ϊ 0.5 ��ʹ����Ĵ�СΪԭ����һ�롣
��Ҫȷ������ת��ֻӰ���ض����ᣬ�뽫������������Ϊ 1������Ϊ 1 ��ʾ�����ض���������κ����Ÿ��ġ� 

appendScale() ���������ڵ�����С�͹���Ť����������ʾ������������������������ĳ��λ���ϵķŴ����С��
����ʾ�������ת��ƽ�ƹ����У����Զ�ִ������ת���� 

ת����˳�����Ҫ��
�ȵ�����С��ƽ�Ƶ�ת����������Ч������ƽ���ٵ�����С��ת����������Ч����ͬ��
**/

INLINE void matrix4x4_appendScale( Matrix4x4 * m, float xScale, float yScale, float zScale )
{
	Matrix4x4 input;

	matrix4x4_append_self( m, matrix4x4_scaleMatrix( &input, xScale, yScale, zScale ) );
}

/**
����һ����X����ת����
**/
INLINE Matrix4x4 * matrix4x4_rotationXMatrix( Matrix4x4 * output, float theta )
{
	float c, s;
	sinCos(&s, &c, theta);

	output->m11 = 1.0f;	output->m12 = 0.0f;	output->m13 = 0.0f;
	output->m21 = 0.0f;	output->m22 = c;	output->m23 = s;
	output->m31 = 0.0f;	output->m32 = -s;	output->m33 = c;

	output->m41 = output->m42 = output->m43 = output->m14 = output->m24 = output->m34 = 0.0f;
	output->m44 = 1.0f;

	return output;
}

/**
����һ����Y����ת����
**/
INLINE Matrix4x4 * matrix4x4_rotationYMatrix( Matrix4x4 * output, float theta )
{
	float c, s;
	sinCos(&s, &c, theta);

	output->m11 = c;	output->m12 = 0.0f;	output->m13 = -s;
	output->m21 = 0.0f;	output->m22 = 1.0f;	output->m23 = 0.0f;
	output->m31 = s;	output->m32 = 0.0f;	output->m33 = c;

	output->m41 = output->m42 = output->m43 = output->m14 = output->m24 = output->m34 = 0.0f;
	output->m44 = 1.0f;

	return output;
}

/**
����һ����Z����ת����
**/
INLINE Matrix4x4 * matrix4x4_rotationZMatrix3D( Matrix4x4 * output, float theta )
{
	float c, s;
	sinCos(&s, &c, theta);

	output->m11 = c;	output->m12 = s;	output->m13 = 0.0f;
	output->m21 = -s;	output->m22 = c;	output->m23 = 0.0f;
	output->m31 = 0.0f;	output->m32 = 0.0f;	output->m33 = 1.0f;

	output->m41 = output->m42 = output->m43 = output->m14 = output->m24 = output->m34 = 0.0f;
	output->m44 = 1.0f;

	return output;
}

/**
����X����ת.
**/
INLINE void matrix4x4_appendRotationX( Matrix4x4 * m, float angle )
{
	Matrix4x4 rotMtr;

	if (USERADIANS == TRUE)
	{
		angle = DEG2RAD(angle);
	}

	matrix4x4_append_self( m, matrix4x4_rotationXMatrix( &rotMtr, angle ) );
}

/**
����Y����ת.
**/
INLINE void matrix4x4_appendRotationY( Matrix4x4 * m, float angle )
{
	Matrix4x4 rotMtr;

	if (USERADIANS == TRUE)
	{
		angle = DEG2RAD(angle);
	}

	matrix4x4_append_self( m, matrix4x4_rotationYMatrix( &rotMtr, angle ) );
}

/**
����Z����ת.
**/
INLINE void matrix4x4_appendRotationZ( Matrix4x4 * m, float angle )
{
	Matrix4x4 rotMtr;

	if (USERADIANS == TRUE)
	{
		angle = DEG2RAD(angle);
	}

	matrix4x4_append_self( m, matrix4x4_rotationZMatrix3D( &rotMtr, angle ) );
}

INLINE Vector3D* matrix4x4_toEuler(Vector3D* output, Matrix4x4* m)
{
	/*if(m->m12 >= 1.0f)
	{
		output->x = 0;
		output->y = RAD2DEG( atan2(m->m33, m->m31) );
		output->z = 90;

		return output;
	}

	if(m->m12 <= - 1.0f)
	{
		output->x = 0;
		output->y = RAD2DEG( atan2(m->m33, m->m31) );
		output->z = - 90;

		return output;
	}

	output->x = RAD2DEG( atan2(m->m22, - m->m32) );
	output->y = RAD2DEG( atan2(m->m11, - m->m13) );
	output->z = RAD2DEG( asin(m->m12)            );*/
 
	/*float sinX = - m->m32;   

	if(sinX >= 1)
		output->x = 90;
	else if(sinX <= - 1)
		output->x = - 90;
	else
		output->x = RAD2DEG( asinf(sinX) );              

	if (sinX < -0.9999 || sinX > 0.9999) 
	{
		output->y = RAD2DEG( atan2f(m->m11, - m->m13) );
		output->z = 0;
	} 
	else 
	{ 
		output->y = RAD2DEG( atan2f(m->m33, m->m31) );
		output->z = RAD2DEG( atan2f(m->m22, m->m12) ); 
	} */

	//float sinX = - m->m32;   

	/*if(sinX >= 1)
		output->x = 90;
	else if(sinX <= - 1)
		output->x = - 90;
	else
		output->x = RAD2DEG( asinf(sinX) );   */

	output->x = RAD2DEG( asinf(-m->m32) );
	output->y = RAD2DEG( atan2f(m->m33, m->m31) );
	output->z = RAD2DEG( atan2f(m->m22, m->m12) ); 

	//output->x = RAD2DEG( asin(-m->m32)            );
	//output->y = RAD2DEG( atan2(m->m33, m->m31) );
	//output->z = RAD2DEG( atan2(m->m22, m->m12) );

	return output;
}

#include "Quaternion.h"

/**
��ת�������ƽ�ơ���ת������������Ϊ������ Vector3D ������ɵ�ʸ�����ء�
��һ�� Vector3D ��������ƽ��Ԫ�ء�
�ڶ��� Vector3D ������������Ԫ�ء�
������ Vector3D ����������תԪ�ء� 
**/
INLINE void matrix4x4_decompose( Matrix4x4 * m, Vector3D * position,  Vector3D * scale, Vector3D * direction, Quaternion * rotation)
{
	Matrix4x4 normalized;

	float sx, sy, sz;

	if( position )
	{
		position->x = m->m41;
		position->y = m->m42;
		position->z = m->m43;
	}

	sx = sqrt(m->m11 * m->m11 + m->m12 * m->m12 + m->m13 * m->m13);
	sy = sqrt(m->m21 * m->m21 + m->m22 * m->m22 + m->m23 * m->m23);
	sz = sqrt(m->m31 * m->m31 + m->m32 * m->m32 + m->m33 * m->m33);

	if( scale )
	{
		scale->x = sx;//vector3D_length( &i );
		scale->y = sy;//vector3D_length( &j );
		scale->z = sz;//vector3D_length( &k );
	}

	if( sx == 0 || sy == 0 || sz == 0 )
		return;

	normalized.m11 = m->m11 / sx;
	normalized.m12 = m->m12 / sx;
	normalized.m13 = m->m13 / sx;
	normalized.m21 = m->m21 / sy;
	normalized.m22 = m->m22 / sy;
	normalized.m23 = m->m23 / sy;
	normalized.m31 = m->m31 / sz;
	normalized.m32 = m->m32 / sz;
	normalized.m33 = m->m33 / sz;

	if( direction )
	{
		//direction->y = RAD2DEG( atan2(normalized.m21, normalized.m11) );
		//direction->x = RAD2DEG( atan2( - normalized.m31, sqrt(normalized.m32 * normalized.m32 + normalized.m33 * normalized.m33) ) );
		//direction->z = RAD2DEG( atan2(normalized.m32, normalized.m33) );

		//direction->x = RAD2DEG(atan2(normalized.m32, normalized.m33) - HALF_PI);
		//direction->y = RAD2DEG(atan2( - normalized.m31, sqrt(normalized.m32 * normalized.m32 + normalized.m33 * normalized.m33)) );
		//direction->z = RAD2DEG(atan2(normalized.m21, normalized.m11) - HALF_PI);

		matrix4x4_toEuler(direction, &normalized);
	}

	if( rotation )
		quaternion_rotationMatrix(rotation, & normalized);

	//t = newMatrix4x4(&data);

	//t->m11 = i->x;	t->m21 = j->x;	t->m31 = k->x;	t->m41 = 0.0f;
	//t->m12 = i->y;	t->m22 = j->y;	t->m32 = k->y;	t->m41 = 0.0f;
	//t->m13 = i->z;	t->m23 = j->z;	t->m33 = k->z;	t->m41 = 0.0f;
	//t->m14 = 0.0f;	t->m24 = 0.0f;	t->m34 = 0.0f;	t->m44 = 1.0f;

	//rotation->x = atan2( t->m32, t->m33 );

	//matrix4x4_append(m, rotationXMatrix( - rotation->x ));

	//rotation->y = atan2( - m->m13, sqrt( m->m11 * m->m11 + m->m12 * m->m12) );
	//rotation->z = atan2( - m->m21, m->m11 );

	//if( rotation->x == PI )
	//{
	//	if( rotation->y > 0 )
	//	{
	//		rotation->y -= PI;
	//	}
	//	else
	//	{
	//		rotation->y += PI;
	//	}

	//	rotation->x = 0.0f;
	//	rotation->z += PI;
	//}

	/*rotation->x *= TODEGREES;
	rotation->y *= TODEGREES;
	rotation->z *= TODEGREES;*/
}

/**
����ת�������ƽ�ơ���ת���������á�
recompose() ���������ĸ����Ǿ��Ը��ġ�
recompose() ���������Ǿ���ת���� 

��Ҫʹ�þ��Ը�������֡���޸ľ���ת������ʹ�� decompose() �����������ã�Ȼ�������ʵ��ĸ��ġ�
Ȼ�󣬿���ʹ�� recompose() ������ Matrix4x4 ��������Ϊ�޸ĺ��ת���� 
**/
INLINE void matrix4x4_recompose( Matrix4x4 * m, Vector3D * position, Vector3D * scale, Vector3D * rotation )
{
	m = matrix4x4_translationMatrix( m, position->x, position->y, position->z );

	matrix4x4_appendScale( m, scale->x,scale->y, scale->z );

	matrix4x4_appendRotationX( m, rotation->x );

	matrix4x4_appendRotationY( m, rotation->y );

	matrix4x4_appendRotationZ( m, rotation->z );
}

/**
ʹ��ת������ Vector3D �����һ���ռ�����ת������һ���ռ����ꡣ
���ص� Vector3D ��������ת����������ꡣ
���� Vector3D ������Ӧ�����о���ת��������ƽ�ƣ��� 
**/
INLINE Vector3D * matrix4x4_transformVector( Vector3D * output, Matrix4x4 * m, Vector3D * v )
{
	if (v->w == 1)
	{
		output->x = m->m11 * v->x + m->m21 * v->y + m->m31 * v->z + m->m41;
		output->y = m->m12 * v->x + m->m22 * v->y + m->m32 * v->z + m->m42;
		output->z = m->m13 * v->x + m->m23 * v->y + m->m33 * v->z + m->m43;
		output->w = m->m14 * v->x + m->m24 * v->y + m->m34 * v->z + m->m44;
	}
	else
	{
		output->x = m->m11 * v->x + m->m21 * v->y + m->m31 * v->z + m->m41 * v->w;
		output->y = m->m12 * v->x + m->m22 * v->y + m->m32 * v->z + m->m42 * v->w;
		output->z = m->m13 * v->x + m->m23 * v->y + m->m33 * v->z + m->m43 * v->w;
		output->w = m->m14 * v->x + m->m24 * v->y + m->m34 * v->z + m->m44 * v->w;
	}

	return output;
}

INLINE void matrix4x4_transformVector_self( Matrix4x4 * m, Vector3D * v )
{
	float x, y, z, w;

	if (v->w == 1)
	{
		x = m->m11 * v->x + m->m21 * v->y + m->m31 * v->z + m->m41;
		y = m->m12 * v->x + m->m22 * v->y + m->m32 * v->z + m->m42;
		z = m->m13 * v->x + m->m23 * v->y + m->m33 * v->z + m->m43;
		w = m->m14 * v->x + m->m24 * v->y + m->m34 * v->z + m->m44;
	}
	else
	{
		x = m->m11 * v->x + m->m21 * v->y + m->m31 * v->z + m->m41 * v->w;
		y = m->m12 * v->x + m->m22 * v->y + m->m32 * v->z + m->m42 * v->w;
		z = m->m13 * v->x + m->m23 * v->y + m->m33 * v->z + m->m43 * v->w;
		w = m->m14 * v->x + m->m24 * v->y + m->m34 * v->z + m->m44 * v->w;
	}

	v->x = x;
	v->y = y;
	v->z = z;
	v->w = w;
}

/**
��ת��ʾ������ʹ�䳯��ָ����λ�á�
�˷�������Է�����о͵��޸ġ�
��ʾ���󣨼� at Vector3D ���󣩵���ǰ����ʸ��ָ��ָ������ʵ�������λ�á�
��ʾ��������Ϸ������� up Vector3D ����ָ���ġ� 

lookAt() ������ʹ�������ʾ������ת����ֵ��Ч��
�˷����ɷֽ���ʾ����ľ����޸���תԪ�أ��Ӷ��ö���ת��ָ��λ�á�
Ȼ�󣬴˷�����������ɣ����£���ʾ����ľ����⽫ִ��ת����
����ö���ָ�������ƶ���Ŀ�꣨���������ƶ��Ķ���λ�ã��������ÿ���������ã��˷��������ö��������ƶ���Ŀ����ת�� 
**/
INLINE Matrix4x4 * lookAt( Matrix4x4 * output, Vector3D * pEye, Vector3D * pAt, Vector3D * pUp )
{
	Vector3D right, up, vec;

	vector3D_subtract(&vec, pAt, pEye);

	vector3D_normalize(&vec);

	vector3D_crossProduct(&right, pUp, &vec);
	vector3D_crossProduct(&up, &right, &vec);

	vector3D_normalize(&right);
	vector3D_normalize(&up);

	/*output->m11 = right.x;
	output->m21 = right.y;
	output->m31 = right.z;
	output->m41 = -vector3D_dotProduct(&right, pEye);

	output->m12 = up.x;
	output->m22 = up.y;
	output->m32 = up.z;
	output->m42 = -vector3D_dotProduct(&up, pEye);

	output->m13 = vec.x;
	output->m23 = vec.y;
	output->m33 = vec.z;
	output->m43 = -vector3D_dotProduct(&vec, pEye);

	output->m14 = 0.0f;
	output->m24 = 0.0f;
	output->m34 = 0.0f;
	output->m44 = 1.0f;*/
	output->m11 = right.x;
	output->m12 = right.y;
	output->m13 = right.z;
	output->m14 = 0.0f;

	output->m21 = up.x;
	output->m22 = up.y;
	output->m23 = up.z;
	output->m24 = 0.0f;

	output->m31 = vec.x;
	output->m32 = vec.y;
	output->m33 = vec.z;
	output->m34 = 0.0f;

	output->m41 = pEye->x;
	output->m42 = pEye->y;
	output->m43 = pEye->z;
	output->m44 = 1.0f;

	return output;
}

//void printfMatrix3D( Matrix4x4 m )
//{
//	printf( "\nMatrix3D(\n%-10.1lf%-10.1lf%-10.1lf%-10.1lf\n%-10.1lf%-10.1lf%-10.1lf%-10.1lf\n%-10.1lf%-10.1lf%-10.1lf%-10.1lf\n%-10.1lf%-10.1lf%-10.1lf%-10.1lf\n)\n", 
//		m.m11, m.m12, m.m13, m.m14, m.m21, m.m22, m.m23, m.m24, m.m31, m.m32, m.m33, m.m34, m.m41, m.m42, m.m43, m.m44 );
//}

#endif
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
将当前矩阵转换为恒等或单位矩阵。恒等矩阵中的主对角线位置上的元素的值为一，而所有其他元素的值为零。
生成的结果是一个矩阵，其中，rawData 值为 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1，旋转设置为 Vector3D(0,0,0)，位置或平移设置为 Vector3D(0,0,0)，缩放设置为 Vector3D(1,1,1)。
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
将当前 Matrix4x4 对象转换为一个矩阵，并将互换其中的行和列。
例如，如果当前 Matrix4x4 对象的 rawData 包含以下 16 个数字：1,2,3,4,11,12,13,14,21,22,23,24,31,32,33,34，则 transpose() 方法会将每四个元素作为一个行读取并将这些行转换为列。
生成的结果是一个矩阵，其 rawData 为：1,11,21,31,2,12,22,32,3,13,23,33,4,14,24,34。 

transpose() 方法会将当前矩阵替换为转置矩阵。
如果要转置矩阵，而不更改当前矩阵，请先使用 clone() 方法复制当前矩阵，然后对生成的副本应用 transpose() 方法。 

正交矩阵是一个正方形矩阵，该矩阵的转置矩阵和逆矩阵相同。
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
一个用于确定矩阵是否可逆的数字。 

Matrix4x4 对象必须是可逆的。可以使用 determinant 属性确保 Matrix4x4 对象是可逆的。
如果行列式为零，则矩阵没有逆矩阵。
例如，如果矩阵的整个行或列为零，或如果两个行或列相等，则行列式为零。
行列式还可用于对一系列方程进行求解。 
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
反转当前矩阵。
逆矩阵具有与原始矩阵相同的大小，但它执行的转换与原始矩阵相反。
例如，如果在原始矩阵中，一个对象按某个方向绕 x 轴旋转，那么在逆矩阵中，该对象将按此方向的反方向绕 x 轴旋转。
对一个对象应用逆矩阵可撤消原始矩阵执行的转换。如果将一个矩阵与其逆矩阵相乘，则将产生一个恒等矩阵。 

利用矩阵的逆矩阵，可以将一个矩阵除以另一个矩阵。通过将矩阵 A 与矩阵 B 的逆矩阵相乘，可以将矩阵 A 除以矩阵 B。
逆矩阵还可以用于摄像头空间。
当摄像头在现实世界空间中移动时，现实世界中的对象需按与摄像头相反的方向移动，这样才能从现实世界视点转换到摄像头或视点空间。
例如，如果摄像头移动得越近，则对象变得越大。
换句话说，如果摄像头沿现实世界的 z 轴下移，则对象会沿此 z 轴上移。

invert() 方法会将当前矩阵替换为逆矩阵。
如果要反转矩阵，而不更改当前矩阵，请先使用 clone() 方法复制当前矩阵，然后对生成的副本应用 invert() 方法。 

Matrix4x4 对象必须是可逆的。
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
一个由 16 个数字组成的矢量，其中，每四个元素可以是 4x4 矩阵的一行或一列。 

如果 rawData 属性设置为一个不可逆的矩阵，则会引发异常。Matrix3D 对象必须是可逆的。
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
一个由 16 个数字组成的矢量，其中，每四个元素可以是 4x4 矩阵的一行或一列。 

如果 rawData 属性设置为一个不可逆的矩阵，则会引发异常。Matrix3D 对象必须是可逆的。
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
创建 Matrix4x4 对象。可以使用一个由 16 个数字组成的矢量来初始化 Matrix4x4 对象，其中，每四个元素可以是一行或一列。
创建 Matrix4x4 对象之后，可以使用 rawData 属性访问该对象的矩阵元素。 

如果未定义任何参数，则构造函数会生成一个恒等或单位 Matrix4x4 对象。
在矩阵表示法中，恒等矩阵中的主对角线位置上的所有元素的值均为一，而所有其他元素的值均为零。
恒等矩阵的 rawData 属性的值为 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1。
恒等矩阵的位置或平移值为 Vector3D(0,0,0)，旋转设置为 Vector3D(0,0,0)，缩放值为 Vector3D(1,1,1)。
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
复制矩阵。
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
一个保存显示对象在转换参照帧中的 3D 坐标 (x,y,z) 位置的 Vector3D 对象。
利用 position 属性，可立即访问显示对象矩阵的平移矢量，而无需分解和重新组合矩阵。 

利用 position 属性，可以获取和设置转换矩阵的平移元素。
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
一个保存显示对象在转换参照帧中的 3D 坐标 (x,y,z) 位置的 Vector3D 对象。
利用 position 属性，可立即访问显示对象矩阵的平移矢量，而无需分解和重新组合矩阵。 

利用 position 属性，可以获取和设置转换矩阵的平移元素。
**/
INLINE void matrix4x4_setPosition( Matrix4x4 * m, Vector3D * v )
{
	m->m41 = v->x;
	m->m42 = v->y;
	m->m43 = v->z;
	m->m44 = v->w;
}

/**
通过将另一个 Matrix4x4 对象与当前 Matrix4x4 对象相乘来后置一个矩阵。
得到的结果将合并两个矩阵转换。可以将一个 Matrix4x4 对象与多个矩阵相乘。最终的 Matrix4x4 对象将包含所有转换的结果。 

矩阵乘法运算与矩阵加法运算不同。矩阵乘法运算是不可交换的。
换句话说，A 乘以 B 并不等于 B 乘以 A。
在使用 append() 方法时，乘法运算将从左侧开始，这意味着 lhs Matrix4x4 对象位于乘法运算符的左侧。 

thisMatrix = lhs * thisMatrix; 
首次调用 append() 方法时，此方法会对父级空间进行相关修改。
后续调用与后置的 Matrix4x4 对象的参照帧相关。 

append() 方法会将当前矩阵替换为后置的矩阵。
如果要后置两个矩阵，而不更改当前矩阵，请使用 clone() 方法复制当前矩阵，然后对生成的副本应用 append() 方法。 
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
获取一个位移矩阵。
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
在 Matrix4x4 对象上后置一个增量平移，沿 x、y 和 z 轴重新定位。
在将 Matrix4x4 对象应用于显示对象时，矩阵会在 Matrix4x4 对象中先执行其他转换，然后再执行平移更改。 

平移将作为沿三个轴（x、y、z）进行的三个增量更改集进行定义。
在对显示对象应用转换时，显示对象会从当前位置沿 x、y 和 z 轴按参数指定的增量移动。
若要确保平移只影响特定的轴，请将其他参数设置为零。参数为零表示不沿特定的轴进行任何更改。

平移更改不是绝对更改。
它们与矩阵的当前位置和方向相关。
若要确保对转换矩阵进行绝对更改，请使用 recompose() 方法。转换的顺序也很重要。
先平移再旋转的转换所产生的效果与先旋转再平移所产生的效果不同。 
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
在 Matrix4x4 对象上前置一个增量旋转。
在将 Matrix4x4 对象应用于显示对象时，矩阵会在 Matrix4x4 对象中先执行旋转，然后再执行其他转换。 

显示对象的旋转由以下元素定义：一个轴、绕该轴旋转的增量角度和对象旋转中心的可选轴点。
轴可以是任何常规方向。
常见的轴为 XAXIS (Vector3D(1,0,0))、YAXIS (Vector3D(0,1,0)) 和 ZAXIS (Vector3D(0,0,1))。
在航空术语中，有关 y 轴的旋转称为偏航。有关 x 轴的旋转称为俯仰。有关 z 轴的旋转称为翻滚。 

转换的顺序很重要。
先旋转再平移的转换所产生的效果与先平移再旋转所产生的效果不同。

旋转效果不是绝对效果。
此效果与对象有关，它与原始位置和方向的参照帧相对。若要确保对转换进行绝对更改，请使用 recompose() 方法。
**/
INLINE void matrix4x4_appendRotation( Matrix4x4 * m, float degrees, int axis )
{
	Matrix4x4 input;

	matrix4x4_append_self( m, matrix4x4_rotationMatrix( &input,  degrees, axis ) );
}

/**
获取一个旋转矩阵.
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
生成一个缩放矩阵.
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
在 Matrix4x4 对象上后置一个增量缩放，沿 x、y 和 z 轴改变位置。
在将 Matrix4x4 对象应用于显示对象时，矩阵会在 Matrix4x4 对象中先执行其他转换，然后再执行缩放更改。
默认的缩放系数为 (1.0f, 1.0f, 1.0f)。 

缩放将作为沿三个轴（x、y、z）进行的三个增量更改集进行定义。可以将每个轴与不同的数字相乘。
在将缩放更改应用于显示对象时，该对象的大小会增大或减小。
例如，将 x、y 和 z 轴设置为 2 将使对象大小为原来的两倍，而将轴设置为 0.5 将使对象的大小为原来的一半。
若要确保缩放转换只影响特定的轴，请将其他参数设置为 1。参数为 1 表示不沿特定的轴进行任何缩放更改。 

appendScale() 方法可用于调整大小和管理扭曲（例如显示对象的拉伸或收缩），或用于某个位置上的放大和缩小。
在显示对象的旋转和平移过程中，将自动执行缩放转换。 

转换的顺序很重要。
先调整大小再平移的转换所产生的效果与先平移再调整大小的转换所产生的效果不同。
**/

INLINE void matrix4x4_appendScale( Matrix4x4 * m, float xScale, float yScale, float zScale )
{
	Matrix4x4 input;

	matrix4x4_append_self( m, matrix4x4_scaleMatrix( &input, xScale, yScale, zScale ) );
}

/**
生成一个绕X轴旋转矩阵。
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
生成一个绕Y轴旋转矩阵。
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
生成一个绕Z轴旋转矩阵。
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
后置X轴旋转.
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
后置Y轴旋转.
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
后置Z轴旋转.
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
将转换矩阵的平移、旋转和缩放设置作为由三个 Vector3D 对象组成的矢量返回。
第一个 Vector3D 对象容纳平移元素。
第二个 Vector3D 对象容纳缩放元素。
第三个 Vector3D 对象容纳旋转元素。 
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
设置转换矩阵的平移、旋转和缩放设置。
recompose() 方法所做的更改是绝对更改。
recompose() 方法将覆盖矩阵转换。 

若要使用绝对父级参照帧来修改矩阵转换，请使用 decompose() 方法检索设置，然后做出适当的更改。
然后，可以使用 recompose() 方法将 Matrix4x4 对象设置为修改后的转换。 
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
使用转换矩阵将 Vector3D 对象从一个空间坐标转换到另一个空间坐标。
返回的 Vector3D 对象将容纳转换后的新坐标。
将对 Vector3D 对象中应用所有矩阵转换（包括平移）。 
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
旋转显示对象以使其朝向指定的位置。
此方法允许对方向进行就地修改。
显示对象（即 at Vector3D 对象）的向前方向矢量指向指定的现实世界相关位置。
显示对象的向上方向是用 up Vector3D 对象指定的。 

lookAt() 方法可使缓存的显示对象旋转属性值无效。
此方法可分解显示对象的矩阵并修改旋转元素，从而让对象转向指定位置。
然后，此方法将重新组成（更新）显示对象的矩阵，这将执行转换。
如果该对象指向正在移动的目标（例如正在移动的对象位置），则对于每个后续调用，此方法都会让对象朝正在移动的目标旋转。 
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
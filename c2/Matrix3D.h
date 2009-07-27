#ifndef __MATRIX3D_H
#define __MATRIX3D_H


/**************************************************************************************
 **----------------------------------------------------------------------------------**
 **                               |Matrix3D verson 1.5|                              **
 **----------------------------------------------------------------------------------**
 **                           include< stdlib.h, Vector3D.h>                         **
 **__________________________________________________________________________________**
 **************************************************************************************/


#include <string.h>
#include <math.h>

#include "Vector3D.h"
#include "Math.h"

/*
============
Matrix2D
============
*/
typedef struct Matrix2D
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
}Matrix2D;

int matrix2D_invert(Matrix2D * m)
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
Matrix3D
============
*/
typedef struct Matrix3D
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
}Matrix3D;

/**
����ǰ����ת��Ϊ��Ȼ�λ���󡣺�Ⱦ����е����Խ���λ���ϵ�Ԫ�ص�ֵΪһ������������Ԫ�ص�ֵΪ�㡣
���ɵĽ����һ���������У�rawData ֵΪ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1����ת����Ϊ Vector3D(0,0,0)��λ�û�ƽ������Ϊ Vector3D(0,0,0)����������Ϊ Vector3D(1,1,1)��
**/
INLINE Matrix3D *  matrix3D_identity( Matrix3D * m )
{
	m->m11 = 1.0f; m->m12 = 0.0f; m->m13 = 0.0f; m->m14 = 0.0f;

	m->m21 = 0.0f; m->m22 = 1.0f; m->m23 = 0.0f; m->m24 = 0.0f;

	m->m31 = 0.0f; m->m32 = 0.0f; m->m33 = 1.0f; m->m34 = 0.0f;

	m->m41 = 0.0f; m->m42 = 0.0f; m->m43 = 0.0f; m->m44 = 1.0f;

	return m;
}

/**
���ƾ���
**/
INLINE Matrix3D * matrix3D_copy( Matrix3D * m, Matrix3D * src )
{
	m->m11 = src->m11; m->m12 = src->m12; m->m13 = src->m13; m->m14 = src->m14;

	m->m21 = src->m21; m->m22 = src->m22; m->m23 = src->m23; m->m24 = src->m24;

	m->m31 = src->m31; m->m32 = src->m32; m->m33 = src->m33; m->m34 = src->m34;

	m->m41 = src->m41; m->m42 = src->m42; m->m43 = src->m43; m->m44 = src->m44;

	//memcpy( m, src, sizeof( * src ) );

	return m;
}

/**
����ǰ Matrix3D ����ת��Ϊһ�����󣬲����������е��к��С�
���磬�����ǰ Matrix3D ����� rawData �������� 16 �����֣�1,2,3,4,11,12,13,14,21,22,23,24,31,32,33,34���� transpose() �����Ὣÿ�ĸ�Ԫ����Ϊһ���ж�ȡ������Щ��ת��Ϊ�С�
���ɵĽ����һ�������� rawData Ϊ��1,11,21,31,2,12,22,32,3,13,23,33,4,14,24,34�� 

transpose() �����Ὣ��ǰ�����滻Ϊת�þ���
���Ҫת�þ��󣬶������ĵ�ǰ��������ʹ�� clone() �������Ƶ�ǰ����Ȼ������ɵĸ���Ӧ�� transpose() ������ 

����������һ�������ξ��󣬸þ����ת�þ�����������ͬ��
**/
INLINE void matrix3D_transpose( Matrix3D * m )
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
}

/**
һ������ȷ�������Ƿ��������֡� 

Matrix3D ��������ǿ���ġ�����ʹ�� determinant ����ȷ�� Matrix3D �����ǿ���ġ�
�������ʽΪ�㣬�����û�������
���磬�������������л���Ϊ�㣬����������л�����ȣ�������ʽΪ�㡣
����ʽ�������ڶ�һϵ�з��̽�����⡣ 
**/
INLINE float matrix3D_determinant( Matrix3D * m )
{
	return m->m11 * ( m->m22 * m->m33 - m->m23 * m->m32 ) + m->m12 * ( m->m23 * m->m31 - m->m21 * m->m33 ) + m->m13 * ( m->m21 * m->m32 - m->m22 * m->m31 );
}

INLINE float matrix3D_determinant4x4( Matrix3D * m )
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

INLINE int matrix3D_invert4x4( Matrix3D * m )
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

INLINE int matrix3D_fastInvert4x4( Matrix3D * m )
{
	Matrix2D r0, r1, r2, r3;
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

Matrix3D ��������ǿ���ġ�
**/
INLINE int matrix3D_invert( Matrix3D * m )
{
	float m11, m12, m13, m21, m22, m23, m31, m32, m33, m41, m42, m43;

	float det = matrix3D_determinant( m );

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

INLINE int matrix3D_fastInvert( Matrix3D * m )
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
INLINE float * matrix3D_getRawData( Matrix3D * m )
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
INLINE void matrix3D_setRawData( Matrix3D * m, float ( * rawData )[16] )
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

	if( !matrix3D_determinant( m ) )
	{
		exit( FALSE );
	}
}

/**
���� Matrix3D ���󡣿���ʹ��һ���� 16 ��������ɵ�ʸ������ʼ�� Matrix3D �������У�ÿ�ĸ�Ԫ�ؿ�����һ�л�һ�С�
���� Matrix3D ����֮�󣬿���ʹ�� rawData ���Է��ʸö���ľ���Ԫ�ء� 

���δ�����κβ��������캯��������һ����Ȼ�λ Matrix3D ����
�ھ����ʾ���У���Ⱦ����е����Խ���λ���ϵ�����Ԫ�ص�ֵ��Ϊһ������������Ԫ�ص�ֵ��Ϊ�㡣
��Ⱦ���� rawData ���Ե�ֵΪ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1��
��Ⱦ����λ�û�ƽ��ֵΪ Vector3D(0,0,0)����ת����Ϊ Vector3D(0,0,0)������ֵΪ Vector3D(1,1,1)��
**/
Matrix3D * newMatrix3D( float ( * rawData )[16] )
{
	Matrix3D * m;

	if( ( m = ( Matrix3D * )malloc( sizeof( Matrix3D ) ) ) == NULL )
	{
		exit( TRUE );
	}

	if( rawData == NULL )
	{
		matrix3D_identity( m );
	}
	else
	{
		matrix3D_setRawData( m, rawData );
	}

	return m;
}

INLINE void sinCos(float *returnSin, float *returnCos, float theta)
{ 
	*returnSin = sinf(theta); 
	*returnCos = cosf(theta); 
} 

/**
һ��������ʾ������ת������֡�е� 3D ���� (x,y,z) λ�õ� Vector3D ����
���� position ���ԣ�������������ʾ��������ƽ��ʸ����������ֽ��������Ͼ��� 

���� position ���ԣ����Ի�ȡ������ת�������ƽ��Ԫ�ء�
**/
INLINE Vector3D * matrix3D_getPosition( Vector3D * output, Matrix3D * m )
{
	output->x = m->m41;
	output->y = m->m42;
	output->z = m->m43;
	output->w = m->m44;

	return output;
}

/**
һ��������ʾ������ת������֡�е� 3D ���� (x,y,z) λ�õ� Vector3D ����
���� position ���ԣ�������������ʾ��������ƽ��ʸ����������ֽ��������Ͼ��� 

���� position ���ԣ����Ի�ȡ������ת�������ƽ��Ԫ�ء�
**/
INLINE void matrix3D_setPosition( Matrix3D * m, Vector3D * v )
{
	m->m41 = v->x;
	m->m42 = v->y;
	m->m43 = v->z;
	m->m44 = v->w;
}

/**
ͨ������һ�� Matrix3D �����뵱ǰ Matrix3D �������������һ������
�õ��Ľ�����ϲ���������ת�������Խ�һ�� Matrix3D ��������������ˡ����յ� Matrix3D ���󽫰�������ת���Ľ���� 

����˷����������ӷ����㲻ͬ������˷������ǲ��ɽ����ġ�
���仰˵��A ���� B �������� B ���� A��
��ʹ�� append() ����ʱ���˷����㽫����࿪ʼ������ζ�� lhs Matrix3D ����λ�ڳ˷����������ࡣ 

thisMatrix = lhs * thisMatrix; 
�״ε��� append() ����ʱ���˷�����Ը����ռ��������޸ġ�
������������õ� Matrix3D ����Ĳ���֡��ء� 

append() �����Ὣ��ǰ�����滻Ϊ���õľ���
���Ҫ�����������󣬶������ĵ�ǰ������ʹ�� clone() �������Ƶ�ǰ����Ȼ������ɵĸ���Ӧ�� append() ������ 
**/
INLINE Matrix3D * matrix3D_append( Matrix3D * output, Matrix3D * m1, Matrix3D * m2 )
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

INLINE Matrix3D * matrix3D_append_self( Matrix3D * thisMatrix, Matrix3D * lhs )
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

INLINE Matrix3D *  matrix3D_append4x4( Matrix3D * thisMatrix, Matrix3D * lhs )
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
INLINE Matrix3D * translationMatrix3D( Matrix3D * output, float x, float y, float z )
{
	output->m12 = output->m13 = output->m14 = output->m21 = output->m23 = output->m24 = output->m31 = output->m32 = output->m34 = 0;
	output->m11= output->m22 = output->m33 = output->m44 =  1;

	output->m41 = x;
	output->m42 = y;
	output->m43 = z;

	return output;
}

/**
�� Matrix3D �����Ϻ���һ������ƽ�ƣ��� x��y �� z �����¶�λ��
�ڽ� Matrix3D ����Ӧ������ʾ����ʱ��������� Matrix3D ��������ִ������ת����Ȼ����ִ��ƽ�Ƹ��ġ� 

ƽ�ƽ���Ϊ�������ᣨx��y��z�����е������������ļ����ж��塣
�ڶ���ʾ����Ӧ��ת��ʱ����ʾ�����ӵ�ǰλ���� x��y �� z �ᰴ����ָ���������ƶ���
��Ҫȷ��ƽ��ֻӰ���ض����ᣬ�뽫������������Ϊ�㡣����Ϊ���ʾ�����ض���������κθ��ġ�

ƽ�Ƹ��Ĳ��Ǿ��Ը��ġ�
���������ĵ�ǰλ�úͷ�����ء�
��Ҫȷ����ת��������о��Ը��ģ���ʹ�� recompose() ������ת����˳��Ҳ����Ҫ��
��ƽ������ת��ת����������Ч��������ת��ƽ����������Ч����ͬ�� 
**/
INLINE void matrix3D_appendTranslation( Matrix3D * m, float x, float y, float z )
{
	Matrix3D input;

	matrix3D_append_self( m, translationMatrix3D( &input, x, y, z ) );
}

INLINE Matrix3D * rotationMatrix3D( Matrix3D * output,  float degrees, int axis )
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
��ȡһ����ת����.
**/
INLINE Matrix3D * rotationMatrix3DByAxis( Matrix3D * output,  float degrees, Vector3D * axis )
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

/**
�� Matrix3D ������ǰ��һ��������ת��
�ڽ� Matrix3D ����Ӧ������ʾ����ʱ��������� Matrix3D ��������ִ����ת��Ȼ����ִ������ת���� 

��ʾ�������ת������Ԫ�ض��壺һ���ᡢ�Ƹ�����ת�������ǶȺͶ�����ת���ĵĿ�ѡ��㡣
��������κγ��淽��
��������Ϊ XAXIS (Vector3D(1,0,0))��YAXIS (Vector3D(0,1,0)) �� ZAXIS (Vector3D(0,0,1))��
�ں��������У��й� y �����ת��Ϊƫ�����й� x �����ת��Ϊ�������й� z �����ת��Ϊ������ 

ת����˳�����Ҫ��
����ת��ƽ�Ƶ�ת����������Ч������ƽ������ת��������Ч����ͬ��

��תЧ�����Ǿ���Ч����
��Ч��������йأ�����ԭʼλ�úͷ���Ĳ���֡��ԡ���Ҫȷ����ת�����о��Ը��ģ���ʹ�� recompose() ������
**/
INLINE void matrix3D_appendRotation( Matrix3D * m, float degrees, int axis )
{
	Matrix3D input;

	matrix3D_append_self( m, rotationMatrix3D( &input,  degrees, axis ) );
}

INLINE void matrix3D_appendRotationByAxis( Matrix3D * m, float degrees, Vector3D * axis )
{
	Matrix3D input;

	matrix3D_append_self( m, rotationMatrix3DByAxis( &input, degrees, axis ) );
}

/**
����һ�����ž���.
**/
INLINE Matrix3D * scaleMatrix3D( Matrix3D * output, float xScale, float yScale, float zScale )
{
	output->m11 = xScale; output->m12 = 0.0f; output->m13 = 0.0f;
	output->m21 = 0.0f; output->m22 = yScale; output->m23 = 0.0f;
	output->m31 = 0.0f; output->m32 = 0.0f; output->m33 = zScale;

	output->m41 = output->m42 = output->m43 = output->m14 = output->m24 = output->m34 = 0.0f;
	output->m44 = 1;

	return output;
}

/**
�� Matrix3D �����Ϻ���һ���������ţ��� x��y �� z ��ı�λ�á�
�ڽ� Matrix3D ����Ӧ������ʾ����ʱ��������� Matrix3D ��������ִ������ת����Ȼ����ִ�����Ÿ��ġ�
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

INLINE void matrix3D_appendScale( Matrix3D * m, float xScale, float yScale, float zScale )
{
	Matrix3D input;

	matrix3D_append_self( m, scaleMatrix3D( &input, xScale, yScale, zScale ) );
}

/**
����һ����X����ת����
**/
INLINE Matrix3D * rotationXMatrix3D( Matrix3D * output, float theta )
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
INLINE Matrix3D * rotationYMatrix3D( Matrix3D * output, float theta )
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
INLINE Matrix3D * rotationZMatrix3D( Matrix3D * output, float theta )
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
INLINE void matrix3D_appendRotationX( Matrix3D * m, float angle )
{
	Matrix3D rotMtr;

	if (USERADIANS == TRUE)
	{
		angle = DEG2RAD(angle);
	}

	matrix3D_append_self( m, rotationXMatrix3D( &rotMtr, angle ) );
}

/**
����Y����ת.
**/
INLINE void matrix3D_appendRotationY( Matrix3D * m, float angle )
{
	Matrix3D rotMtr;

	if (USERADIANS == TRUE)
	{
		angle = DEG2RAD(angle);
	}

	matrix3D_append_self( m, rotationYMatrix3D( &rotMtr, angle ) );
}

/**
����Z����ת.
**/
INLINE void matrix3D_appendRotationZ( Matrix3D * m, float angle )
{
	Matrix3D rotMtr;

	if (USERADIANS == TRUE)
	{
		angle = DEG2RAD(angle);
	}

	matrix3D_append_self( m, rotationZMatrix3D( &rotMtr, angle ) );
}

#include "Quaternion.h"

/**
��ת�������ƽ�ơ���ת������������Ϊ������ Vector3D ������ɵ�ʸ�����ء�
��һ�� Vector3D ��������ƽ��Ԫ�ء�
�ڶ��� Vector3D ������������Ԫ�ء�
������ Vector3D ����������תԪ�ء� 
**/
INLINE void matrix3D_decompose( Matrix3D * m, Vector3D * position, Quaternion * rotation, Vector3D * scale )
{
	Matrix3D normalized;

	Vector3D i;
	Vector3D j;
	Vector3D k;

	i.x = m->m11; i.y = m->m12; i.z = m->m13, i.w = 1;
	j.x = m->m21; j.y = m->m22; j.z = m->m23, j.w = 1;
	k.x = m->m31; k.y = m->m32; k.z = m->m33, k.w = 1;

	if( scale )
	{
		scale->x = vector3D_length( &i );
		scale->y = vector3D_length( &j );
		scale->z = vector3D_length( &k );
	}

	if( scale->x == 0 || scale->y == 0 || scale->z == 0 )
	{
		exit( TRUE );
	}

	if( position )
	{
		position->x = m->m41;
		position->y = m->m42;
		position->z = m->m43;
	}

	if( rotation )
	{
		normalized.m11 = m->m11 / scale->x;
		normalized.m12 = m->m12 / scale->x;
		normalized.m13 = m->m13 / scale->x;
		normalized.m21 = m->m21 / scale->y;
		normalized.m22 = m->m22 / scale->y;
		normalized.m23 = m->m23 / scale->y;
		normalized.m31 = m->m31 / scale->z;
		normalized.m32 = m->m32 / scale->z;
		normalized.m33 = m->m33 / scale->z;

		quaternion_rotationMatrix(rotation, & normalized);
	}

	//t = newMatrix3D(&data);

	//t->m11 = i->x;	t->m21 = j->x;	t->m31 = k->x;	t->m41 = 0.0f;
	//t->m12 = i->y;	t->m22 = j->y;	t->m32 = k->y;	t->m41 = 0.0f;
	//t->m13 = i->z;	t->m23 = j->z;	t->m33 = k->z;	t->m41 = 0.0f;
	//t->m14 = 0.0f;	t->m24 = 0.0f;	t->m34 = 0.0f;	t->m44 = 1.0f;

	//rotation->x = atan2( t->m32, t->m33 );

	//matrix3D_append(m, rotationXMatrix3D( - rotation->x ));

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
Ȼ�󣬿���ʹ�� recompose() ������ Matrix3D ��������Ϊ�޸ĺ��ת���� 
**/
INLINE void matrix3D_recompose( Matrix3D * m, Vector3D * position, Vector3D * scale, Vector3D * rotation )
{
	m = translationMatrix3D( m, position->x, position->y, position->z );

	matrix3D_appendScale( m, scale->x,scale->y, scale->z );

	matrix3D_appendRotationX( m, rotation->x );

	matrix3D_appendRotationY( m, rotation->y );

	matrix3D_appendRotationZ( m, rotation->z );
}

/**
ʹ��ת������ Vector3D �����һ���ռ�����ת������һ���ռ����ꡣ
���ص� Vector3D ��������ת����������ꡣ
���� Vector3D ������Ӧ�����о���ת��������ƽ�ƣ��� 
**/
INLINE Vector3D * matrix3D_transformVector( Vector3D * output, Matrix3D * m, Vector3D * v )
{
	//��ͨ�任 12�γ˷� + 12�μӷ�
	//ͶӰ�任 12�γ˷� + 12�μӷ� + 3�γ���
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

	if (output->w != 1)
		vector3D_project( output );

	return output;
}

INLINE void matrix3D_transformVector_self( Matrix3D * m, Vector3D * v )
{
	//��ͨ�任 12�γ˷� + 12�μӷ�
	//ͶӰ�任 12�γ˷� + 12�μӷ� + 3�γ���
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

	if (v->w != 1)
		vector3D_project( v );
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
INLINE Matrix3D * lookAt( Matrix3D * output, Vector3D * pEye, Vector3D * pAt, Vector3D * pUp )
{
	Vector3D right, up, vec;

	vector3D_subtract(&vec, pAt, pEye);

	vector3D_normalize(&vec);

	vector3D_crossProduct(&right, pUp, &vec);
	vector3D_crossProduct(&up, &right, &vec);

	vector3D_normalize(&right);
	vector3D_normalize(&up);

	output->m11 = right.x;
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
	output->m44 = 1.0f;

	return output;
}

//void printfMatrix3D( Matrix3D m )
//{
//	printf( "\nMatrix3D(\n%-10.1lf%-10.1lf%-10.1lf%-10.1lf\n%-10.1lf%-10.1lf%-10.1lf%-10.1lf\n%-10.1lf%-10.1lf%-10.1lf%-10.1lf\n%-10.1lf%-10.1lf%-10.1lf%-10.1lf\n)\n", 
//		m.m11, m.m12, m.m13, m.m14, m.m21, m.m22, m.m23, m.m24, m.m31, m.m32, m.m33, m.m34, m.m41, m.m42, m.m43, m.m44 );
//}

#endif
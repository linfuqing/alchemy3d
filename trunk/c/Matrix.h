# ifndef MATRIX_H
# define MATRIX_H


/**************************************************************************************
 **----------------------------------------------------------------------------------**
 **                               |Matrix3D verson 1.6F|                              **
 **----------------------------------------------------------------------------------**
 **                           include< stdlib.h, Vector3D.h>                         **
 **__________________________________________________________________________________**
 **************************************************************************************/


#include <stdlib.h>
#include <string.h>

#include "Vector.h"

//RW
typedef struct
{
	Number m11;
	Number m12;
	Number m13;
	Number m14;

	Number m21;
	Number m22;
	Number m23;
	Number m24;

	Number m31;
	Number m32;
	Number m33;
	Number m34;

	Number m41;
	Number m42;
	Number m43;
	Number m44;
}Matrix3D;

/**************************************************************************************
 **----------------------------------------------------------------------------------**
 **                                    |matrix3D|                                    **
 **__________________________________________________________________________________**
 **************************************************************************************/

/**
����ǰ����ת��Ϊ��Ȼ�λ���󡣺�Ⱦ����е����Խ���λ���ϵ�Ԫ�ص�ֵΪһ������������Ԫ�ص�ֵΪ�㡣
���ɵĽ����һ���������У�rawData ֵΪ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1����ת����Ϊ Vector3D(0,0,0)��λ�û�ƽ������Ϊ Vector3D(0,0,0)����������Ϊ Vector3D(1,1,1)��
**/
void matrix3D_identity( Matrix3D * m )
{
	m -> m11 = 1.0; m -> m12 = 0.0; m -> m13 = 0.0; m -> m14 = 0.0;

	m -> m21 = 0.0; m -> m22 = 1.0; m -> m23 = 0.0; m -> m24 = 0.0;

	m -> m31 = 0.0; m -> m32 = 0.0; m -> m33 = 1.0; m -> m34 = 0.0;

	m -> m41 = 0.0; m -> m42 = 0.0; m -> m43 = 0.0; m -> m44 = 1.0;
}
/**
���ƾ���
**/
void matrix3D_copy( Matrix3D * m, Matrix3D * c )
{
	m -> m11 = c -> m11; m -> m12 = c -> m12; m -> m13 = c -> m13; m -> m14 = c -> m14;

	m -> m21 = c -> m21; m -> m22 = c -> m22; m -> m23 = c -> m23; m -> m24 = c -> m24;

	m -> m31 = c -> m31; m -> m32 = c -> m32; m -> m33 = c -> m33; m -> m34 = c -> m34;

	m -> m41 = c -> m41; m -> m42 = c -> m42; m -> m43 = c -> m43; m -> m44 = c -> m44;
}
/**
һ������ȷ�������Ƿ��������֡� 

Matrix3D ��������ǿ���ġ�����ʹ�� determinant ����ȷ�� Matrix3D �����ǿ���ġ�
�������ʽΪ�㣬�����û�������
���磬�������������л���Ϊ�㣬����������л�����ȣ�������ʽΪ�㡣
����ʽ�������ڶ�һϵ�з��̽�����⡣ 
**/
Number matrix3D_determinant( Matrix3D * m )
{
	return ( m -> m11 * m -> m22 - m -> m21 * m -> m12 ) * m -> m33 - ( m -> m11 * m -> m32 - m -> m31 * m -> m12 ) * m -> m23 + ( m -> m21 * m -> m32 - m -> m31 * m -> m22 ) * m -> m13;
}

/**
һ���� 16 ��������ɵ�ʸ�������У�ÿ�ĸ�Ԫ�ؿ����� 4x4 �����һ�л�һ�С� 

��� rawData ��������Ϊһ��������ľ�����������쳣��Matrix3D ��������ǿ���ġ�
**/
Number ( * matrix3D_getRawData( Matrix3D * m ) )[16]
{
	Number ( * rawData )[16];

	rawData = calloc( 16, sizeof( Number ) );

	( * rawData )[0]  = m -> m11;
	( * rawData )[1]  = m -> m21;
	( * rawData )[2]  = m -> m31;
	( * rawData )[3]  = m -> m41;
	
	( * rawData )[4]  = m -> m12;
	( * rawData )[5]  = m -> m22;
	( * rawData )[6]  = m -> m32;
	( * rawData )[7]  = m -> m42;

	( * rawData )[8]  = m -> m13;
	( * rawData )[9]  = m -> m23;
	( * rawData )[10] = m -> m33;
	( * rawData )[11] = m -> m43;

	( * rawData )[12] = m -> m14;
	( * rawData )[13] = m -> m24;
	( * rawData )[14] = m -> m34;
	( * rawData )[15] = m -> m44;

	return rawData;
}

/**
һ���� 16 ��������ɵ�ʸ�������У�ÿ�ĸ�Ԫ�ؿ����� 4x4 �����һ�л�һ�С� 

��� rawData ��������Ϊһ��������ľ�����������쳣��Matrix3D ��������ǿ���ġ�
**/
void matrix3D_setRawData( Matrix3D * m, Number ( * rawData )[16] )
{
	m -> m11 = ( * rawData )[0];
	m -> m21 = ( * rawData )[1];
	m -> m31 = ( * rawData )[2];
	m -> m41 = ( * rawData )[3];

	m -> m12 = ( * rawData )[4];
	m -> m22 = ( * rawData )[5];
	m -> m32 = ( * rawData )[6];
	m -> m42 = ( * rawData )[7];

	m -> m13 = ( * rawData )[8];
	m -> m23 = ( * rawData )[9];
	m -> m33 = ( * rawData )[10];
	m -> m43 = ( * rawData )[11];

	m -> m14 = ( * rawData )[12];
	m -> m24 = ( * rawData )[13];
	m -> m34 = ( * rawData )[14];
	m -> m44 = ( * rawData )[15];

	if( !matrix3D_determinant( m ) )
	{
		matrix3D_identity( m );
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
Matrix3D * newMatrix3D( Number ( * rawData )[16] )
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

/**
����һ���� Matrix3D ���������뵱ǰ Matrix3D ������ȫ��ͬ�ĸ�����
**/
Matrix3D * matrix3D_clone( Matrix3D * src )
{
	Matrix3D  * m;

	if( ( m = ( Matrix3D * )malloc( sizeof( Matrix3D ) ) ) == NULL )
	{
		exit( TRUE );
	}

	if( memcpy( m, src, sizeof( Matrix3D ) ) == NULL )
	{
		exit( TRUE );
	}

	return m;
}

/**
һ��������ʾ������ת������֡�е� 3D ���� (x,y,z) λ�õ� Vector3D ����
���� position ���ԣ�������������ʾ��������ƽ��ʸ����������ֽ��������Ͼ��� 

���� position ���ԣ����Ի�ȡ������ת�������ƽ��Ԫ�ء�
**/
Vector3D * matrix3D_getPosition( Matrix3D * m )
{
	Vector3D * v;

	if( ( v = ( Vector3D * )malloc( sizeof( Vector3D ) ) ) == NULL )
	{
		exit( TRUE );
	}

	v -> x = m -> m14;
	v -> y = m -> m24;
	v -> z = m -> m34;
	v -> w = m -> m44;

	return v;
}

/**
һ��������ʾ������ת������֡�е� 3D ���� (x,y,z) λ�õ� Vector3D ����
���� position ���ԣ�������������ʾ��������ƽ��ʸ����������ֽ��������Ͼ��� 

���� position ���ԣ����Ի�ȡ������ת�������ƽ��Ԫ�ء�
**/
void matrix3D_setPosition( Matrix3D * m, Vector3D * v )
{
	m -> m14 = v -> x;
	m -> m24 = v -> y;
	m -> m34 = v -> z;
	m -> m44 = v -> w;
}

/**
������ˡ�
**/
Matrix3D * matrix3D_multiply( Matrix3D * m1, Matrix3D * m2 )
{
	Matrix3D * m;

	if( ( m = ( Matrix3D * )malloc( sizeof( Matrix3D ) ) ) == NULL )
	{
		exit( TRUE );
	}

	m -> m11 = m1 -> m11 * m2 -> m11 + m1 -> m12 * m2 -> m21 + m1 -> m13 * m2 -> m31;
	m -> m12 = m1 -> m11 * m2 -> m12 + m1 -> m12 * m2 -> m22 + m1 -> m13 * m2 -> m32;
	m -> m13 = m1 -> m11 * m2 -> m13 + m1 -> m12 * m2 -> m23 + m1 -> m13 * m2 -> m33;
	m -> m14 = m1 -> m11 * m2 -> m14 + m1 -> m12 * m2 -> m24 + m1 -> m13 * m2 -> m34 + m1 -> m14;

	m -> m21 = m1 -> m21 * m2 -> m11 + m1 -> m22 * m2 -> m21 + m1 -> m23 * m2 -> m31;
	m -> m22 = m1 -> m21 * m2 -> m12 + m1 -> m22 * m2 -> m22 + m1 -> m23 * m2 -> m32;
	m -> m23 = m1 -> m21 * m2 -> m13 + m1 -> m22 * m2 -> m23 + m1 -> m23 * m2 -> m33;
	m -> m24 = m1 -> m21 * m2 -> m14 + m1 -> m22 * m2 -> m24 + m1 -> m23 * m2 -> m34 + m1 -> m24;

	m -> m31 = m1 -> m31 * m2 -> m11 + m1 -> m32 * m2 -> m21 + m1 -> m33 * m2 -> m31;
	m -> m32 = m1 -> m31 * m2 -> m12 + m1 -> m32 * m2 -> m22 + m1 -> m33 * m2 -> m32;
	m -> m33 = m1 -> m31 * m2 -> m13 + m1 -> m32 * m2 -> m23 + m1 -> m33 * m2 -> m33;
	m -> m34 = m1 -> m31 * m2 -> m14 + m1 -> m32 * m2 -> m24 + m1 -> m33 * m2 -> m34 + m1 -> m34;


	/*m.m41 = m1.m41 * m2.m11 + m1.m42 * m2.m21 + m1.m43 * m2.m31;
	m.m42 = m1.m41 * m2.m12 + m1.m42 * m2.m22 + m1.m43 * m2.m32;
	m.m43 = m1.m41 * m2.m13 + m1.m42 * m2.m23 + m1.m43 * m2.m33;
	m.m44 = m1.m41 * m2.m14 + m1.m42 * m2.m24 + m1.m43 * m2.m34 + m1.m44;*/

	m -> m41 = 0;
	m -> m42 = 0;
	m -> m43 = 0;
	m -> m44 = 1;

	return m;
}

Matrix3D * matrix3D_project( Matrix3D * m1, Matrix3D * m2 )
{
	Matrix3D * m;

	if( ( m = ( Matrix3D * )malloc( sizeof( Matrix3D ) ) ) == NULL )
	{
		exit( TRUE );
	}

	m -> m11 = m1 -> m11 * m2 -> m11 + m1 -> m12 * m2 -> m21 + m1 -> m13 * m2 -> m31;
	m -> m12 = m1 -> m11 * m2 -> m12 + m1 -> m12 * m2 -> m22 + m1 -> m13 * m2 -> m32;
	m -> m13 = m1 -> m11 * m2 -> m13 + m1 -> m12 * m2 -> m23 + m1 -> m13 * m2 -> m33;
	m -> m14 = m1 -> m11 * m2 -> m14 + m1 -> m12 * m2 -> m24 + m1 -> m13 * m2 -> m34 + m1 -> m14;

	m -> m21 = m1 -> m21 * m2 -> m11 + m1 -> m22 * m2 -> m21 + m1 -> m23 * m2 -> m31;
	m -> m22 = m1 -> m21 * m2 -> m12 + m1 -> m22 * m2 -> m22 + m1 -> m23 * m2 -> m32;
	m -> m23 = m1 -> m21 * m2 -> m13 + m1 -> m22 * m2 -> m23 + m1 -> m23 * m2 -> m33;
	m -> m24 = m1 -> m21 * m2 -> m14 + m1 -> m22 * m2 -> m24 + m1 -> m23 * m2 -> m34 + m1 -> m24;

	m -> m31 = m1 -> m31 * m2 -> m11 + m1 -> m32 * m2 -> m21 + m1 -> m33 * m2 -> m31;
	m -> m32 = m1 -> m31 * m2 -> m12 + m1 -> m32 * m2 -> m22 + m1 -> m33 * m2 -> m32;
	m -> m33 = m1 -> m31 * m2 -> m13 + m1 -> m32 * m2 -> m23 + m1 -> m33 * m2 -> m33;
	m -> m34 = m1 -> m31 * m2 -> m14 + m1 -> m32 * m2 -> m24 + m1 -> m33 * m2 -> m34 + m1 -> m34;


	m -> m41 = m1 -> m41 * m2 -> m11 + m1 -> m42 * m2 -> m21 + m1 -> m43 * m2 -> m31;
	m -> m42 = m1 -> m41 * m2 -> m12 + m1 -> m42 * m2 -> m22 + m1 -> m43 * m2 -> m32;
	m -> m43 = m1 -> m41 * m2 -> m13 + m1 -> m42 * m2 -> m23 + m1 -> m43 * m2 -> m33;
	m -> m44 = m1 -> m41 * m2 -> m14 + m1 -> m42 * m2 -> m24 + m1 -> m43 * m2 -> m34 + m1 -> m44;

	return m;
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
void matrix3D_apprend( Matrix3D * thisMatrix, Matrix3D * lhs )
{
	Matrix3D m = * thisMatrix;

	thisMatrix -> m11 = m.m11 * lhs -> m11 + m.m12 * lhs -> m21 + m.m13 * lhs -> m31;
	thisMatrix -> m12 = m.m11 * lhs -> m12 + m.m12 * lhs -> m22 + m.m13 * lhs -> m32;
	thisMatrix -> m13 = m.m11 * lhs -> m13 + m.m12 * lhs -> m23 + m.m13 * lhs -> m33;
	thisMatrix -> m14 = m.m11 * lhs -> m14 + m.m12 * lhs -> m24 + m.m13 * lhs -> m34 + m.m14;

	thisMatrix -> m21 = m.m21 * lhs -> m11 + m.m22 * lhs -> m21 + m.m23 * lhs -> m31;
	thisMatrix -> m22 = m.m21 * lhs -> m12 + m.m22 * lhs -> m22 + m.m23 * lhs -> m32;
	thisMatrix -> m23 = m.m21 * lhs -> m13 + m.m22 * lhs -> m23 + m.m23 * lhs -> m33;
	thisMatrix -> m24 = m.m21 * lhs -> m14 + m.m22 * lhs -> m24 + m.m23 * lhs -> m34 + m.m24;

	thisMatrix -> m31 = m.m31 * lhs -> m11 + m.m32 * lhs -> m21 + m.m33 * lhs -> m31;
	thisMatrix -> m32 = m.m31 * lhs -> m12 + m.m32 * lhs -> m22 + m.m33 * lhs -> m32;
	thisMatrix -> m33 = m.m31 * lhs -> m13 + m.m32 * lhs -> m23 + m.m33 * lhs -> m33;
	thisMatrix -> m34 = m.m31 * lhs -> m14 + m.m32 * lhs -> m24 + m.m33 * lhs -> m34 + m.m34;
}


void matrix3D_prepend( Matrix3D * thisMatrix, Matrix3D * lhs )
{
	Matrix3D m = * thisMatrix;

	thisMatrix -> m11 = lhs -> m11 * m.m11 + lhs -> m12 * m.m21 + lhs -> m13 * m.m31;
	thisMatrix -> m12 = lhs -> m11 * m.m12 + lhs -> m12 * m.m22 + lhs -> m13 * m.m32;
	thisMatrix -> m13 = lhs -> m11 * m.m13 + lhs -> m12 * m.m23 + lhs -> m13 * m.m33;
	thisMatrix -> m14 = lhs -> m11 * m.m14 + lhs -> m12 * m.m24 + lhs -> m13 * m.m34 + lhs -> m14;

	thisMatrix -> m21 = lhs -> m21 * m.m11 + lhs -> m22 * m.m21 + lhs -> m23 * m.m31;
	thisMatrix -> m22 = lhs -> m21 * m.m12 + lhs -> m22 * m.m22 + lhs -> m23 * m.m32;
	thisMatrix -> m23 = lhs -> m21 * m.m13 + lhs -> m22 * m.m23 + lhs -> m23 * m.m33;
	thisMatrix -> m24 = lhs -> m21 * m.m14 + lhs -> m22 * m.m24 + lhs -> m23 * m.m34 + lhs -> m24;

	thisMatrix -> m31 = lhs -> m31 * m.m11 + lhs -> m32 * m.m21 + lhs -> m33 * m.m31;
	thisMatrix -> m32 = lhs -> m31 * m.m12 + lhs -> m32 * m.m22 + lhs -> m33 * m.m32;
	thisMatrix -> m33 = lhs -> m31 * m.m13 + lhs -> m32 * m.m23 + lhs -> m33 * m.m33;
	thisMatrix -> m34 = lhs -> m31 * m.m14 + lhs -> m32 * m.m24 + lhs -> m33 * m.m34 + lhs -> m34;
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
void matrix3D_apprendTranslation( Matrix3D * m, Number x, Number y, Number z )
{
	Number mx = m -> m41, my = m -> m42, mz = m -> m43;

	m -> m14 = m -> m11 * x + m -> m12 * y + m -> m13 * z + mx;
	m -> m24 = m -> m21 * x + m -> m22 * y + m -> m23 * z + my;
	m -> m34 = m -> m31 * x + m -> m32 * y + m -> m33 * z + mz;
}

/**
�� Matrix3D ������ǰ��һ������ƽ�ƣ��� x��y �� z �����¶�λ��
�ڽ� Matrix3D ����Ӧ������ʾ����ʱ��������� Matrix3D ��������ִ��ƽ�Ƹ��ģ�Ȼ����ִ������ת���� 

ƽ��ָ����ʾ������䵱ǰλ���� x��y �� z ���ƶ��ľ��롣
prependTranslation() ������ƽ������Ϊ�������ᣨx��y��z�����е������������ļ���
��Ҫ��ƽ��ֻӰ���ض����ᣬ�뽫������������Ϊ�㡣
����Ϊ���ʾ�����ض���������κθ��ġ� 

ƽ�Ƹ��Ĳ��Ǿ��Ը��ġ�
��Ч��������йأ�����ԭʼλ�úͷ���Ĳ���֡��ԡ�
��Ҫȷ����ת��������о��Ը��ģ���ʹ�� recompose() ������
ת����˳��Ҳ����Ҫ��
��ƽ������ת��ת����������Ч��������ת��ƽ�Ƶ�ת����������Ч����ͬ��
��ʹ�� prependTranslation() ʱ����ʾ���󽫼�����������Եķ����ƶ�����������ת���޹ء�
���磬�����ʾ��������������� x �ᣬ��ö��󽫼������� prependTranslation() ����ָ���ķ����ƶ������������ת�ķ�ʽ�޹ء�
��Ҫ��ƽ�Ƹ���������ת��֮��������ʹ�� appendTranslation() ������ 
**/
void matrix3D_prependTranslation( Matrix3D * m, Number x, Number y, Number z )
{
	m -> m14 = m -> m14 + x;
	m -> m24 = m -> m24 + y;
	m -> m34 = m -> m34 + z;
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
void matrix3D_apprendRotation( Matrix3D * m, Number degrees,Vector3D * axis )
{
	Number angle = degrees * TORADIANS;
	Number c     = cos( angle );
	Number s     = sin( angle );

	Number _c    = 1 - c;

	Number xx    = axis -> x * axis -> x;
	Number yy    = axis -> y * axis -> y;
	Number zz    = axis -> z * axis -> z;

	Number xy    = axis -> x * axis -> y;
	Number xz    = axis -> x * axis -> z;
	Number yz    = axis -> y * axis -> z;

	Number xs    = axis -> x * s;
	Number zs    = axis -> z * s;
	Number ys    = axis -> y * s;

	Number xx_c  = xx * _c;
	Number yy_c  = yy * _c;
	Number zz_c  = zz * _c;

	Number xy_c  = xy * _c;
	Number xz_c  = xz * _c;
	Number yz_c  = yz * _c;

	Matrix3D rot;

	rot.m11 = c + xx_c;
	rot.m12 = xy_c - zs;
	rot.m13 = xz_c + ys;

	rot.m21 = xy_c + zs;
	rot.m22 = c + yy_c;
	rot.m23 = yz_c - xs;

	rot.m31 = xz_c - ys;
	rot.m32 = yz_c + xs;
	rot.m33 = c + zz_c;

	matrix3D_apprend( m, &rot );
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
��Ч��������йأ�����ԭʼλ�úͷ���Ĳ���֡��ԡ�
��Ҫȷ����ת�����о��Ը��ģ���ʹ�� recompose() ������ 
**/
void matrix3D_prependRotation( Matrix3D * m, Number degrees,Vector3D * axis )
{
	Number angle = degrees * TORADIANS;
	Number c     = cos( angle );
	Number s     = sin( angle );

	Number _c    = 1 - c;

	Number xx    = axis -> x * axis -> x;
	Number yy    = axis -> y * axis -> y;
	Number zz    = axis -> z * axis -> z;

	Number xy    = axis -> x * axis -> y;
	Number xz    = axis -> x * axis -> z;
	Number yz    = axis -> y * axis -> z;

	Number xs    = axis -> x * s;
	Number zs    = axis -> z * s;
	Number ys    = axis -> y * s;

	Number xx_c  = xx * _c;
	Number yy_c  = yy * _c;
	Number zz_c  = zz * _c;

	Number xy_c  = xy * _c;
	Number xz_c  = xz * _c;
	Number yz_c  = yz * _c;

	Matrix3D rot;

	rot.m11 = c + xx_c;
	rot.m12 = xy_c - zs;
	rot.m13 = xz_c + ys;
	rot.m14 = 0.0;

	rot.m21 = xy_c + zs;
	rot.m22 = c + yy_c;
	rot.m23 = yz_c - xs;
	rot.m24 = 0.0;

	rot.m31 = xz_c - ys;
	rot.m32 = yz_c + xs;
	rot.m33 = c + zz_c;
	rot.m34 = 0.0;

	matrix3D_prepend( m, & rot );
}

/**
�� Matrix3D �����Ϻ���һ���������ţ��� x��y �� z ��ı�λ�á�
�ڽ� Matrix3D ����Ӧ������ʾ����ʱ��������� Matrix3D ��������ִ������ת����Ȼ����ִ�����Ÿ��ġ�
Ĭ�ϵ�����ϵ��Ϊ (1.0, 1.0, 1.0)�� 

���Ž���Ϊ�������ᣨx��y��z�����е������������ļ����ж��塣���Խ�ÿ�����벻ͬ��������ˡ�
�ڽ����Ÿ���Ӧ������ʾ����ʱ���ö���Ĵ�С��������С��
���磬�� x��y �� z ������Ϊ 2 ��ʹ�����СΪԭ��������������������Ϊ 0.5 ��ʹ����Ĵ�СΪԭ����һ�롣
��Ҫȷ������ת��ֻӰ���ض����ᣬ�뽫������������Ϊ 1������Ϊ 1 ��ʾ�����ض���������κ����Ÿ��ġ� 

appendScale() ���������ڵ�����С�͹���Ť����������ʾ������������������������ĳ��λ���ϵķŴ����С��
����ʾ�������ת��ƽ�ƹ����У����Զ�ִ������ת���� 

ת����˳�����Ҫ��
�ȵ�����С��ƽ�Ƶ�ת����������Ч������ƽ���ٵ�����С��ת����������Ч����ͬ��
**/

void matrix3D_apprendScale( Matrix3D * m, Number xScale, Number yScale, Number zScale )
{
	m -> m11 = m -> m11 * xScale;
	m -> m12 = m -> m12 * yScale;
	m -> m13 = m -> m13 * zScale;

	m -> m21 = m -> m21 * xScale;
	m -> m22 = m -> m22 * yScale;
	m -> m23 = m -> m23 * zScale;

	m -> m31 = m -> m31 * xScale;
	m -> m32 = m -> m32 * yScale;
	m -> m33 = m -> m33 * zScale;
}

/**
�� Matrix3D ������ǰ��һ���������ţ��� x��y �� z ��ı�λ�á�
�ڽ� Matrix3D ����Ӧ������ʾ����ʱ��������� Matrix3D ��������ִ�����Ÿ��ģ�Ȼ����ִ������ת����
��Щ����������йأ�����ԭʼλ�úͷ���Ĳ���֡��ԡ�
Ĭ�ϵ�����ϵ��Ϊ (1.0, 1.0, 1.0)�� 

���Ž���Ϊ�������ᣨx��y��z�����е������������ļ����ж��塣
���Խ�ÿ�����벻ͬ��������ˡ�
�ڽ����Ÿ���Ӧ������ʾ����ʱ���ö���Ĵ�С��������С��
���磬�� x��y �� z ������Ϊ 2 ��ʹ�����СΪԭ��������������������Ϊ 0.5 ��ʹ����Ĵ�СΪԭ����һ�롣
��Ҫȷ������ת��ֻӰ���ض����ᣬ�뽫������������Ϊ 1��
����Ϊ 1 ��ʾ�����ض���������κ����Ÿ��ġ� 

prependScale() ���������ڵ�����С�͹���Ť����������ʾ��������������������������ĳ��λ���ϵķŴ����С��
����ʾ�������ת��ƽ�ƹ����У����Զ�ִ������ת���� 

ת����˳�����Ҫ��
�ȵ�����С��ƽ�Ƶ�ת����������Ч������ƽ���ٵ�����С��ת����������Ч����ͬ��
**/
void matrix3D_prependScale( Matrix3D * m, Number xScale, Number yScale, Number zScale )
{
	m -> m11 = m -> m11 * xScale;
	m -> m12 = m -> m12 * xScale;
	m -> m13 = m -> m13 * xScale;
	m -> m14 = m -> m14 * xScale;

	m -> m21 = m -> m21 * yScale;
	m -> m22 = m -> m22 * yScale;
	m -> m23 = m -> m23 * yScale;
	m -> m24 = m -> m24 * yScale;

	m -> m31 = m -> m31 * zScale;
	m -> m32 = m -> m32 * zScale;
	m -> m33 = m -> m33 * zScale;
	m -> m34 = m -> m34 * zScale;
}

/**
ʹ��ת������ Vector3D �����һ���ռ�����ת������һ���ռ����ꡣ
���ص� Vector3D ��������ת����������ꡣ
���� Vector3D ������Ӧ�����о���ת��������ƽ�ƣ��� 
**/
void matrix3D_transformVector( Matrix3D * m, Vector3D * v )
{
	Number x = v -> x, y = v -> y, z = v -> z; 

	v -> x = m -> m11 * x + m -> m12 * y + m -> m13 * z + m -> m14;
	v -> y = m -> m21 * x + m -> m22 * y + m -> m23 * z + m -> m24;
	v -> z = m -> m31 * x + m -> m32 * y + m -> m33 * z + m -> m34;
}

/**
����ͶӰ Matrix3D ���󣬽� Vector3D �����һ���ռ�����ͶӰ����һ���ռ����ꡣ
projectVector() ������ transformVector() �������ƣ�ֻ���� projectVector() ����������ͶӰ���ֵ������ԭʼ Vector3D ����� x��y �� z Ԫ�ء�
���ֵ��ָ��ͼ���ӽǿռ��д��ӵ㵽 Vector3D ����ľ��롣�˾����Ĭ��ֵΪ z Ԫ�ص�ֵ�� 
**/
void matrix3D_projectVector( Matrix3D * m, Vector3D * v )
{
	Number z = 1.0 / ( v -> x * m -> m41 + v -> y * m -> m42 + v -> z * m -> m43 + m -> m44 );


	matrix3D_transformVector( m, v );

	v -> x = v -> x * z;
	v -> y = v -> y * z;
	v -> z = z;
}

/**
����ǰ Matrix3D ����ת��Ϊһ�����󣬲����������е��к��С�
���磬�����ǰ Matrix3D ����� rawData �������� 16 �����֣�1,2,3,4,11,12,13,14,21,22,23,24,31,32,33,34���� transpose() �����Ὣÿ�ĸ�Ԫ����Ϊһ���ж�ȡ������Щ��ת��Ϊ�С�
���ɵĽ����һ�������� rawData Ϊ��1,11,21,31,2,12,22,32,3,13,23,33,4,14,24,34�� 

transpose() �����Ὣ��ǰ�����滻Ϊת�þ���
���Ҫת�þ��󣬶������ĵ�ǰ��������ʹ�� clone() �������Ƶ�ǰ����Ȼ������ɵĸ���Ӧ�� transpose() ������ 

����������һ�������ξ��󣬸þ����ת�þ�����������ͬ��
**/
void matrix3D_transpose( Matrix3D * m )
{
	m -> m12 = m -> m21 + ( m -> m21 = m -> m12 ) * 0.0;
	m -> m13 = m -> m31 + ( m -> m31 = m -> m13 ) * 0.0;
	m -> m14 = m -> m41 + ( m -> m41 = m -> m14 ) * 0.0;

	m -> m23 = m -> m32 + ( m -> m32 = m -> m23 ) * 0.0;
	m -> m24 = m -> m42 + ( m -> m42 = m -> m24 ) * 0.0;

	m -> m34 = m -> m43 + ( m -> m43 = m -> m34 ) * 0.0;
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
void matrix3D_invert( Matrix3D * m )
{
	Number d = matrix3D_determinant( m );

	Number m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34;

	if( d )
	{			
		m11 = m -> m11; m21 = m -> m21; m31 = m -> m31;
		m12 = m -> m12; m22 = m -> m22; m32 = m -> m32;
		m13 = m -> m13; m23 = m -> m23; m33 = m -> m33;
		m14 = m -> m14; m24 = m -> m24; m34 = m -> m34;
	
		d = 1.0 /d;

		m -> m11 =	 d * ( m22 * m33 - m32 * m23 );
		m -> m12 =	-d * ( m12 * m33 - m32 * m13 );
		m -> m13 =	 d * ( m12 * m23 - m22 * m13 );
		m -> m14 =	-d * ( m12 * ( m23 * m34 - m33 * m24 ) - m22 * ( m13 * m34 - m33 * m14 ) + m32 * ( m13 * m24 - m23 * m14 ) );
	
		m -> m21 =	-d * ( m21 * m33 - m31 * m23 );
		m -> m22 =	 d * ( m11 * m33 - m31 * m13 );
		m -> m23 =	-d * ( m11 * m23 - m21 * m13 );
		m -> m24 =	 d * ( m11 * ( m23 * m34 - m33 * m24 ) - m21 * ( m13 * m34 - m33 * m14 ) + m31 * ( m13 * m24 - m23 * m14 ) );
	
		m -> m31 =	 d * ( m21 * m32 - m31 * m22 );
		m -> m32 =	-d * ( m11 * m32 - m31 * m12 );
		m -> m33 =	 d * ( m11 * m22 - m21 * m12 );
		m -> m34 =	-d * ( m11 * ( m22 * m34 - m32 * m24 ) - m21 * ( m12 * m34 - m32 * m14 ) + m31 * ( m12 * m24 - m22 * m14 ) );
	}
}

/**
��ת�������ƽ�ơ���ת������������Ϊ������ Vector3D ������ɵ�ʸ�����ء�
��һ�� Vector3D ��������ƽ��Ԫ�ء�
�ڶ��� Vector3D ������������Ԫ�ء�
������ Vector3D ����������תԪ�ء� 
**/
void matrix3D_decompose( Matrix3D * m, Vector3D * position, Vector3D * scale, Vector3D * rotation )
{
	Matrix3D t;

	Number cosX,sinX;

	Vector3D * i = newVector3D( m -> m11, m -> m21, m -> m31, 1 );
	Vector3D * j = newVector3D( m -> m12, m -> m22, m -> m32, 1 );
	Vector3D * k = newVector3D( m -> m13, m -> m23, m -> m33, 1 );

	if( position )
	{
		position -> x = m -> m14;
		position -> y = m -> m24;
		position -> z = m -> m34;
	}

	if( scale && rotation )
	{
		scale -> x = vector3D_normalize( i );
		scale -> y = vector3D_normalize( j );
		scale -> z = vector3D_normalize( k );
	}
	else if( scale )
	{
		scale -> x = vector3D_length( i );
		scale -> y = vector3D_length( j );
		scale -> z = vector3D_length( k );

		return;
	}
	else if( rotation )
	{
		vector3D_normalize( i );
		vector3D_normalize( j );
		vector3D_normalize( k );
	}
	else
	{
		return;
	}

	t.m11 = i -> x; t.m12 = j -> x; t.m13 = k -> x; t.m14 = 0.0;
	t.m21 = i -> y; t.m22 = j -> y; t.m23 = k -> y; t.m24 = 0.0;
	t.m31 = i -> z; t.m32 = j -> z; t.m33 = k -> z; t.m34 = 0.0;
	t.m41 = 0.0;    t.m42 = 0.0;    t.m43 = 0.0;    t.m44 = 1.0;

	rotation -> x = atan2( t.m23, t.m33 );

	sinX = sin( rotation -> x );
	cosX = cos( rotation -> x );

	m -> m21 =   cosX * m -> m21 + sinX * m -> m31;
	m -> m22 =   cosX * m -> m22 + sinX * m -> m32;
	m -> m23 =   cosX * m -> m23 + sinX * m -> m33;
	m -> m24 =   cosX * m -> m24 + sinX * m -> m34;

	m -> m31 = - sinX * m -> m21 + cosX * m -> m31;
	m -> m32 = - sinX * m -> m22 + cosX * m -> m32;
	m -> m33 = - sinX * m -> m23 + cosX * m -> m33;
	m -> m34 = - sinX * m -> m24 + cosX * m -> m34;

	rotation -> y = atan2( - m -> m31, sqrt( m -> m11 * m -> m11 + m -> m21 * m -> m21) );
	rotation -> z = atan2( - m -> m12, m -> m11 );

	if( rotation -> x == PI )
	{
		if( rotation -> y > 0 )
		{
			rotation -> y -= PI;
		}
		else
		{
			rotation -> y += PI;
		}

		rotation -> x  = 0.0;
		rotation -> z += PI;
	}

	rotation -> x *= TODEGREES;
	rotation -> y *= TODEGREES;
	rotation -> z *= TODEGREES;
}

/**
����ת�������ƽ�ơ���ת���������á�
recompose() ���������ĸ����Ǿ��Ը��ġ�
recompose() ���������Ǿ���ת���� 

��Ҫʹ�þ��Ը�������֡���޸ľ���ת������ʹ�� decompose() �����������ã�Ȼ�������ʵ��ĸ��ġ�
Ȼ�󣬿���ʹ�� recompose() ������ Matrix3D ��������Ϊ�޸ĺ��ת���� 
**/
void matrix3D_recompose( Matrix3D * m, Vector3D * position, Vector3D * scale, Vector3D * rotation )
{
	matrix3D_identity( m );

	matrix3D_apprendTranslation( m, position -> x, position -> y, position -> z );

	matrix3D_apprendScale( m, scale -> x, scale -> y, scale -> z );

	matrix3D_apprendRotation( m, rotation -> x, X_AXIS() );

	matrix3D_apprendRotation( m, rotation -> y, Y_AXIS() );

	matrix3D_apprendRotation( m, rotation -> z, Z_AXIS() );
}

/**
��ת��ʾ������ʹ�䳯��ָ����λ�á�
�˷�������Է�����о͵��޸ġ�
��ʾ���󣨼� at Vector3D ���󣩵���ǰ����ʸ��ָ��ָ������ʵ�������λ�á�
��ʾ��������Ϸ������� up Vector3D ����ָ���ġ� 

pointAt() ������ʹ�������ʾ������ת����ֵ��Ч��
�˷����ɷֽ���ʾ����ľ����޸���תԪ�أ��Ӷ��ö���ת��ָ��λ�á�
Ȼ�󣬴˷�����������ɣ����£���ʾ����ľ����⽫ִ��ת����
����ö���ָ�������ƶ���Ŀ�꣨���������ƶ��Ķ���λ�ã��������ÿ���������ã��˷��������ö��������ƶ���Ŀ����ת�� 
**/
void matrix3D_pointAt( Matrix3D * m, Vector3D * pos, Vector3D * at, Vector3D * up )
{
	Vector3D * xAxis, * yAxis, * zAxis, scale;

	vector3D_normalize( at );

	if( vector3D_length( at ) )
	{
		xAxis = vector3D_crossProduct( at, up );
		vector3D_normalize( xAxis );

		yAxis = vector3D_crossProduct( at, xAxis );
		vector3D_normalize( yAxis );

		zAxis = at;

		matrix3D_decompose( m, NULL, & scale, NULL );

		m -> m11 =   xAxis -> x * scale.x;
		m -> m21 =   xAxis -> y * scale.x;
		m -> m31 =   xAxis -> z * scale.x;
				
		m -> m12 = - yAxis -> x * scale.y;
		m -> m22 = - yAxis -> y * scale.y;
		m -> m32 = - yAxis -> z * scale.y;
				
		m -> m13 =   zAxis -> x * scale.z;
		m -> m23 =   zAxis -> y * scale.z;
		m -> m33 =   zAxis -> z * scale.z;

		m -> m14 =   pos -> x;
		m -> m24 =   pos -> y;
		m -> m34 =   pos -> z;
	}
}

/**************************************************************************************
 **----------------------------------------------------------------------------------**
 **                                    |quaternion|                                  **
 **__________________________________________________________________________________**
 **************************************************************************************/

/**
�Ӿ���ת����Ԫ����
**/
Vector3D * matrix3D_toQuaternion( Matrix3D * m )
{
	Vector3D * q;

	Number s, tr = m -> m11 + m -> m22 + m -> m33, c[4];

	int i, j, k;

	if( ( q = ( Vector3D * )malloc( sizeof( Vector3D ) ) ) == NULL )
	{
		exit( TRUE );
	}

	if (tr > 0.0) 
	{
		s      = sqrt( tr + 1.0 );
		q -> w = s / 2.0;
		s      = 0.5 / s;
				
		q -> x = ( m -> m32 -  m -> m23 ) * s;
		q -> y = ( m -> m13 -  m -> m31 ) * s;
		q -> z = ( m -> m21 -  m -> m12 ) * s;
	} 
	else 
	{
		int nxt[3] = { 1, 2, 0 };

		Number a[3][4] = {
			{ m -> m11, m -> m12, m -> m13, m -> m14 },
			{ m -> m21, m -> m22, m -> m23, m -> m24 },
			{ m -> m31, m -> m32, m -> m33, m -> m34 }
			};
				
		i = 0;

		if ( a[1][1] > a[0][0]) 
		{
			i = 1;
		}

		if ( a[2][2] > a[i][i]) 
		{
			i = 2;
		}

		j = nxt[i];
		k = nxt[j];
		s = sqrt( ( a[i][i] - ( a[j][j] + a[k][k] ) ) + 1.0 );

		c[i] = s * 0.5;

		if ( s ) 
		{
			s = 0.5 / s;
		}

		c[3] = ( a[k][j] - a[j][k] ) * s;
		c[j] = ( a[j][i] + a[i][j] ) * s;
		c[k] = ( a[k][i] + a[i][k] ) * s;

		q -> x = c[0];
		q -> y = c[1];
		q -> z = c[2];
		q -> w = c[3];
	}

	return q;
}

/**
��Ԫ��ת�ɾ���
**/
Matrix3D * vector3D_toMatrix3D( Vector3D * q )
{
	Matrix3D * m;

	Number xx = q -> x * q -> x;
	Number xy = q -> x * q -> y;
	Number xz = q -> x * q -> z;
	Number xw = q -> x * q -> w;

	Number yy = q -> y * q -> y;
	Number yz = q -> y * q -> z;
	Number yw = q -> y * q -> w;

	Number zz = q -> z * q -> z;
	Number zw = q -> z * q -> w;
		
	if( ( m = ( Matrix3D * )malloc( sizeof( Matrix3D ) ) ) == NULL )
	{
		exit( TRUE );
	}

	m -> m11 = 1.0 - 2.0 * ( yy + zz );
	m -> m12 =       2.0 * ( xy - zw );
	m -> m13 =       2.0 * ( xz + yw );
	m -> m14 = 0.0;

	m -> m21 =       2.0 * ( xy + zw );
	m -> m22 = 1.0 - 2.0 * ( xx + zz );
	m -> m23 =       2.0 * ( yz - xw );
	m -> m24 = 0.0;

	m -> m31 =       2.0 * ( xz - yw );
	m -> m32 =       2.0 * ( yz + xw );
	m -> m33 = 1.0 - 2.0 * ( xx + yy );
	m -> m34 = 0.0;
	
	m -> m41 = 0.0;
	m -> m42 = 0.0;
	m -> m43 = 0.0;
	m -> m44 = 1.0;

	return m;
}

Vector3D vector3D_toEuler( Vector3D q )
{
	Vector3D euler;
			
	Number test = q.x * q.y + q.z * q.w, sqx, sqy, sqz;

	if ( test > 0.5 ) {
		euler.x = 2.0 * atan2( q.x,q.w );
		euler.y = PI / 2.0;
		euler.z = 0.0;
	}
	else if ( test < -0.5 ) {
		euler.x = -2.0 * atan2( q.x,q.w );
		euler.y = - PI / 2.0;
		euler.z = 0.0;
	}
	else
	{
		sqx = q.x * q.x;
		sqy = q.y * q.y;
		sqz = q.z * q.z;
		    
		euler.x = atan2( 2.0 * q.y * q.w - 2.0 * q.x * q.z, 1 - 2.0 * sqy - 2.0 * sqz );
		euler.y = asin( 2.0 * test );   
		euler.z = atan2( 2.0 * q.x * q.w - 2.0 * q.y * q.z, 1 - 2.0 * sqx - 2.0 * sqz );
		euler.w = 1.0;
	}
			
	return euler;
}

/**************************************************************************************
 **----------------------------------------------------------------------------------**
 **                                 |interpolation|                                  **
 **__________________________________________________________________________________**
 **************************************************************************************/

/*
ͨ����һ����ʾ����岹������Ŀ����ʾ�����һ���ٷֱ�λ�ã�������������֮֡���ֵ�Ĺ��̡�
���ɵĽ����һ���� Matrix3D �������У�����ƽ�ơ���ת�����ŵ�����Ԫ�ض��岹����ǰ��ʾ�����Ŀ����ʾ����֮���ֵ�� 

interpolate() �����ɱ�����ʹ�÷�����������ʾ���������ת���ԣ�ʱ����һЩ����Ҫ�Ľ����
interpolate() ������ʹ�������ʾ������ת����ֵ��Ч�����ڲ岹ǰ����ʾ�������ķ���Ԫ��ת��Ϊ��Ԫ����
�˷�������ȷ����ת��·�����������Ч��
��������ƽ���ġ����������������ת��
��ʹ��ŷ����ʱ����ʱ����������ÿ���ᣩ����������������
�����������������ת�Ĺ����У���Щ����ܻ���룬�Ӷ�������������
��Ԫ����ת�ɱ�������������� 

�� interpolate() �������������û����������Ч������ʾ����Ѹ��������Ȼ�����ӽ���һ����ʾ����
���磬����� thisMat ��������Ϊ���ص� Matrix3D ���󣬽� toMat ��������ΪĿ����ʾ��������� Matrix3D ���󣬲��� percent ��������Ϊ 0.1������ʾ����ᳯĿ������ƶ��ٷ�֮ʮ�����ں������û��ں�����֡�У�������ƶ�ʣ��� 90% �İٷ�֮ʮ��Ȼ���ƶ�ʣ��� 80% �İٷ�֮ʮ��ֱ�����󵽴�Ŀ�ꡣ 
**/
void matrix3D_interpolate( Matrix3D * thisMat, Matrix3D * toMat, Number percent, Matrix3D * target )
{
	Vector3D res;

	vector3D_slerp( matrix3D_toQuaternion( thisMat ), matrix3D_toQuaternion( toMat ), &res, percent );

	* target = * vector3D_toMatrix3D( & res );

	vector3D_slerp( matrix3D_getPosition( thisMat ), matrix3D_getPosition( toMat ), & res, percent );

	matrix3D_setPosition( target, & res );
}

/**
����ĳ��λ�ò岹����ķ���
pointTowards() ��������� Matrix3D.pointAt() �� Matrix3D.interpolateTo() �����Ĺ��ܡ� 

pointTowards() ��������Է�����о͵��޸ġ�
�˷���������ʾ����� Matrix3D ���зֽ⣬������תԪ���滻Ϊ��ʹ������Ŀ��λ�ý��в�ͬ�İٷֱ�ת���Ԫ�ء�
�˶���������԰��Լ��ķ����ƶ���ͬʱ������Ŀ��ת�䡣�� pointTowards() ���������ã����һ��ת�������������ɶ���׷�������ƶ���Ŀ���˶��Ķ�����
���Ƚ�����ָ��һ������Ŀ��İٷֱȵ㣬Ȼ����ĳ�������ƶ����� 
**/
void matrix3D_pointTowards( Number percent, Matrix3D * mat, Vector3D * pos, Vector3D * at, Vector3D * up, Matrix3D * target )
{
	Matrix3D m;

	Vector3D res;

	matrix3D_pointAt( & m, pos, at, up );

	vector3D_slerp( matrix3D_toQuaternion( mat ), matrix3D_toQuaternion( & m ), & res, percent );

	* target = * vector3D_toMatrix3D( & res );

	matrix3D_setPosition( target, pos );
}

Matrix3D * projectMatrix3D( Number top, Number bottom, Number left, Number right, Number near, Number far )
{
	Matrix3D * m;

	if( ( m = ( Matrix3D * )malloc( sizeof( Matrix3D ) ) ) == NULL )
	{
		exit( TRUE );
	}

	m -> m11 = 2.0 * near / ( right - left );
	m -> m12 = 0.0;
	m -> m13 = 0.0;
	m -> m14 = 0.0;

	m -> m21 = 0.0;
	m -> m22 = 2.0 * near / ( top - bottom );
	m -> m23 = 0.0;
	m -> m24 = 0.0;

	m -> m31 = 0.0;
	m -> m32 = 0.0;
	m -> m33 = - ( far + near ) / ( far - near );
	m -> m34 = - 2.0 * far      / ( far - near );

	m -> m41 = 0.0;
	m -> m42 = 0.0;
	m -> m43 = - 1;
	m -> m44 = 0.0;

	return m;
}

void matrix3D_projectMatrix( Matrix3D * m, Number top, Number bottom, Number left, Number right, Number near, Number far )
{
	m -> m11 =   2.0 * near     / ( right - left );
	m -> m22 =   2.0 * near     / ( top - bottom );
	m -> m33 = - ( far + near ) / ( far - near );
	m -> m34 = - 2.0 * far      / ( far - near );
}

void printfMatrix3D( Matrix3D m )
{
	printf( "\nMatrix3D(\n%-10.1lf%-10.1lf%-10.1lf%-10.1lf\n%-10.1lf%-10.1lf%-10.1lf%-10.1lf\n%-10.1lf%-10.1lf%-10.1lf%-10.1lf\n%-10.1lf%-10.1lf%-10.1lf%-10.1lf\n)\n", 
		m.m11, m.m12, m.m13, m.m14, m.m21, m.m22, m.m23, m.m24, m.m31, m.m32, m.m33, m.m34, m.m41, m.m42, m.m43, m.m44 );
}

# endif
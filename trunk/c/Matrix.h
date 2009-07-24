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
将当前矩阵转换为恒等或单位矩阵。恒等矩阵中的主对角线位置上的元素的值为一，而所有其他元素的值为零。
生成的结果是一个矩阵，其中，rawData 值为 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1，旋转设置为 Vector3D(0,0,0)，位置或平移设置为 Vector3D(0,0,0)，缩放设置为 Vector3D(1,1,1)。
**/
void matrix3D_identity( Matrix3D * m )
{
	m -> m11 = 1.0; m -> m12 = 0.0; m -> m13 = 0.0; m -> m14 = 0.0;

	m -> m21 = 0.0; m -> m22 = 1.0; m -> m23 = 0.0; m -> m24 = 0.0;

	m -> m31 = 0.0; m -> m32 = 0.0; m -> m33 = 1.0; m -> m34 = 0.0;

	m -> m41 = 0.0; m -> m42 = 0.0; m -> m43 = 0.0; m -> m44 = 1.0;
}
/**
复制矩阵。
**/
void matrix3D_copy( Matrix3D * m, Matrix3D * c )
{
	m -> m11 = c -> m11; m -> m12 = c -> m12; m -> m13 = c -> m13; m -> m14 = c -> m14;

	m -> m21 = c -> m21; m -> m22 = c -> m22; m -> m23 = c -> m23; m -> m24 = c -> m24;

	m -> m31 = c -> m31; m -> m32 = c -> m32; m -> m33 = c -> m33; m -> m34 = c -> m34;

	m -> m41 = c -> m41; m -> m42 = c -> m42; m -> m43 = c -> m43; m -> m44 = c -> m44;
}
/**
一个用于确定矩阵是否可逆的数字。 

Matrix3D 对象必须是可逆的。可以使用 determinant 属性确保 Matrix3D 对象是可逆的。
如果行列式为零，则矩阵没有逆矩阵。
例如，如果矩阵的整个行或列为零，或如果两个行或列相等，则行列式为零。
行列式还可用于对一系列方程进行求解。 
**/
Number matrix3D_determinant( Matrix3D * m )
{
	return ( m -> m11 * m -> m22 - m -> m21 * m -> m12 ) * m -> m33 - ( m -> m11 * m -> m32 - m -> m31 * m -> m12 ) * m -> m23 + ( m -> m21 * m -> m32 - m -> m31 * m -> m22 ) * m -> m13;
}

/**
一个由 16 个数字组成的矢量，其中，每四个元素可以是 4x4 矩阵的一行或一列。 

如果 rawData 属性设置为一个不可逆的矩阵，则会引发异常。Matrix3D 对象必须是可逆的。
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
一个由 16 个数字组成的矢量，其中，每四个元素可以是 4x4 矩阵的一行或一列。 

如果 rawData 属性设置为一个不可逆的矩阵，则会引发异常。Matrix3D 对象必须是可逆的。
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
创建 Matrix3D 对象。可以使用一个由 16 个数字组成的矢量来初始化 Matrix3D 对象，其中，每四个元素可以是一行或一列。
创建 Matrix3D 对象之后，可以使用 rawData 属性访问该对象的矩阵元素。 

如果未定义任何参数，则构造函数会生成一个恒等或单位 Matrix3D 对象。
在矩阵表示法中，恒等矩阵中的主对角线位置上的所有元素的值均为一，而所有其他元素的值均为零。
恒等矩阵的 rawData 属性的值为 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1。
恒等矩阵的位置或平移值为 Vector3D(0,0,0)，旋转设置为 Vector3D(0,0,0)，缩放值为 Vector3D(1,1,1)。
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
返回一个新 Matrix3D 对象，它是与当前 Matrix3D 对象完全相同的副本。
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
一个保存显示对象在转换参照帧中的 3D 坐标 (x,y,z) 位置的 Vector3D 对象。
利用 position 属性，可立即访问显示对象矩阵的平移矢量，而无需分解和重新组合矩阵。 

利用 position 属性，可以获取和设置转换矩阵的平移元素。
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
一个保存显示对象在转换参照帧中的 3D 坐标 (x,y,z) 位置的 Vector3D 对象。
利用 position 属性，可立即访问显示对象矩阵的平移矢量，而无需分解和重新组合矩阵。 

利用 position 属性，可以获取和设置转换矩阵的平移元素。
**/
void matrix3D_setPosition( Matrix3D * m, Vector3D * v )
{
	m -> m14 = v -> x;
	m -> m24 = v -> y;
	m -> m34 = v -> z;
	m -> m44 = v -> w;
}

/**
矩阵相乘。
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
通过将另一个 Matrix3D 对象与当前 Matrix3D 对象相乘来后置一个矩阵。
得到的结果将合并两个矩阵转换。可以将一个 Matrix3D 对象与多个矩阵相乘。最终的 Matrix3D 对象将包含所有转换的结果。 

矩阵乘法运算与矩阵加法运算不同。矩阵乘法运算是不可交换的。
换句话说，A 乘以 B 并不等于 B 乘以 A。
在使用 append() 方法时，乘法运算将从左侧开始，这意味着 lhs Matrix3D 对象位于乘法运算符的左侧。 

thisMatrix = lhs * thisMatrix; 
首次调用 append() 方法时，此方法会对父级空间进行相关修改。
后续调用与后置的 Matrix3D 对象的参照帧相关。 

append() 方法会将当前矩阵替换为后置的矩阵。
如果要后置两个矩阵，而不更改当前矩阵，请使用 clone() 方法复制当前矩阵，然后对生成的副本应用 append() 方法。 
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
在 Matrix3D 对象上后置一个增量平移，沿 x、y 和 z 轴重新定位。
在将 Matrix3D 对象应用于显示对象时，矩阵会在 Matrix3D 对象中先执行其他转换，然后再执行平移更改。 

平移将作为沿三个轴（x、y、z）进行的三个增量更改集进行定义。
在对显示对象应用转换时，显示对象会从当前位置沿 x、y 和 z 轴按参数指定的增量移动。
若要确保平移只影响特定的轴，请将其他参数设置为零。参数为零表示不沿特定的轴进行任何更改。

平移更改不是绝对更改。
它们与矩阵的当前位置和方向相关。
若要确保对转换矩阵进行绝对更改，请使用 recompose() 方法。转换的顺序也很重要。
先平移再旋转的转换所产生的效果与先旋转再平移所产生的效果不同。 
**/
void matrix3D_apprendTranslation( Matrix3D * m, Number x, Number y, Number z )
{
	Number mx = m -> m41, my = m -> m42, mz = m -> m43;

	m -> m14 = m -> m11 * x + m -> m12 * y + m -> m13 * z + mx;
	m -> m24 = m -> m21 * x + m -> m22 * y + m -> m23 * z + my;
	m -> m34 = m -> m31 * x + m -> m32 * y + m -> m33 * z + mz;
}

/**
在 Matrix3D 对象上前置一个增量平移，沿 x、y 和 z 轴重新定位。
在将 Matrix3D 对象应用于显示对象时，矩阵会在 Matrix3D 对象中先执行平移更改，然后再执行其他转换。 

平移指定显示对象从其当前位置沿 x、y 和 z 轴移动的距离。
prependTranslation() 方法将平移设置为沿三个轴（x、y、z）进行的三个增量更改集。
若要让平移只影响特定的轴，请将其他参数设置为零。
参数为零表示不沿特定的轴进行任何更改。 

平移更改不是绝对更改。
此效果与对象有关，它与原始位置和方向的参照帧相对。
若要确保对转换矩阵进行绝对更改，请使用 recompose() 方法。
转换的顺序也很重要。
先平移再旋转的转换所产生的效果与先旋转再平移的转换所产生的效果不同。
当使用 prependTranslation() 时，显示对象将继续按照其面对的方向移动，这与其他转换无关。
例如，如果显示对象面向的是正向 x 轴，则该对象将继续按照 prependTranslation() 方法指定的方向移动，这与对象旋转的方式无关。
若要让平移更改在其他转换之后发生，请使用 appendTranslation() 方法。 
**/
void matrix3D_prependTranslation( Matrix3D * m, Number x, Number y, Number z )
{
	m -> m14 = m -> m14 + x;
	m -> m24 = m -> m24 + y;
	m -> m34 = m -> m34 + z;
}

/**
在 Matrix3D 对象上前置一个增量旋转。
在将 Matrix3D 对象应用于显示对象时，矩阵会在 Matrix3D 对象中先执行旋转，然后再执行其他转换。 

显示对象的旋转由以下元素定义：一个轴、绕该轴旋转的增量角度和对象旋转中心的可选轴点。
轴可以是任何常规方向。
常见的轴为 XAXIS (Vector3D(1,0,0))、YAXIS (Vector3D(0,1,0)) 和 ZAXIS (Vector3D(0,0,1))。
在航空术语中，有关 y 轴的旋转称为偏航。有关 x 轴的旋转称为俯仰。有关 z 轴的旋转称为翻滚。 

转换的顺序很重要。
先旋转再平移的转换所产生的效果与先平移再旋转所产生的效果不同。

旋转效果不是绝对效果。
此效果与对象有关，它与原始位置和方向的参照帧相对。若要确保对转换进行绝对更改，请使用 recompose() 方法。
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
在 Matrix3D 对象上前置一个增量旋转。
在将 Matrix3D 对象应用于显示对象时，矩阵会在 Matrix3D 对象中先执行旋转，然后再执行其他转换。 

显示对象的旋转由以下元素定义：一个轴、绕该轴旋转的增量角度和对象旋转中心的可选轴点。
轴可以是任何常规方向。
常见的轴为 XAXIS (Vector3D(1,0,0))、YAXIS (Vector3D(0,1,0)) 和 ZAXIS (Vector3D(0,0,1))。
在航空术语中，有关 y 轴的旋转称为偏航。有关 x 轴的旋转称为俯仰。有关 z 轴的旋转称为翻滚。 

转换的顺序很重要。
先旋转再平移的转换所产生的效果与先平移再旋转所产生的效果不同。

旋转效果不是绝对效果。
此效果与对象有关，它与原始位置和方向的参照帧相对。
若要确保对转换进行绝对更改，请使用 recompose() 方法。 
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
在 Matrix3D 对象上后置一个增量缩放，沿 x、y 和 z 轴改变位置。
在将 Matrix3D 对象应用于显示对象时，矩阵会在 Matrix3D 对象中先执行其他转换，然后再执行缩放更改。
默认的缩放系数为 (1.0, 1.0, 1.0)。 

缩放将作为沿三个轴（x、y、z）进行的三个增量更改集进行定义。可以将每个轴与不同的数字相乘。
在将缩放更改应用于显示对象时，该对象的大小会增大或减小。
例如，将 x、y 和 z 轴设置为 2 将使对象大小为原来的两倍，而将轴设置为 0.5 将使对象的大小为原来的一半。
若要确保缩放转换只影响特定的轴，请将其他参数设置为 1。参数为 1 表示不沿特定的轴进行任何缩放更改。 

appendScale() 方法可用于调整大小和管理扭曲（例如显示对象的拉伸或收缩），或用于某个位置上的放大和缩小。
在显示对象的旋转和平移过程中，将自动执行缩放转换。 

转换的顺序很重要。
先调整大小再平移的转换所产生的效果与先平移再调整大小的转换所产生的效果不同。
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
在 Matrix3D 对象上前置一个增量缩放，沿 x、y 和 z 轴改变位置。
在将 Matrix3D 对象应用于显示对象时，矩阵会在 Matrix3D 对象中先执行缩放更改，然后再执行其他转换。
这些更改与对象有关，它与原始位置和方向的参照帧相对。
默认的缩放系数为 (1.0, 1.0, 1.0)。 

缩放将作为沿三个轴（x、y、z）进行的三个增量更改集进行定义。
可以将每个轴与不同的数字相乘。
在将缩放更改应用于显示对象时，该对象的大小会增大或减小。
例如，将 x、y 和 z 轴设置为 2 将使对象大小为原来的两倍，而将轴设置为 0.5 将使对象的大小为原来的一半。
若要确保缩放转换只影响特定的轴，请将其他参数设置为 1。
参数为 1 表示不沿特定的轴进行任何缩放更改。 

prependScale() 方法可用于调整大小和管理扭曲（例如显示对象的拉伸或收缩），还可用于某个位置上的放大和缩小。
在显示对象的旋转和平移过程中，将自动执行缩放转换。 

转换的顺序很重要。
先调整大小再平移的转换所产生的效果与先平移再调整大小的转换所产生的效果不同。
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
使用转换矩阵将 Vector3D 对象从一个空间坐标转换到另一个空间坐标。
返回的 Vector3D 对象将容纳转换后的新坐标。
将对 Vector3D 对象中应用所有矩阵转换（包括平移）。 
**/
void matrix3D_transformVector( Matrix3D * m, Vector3D * v )
{
	Number x = v -> x, y = v -> y, z = v -> z; 

	v -> x = m -> m11 * x + m -> m12 * y + m -> m13 * z + m -> m14;
	v -> y = m -> m21 * x + m -> m22 * y + m -> m23 * z + m -> m24;
	v -> z = m -> m31 * x + m -> m32 * y + m -> m33 * z + m -> m34;
}

/**
利用投影 Matrix3D 对象，将 Vector3D 对象从一个空间坐标投影到另一个空间坐标。
projectVector() 方法与 transformVector() 方法类似，只不过 projectVector() 方法将按照投影深度值来划分原始 Vector3D 对象的 x、y 和 z 元素。
深度值是指视图或视角空间中从视点到 Vector3D 对象的距离。此距离的默认值为 z 元素的值。 
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
将当前 Matrix3D 对象转换为一个矩阵，并将互换其中的行和列。
例如，如果当前 Matrix3D 对象的 rawData 包含以下 16 个数字：1,2,3,4,11,12,13,14,21,22,23,24,31,32,33,34，则 transpose() 方法会将每四个元素作为一个行读取并将这些行转换为列。
生成的结果是一个矩阵，其 rawData 为：1,11,21,31,2,12,22,32,3,13,23,33,4,14,24,34。 

transpose() 方法会将当前矩阵替换为转置矩阵。
如果要转置矩阵，而不更改当前矩阵，请先使用 clone() 方法复制当前矩阵，然后对生成的副本应用 transpose() 方法。 

正交矩阵是一个正方形矩阵，该矩阵的转置矩阵和逆矩阵相同。
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

Matrix3D 对象必须是可逆的。
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
将转换矩阵的平移、旋转和缩放设置作为由三个 Vector3D 对象组成的矢量返回。
第一个 Vector3D 对象容纳平移元素。
第二个 Vector3D 对象容纳缩放元素。
第三个 Vector3D 对象容纳旋转元素。 
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
设置转换矩阵的平移、旋转和缩放设置。
recompose() 方法所做的更改是绝对更改。
recompose() 方法将覆盖矩阵转换。 

若要使用绝对父级参照帧来修改矩阵转换，请使用 decompose() 方法检索设置，然后做出适当的更改。
然后，可以使用 recompose() 方法将 Matrix3D 对象设置为修改后的转换。 
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
旋转显示对象以使其朝向指定的位置。
此方法允许对方向进行就地修改。
显示对象（即 at Vector3D 对象）的向前方向矢量指向指定的现实世界相关位置。
显示对象的向上方向是用 up Vector3D 对象指定的。 

pointAt() 方法可使缓存的显示对象旋转属性值无效。
此方法可分解显示对象的矩阵并修改旋转元素，从而让对象转向指定位置。
然后，此方法将重新组成（更新）显示对象的矩阵，这将执行转换。
如果该对象指向正在移动的目标（例如正在移动的对象位置），则对于每个后续调用，此方法都会让对象朝正在移动的目标旋转。 
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
从矩阵转成四元数。
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
四元数转成矩阵。
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
通过将一个显示对象插补到靠近目标显示对象的一个百分比位置，简化在两个参照帧之间插值的过程。
生成的结果是一个新 Matrix3D 对象，其中，用于平移、旋转和缩放的所有元素都插补到当前显示对象和目标显示对象之间的值。 

interpolate() 方法可避免在使用方法（例如显示对象的轴旋转属性）时出现一些不需要的结果。
interpolate() 方法可使缓存的显示对象旋转属性值无效，并在插补前将显示对象矩阵的方向元素转换为四元数。
此方法可以确保旋转的路径最短且最有效。
它还生成平滑的、无万向节锁定的旋转。
当使用欧拉角时（此时将单独处理每个轴）会出现万向节锁定。
在绕两个或更多轴旋转的过程中，这些轴可能会对齐，从而导致意外结果。
四元数旋转可避免万向节锁定。 

对 interpolate() 方法的连续调用会产生这样的效果：显示对象迅速启动，然后缓慢接近另一个显示对象。
例如，如果将 thisMat 参数设置为返回的 Matrix3D 对象，将 toMat 参数设置为目标显示对象关联的 Matrix3D 对象，并将 percent 参数设置为 0.1，则显示对象会朝目标对象移动百分之十。对于后续调用或在后续的帧中，对象会移动剩余的 90% 的百分之十，然后移动剩余的 80% 的百分之十，直到对象到达目标。 
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
朝着某个位置插补对象的方向。
pointTowards() 方法结合了 Matrix3D.pointAt() 和 Matrix3D.interpolateTo() 方法的功能。 

pointTowards() 方法允许对方向进行就地修改。
此方法将对显示对象的 Matrix3D 进行分解，并将旋转元素替换为可使对象朝着目标位置进行不同的百分比转变的元素。
此对象可以在仍按自己的方向移动的同时，逐步向目标转变。对 pointTowards() 的连续调用（后跟一个转换方法）可生成对象追逐或紧随移动的目标运动的动画。
首先将对象指向一个朝向目标的百分比点，然后沿某个轴逐步移动对象。 
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
#ifndef __QUATERNION_H_INCLUDED__
#define __QUATERNION_H_INCLUDED__

#include <stdio.h>

#include "Base.h"
#include "Matrix3D.h"

typedef struct
{
	Number w;
	Number x;
	Number y;
	Number z;
}Quaternion;

/**
* 从指定欧拉角构造四元数
*/
Quaternion * quaternoin_setFromEuler(Number ay, Number ax, Number az)
{
	Number sp;
	Number sb;
	Number sh;
	Number cp;
	Number cb;
	Number ch;

	Quaternion * qua;

	if( ( qua = ( Quaternion * )malloc( sizeof( Quaternion ) ) ) == NULL )
	{
		exit( 0 );
	}

	sp = sin(ax * 0.5); 
	cp = cos(ax * 0.5); 
	sb = sin(az * 0.5);
	cb = cos(az * 0.5);
	sh = sin(ay * 0.5);
	ch = cos(ay * 0.5);

	qua->w =  ch*cp*cb + sh*sp*sb;
	qua->x = -ch*sp*cb - sh*cp*sb;
	qua->y =  ch*sp*sb - sh*cb*cp;
	qua->z =  sh*sp*cb - ch*cp*sb;

	return qua;
}

/**
* 从四元数返回矩阵
*/
Matrix3D * quaternoin_getMatrix(Quaternion * qua)
{
	Number w = qua->w;
	Number x = qua->x;
	Number y = qua->y;
	Number z = qua->z;

	Number xx = x * x;
	Number xy = x * y;
	Number xz = x * z;
	Number xw = x * w;
	Number yy = y * y;
	Number yz = y * z;
	Number yw = y * w;
	Number zz = z * z;
	Number zw = z * w;

	Matrix3D * mtr;
	if( (mtr = (Matrix3D *)malloc( sizeof( Matrix3D ) ) ) == NULL )
	{
		exit( 0 );
	}

	mtr->m11 = 1 - 2 * ( yy + zz );
	mtr->m12 = 2 * ( xy + zw );
	mtr->m13 = 2 * ( xz - yw );
	mtr->m14 = 0;
	mtr->m21 = 2 * ( xy - zw );
	mtr->m22 = 1 - 2 * ( xx + zz );
	mtr->m23 = 2 * ( yz + xw );
	mtr->m24 = 0;
	mtr->m31 = 2 * ( xz + yw );
	mtr->m32 = 2 * ( yz - xw );
	mtr->m33 = 1 - 2 * ( xx + yy );
	mtr->m34 = 0;
	mtr->m41 = 0;
	mtr->m42 = 0;
	mtr->m43 = 0;
	mtr->m44 = 1;

	return mtr;
}

#endif
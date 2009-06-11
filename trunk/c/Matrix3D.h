# include < stdlib.h >

# include "Vector3D.h"

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

void matrix3D_identity( Matrix3D * m )
{
	( * m ).m11 = 1; ( * m ).m12 = 0; ( * m ).m13 = 0; ( * m ).m14 = 0;

	( * m ).m21 = 0; ( * m ).m22 = 1; ( * m ).m23 = 0; ( * m ).m24 = 0;

	( * m ).m31 = 0; ( * m ).m32 = 0; ( * m ).m33 = 1; ( * m ).m34 = 0;

	( * m ).m41 = 0; ( * m ).m42 = 0; ( * m ).m43 = 0; ( * m ).m44 = 1;
}

/*Matrix3D clone( Matrix3D m )
{
	return newMatrix3D( getRawData( m ) );
}*/

void matrix3D_copy( Matrix3D * m, Matrix3D c )
{
	( * m ).m11 = c.m11; ( * m ).m12 = c.m12; ( * m ).m13 = c.m13; ( * m ).m14 = c.m14;

	( * m ).m21 = c.m21; ( * m ).m22 = c.m22; ( * m ).m23 = c.m23; ( * m ).m24 = c.m24;

	( * m ).m31 = c.m31; ( * m ).m32 = c.m32; ( * m ).m33 = c.m33; ( * m ).m34 = c.m34;

	( * m ).m41 = c.m41; ( * m ).m42 = c.m42; ( * m ).m43 = c.m43; ( * m ).m44 = c.m44;
}

Number ( * matrix3D_getRawData( Matrix3D m ) )[16]
{
	Number ( * rawData )[16];

	rawData = calloc( 16, sizeof( Number ) );

	( * rawData )[0]  = m.m11;
	( * rawData )[1]  = m.m21;
	( * rawData )[2]  = m.m31;
	( * rawData )[3]  = m.m41;
	
	( * rawData )[4]  = m.m12;
	( * rawData )[5]  = m.m22;
	( * rawData )[6]  = m.m32;
	( * rawData )[7]  = m.m42;

	( * rawData )[8]  = m.m13;
	( * rawData )[9]  = m.m23;
	( * rawData )[10] = m.m33;
	( * rawData )[11] = m.m43;

	( * rawData )[12] = m.m14;
	( * rawData )[13] = m.m24;
	( * rawData )[14] = m.m34;
	( * rawData )[15] = m.m44;

	return rawData;
}

void matrix3D_setRawData( Matrix3D * m, Number rawData[16] )
{
	( * m ).m11 = rawData[0];
	( * m ).m21 = rawData[1];
	( * m ).m31 = rawData[2];
	( * m ).m41 = rawData[3];

	( * m ).m12 = rawData[4];
	( * m ).m22 = rawData[5];
	( * m ).m32 = rawData[6];
	( * m ).m42 = rawData[7];

	( * m ).m13 = rawData[8];
	( * m ).m23 = rawData[9];
	( * m ).m33 = rawData[10];
	( * m ).m43 = rawData[11];

	( * m ).m14 = rawData[12];
	( * m ).m24 = rawData[13];
	( * m ).m34 = rawData[14];
	( * m ).m44 = rawData[15];
}

Matrix3D newMatrix3D( Number rawData[16] )
{
	Matrix3D m;

	matrix3D_setRawData( & m, rawData );

	return m;
}

Vector3D matrix3D_getPosition( Matrix3D m )
{
	Vector3D v;

	v.x = m.m14;
	v.y = m.m24;
	v.z = m.m34;
	v.w = m.m44;

	return v;
}

void matrix3D_setPosition( Matrix3D * m, Vector3D v )
{
	( * m ).m14 = v.x;
	( * m ).m24 = v.y;
	( * m ).m34 = v.z;
	( * m ).m44 = v.w;
}

Matrix3D matrix3D_multiply( Matrix3D m1, Matrix3D m2 )
{
	Matrix3D m;

	m.m11 = m1.m11 * m2.m11 + m1.m12 * m2.m21 + m1.m13 * m2.m31 + m1.m14 * m2.m41;
	m.m12 = m1.m11 * m2.m12 + m1.m12 * m2.m22 + m1.m13 * m2.m32 + m1.m14 * m2.m42;
	m.m13 = m1.m11 * m2.m13 + m1.m12 * m2.m23 + m1.m13 * m2.m33 + m1.m14 * m2.m43;
	m.m14 = m1.m11 * m2.m14 + m1.m12 * m2.m24 + m1.m13 * m2.m34 + m1.m14 * m2.m44;

	m.m21 = m1.m21 * m2.m11 + m1.m22 * m2.m21 + m1.m23 * m2.m31 + m1.m24 * m2.m41;
	m.m22 = m1.m21 * m2.m12 + m1.m22 * m2.m22 + m1.m23 * m2.m32 + m1.m24 * m2.m42;
	m.m23 = m1.m21 * m2.m13 + m1.m22 * m2.m23 + m1.m23 * m2.m33 + m1.m24 * m2.m43;
	m.m24 = m1.m21 * m2.m14 + m1.m22 * m2.m24 + m1.m23 * m2.m34 + m1.m24 * m2.m44;

	m.m31 = m1.m31 * m2.m11 + m1.m32 * m2.m21 + m1.m33 * m2.m31 + m1.m34 * m2.m41;
	m.m32 = m1.m31 * m2.m12 + m1.m32 * m2.m22 + m1.m33 * m2.m32 + m1.m34 * m2.m42;
	m.m33 = m1.m31 * m2.m13 + m1.m32 * m2.m23 + m1.m33 * m2.m33 + m1.m34 * m2.m43;
	m.m34 = m1.m31 * m2.m14 + m1.m32 * m2.m24 + m1.m33 * m2.m34 + m1.m34 * m2.m44;


	m.m41 = m1.m41 * m2.m11 + m1.m42 * m2.m21 + m1.m43 * m2.m31 + m1.m44 * m2.m41;
	m.m42 = m1.m41 * m2.m12 + m1.m42 * m2.m22 + m1.m43 * m2.m32 + m1.m44 * m2.m42;
	m.m43 = m1.m41 * m2.m13 + m1.m42 * m2.m23 + m1.m43 * m2.m33 + m1.m44 * m2.m43;
	m.m44 = m1.m41 * m2.m14 + m1.m42 * m2.m24 + m1.m43 * m2.m34 + m1.m44 * m2.m44;

	return m;
}

void matrix3D_apprend( Matrix3D * m, Matrix3D a )
{
	matrix3D_copy( m, matrix3D_multiply( * m, a ) );
}

void matrix3D_prepend( Matrix3D * m, Matrix3D a )
{
	matrix3D_copy( m, matrix3D_multiply( a, * m ) );
}

Matrix3D translationMatrix3D( x, y, z )
{
	Matrix3D tran;

	tran.m11 = 1; tran.m12 = 0; tran.m13 = 0; tran.m14 = x;

	tran.m21 = 0; tran.m22 = 1; tran.m23 = 0; tran.m24 = y;

	tran.m31 = 0; tran.m32 = 0; tran.m33 = 1; tran.m34 = z;

	tran.m41 = 0; tran.m42 = 0; tran.m43 = 0; tran.m44 = 1;

	return tran;
}

void matrix3D_apprendTranslation( Matrix3D * m, Number x, Number y, Number z )
{
	matrix3D_apprend( m, translationMatrix3D( x, y, z ) );
}

void matrix3D_prependTranslation( Matrix3D * m, Number x, Number y, Number z )
{
	matrix3D_prepend( m, translationMatrix3D( x, y, z ) );
}

Matrix3D rotationMatrix3D(  Number degrees, Vector3D axis )
{
	Number angle = degrees * TORADIANS;
	Number c     = cos( angle );
	Number s     = sin( angle );

	Number _c    = 1 - c;

	Number xx    = axis.x * axis.x;
	Number yy    = axis.y * axis.y;
	Number zz    = axis.z * axis.z;

	Number xy    = axis.x * axis.y;
	Number xz    = axis.x * axis.z;
	Number yz    = axis.y * axis.z;

	Number xs    = axis.x * s;
	Number zs    = axis.z * s;
	Number ys    = axis.y * s;

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
	rot.m14 = 0;

	rot.m21 = xy_c + zs;
	rot.m22 = c + yy_c;
	rot.m23 = yz_c - xs;
	rot.m24 = 0;

	rot.m31 = xz_c - ys;
	rot.m32 = yz_c + xs;
	rot.m33 = c + zz_c;
	rot.m34 = 0;

	rot.m41 = 0;
	rot.m42 = 0;
	rot.m43 = 0;
	rot.m44 = 1.0;

	return rot;
}

void matrix3D_apprendRotation( Matrix3D * m, Number degrees,Vector3D axis )
{
	matrix3D_apprend( m, rotationMatrix3D(  degrees, axis ) );
}

void matrix3D_prependRotation( Matrix3D * m, Number degrees,Vector3D axis )
{
	matrix3D_prepend( m, rotationMatrix3D(  degrees, axis ) );
}

Matrix3D scaleMatrix3D( Number xScale, Number yScale, Number zScale )
{
	Matrix3D sca;
	
	sca.m11 = xScale;
	sca.m12 = 0;
	sca.m13 = 0;
	sca.m14 = 0;

	sca.m21 = 0;
	sca.m22 = yScale;
	sca.m23 = 0;
	sca.m24 = 0;

	sca.m31 = 0;
	sca.m32 = 0;
	sca.m33 = zScale;
	sca.m34 = 0;

	sca.m41 = 0;
	sca.m42 = 0;
	sca.m43 = 0;
	sca.m44 = 1;

	return sca;
}

void matrix3D_apprendScale( Matrix3D * m, Number xScale, Number yScale, Number zScale )
{
	matrix3D_apprend( m, scaleMatrix3D( xScale, yScale, zScale ) );
}

void matrix3D_prependScale( Matrix3D * m, Number xScale, Number yScale, Number zScale )
{
	matrix3D_prepend( m, scaleMatrix3D( xScale, yScale, zScale ) );
}

void matrix3D_transformVector( Matrix3D m, Vector3D * v )
{
	Number x,y,z,w;

	x = m.m11 * ( * v ).x + m.m12 * ( * v ).y + m.m13 * ( * v ).z + m.m14 * ( * v ).w;
	y = m.m21 * ( * v ).x + m.m22 * ( * v ).y + m.m23 * ( * v ).z + m.m24 * ( * v ).w;
	z = m.m31 * ( * v ).x + m.m32 * ( * v ).y + m.m33 * ( * v ).z + m.m34 * ( * v ).w;
	w = m.m41 * ( * v ).x + m.m42 * ( * v ).y + m.m43 * ( * v ).z + m.m44 * ( * v ).w;

	( * v ).x = x;
	( * v ).y = y;
	( * v ).z = z;
	( * v ).w = w;
}

void matrix3D_transpose( Matrix3D * m )
{
	( * m ).m12 = ( * m ).m21 + ( ( * m ).m21 = ( * m ).m12 ) * 0;
	( * m ).m13 = ( * m ).m31 + ( ( * m ).m31 = ( * m ).m13 ) * 0;
	( * m ).m14 = ( * m ).m41 + ( ( * m ).m41 = ( * m ).m14 ) * 0;

	( * m ).m23 = ( * m ).m32 + ( ( * m ).m32 = ( * m ).m23 ) * 0;
	( * m ).m24 = ( * m ).m42 + ( ( * m ).m42 = ( * m ).m24 ) * 0;

	( * m ).m34 = ( * m ).m43 + ( ( * m ).m43 = ( * m ).m34 ) * 0;
}

Number matrix3D_determinant( Matrix3D m )
{
	return ( m.m11 * m.m22 - m.m21 * m.m12 ) * m.m33 - ( m.m11 * m.m32 - m.m31 * m.m12 ) * m.m23 + ( m.m21 * m.m32 - m.m31 * m.m22 ) * m.m13;
}

void matrix3D_invert( Matrix3D * m )
{
	Number d = matrix3D_determinant( * m );

	Number m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34;

	if( d != 0 )
	{			
		m11 = ( * m ).m11; m21 = ( * m ).m21; m31 = ( * m ).m31;
		m12 = ( * m ).m12; m22 = ( * m ).m22; m32 = ( * m ).m32;
		m13 = ( * m ).m13; m23 = ( * m ).m23; m33 = ( * m ).m33;
		m14 = ( * m ).m14; m24 = ( * m ).m24; m34 = ( * m ).m34;
	
		d = 1.0 /d;

		( * m ).m11 =	 d * ( m22 * m33 - m32 * m23 );
		( * m ).m12 =	-d * ( m12 * m33 - m32 * m13 );
		( * m ).m13 =	 d * ( m12 * m23 - m22 * m13 );
		( * m ).m14 =	-d * ( m12 * ( m23 * m34 - m33 * m24 ) - m22 * ( m13 * m34 - m33 * m14 ) + m32 * ( m13 * m24 - m23 * m14 ) );
	
		( * m ).m21 =	-d * ( m21 * m33 - m31 * m23 );
		( * m ).m22 =	 d * ( m11 * m33 - m31 * m13 );
		( * m ).m23 =	-d * ( m11 * m23 - m21 * m13 );
		( * m ).m24 =	 d * ( m11 * ( m23 * m34 - m33 * m24 ) - m21 * ( m13 * m34 - m33 * m14 ) + m31 * ( m13 * m24 - m23 * m14 ) );
	
		( * m ).m31 =	 d * ( m21 * m32 - m31 * m22 );
		( * m ).m32 =	-d * ( m11 * m32 - m31 * m12 );
		( * m ).m33 =	 d * ( m11 * m22 - m21 * m12 );
		( * m ).m34 =	-d * ( m11 * ( m22 * m34 - m32 * m24 ) - m21 * ( m12 * m34 - m32 * m14 ) + m31 * ( m12 * m24 - m22 * m14 ) );
	}
}

Matrix3D rotationXMatrix3D( Number angle )
{
	Matrix3D m;

	Number c = cos( angle );
	Number s = sin( angle );

	m.m11 = 1; m.m12 = 0; m.m13 =    0; m.m14 = 0;
	m.m21 = 0; m.m22 = c; m.m23 =  - s; m.m24 = 0;
	m.m31 = 0; m.m32 = s; m.m33 =    c; m.m34 = 0;
	m.m41 = 0; m.m42 = 0; m.m43 =    0; m.m44 = 1;

	return m;
}

Matrix3D rotationYMatrix3D( Number angle )
{
	Matrix3D m;

	Number c = cos( angle );
	Number s = sin( angle );

	m.m11 = c; m.m12 = 0; m.m13 =  - s; m.m14 = 0;
	m.m21 = 0; m.m22 = 1; m.m23 =    0; m.m24 = 0;
	m.m31 = s; m.m32 = 0; m.m33 =    c; m.m34 = 0;
	m.m41 = 0; m.m42 = 0; m.m43 =    0; m.m44 = 1;

	return m;
}

Matrix3D rotationZMatrix3D( Number angle )
{
	Matrix3D m;

	Number c = cos( angle );
	Number s = sin( angle );

	m.m11 = c; m.m12 = - s; m.m13 = 0; m.m14 = 0;
	m.m21 = s; m.m22 =   c; m.m23 = 0; m.m24 = 0;
	m.m31 = 0; m.m32 =   0; m.m33 = 1; m.m34 = 0;
	m.m41 = 0; m.m42 =   0; m.m43 = 0; m.m44 = 1;

	return m;
}

void matrix3D_decompose( Matrix3D m, Vector3D * position, Vector3D * scale, Vector3D * rotation )
{
	Matrix3D t;

	Vector3D i = newVector3D( m.m11, m.m21, m.m31, 1 );
	Vector3D j = newVector3D( m.m12, m.m22, m.m32, 1 );
	Vector3D k = newVector3D( m.m13, m.m23, m.m33, 1 );

	if( position )
	{
		( * position ).x = m.m14;
		( * position ).y = m.m24;
		( * position ).z = m.m34;
	}

	if( scale && rotation )
	{
		( * scale ).x = vector3D_normalize( & i );
		( * scale ).y = vector3D_normalize( & j );
		( * scale ).z = vector3D_normalize( & k );
	}
	else if( scale )
	{
		( * scale ).x = vector3D_length( i );
		( * scale ).y = vector3D_length( j );
		( * scale ).z = vector3D_length( k );

		return;
	}
	else if( rotation )
	{
		vector3D_normalize( & i );
		vector3D_normalize( & j );
		vector3D_normalize( & k );
	}
	else
	{
		return;
	}

	t.m11 = i.x; t.m12 = j.x; t.m13 = k.x; t.m14 = 0;
	t.m21 = i.y; t.m22 = j.y; t.m23 = k.y; t.m24 = 0;
	t.m31 = i.z; t.m32 = j.z; t.m33 = k.z; t.m34 = 0;
	t.m41 =   0; t.m42 =   0; t.m43 =   0; t.m44 = 1;

	( * rotation ).x = atan2( t.m23, t.m33 );

	matrix3D_prepend( & m, rotationXMatrix3D( - ( * rotation ).x ) );

	( * rotation ).y = atan2( - m.m31, sqrt( m.m11 * m.m11 + m.m21 * m.m21) );
	( * rotation ).z = atan2( - m.m12, m.m11 );

	if( ( * rotation ).x == PI )
	{
		if( ( * rotation ).y > 0 )
		{
			( * rotation ).y -= PI;
		}
		else
		{
			( * rotation ).y += PI;
		}

		( * rotation ).x = 0;
		( * rotation ).z += PI;
	}

	( * rotation ).x *= TODEGREES;
	( * rotation ).y *= TODEGREES;
	( * rotation ).z *= TODEGREES;
}

void matrix3D_recompose( Matrix3D * m, Vector3D position, Vector3D scale, Vector3D rotation )
{
	matrix3D_copy( m, translationMatrix3D( position.x, position.y, position.z ) );

	matrix3D_apprend( m, scaleMatrix3D( scale.x,scale.y, scale.z ) );

	matrix3D_apprend( m, rotationXMatrix3D( rotation.x ) );

	matrix3D_apprend( m, rotationYMatrix3D( rotation.y ) );

	matrix3D_apprend( m, rotationZMatrix3D( rotation.z ) );
}

void matrix3D_pointAt( Matrix3D * m, Vector3D pos, Vector3D at, Vector3D up )
{
	Vector3D xAxis, yAxis, zAxis, scale;

	vector3D_normalize( & at );

	if( vector3D_length( at ) )
	{
		xAxis = vector3D_crossProduct( at, up );
		vector3D_normalize( & xAxis );

		yAxis = vector3D_crossProduct( at, xAxis );
		vector3D_normalize( & yAxis );

		zAxis = at;

		matrix3D_decompose( * m, NULL, & scale, NULL );

		( * m ).m11 =   xAxis.x * scale.x;
		( * m ).m21 =   xAxis.y * scale.x;
		( * m ).m31 =   xAxis.z * scale.x;
				
		( * m ).m12 = - yAxis.x * scale.y;
		( * m ).m22 = - yAxis.y * scale.y;
		( * m ).m32 = - yAxis.z * scale.y;
				
		( * m ).m13 =   zAxis.x * scale.z;
		( * m ).m23 =   zAxis.y * scale.z;
		( * m ).m33 =   zAxis.z * scale.z;

		( * m ).m14 =   pos.x;
		( * m ).m24 =   pos.y;
		( * m ).m34 =   pos.z;
	}
}

void matrix3D_projectVector( Matrix3D m, Vector3D * v )
{
	Number c = 1.0 / ( ( * v ).x * m.m41 + ( * v ).y * m.m42 + ( * v ).z * m.m43 + 1 );

	matrix3D_transformVector( m, v );

	( * v ).x = ( * v ).x * c;
	( * v ).y = ( * v ).y * c;
	( * v ).z = 0;
}


/**************************************************************************************
 **----------------------------------------------------------------------------------**
 **                                    |quaternion|                                  **
 **__________________________________________________________________________________**
 **************************************************************************************/

Vector3D matrix3D_toQuaternion( Matrix3D m )
{
	Vector3D q;

	Number s, tr = m.m11 + m.m22 + m.m33, c[4];

	int i, j, k;

	if (tr > 0.0) 
	{
		s   = sqrt( tr + 1.0 );
		q.w = s / 2.0;
		s   = 0.5 / s;
				
		q.x = ( m.m32 -  m.m23) * s;
		q.y = ( m.m13 -  m.m31) * s;
		q.z = ( m.m21 -  m.m12) * s;
	} 
	else 
	{
		int nxt[3] = { 1, 2, 0 };

		Number a[3][4] = {
			{ m.m11, m.m12, m.m13, m.m14 },
			{ m.m21, m.m22, m.m23, m.m24 },
			{ m.m31, m.m32, m.m33, m.m34 }
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

		q.x = c[0];
		q.y = c[1];
		q.z = c[2];
		q.w = c[3];
	}

	return q;
}

Vector3D quaternion_multiply( Vector3D q1, Vector3D q2 )
{
	Vector3D q;
	
	q.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
    q.y = q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x;
    q.z = q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w;
    q.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;

	return q;
}

Vector3D rotationQuaternion( Number degrees, Vector3D axis )
{
	Vector3D q;

	Number angle = degrees * TORADIANS / 2, s = sin( angle );

	q.x = axis.x * s;
	q.y = axis.y * s;
	q.z = axis.z * s;
	q.w = cos( angle );

	vector3D_normalize( & q );

	return q;
}

void quaternion_slerp( Vector3D from, Vector3D to, Vector3D * res, Number t )
{
	Vector3D tol = newVector3D( from.x, from.y, from.z, from.w );
	Number cosom  = vector3D_dotProduct( from, to ), omega, sinom, scale0, scale1;

	if (cosom < 0.0)
	{
		cosom = - cosom;
		vector3D_negate( & tol );
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

	( * res ).x = scale0 * from.x + scale1 * tol.x;
	( * res ).y = scale0 * from.y + scale1 * tol.y;
	( * res ).z = scale0 * from.z + scale1 * tol.z;
	( * res ).w = scale0 * from.w + scale1 * tol.w;
}

void quaternion_toMatrix3D( Vector3D q, Matrix3D * m )
{
	Number xx = q.x * q.x;
	Number xy = q.x * q.y;
	Number xz = q.x * q.z;
	Number xw = q.x * q.w;

	Number yy = q.y * q.y;
	Number yz = q.y * q.z;
	Number yw = q.y * q.w;

	Number zz = q.z * q.z;
	Number zw = q.z * q.w;
		
	( * m ).m11 = 1 - 2 * ( yy + zz );
	( * m ).m12 =     2 * ( xy - zw );
	( * m ).m13 =     2 * ( xz + yw );

	( * m ).m21 =     2 * ( xy + zw );
	( * m ).m22 = 1 - 2 * ( xx + zz );
	( * m ).m23 =     2 * ( yz - xw );

	( * m ).m31 =     2 * ( xz - yw );
	( * m ).m32 =     2 * ( yz + xw );
	( * m ).m33 = 1 - 2 * ( xx + yy );
}

/**************************************************************************************
 **----------------------------------------------------------------------------------**
 **                                 |interpolation|                                  **
 **__________________________________________________________________________________**
 **************************************************************************************/

void matrix3D_interpolate( Matrix3D thisMat, Matrix3D toMat, Number percent, Matrix3D * target )
{
	Vector3D res;

	quaternion_slerp( matrix3D_toQuaternion( thisMat ), matrix3D_toQuaternion( toMat ), &res, percent );

	quaternion_toMatrix3D( res, target );

	quaternion_slerp( matrix3D_getPosition( thisMat ), matrix3D_getPosition( toMat ), & res, percent );

	matrix3D_setPosition( target, res );
}

void matrix3D_pointTowards( Number percent, Matrix3D mat, Vector3D pos, Vector3D at, Vector3D up,Matrix3D * target )
{
	Matrix3D m;

	Vector3D res;

	matrix3D_pointAt( & m, pos, at, up );

	quaternion_slerp( matrix3D_toQuaternion( mat ), matrix3D_toQuaternion( m ), &res, percent );

	quaternion_toMatrix3D( res, target );
}
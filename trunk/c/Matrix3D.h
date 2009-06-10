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

void identity( Matrix3D * m )
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

void copy( Matrix3D * m, Matrix3D c )
{
	( * m ).m11 = c.m11; ( * m ).m12 = c.m12; ( * m ).m13 = c.m13; ( * m ).m14 = c.m14;

	( * m ).m21 = c.m21; ( * m ).m22 = c.m22; ( * m ).m23 = c.m23; ( * m ).m24 = c.m24;

	( * m ).m31 = c.m31; ( * m ).m32 = c.m32; ( * m ).m33 = c.m33; ( * m ).m34 = c.m34;

	( * m ).m41 = c.m41; ( * m ).m42 = c.m42; ( * m ).m43 = c.m43; ( * m ).m44 = c.m44;
}

Number ( * getRawData( Matrix3D m ) )[16]
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

void setRawData( Matrix3D * m, Number rawData[16] )
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

	setRawData( & m, rawData );

	return m;
}

Vector3D getPosition( Matrix3D m )
{
	Vector3D v;

	v.x = m.m14;
	v.y = m.m24;
	v.z = m.m34;
	v.w = m.m44;

	return v;
}

void setPosition( Matrix3D * m, Vector3D v )
{
	( * m ).m14 = v.x;
	( * m ).m24 = v.y;
	( * m ).m34 = v.z;
	( * m ).m44 = v.w;
}

Matrix3D multiply( Matrix3D m1, Matrix3D m2 )
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

void apprend( Matrix3D * m, Matrix3D a )
{
	copy( m, multiply( * m, a ) );
}

void prepend( Matrix3D * m, Matrix3D a )
{
	copy( m, multiply( a, * m ) );
}

Matrix3D translationMatrix( x, y, z )
{
	Matrix3D tran;

	tran.m11 = 1; tran.m12 = 0; tran.m13 = 0; tran.m14 = x;

	tran.m21 = 0; tran.m22 = 1; tran.m23 = 0; tran.m24 = y;

	tran.m31 = 0; tran.m32 = 0; tran.m33 = 1; tran.m34 = z;

	tran.m41 = 0; tran.m42 = 0; tran.m43 = 0; tran.m44 = 1;

	return tran;
}

void apprendTranslation( Matrix3D * m, Number x, Number y, Number z )
{
	apprend( m, translationMatrix( x, y, z ) );
}

void prependTranslation( Matrix3D * m, Number x, Number y, Number z )
{
	prepend( m, translationMatrix( x, y, z ) );
}

Matrix3D rotationMatrix(  Number degrees, Vector3D axis )
{
	Number angle = degrees * TOANGLE;
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

void apprendRotation( Matrix3D * m, Number degrees,Vector3D axis )
{
	apprend( m, rotationMatrix(  degrees, axis ) );
}

void prependRotation( Matrix3D * m, Number degrees,Vector3D axis )
{
	prepend( m, rotationMatrix(  degrees, axis ) );
}

Matrix3D scaleMatrix( Number xScale, Number yScale, Number zScale )
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

void apprendScale( Matrix3D * m, Number xScale, Number yScale, Number zScale )
{
	apprend( m, scaleMatrix( xScale, yScale, zScale ) );
}

void prependScale( Matrix3D * m, Number xScale, Number yScale, Number zScale )
{
	prepend( m, scaleMatrix( xScale, yScale, zScale ) );
}

void transformVector( Matrix3D m, Vector3D * v )
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

void transpose( Matrix3D * m )
{
	( * m ).m12 = ( * m ).m21 + ( ( * m ).m21 = ( * m ).m12 ) * 0;
	( * m ).m13 = ( * m ).m31 + ( ( * m ).m31 = ( * m ).m13 ) * 0;
	( * m ).m14 = ( * m ).m41 + ( ( * m ).m41 = ( * m ).m14 ) * 0;

	( * m ).m23 = ( * m ).m32 + ( ( * m ).m32 = ( * m ).m23 ) * 0;
	( * m ).m24 = ( * m ).m42 + ( ( * m ).m42 = ( * m ).m24 ) * 0;

	( * m ).m34 = ( * m ).m43 + ( ( * m ).m43 = ( * m ).m34 ) * 0;
}

Number determinant( Matrix3D m )
{
	return ( m.m11 * m.m22 - m.m21 * m.m12 ) * m.m33 - ( m.m11 * m.m32 - m.m31 * m.m12 ) * m.m23 + ( m.m21 * m.m32 - m.m31 * m.m22 ) * m.m13;
}

void invert( Matrix3D * m )
{
	Number d = determinant( * m );

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

Matrix3D rotationX( Number angle )
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

Matrix3D rotationY( Number angle )
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

Matrix3D rotationZ( Number angle )
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

void decompose( Matrix3D m, Vector3D * position, Vector3D * scale, Vector3D * rotation )
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
		( * scale ).x = normalize( & i );
		( * scale ).y = normalize( & j );
		( * scale ).z = normalize( & k );
	}
	else if( scale )
	{
		( * scale ).x = length( i );
		( * scale ).y = length( j );
		( * scale ).z = length( k );

		return;
	}
	else if( rotation )
	{
		normalize( & i );
		normalize( & j );
		normalize( & k );
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

	prepend( & m, rotationX( - ( * rotation ).x ) );

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

void recompose( Matrix3D * m, Vector3D position, Vector3D scale, Vector3D rotation )
{
	copy( m, translationMatrix( position.x, position.y, position.z ) );

	apprend( m, scaleMatrix( scale.x,scale.y, scale.z ) );

	apprend( m, rotationX( rotation.x ) );

	apprend( m, rotationY( rotation.y ) );

	apprend( m, rotationZ( rotation.z ) );
}

void pointAt( Matrix3D * m, Vector3D pos, Vector3D at, Vector3D up )
{
	Vector3D xAxis, yAxis, zAxis, scale;

	normalize( & at );

	if( length( at ) )
	{
		xAxis = crossProduct( at, up );
		normalize( & xAxis );

		yAxis = crossProduct( at, xAxis );
		normalize( & yAxis );

		zAxis = at;

		decompose( * m, NULL, & scale, NULL );

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

void projectVector( Matrix3D m, Vector3D * v )
{
	Number c = 1.0 / ( ( * v ).x * m.m41 + ( * v ).y * m.m42 + ( * v ).z * m.m43 + 1 );

	transformVector( m, v );

	( * v ).x = ( * v ).x * c;
	( * v ).y = ( * v ).y * c;
	( * v ).z = 0;
}


# include<math.h>
# include<malloc.h>
//# include<string.h>

# include"Base.h"

typedef struct
{
	Number x;
	Number y;
	Number z;
	Number w;
}Vector3D;

Number lengthSquared( Vector3D v )
{
	return v.x * 2 + v.y * 2 + v.z * 2;
}

Number length( Vector3D v )
{
	return sqrt( lengthSquared( v ) );
}

Vector3D newVector3D( Number x, Number y, Number z, Number w )
{
	Vector3D v;

	v.x = x;
	v.y = y;
	v.z = z;
	v.w = w;

	return v;
}


Vector3D defaultVector3D( void )
{
	return newVector3D( 0,0,0,1 );
}

void add( Vector3D * v1, Vector3D v2 )
{
	( * v1 ).x = ( * v1 ).x + v2.x;
	( * v1 ).y = ( * v1 ).y + v2.y;
	( * v1 ).z = ( * v1 ).z + v2.z;
}

void subtract( Vector3D * v1, Vector3D v2 )
{
	( * v1 ).x = ( * v1 ).x - v2.x;
	( * v1 ).y = ( * v1 ).y - v2.y;
	( * v1 ).z = ( * v1 ).z - v2.z;
}

/*Vector3D clone( Vector3D v )
{
	return newVector3D( v.x, v.y, v.z, v.w );
}*/

Vector3D crossProduct( Vector3D v1, Vector3D v2 )
{
	return newVector3D( v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x, 1 );
}

Number distance( Vector3D v1, Vector3D v2 )
{
	subtract( & v2, v1 );

	return length( v2 );
}

Number dotMetrix( Vector3D v1, Vector3D v2 )
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Number dotProduct( Vector3D v1, Vector3D v2 )
{
	return dotMetrix( v1, v2 ) / ( length( v1 ) * length( v2 ) );
}

Number angleBetween( Vector3D v1, Vector3D v2 )
{
	return acos( dotProduct( v1, v2 ) );
}

int equals( Vector3D v1, Vector3D v2, int allFour )
{
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && ( !allFour || v1.w == v2.w );
}


int nearEquals( Vector3D v1, Vector3D v2, Number lerance, int allFour )
{
	return ( fabs( v1.x - v2.x ) < lerance ) && ( fabs( v1.y - v2.y ) < lerance ) && ( fabs( v1.z - v2.z ) < lerance ) && ( !allFour || ( fabs( v1.w - v2.w ) < lerance ) );
}

void negate( Vector3D * v )
{
	( * v ).x = - ( * v ).x;
	( * v ).y = - ( * v ).y;
	( * v ).z = - ( * v ).z;
}

Number normalize( Vector3D * v )
{
	Number len = length( * v );

	( * v ).x /= len;
	( * v ).y /= len;
	( * v ).z /= len;

	return len;
}

void project( Vector3D * v )
{
	( * v ).x /= ( * v ).w;
	( * v ).y /= ( * v ).w;
	( * v ).z /= ( * v ).w;
}

void scaleBy( Vector3D * v, Number value )
{
	( * v ).x *= value;
	( * v ).y *= value;
	( * v ).z *= value;
}

/*char *toString( Vector3D v )
{
	return strcat( "Vector3D( ", strcat( toChar( v.x ), strcat( toChar( v.y ), toChar( v.z ) ) ) );
}*/

Vector3D X_AXIS()
{
	return newVector3D( 1, 0, 0, 1 );
}

Vector3D Y_AXIS()
{
	return newVector3D( 0, 1, 0, 1 );
}

Vector3D Z_AXIS()
{
	return newVector3D( 0, 0, 1, 1 );
}
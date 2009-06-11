# include<math.h>
# include<malloc.h>
//# include<string.h>

# include "Base.h"

typedef struct
{
	Number x;
	Number y;
	Number z;
	Number w;
}Vector3D;

/**************************************************************************************
 **----------------------------------------------------------------------------------**
 **                                    |vector3D|                                    **
 **__________________________________________________________________________________**
 **************************************************************************************/

Number vector3D_lengthSquared( Vector3D v )
{
	return v.x * 2 + v.y * 2 + v.z * 2;
}

Number vector3D_length( Vector3D v )
{
	return sqrt( vector3D_lengthSquared( v ) );
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



void vector3D_add( Vector3D * v1, Vector3D v2 )
{
	( * v1 ).x = ( * v1 ).x + v2.x;
	( * v1 ).y = ( * v1 ).y + v2.y;
	( * v1 ).z = ( * v1 ).z + v2.z;
}

void vector3D_subtract( Vector3D * v1, Vector3D v2 )
{
	( * v1 ).x = ( * v1 ).x - v2.x;
	( * v1 ).y = ( * v1 ).y - v2.y;
	( * v1 ).z = ( * v1 ).z - v2.z;
}

Vector3D vector3D_clone( Vector3D v )
{
	return newVector3D( v.x, v.y, v.z, v.w );
}

void vector3D_copy( Vector3D * v, Vector3D c )
{
	( * v ).x = c.x;
	( * v ).y = c.y;
	( * v ).z = c.z;
	( * v ).w = c.w;
}

Vector3D vector3D_crossProduct( Vector3D v1, Vector3D v2 )
{
	return newVector3D( v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x, 1 );
}

Number vector3D_distance( Vector3D v1, Vector3D v2 )
{
	vector3D_subtract( & v2, v1 );

	return vector3D_length( v2 );
}

Number vector3D_dotMetrix( Vector3D v1, Vector3D v2 )
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Number vector3D_dotProduct( Vector3D v1, Vector3D v2 )
{
	return vector3D_dotMetrix( v1, v2 ) / ( vector3D_length( v1 ) * vector3D_length( v2 ) );
}

Number vector3D_angleBetween( Vector3D v1, Vector3D v2 )
{
	return acos( vector3D_dotProduct( v1, v2 ) );
}

int vector3D_equals( Vector3D v1, Vector3D v2, int allFour )
{
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && ( !allFour || v1.w == v2.w );
}


int vector3D_nearEquals( Vector3D v1, Vector3D v2, Number lerance, int allFour )
{
	return ( fabs( v1.x - v2.x ) < lerance ) && ( fabs( v1.y - v2.y ) < lerance ) && ( fabs( v1.z - v2.z ) < lerance ) && ( !allFour || ( fabs( v1.w - v2.w ) < lerance ) );
}

void vector3D_negate( Vector3D * v )
{
	( * v ).x = - ( * v ).x;
	( * v ).y = - ( * v ).y;
	( * v ).z = - ( * v ).z;
}

Number vector3D_normalize( Vector3D * v )
{
	Number len = vector3D_length( * v );

	if( len )
	{
		( * v ).x /= len;
		( * v ).y /= len;
		( * v ).z /= len;
	}

	return len;
}

void vector3D_project( Vector3D * v )
{
	( * v ).x /= ( * v ).w;
	( * v ).y /= ( * v ).w;
	( * v ).z /= ( * v ).w;
}

void vector3D_scaleBy( Vector3D * v, Number value )
{
	( * v ).x *= value;
	( * v ).y *= value;
	( * v ).z *= value;
}

/*char *toString( Vector3D v )
{
	return strcat( "Vector3D( ", strcat( toChar( v.x ), strcat( toChar( v.y ), toChar( v.z ) ) ) );
}*/

/**************************************************************************************
 **----------------------------------------------------------------------------------**
 **                                    |axisAngle|                                   **
 **__________________________________________________________________________________**
 **************************************************************************************/

Vector3D vector3D_X_AXIS()
{
	return newVector3D( 1, 0, 0, 1 );
}

Vector3D vector3D_Y_AXIS()
{
	return newVector3D( 0, 1, 0, 1 );
}

Vector3D vector3D_Z_AXIS()
{
	return newVector3D( 0, 0, 1, 1 );
}
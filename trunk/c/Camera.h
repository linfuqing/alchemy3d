# ifndef CAMERA_H
# define CAMERA_H

# include "Matrix.h"

# define MOVE_TYPE_TRANSFORM   1
# define MOVE_TYPE_TRANSLATION 2
# define MOVE_TYPE_ADDED_SCENE 3

//N
typedef struct
{
	Vector3D * quaternion;

	int        move;
}Rotation;

//N
typedef struct
{
	Vector3D * position;
	Vector3D * direction;
	Vector3D * scale;

	Matrix3D * world;
	Matrix3D * transform;
	Rotation * rotation;

	int      move;
}Camera;

Rotation * newRotation( Vector3D * q )
{
	Rotation * r;
	if( ( r = ( Rotation * )malloc( sizeof( Rotation ) ) ) == NULL )
	{
		exit( TRUE );
	}

	r -> quaternion = q;
	r -> move       = FALSE;

	return r;
}
	
Camera * newCamera( Vector3D * position, Vector3D * direction, Vector3D * scale )
{
	Camera * c;

	if( ( c = ( Camera * )malloc( sizeof( Camera ) ) ) == NULL || ( c -> transform = ( Matrix3D * )malloc( sizeof( Matrix3D ) ) ) == NULL )
	{
		exit( TRUE );
	}

	if( position == NULL )
	{
		position = newVector3D( 0, 0, 0, 1 );
	}

	if( direction == NULL )
	{
		direction = newVector3D( 0, 0, 0, 1 );
	}

	if( scale == NULL )
	{
		scale = newVector3D( 0, 0, 0, 1 );
	}

	c -> position  = position;
	c -> direction = direction;
	c -> scale     = scale;

	c -> world     = newMatrix3D( NULL );
	c -> transform = newMatrix3D( NULL );

	matrix3D_recompose( c -> transform, position, scale, direction );

	c -> rotation = newRotation( NULL );

	c -> move     = FALSE;

	return c;
}

Matrix3D * camera_getTransform( Camera * c )
{
	Vector3D * v;
	Rotation * rotation = c -> rotation;

	if( rotation -> quaternion != NULL )
	{
		if( rotation -> move )
		{
			//do something

			rotation -> move = FALSE;
		}

		matrix3D_apprend( c -> transform, vector3D_toMatrix3D( rotation -> quaternion ) );

		v = rotation -> quaternion;

		rotation -> quaternion = NULL;
		
		free( v );
	}

	if( c -> move == MOVE_TYPE_TRANSLATION )
	{
		matrix3D_setPosition( c -> transform, c -> position );

		c -> move = FALSE;
	}

	return c -> transform;
}

void camera_setTransform( Camera * c, Matrix3D transform )
{
	* ( c -> transform ) = transform;

	c -> move = MOVE_TYPE_TRANSFORM;
}

/**
更新相机变换,在使用set transfrom之后读取变换数值会不准确,这时候可以使用这个方法.
**/
void camera_updateTransform( Camera * c )
{
	Vector3D * rotation = c -> rotation -> quaternion;

	if( c -> move == MOVE_TYPE_TRANSFORM )
	{
		matrix3D_decompose( c -> transform, c -> position, c -> scale, c -> direction );

		if( rotation != NULL )
		{
			c -> rotation -> quaternion = NULL;

			free( rotation );
		}

		c -> move = FALSE;
	}
}

void camera_setPosition( Camera * c, Number x, Number y, Number z )
{
	Vector3D * position = c -> position;

	camera_updateTransform( c );

	position -> x = x;
	position -> y = y;
	position -> z = z;

	c -> move = MOVE_TYPE_TRANSLATION;
}

void camera_setScale( Camera * c, Number xScale, Number yScale, Number zScale )
{
	Vector3D * scale = c -> scale;

	camera_updateTransform( c );

	matrix3D_apprendScale( c -> transform, xScale - ( scale -> x ), yScale - ( scale -> y ), zScale - ( scale -> z ) );

	scale -> x = xScale;
	scale -> y = yScale;
	scale -> z = zScale;

	c -> move = MOVE_TYPE_TRANSLATION;
}

void camera_setRotation( Camera * c, Number degrees, Vector3D * axis )
{
	if( c -> rotation -> quaternion == NULL )
	{
		if( ( c -> rotation -> quaternion = ( Vector3D * )malloc( sizeof( Vector3D ) ) ) == NULL )
		{
			exit( TRUE );
		}

		c -> rotation -> quaternion = newVector3D( 0, 0, 0, 1 );
	}

	vector3D_apprendRotation( c -> rotation -> quaternion, degrees, axis );

	c -> rotation -> move = TRUE;
}

/**
效率与单个rotationX,rotationY,rotationZ无差别.
**/
void camera_setDirection( Camera * c, Number rotationX, Number rotationY, Number rotationZ )
{
	Vector3D * direction = c -> direction;

	camera_updateTransform( c );

	if( direction -> x != rotationX )
	{
		camera_setRotation( c, rotationX - ( direction -> x ), X_AXIS() );

		direction -> x = rotationX;
	}

	if( direction -> y != rotationY )
	{
		camera_setRotation( c, rotationY - ( direction -> y ), Y_AXIS() );

		direction -> y = rotationY;
	}

	if( direction -> x != rotationZ )
	{
		camera_setRotation( c, rotationZ - ( direction -> z ), Z_AXIS() );

		direction -> z = rotationZ;
	}

	c -> rotation -> move = FALSE;
}

# endif
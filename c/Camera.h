# ifndef CAMERA_H
# define CAMERA_H

// verson 1.4

# include "Matrix3D.h"

# define MOVE_TYPE_TRANSFORM   1
# define MOVE_TYPE_ROTATION    2
# define MOVE_TYPE_TRANSLATION 3
# define MOVE_TYPE_ADDED_SCENE 4

//N
typedef struct
{
	Quaternion * quaternion;

	int          move;
}Rotation;

typedef struct
{
	//RW
	Vector3D * target;
	Number     interpolation;

	//N
	Vector3D * position;
	Vector3D * direction;
	Vector3D * scale;

	Matrix3D * world;
	Matrix3D * transform;
	Rotation * rotation;

	int      move;
}Camera;
	
Camera * newCamera( Vector3D * target )
{
	Camera * c;

	if( ( c = ( Camera * )malloc( sizeof( Camera ) ) ) == NULL || ( c -> rotation = ( Rotation * )malloc( sizeof( Rotation ) ) ) == NULL )
	{
		exit( TRUE );
	}

	c -> target                 = target;
	c -> position               = newVector3D( 0, 0, 0, 1 );
	c -> direction              = newVector3D( 0, 0, 0, 1 );
	c -> scale                  = newVector3D( 0, 0, 0, 1 );

	c -> world                  = newMatrix3D( NULL );
	c -> transform              = newMatrix3D( NULL );

	c -> rotation -> quaternion = newQuaternion( 0, 0, 0, 1 );

	c -> rotation -> move       = FALSE;

	c -> move                   = FALSE;

	return c;
}

void lookAt( Camera * c, Vector3D * target, Number interpolation, Vector3D * upAxis )
{
	Matrix3D world;
	Vector3D pos;

	quaternion_toMatrix3D( c -> rotation -> quaternion, c -> transform );

	matrix3D_prependScale( c -> transform, c -> scale -> x, c -> scale -> y, c -> scale -> z );

	matrix3D_setPosition( c -> transform, c -> position );

	matrix3D_apprend( c -> transform, c -> world );

	matrix3D_getPosition( c -> transform, & pos );

	if( interpolation && interpolation != 1 )
	{
		matrix3D_pointTowards( c -> transform, interpolation, & pos, c -> target, upAxis );
	}
	else
	{
		matrix3D_pointAt( c -> transform, & pos, c -> target, upAxis );
	}

	world = * ( c -> world );

	matrix3D_invert( & world );

	matrix3D_apprend( c -> transform, & world );

	c -> move = MOVE_TYPE_TRANSFORM;
}

void camera_updateRotation( Camera * c )
{
	if( c -> rotation -> move )
	{
		quaternion_decompose( c -> rotation -> quaternion, c -> direction );

		c -> rotation -> move = FALSE;
	}
}

Matrix3D * camera_getTransform( Camera * c )
{
	if( c -> target != NULL )
	{
		lookAt( c, c -> target, c -> interpolation, & Y_AXIS );
	}
	else if( c -> move == MOVE_TYPE_ROTATION )
	{
		camera_updateRotation( c );

		quaternion_toMatrix3D( c -> rotation -> quaternion, c -> transform );

		matrix3D_prependScale( c -> transform, c -> scale -> x, c -> scale -> y, c -> scale -> z );

		matrix3D_setPosition( c -> transform, c -> position );

		c -> move             = FALSE;
	}
	else if( c -> move == MOVE_TYPE_TRANSLATION )
	{
		matrix3D_setPosition( c -> transform, c -> position );

		c -> move = FALSE;
	}

	return c -> transform;
}

void camera_setTransform( Camera * c, Matrix3D * transform )
{
	free( c -> transform );

	c -> transform = transform;

	c -> move = MOVE_TYPE_TRANSFORM;
}

/**
更新相机变换,在使用set transfrom之后读取变换数值会不准确,这时候可以使用这个方法.
**/
void camera_updateTransform( Camera * c )
{
	if( c -> move == MOVE_TYPE_TRANSFORM )
	{
		matrix3D_decompose( c -> transform, c -> position, c -> scale, c -> direction );

		matrix3D_toQuaternion( c -> transform, c -> rotation -> quaternion );

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

	c -> move = c -> move == MOVE_TYPE_ROTATION ? MOVE_TYPE_ROTATION : MOVE_TYPE_TRANSLATION;
}

void camera_setScale( Camera * c, Number xScale, Number yScale, Number zScale )
{
	camera_updateTransform( c );

	matrix3D_apprendScale( c -> transform, xScale - c -> scale -> x, yScale - c -> scale -> y, zScale - c -> scale -> z );

	c -> scale -> x = xScale;
	c -> scale -> y = yScale;
	c -> scale -> z = zScale;

	c -> move = c -> move == MOVE_TYPE_ROTATION ? MOVE_TYPE_ROTATION : MOVE_TYPE_TRANSLATION;
}

void camera_setRotation( Camera * c, Number degrees, Vector3D * axis )
{
	camera_updateTransform( c );

	quaternion_apprendRotation( c -> rotation -> quaternion, degrees, axis );

	c -> rotation -> move = TRUE;

	c             -> move = MOVE_TYPE_ROTATION;
}

void camera_setDirection( Camera * c, Vector3D * direction )
{
	camera_updateTransform( c );

	quaternion_recompose( c -> rotation -> quaternion, direction );

	* ( c -> direction )   = * direction;

	c -> rotation -> move = FALSE;

	c -> move             = MOVE_TYPE_ROTATION;
}

void camera_rotationX( Camera * c, Number degrees )
{
	camera_updateTransform( c );

	camera_updateRotation( c );

	quaternion_apprendRotation( c -> rotation -> quaternion, degrees - c -> direction -> x, & X_AXIS );

	c -> direction -> x = degrees;

	c -> move           = MOVE_TYPE_ROTATION;
}

void camera_rotationY( Camera * c, Number degrees )
{
	camera_updateTransform( c );

	camera_updateRotation( c );

	quaternion_apprendRotation( c -> rotation -> quaternion, degrees - c -> direction -> y, & Y_AXIS );

	c -> direction -> y = degrees;

	c -> move           = MOVE_TYPE_ROTATION;
}

void camera_rotationZ( Camera * c, Number degrees )
{
	camera_updateTransform( c );

	camera_updateRotation( c );

	quaternion_apprendRotation( c -> rotation -> quaternion, degrees - c -> direction -> z, & Z_AXIS );

	c -> direction -> z = degrees;

	c -> move           = MOVE_TYPE_ROTATION;
}

void camera_destroy( Camera * * c )
{
	free( ( * c ) -> position  );
	free( ( * c ) -> scale     );
	free( ( * c ) -> direction );
	free( ( * c ) -> rotation -> quaternion );
	free( ( * c ) -> rotation  );
	free( ( * c ) -> transform );
	free( ( * c ) -> world     );
	free( ( * c )              );

	* c = NULL;
}

# endif
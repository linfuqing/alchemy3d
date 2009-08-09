# ifndef Object_H
# define Object_H

// verson 1.4

# include "Matrix3D.h"

# define MOVE_TYPE_TRANSFORM   1
//# define MOVE_TYPE_ROTATION    2
# define MOVE_TYPE_TRANSLATION 2
# define MOVE_TYPE_ADDED_SCENE 3

//N
/*typedef struct
{
	Quaternion * quaternion;

	int          move;
}Rotation;*/

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
	//Rotation * rotation;
	Quaternion * rotation;

	//L
	int      move;
}Object;
	
Object * newObject( Vector3D * target )
{
	Object * o;

	if( ( o = ( Object * )malloc( sizeof( Object ) ) ) == NULL )//|| ( o -> rotation = ( Rotation * )malloc( sizeof( Rotation ) ) ) == NULL )
	{
		exit( TRUE );
	}

	o -> target                 = target;
	o -> position               = newVector3D( 0, 0, 0, 1 );
	o -> direction              = newVector3D( 0, 0, 0, 1 );
	o -> scale                  = newVector3D( 0, 0, 0, 1 );

	o -> world                  = newMatrix3D( NULL );
	o -> transform              = newMatrix3D( NULL );

	//o -> rotation -> quaternion = newQuaternion( 0, 0, 0, 1 );

	//o -> rotation -> move       = FALSE;

	o -> rotation               = newQuaternion( 0, 0, 0, 1 );

	o -> move                   = FALSE;

	return o;
}

void object_updateTransform( Object * o )
{
	quaternion_recompose( o -> rotation, o -> direction );
	quaternion_toMatrix3D( o -> rotation, o -> transform );
	
	matrix3D_prependScale( o -> transform, o -> scale -> x, o -> scale -> y, o -> scale -> z );
	matrix3D_setPosition( o -> transform, o -> position );
}

void object_lookAt( Object * o, Vector3D * target, Number interpolation, Vector3D * upAxis )
{
	Matrix3D world;
	Vector3D pos;

	/*quaternion_toMatrix3D( o -> rotation -> quaternion, o -> transform );

	matrix3D_prependScale( o -> transform, o -> scale -> x, o -> scale -> y, o -> scale -> z );

	matrix3D_setPosition( o -> transform, o -> position );*/

	object_updateTransform( o );

	matrix3D_apprend( o -> transform, o -> world );

	matrix3D_getPosition( o -> transform, & pos );

	if( interpolation && interpolation != 1 )
	{
		matrix3D_pointTowards( o -> transform, interpolation, & pos, o -> target, upAxis );
	}
	else
	{
		matrix3D_pointAt( o -> transform, & pos, o -> target, upAxis );
	}

	world = * ( o -> world );

	matrix3D_invert( & world );

	matrix3D_apprend( o -> transform, & world );

	//o -> move = MOVE_TYPE_TRANSFORM;
}

/*void object_updateRotation( Object * o )
{
	if( o -> rotation -> move )
	{
		quaternion_decompose( o -> rotation -> quaternion, o -> direction );

		o -> rotation -> move = FALSE;
	}
}*/

Matrix3D * object_getTransform( Object * o )
{
	if( o -> target != NULL )
	{
		object_lookAt( o, o -> target, o -> interpolation, & Y_AXIS );
	}
	//else if( o -> move == MOVE_TYPE_ROTATION )
	else if( o -> move == MOVE_TYPE_TRANSFORM )
	{
		/*object_updateRotation( o );

		quaternion_toMatrix3D( o -> rotation -> quaternion, o -> transform );

		matrix3D_prependScale( o -> transform, o -> scale -> x, o -> scale -> y, o -> scale -> z );

		matrix3D_setPosition( o -> transform, o -> position );*/

		object_updateTransform( o );
	}
	else if( o -> move == MOVE_TYPE_TRANSLATION )
	{
		matrix3D_setPosition( o -> transform, o -> position );
	}

	o -> move = FALSE;

	return o -> transform;
}

/*void object_setTransform( Object * o, Matrix3D * transform )
{
	free( o -> transform );

	o -> transform = transform;

	o -> move = MOVE_TYPE_TRANSFORM;
}

**
更新变换,在使用set transfrom之后读取变换数值会不准确,这时候可以使用这个方法.
**
void object_updateTransform( Object * o )
{
	if( o -> move == MOVE_TYPE_TRANSFORM )
	{
		matrix3D_decompose( o -> transform, o -> position, o -> scale, o -> direction );

		matrix3D_toQuaternion( o -> transform, o -> rotation -> quaternion );

		o -> move = FALSE;
	}
}

void object_setPosition( Object * o, Number x, Number y, Number z )
{
	Vector3D * position = o -> position;

	object_updateTransform( o );

	position -> x = x;
	position -> y = y;
	position -> z = z;

	o -> move = o -> move == MOVE_TYPE_ROTATION ? MOVE_TYPE_ROTATION : MOVE_TYPE_TRANSLATION;
}

void object_setScale( Object * o, Number xScale, Number yScale, Number zScale )
{
	object_updateTransform( o );

	matrix3D_apprendScale( o -> transform, xScale - o -> scale -> x, yScale - o -> scale -> y, zScale - o -> scale -> z );

	o -> scale -> x = xScale;
	o -> scale -> y = yScale;
	o -> scale -> z = zScale;

	o -> move = o -> move == MOVE_TYPE_ROTATION ? MOVE_TYPE_ROTATION : MOVE_TYPE_TRANSLATION;
}

void object_setRotation( Object * o, Number degrees, Vector3D * axis )
{
	object_updateTransform( o );

	quaternion_apprendRotation( o -> rotation -> quaternion, degrees, axis );

	o -> rotation -> move = TRUE;

	o             -> move = MOVE_TYPE_ROTATION;
}

void object_setDirection( Object * o, Vector3D * direction )
{
	object_updateTransform( o );

	quaternion_recompose( o -> rotation -> quaternion, direction );

	* ( o -> direction )   = * direction;

	o -> rotation -> move = FALSE;

	o -> move             = MOVE_TYPE_ROTATION;
}

void object_rotationX( Object * o, Number degrees )
{
	object_updateTransform( o );

	object_updateRotation( o );

	quaternion_apprendRotation( o -> rotation -> quaternion, degrees - o -> direction -> x, & X_AXIS );

	o -> direction -> x = degrees;

	o -> move           = MOVE_TYPE_ROTATION;
}

void object_rotationY( Object * o, Number degrees )
{
	object_updateTransform( o );

	object_updateRotation( o );

	quaternion_apprendRotation( o -> rotation -> quaternion, degrees - o -> direction -> y, & Y_AXIS );

	o -> direction -> y = degrees;

	o -> move           = MOVE_TYPE_ROTATION;
}

void object_rotationZ( Object * o, Number degrees )
{
	object_updateTransform( o );

	object_updateRotation( o );

	quaternion_apprendRotation( o -> rotation -> quaternion, degrees - o -> direction -> z, & Z_AXIS );

	o -> direction -> z = degrees;

	o -> move           = MOVE_TYPE_ROTATION;
}*/

void object_destroy( Object * * o )
{
	free( ( * o ) -> position  );
	free( ( * o ) -> scale     );
	free( ( * o ) -> direction );
	//free( ( * o ) -> rotation -> quaternion );
	free( ( * o ) -> rotation  );
	free( ( * o ) -> transform );
	free( ( * o ) -> world     );
	free( ( * o )              );

	* o = NULL;
}

# endif
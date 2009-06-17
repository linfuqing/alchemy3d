# ifndef CAMERA_H
# define CAMERA_H

# include"Matrix3D.h"

typedef struct
{
	Vector3D * position;
	Vector3D * direction;
	Vector3D * scale;

	Matrix3D * transform;
	Vector3D * rotation;

	int      move;
}Camera;

Matrix3D camera_getTransfrom( Camera c )
{
	Vector3D * v;

	if( c.rotation != NULL )
	{
		matrix3D_apprend( c.transform, quaternion_toMatrix3D( * c.rotation ) );

		v = c.rotation;

		c.rotation = NULL;

		free( v );
	}

	matrix3D_setPosition( c.transform, * c.position );

	return * c.transform;
}

# endif
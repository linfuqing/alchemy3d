# ifndef Camera_H
# define Camera_H

//RW
typedef struct
{
	Number width;
	Number height;
}Stage;

typedef struct
{
	//RW
	Object   * eye;

	//N
	Matrix3D * projectionMatrix;

	//R
	Number     fieldOfView;

	//R
	Number     nearClip;
	
	//R
	Number     farClip;

	//N
	Number     aspect;

	//L
	int        move;
}Camera;

Camera * newCamera( Number fieldOfView, Number nearClip, Number farClip, Object * eye )
{
	Camera * c;

	if( ( c = ( Camera * )malloc( sizeof( Camera * ) ) ) == NULL )
	{
		exit( TRUE );
	}

	c -> eye              = eye == NULL ? newObject( NULL ) : eye;
	c -> projectionMatrix = projectionMatrix3D( 0, 0, 0, 0, nearClip, farClip );
	c -> fieldOfView      = fieldOfView;
	c -> nearClip         = nearClip;
	c -> farClip          = farClip;
	c -> aspect           = 0;
	c -> move             = FALSE;

	return c;
}

Matrix3D * camera_getProjectMatrix( Camera * c, Stage * s )
{
	Number aspect = s -> width / s -> height;

	if( c -> move || c -> aspect != aspect )
	{
		Number top    =   c -> nearClip * tanf( c-> fieldOfView * 0.5 * TORADIANS );
		Number bottom = - top;
		Number right  =   top * aspect;
		Number left   = - right;

		c -> aspect = aspect;

		matrix3D_projectMatrix( c -> projectionMatrix, top, bottom, left, right, c->nearClip, c->farClip );

		matrix3D_apprendProject( c -> projectionMatrix, c -> eye -> world );

		matrix3D_apprendProject( c -> projectionMatrix, object_getTransform( c -> eye ) );
	}

	return c -> projectionMatrix;
}

# endif

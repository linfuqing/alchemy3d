# ifndef VERTEX_H
# define VERTEX_H


# include <malloc.h>
# include <stdlib.h>

# include "Matrix3D.h"

//RW
typedef struct
{
	Vector3D * position;
	Vector3D * worldPosition;
}Vertex;

Vertex * newVertex( Vector3D * position )
{
	Vertex * v;

	if( ( v = ( Vertex * )malloc( sizeof( Vector3D ) ) ) == NULL )
	{
		exit( TRUE );
	}

	v -> position = position;

	if( ( v -> worldPosition = ( Vector3D * )malloc( sizeof( Vector3D ) ) ) == NULL )
	{
		exit( TRUE );
	}

	* ( v -> worldPosition ) = * ( v -> position );

	return v;
}

int vertex_check( Vertex * v )
{
	return ( v != NULL ) && ( v -> position != NULL ) && ( v -> worldPosition != NULL );
}

void transformVertex( Matrix3D m, Vertex * v )
{
	* ( v -> worldPosition ) = matrix3D_transformVector( m, * ( v -> position ) );
}

# endif
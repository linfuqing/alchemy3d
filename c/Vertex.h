#ifndef __VERTEX_H_INCLUDED__ 
#define __VERTEX_H_INCLUDED__ 

# include < malloc.h >
# include < stdlib.h >

# include "Matrix3D.h"

typedef struct
{
	Vector3D * position;
	Vector3D * worldPosition;
}Vertex;

Vertex newVertex( Vector3D * position )
{
	Vertex v;

	v.position = position;
	if( ( v.worldPosition = ( Vector3D * )malloc( sizeof( Vector3D ) ) ) == NULL )
	{
		exit( 0 );
	}

	return v;
}

int vertex_check( Vertex * v )
{
	return v && v -> position && v -> worldPosition;
}

void transformVertex( Matrix3D m, Vertex * v )
{
	vector3D_copy( v -> worldPosition, * ( v -> position ) );

	matrix3D_transformVector( m, v -> worldPosition );
}

#endif
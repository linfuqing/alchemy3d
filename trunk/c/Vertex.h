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

	( & v ) -> position = position;

	return v;
}

void transformVertex( Matrix3D m, Vertex * v )
{
	Vector3D c;

	vector3D_copy( & c, * ( v -> position ) );

	matrix3D_transformVector( m, &c );

	vector3D_copy( v -> worldPosition, c );
}
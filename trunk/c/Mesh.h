#ifndef MESH_H
#define MESH_H

# include "Faces.h"
# include "Vertices.h"

//RW
typedef struct
{
	Faces    * faces;
	Vertices * vertices;
}Mesh;

Mesh * newMesh( Faces * faces, Vertices * vertices )
{
	Mesh * m;

	if( ( m = ( Mesh * )malloc( sizeof( Mesh ) ) ) == NULL )
	{
		exit( TRUE );
	}

	m -> faces    = faces;
	m -> vertices = vertices;

	return m;
}

int mesh_check( Mesh * m )
{
	return ( m != NULL ) && ( m -> vertices != NULL );
}

#endif
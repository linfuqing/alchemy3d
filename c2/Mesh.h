#ifndef __MESH_H
#define __MESH_H

#include "Polygon.h"
#include "Vertex.h"
#include "Vector.h"
#include "AABB.h"

typedef struct Mesh
{
	int nFaces, nVertices;

	Polygon  * faces;

	Vertex * vertices;

	AABB * aabb, * worldAABB;

#ifdef __AS3__
	float * meshBuffer;
#endif

}Mesh;

#ifdef __AS3__
Mesh * newMesh( int nVertices, int nFaces, float * meshBuffer )
#else
Mesh * newMesh( int nVertices, int nFaces )
#endif
{
	Mesh * m;

	if( ( m = ( Mesh * )malloc( sizeof( Mesh ) ) ) == NULL)
	{
		exit( TRUE );
	}

	if( ( m->faces = ( Polygon * )malloc( sizeof( Polygon ) * nFaces ) ) == NULL )
	{
		exit( TRUE );
	}

	if( ( m->vertices = ( Vertex * )malloc( sizeof( Vertex ) * nVertices ) ) == NULL )
	{
		exit( TRUE );
	}

	m->nFaces		= 0;
	m->nVertices	= 0;

#ifdef __AS3__
	m->meshBuffer = meshBuffer;
#endif

	return m;
}

void mesh_push_vertices( Mesh * m, float x, float y, float z )
{
	newVertex( & m->vertices[m->nVertices], x, y, z );

	m->nVertices ++;
}

void mesh_push_faces( Mesh * m, Vertex * va, Vertex * vb, Vertex * vc, Vector *uva, Vector * uvb, Vector * uvc )
{
	newTriangle3D( & m->faces[m->nFaces], va, vb, vc, uva, uvb, uvc );

	m->nFaces ++;
}

void mesh_dispose( Mesh * mesh )
{
	int i = 0;

	aabb_dispose( mesh->aabb );
	aabb_dispose( mesh->worldAABB );

	for ( ; i < mesh->nVertices; i ++ )
	{
		vertex_dispose( & mesh->vertices[i] );
	}
	for ( i = 0; i < mesh->nFaces; i ++ )
	{
		polygon_dispose( & mesh->faces[i] );
	}

	free( mesh->vertices );
	free( mesh->faces );
	free( mesh );
}

#endif
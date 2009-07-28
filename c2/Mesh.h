#ifndef __MESH_H
#define __MESH_H

#include "Faces.h"
#include "Vertices.h"
#include "AABB.h"

typedef struct Mesh
{
	int nFaces, nVertices;

	Faces    * faces;

	Vertices * vertices;

	AABB * aabb, * worldAABB;
}Mesh;

Mesh * newMesh( Faces * faces, Vertices * vertices )
{
	Mesh * m;
	VertexNode * v;

	if( (m = ( Mesh * )malloc( sizeof( Mesh ) ) ) == NULL)
	{
		exit( TRUE );
	}

	m->faces		= faces;
	m->vertices		= vertices;
	m->nFaces		= faces->nFaces;
	m->nVertices	= vertices->nVertices;
	m->aabb			= newAABB();
	m->worldAABB	= newAABB();

	v = vertices->nodes;
	
	while ( NULL != v->next)
	{
		aabb_add(m->aabb, v->vertex->position);

		v = v->next;
	}

	aabb_copy( m->worldAABB, m->aabb );

	return m;
}

void mesh_dispose( Mesh * mesh )
{
	aabb_dispose( mesh->aabb );
	aabb_dispose( mesh->worldAABB );
	free( mesh );
}

#endif
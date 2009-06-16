#ifndef MESH_H
#define MESH_H

# include "Faces.h"
# include "Vertices.h"

typedef struct
{
	Faces    * faces;
	Vertices * vertices;
}Mesh;

Mesh newMesh( * faces, * vertices )
{
	Mesh m;

	m.faces    = faces;
	m.vertices = vertices;

	return m;
}

#endif
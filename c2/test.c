#include <malloc.h>
#include <stdlib.h>
#include <time.h>

#define INLINE

#include "Base.h"
#include "Math.h"
#include "Device.h"
#include "Entity.h"
#include "Scene.h"
#include "Viewport.h"
#include "Triangle.h"
#include "Mesh.h"
#include "Render.h"
#include "Vector3D.h"
#include "Matrix3D.h"
#include "AABB.h"
#include "ARGBColor.h"
#include "Material.h"
#include "Texture.h"
#include "Light.h"

int main()
{
	Triangle * face, *face2;
	Mesh * mesh2;
	Vertex * vArr2[9];
	Vector * point2;
	float pos2[18];

	//test***************************

	time_t ts,te;
	double delT;

	int i = 0, j = 0;

	pos2[0] = -50.0f;
	pos2[1] = -50.0f;

	pos2[2] = -50.0f;
	pos2[3] = 0.0f;

	pos2[4] = -50.0f;
	pos2[5] = 50.0f;

	pos2[6] = 0.0f;
	pos2[7] = -50.0f;

	pos2[8] = 0.0f;
	pos2[9] = 0.0f;

	pos2[10] = 0.0f;
	pos2[11] = 50.0f;

	pos2[12] = 50.0f;
	pos2[13] = -50.0f;

	pos2[14] = 50.0f;
	pos2[15] = 0.0f;

	pos2[16] = 50.0f;
	pos2[17] = 50.0f;

	mesh2 = newMesh( 18, 9 );

	for (; i < 18; i += 2)
	{
		mesh_push_vertex(mesh2, pos2[i], pos2[i+1], 0.0f);
		vArr2[j] = & mesh2->vertices[j];
		j++;
	}

	point2 = newVector(.5f, .5f);

	mesh_push_triangle(mesh2, vArr2[0], vArr2[3], vArr2[1], point2, point2, point2, NULL);
	mesh_push_triangle(mesh2, vArr2[4], vArr2[1], vArr2[3], point2, point2, point2, NULL);
	mesh_push_triangle(mesh2, vArr2[1], vArr2[4], vArr2[2], point2, point2, point2, NULL);
	mesh_push_triangle(mesh2, vArr2[5], vArr2[2], vArr2[4], point2, point2, point2, NULL);

	
	mesh_push_triangle(mesh2, vArr2[3], vArr2[6], vArr2[4], point2, point2, point2, NULL);
	mesh_push_triangle(mesh2, vArr2[7], vArr2[4], vArr2[6], point2, point2, point2, NULL);
	mesh_push_triangle(mesh2, vArr2[4], vArr2[7], vArr2[5], point2, point2, point2, NULL);
	mesh_push_triangle(mesh2, vArr2[8], vArr2[5], vArr2[7], point2, point2, point2, NULL);

	face = triangle_clone( & mesh2->faces[0] );

	ts=clock();

	for (i = 0; i < 10000; i ++)
	{
		//newVector3D(100, 200, 0, 1);
		//matrix3D_transformVector(&testM, v3d);
		//matrix3D_append4x4(&testM, entity->transform);
		//testM1 = matrix3D_clone(&testM);
		//matrix3D_copy(&testMP, &testM);
		//memcpy(&entity1, entity, sizeof( * entity));
		//matrix3D_invert4x4(&testM);
		//matrix3D_invert(testMP);
		//MAX(d, 1.0f);
		//if ( d > 1.0f ) d = 1.0f;
		
		face = triangle_clone( & mesh2->faces[0] );
		//face2 = newTriangle( face->vertex[0], face->vertex[1], face->vertex[2], face->vertex[0]->uv, face->vertex[1]->uv,  face->vertex[2]->uv, face->texture );
	}

	//memcpy(buffer, buffer2, 1024 * 1024 * sizeof(float));

	te=clock();

	delT = (double)(te-ts) / CLOCKS_PER_SEC;
	printf("%f\n",delT);

	return 0;
}
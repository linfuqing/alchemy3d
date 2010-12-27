#ifndef SKINMESHCONTROLLER_H
#define SKINMESHCONTROLLER_H

#include "Entity.h"

typedef struct
{
	float weight;
	int index;
}Weight;

typedef struct SkinMeshController
{
	int numBones;
	Weight** weights;
	int* weightsLength;
	Vector3D* vertices;
	Matrix4x4* boneOffsetMatrices;
	Entity** bones;
}SkinMeshController;

SkinMeshController * newSkinMeshController(int numBones, Weight** weights, int* weightsLength, Vector3D* vertices, Matrix4x4* boneOffsetMatrices, Entity** bones)
{
	SkinMeshController * skinMeshController;

	if( ( skinMeshController = ( SkinMeshController * )malloc( sizeof( SkinMeshController ) ) ) == NULL ) exit( TRUE );

	skinMeshController->numBones           = numBones;
	skinMeshController->weights            = weights;
	skinMeshController->weightsLength      = weightsLength;
	skinMeshController->vertices           = vertices;
	skinMeshController->boneOffsetMatrices = boneOffsetMatrices;
	skinMeshController->bones              = bones;

	return skinMeshController;
}

void skinMeshController_destroy(SkinMeshController **controller)
{
	free(*controller);

	*controller = NULL;
}

void skinMeshController_updateSkinnedMesh(SkinMeshController* controller, Mesh* skinMesh)
{
	int i, j, length;

	Weight* weight;

	Matrix4x4 matrix;

	Vector3D position;

	if(!skinMesh)
		return;

	if(!controller->numBones)
		return;

	for(i = 0; i < skinMesh->nVertices; i ++)
		vector3D_set(skinMesh->vertices[i]->position, 0.0f, 0.0f, 0.0f, 1.0f);
			
	for(i = 0; i < controller->numBones; i ++)
	{
		if( (length = controller->weightsLength[i]) != 0 )
		{
			matrix4x4_append4x4(&matrix, &controller->boneOffsetMatrices[i], controller->bones[i]->world);

			for(j = 0; j < length; j ++)
			{
				if( (weight = &controller->weights[i][j])->weight < 0.0001f || weight->weight > 1.0001f )
					continue;

				matrix4x4_transformVector(&position, &matrix, &controller->vertices[weight->index]);

				vector3D_scaleBy(&position, weight->weight);

				vector3D_add_self(skinMesh->vertices[weight->index]->position, &position);
			}
		}
	}

	mesh_updateFaces(skinMesh);
}

#endif
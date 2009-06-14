#ifndef __DISPLAYOBJECT3D_H_INCLUDED__ 
#define __DISPLAYOBJECT3D_H_INCLUDED__ 

#include "Vector3D.h"
#include "Matrix3D.h"

typedef struct
{
	Vector3D * position;
	Vector3D * direction;
	Vector3D * scale;

	Matrix3D * transform;
	Matrix3D * world;
	Matrix3D * view;
	Matrix3D * tmpMatrix;

	int visible;
}DisplayObject3D;

DisplayObject3D newDisplayObject3D()
{
	DisplayObject3D do3d;

	Vector3D vPosition = newVector3D(0, 0, 0, 1);
	Vector3D vDirection = newVector3D(0, 0, 0, 1);
	Vector3D vScale = newVector3D(0, 0, 0, 1);

	do3d.position = &vPosition;
	do3d.direction = &vDirection;
	do3d.scale = &vScale;

	Number transformData[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
	Number worldData[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
	Number viewData[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
	Number tmpMatrixData[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};

	do3d.transform = &newMatrix3D(&transformData);
	do3d.world = &newMatrix3D(&worldData);
	do3d.view = &newMatrix3D(&viewData);
	do3d.tmpMatrix = &newMatrix3D(&tmpMatrixData);

	return do3d;
}

#endif
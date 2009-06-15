#ifndef __DISPLAYOBJECT3D_H_INCLUDED__ 
#define __DISPLAYOBJECT3D_H_INCLUDED__ 

#include <stdio.h>

#include "Vector3D.h"
#include "Matrix3D.h"
#include "Quaternion.h"

typedef struct Node
{
	Vector3D position;
	Vector3D direction;
	Vector3D scale;

	Matrix3D transform;
	Matrix3D world;
	Matrix3D view;
}DisplayObject3D;

DisplayObject3D newDisplayObject3D()
{
	DisplayObject3D do3d;

	Number transformData[16] = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
	Number worldData[16] = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
	Number viewData[16] = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};

	do3d.position = newVector3D(0.0, 0.0, 0.0, 1.0);
	do3d.direction = newVector3D(0.0, 0.0, 0.0, 1.0);
	do3d.scale = newVector3D(1.0, 1.0, 1.0, 1.0);

	do3d.transform = newMatrix3D(&transformData);
	do3d.world = newMatrix3D(&worldData);
	do3d.view = newMatrix3D(&viewData);

	return do3d;
}

void do3d_updateTransform(DisplayObject3D * do3d)
{
	Quaternion * qua;
	Matrix3D * mtr;

	matrix3D_apprendScale(&(do3d->transform), do3d->scale.x, do3d->scale.y, do3d->scale.z);
	qua = quaternoin_setFromEuler(do3d->direction.y * TORADIANS, do3d->direction.x * TORADIANS, do3d->direction.z * TORADIANS);
	mtr = quaternoin_getMatrix(qua);
	matrix3D_apprend(&(do3d->transform), * mtr);
	matrix3D_apprendTranslation(&(do3d->transform), do3d->position.x, do3d->position.y, do3d->position.z);
}

#endif
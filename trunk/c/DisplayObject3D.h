#ifndef DISPLAYOBJECT3D_H
#define DISPLAYOBJECT3D_H

/*#include <malloc.h>

#include "Vector3D.h"
#include "Matrix3D.h"
#include "Quaternion.h"*/
# include "Camera.h"
# include "Mesh.h"

typedef struct DisplayObject3D
{
	/*Vector3D * position;
	Vector3D * direction;
	Vector3D * scale;

	Matrix3D * transform;
	Matrix3D * world;
	Matrix3D * view;

	int visible;
	struct DisplayObject3D * parent;*/

	Camera * camera;
	Mesh   * mesh;

}DisplayObject3D;

DisplayObject3D * newDisplayObject3D( Mesh * mesh )
{
	DisplayObject3D * do3d;

	/*Number transformData[16] = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
	Number worldData[16] = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
	Number viewData[16] = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};*/

	if( ( do3d = ( DisplayObject3D * )malloc( sizeof( DisplayObject3D ) ) ) == NULL )
	{
		exit( TRUE );
	}

	/*do3d->position = newVector3D(0.0, 0.0, 0.0, 1.0);
	do3d->direction = newVector3D(0.0, 0.0, 0.0, 1.0);
	do3d->scale = newVector3D(1.0, 1.0, 1.0, 1.0);

	do3d->transform = newMatrix3D(&transformData);
	do3d->world = newMatrix3D(&worldData);
	do3d->view = newMatrix3D(&viewData);

	do3d->parent = NULL;*/

	do3d -> camera = newCamera( newVector3D( 0, 0, 0, 1 ), newVector3D( 0, 0, 0, 1 ), newVector3D( 0, 0, 0, 1 ) );
	do3d -> mesh   = mesh;

	return do3d;
}

/*void do3d_updateTransform(DisplayObject3D * do3d)
{
	Quaternion * qua;
	Matrix3D * mtr;

	matrix3D_apprendScale(do3d->transform, do3d->scale->x, do3d->scale->y, do3d->scale->z);

	qua = quaternoin_setFromEuler(do3d->direction->y * TORADIANS, do3d->direction->x * TORADIANS, do3d->direction->z * TORADIANS);
	mtr = quaternoin_getMatrix(qua);

	matrix3D_apprend(do3d->transform, * mtr);

	matrix3D_apprendTranslation(do3d->transform, do3d->position->x, do3d->position->y, do3d->position->z);

	do3d->world = matrix3D_clone(do3d->transform);

	do3d->view = matrix3D_clone(do3d->transform);
}*/

/*void do3d_project(DisplayObject3D * do3d)
{

}*/

int transformDisplayObject( DisplayObject * d )
{
	if( d -> camera -> move )
	{
		transformVertices( camera_getTransform( * ( d -> camera ) ) );
		return TRUE;
	}
	
	return FALSE;
}

#endif
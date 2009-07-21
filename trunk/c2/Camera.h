#ifndef CAMERA_H
#define CAMERA_H

#include <malloc.h>

#include "Matrix3D.h"
#include "Entity.h"

typedef struct Camera
{
	Entity * eye;

	Vector3D * target;

	Matrix3D * projectionMatrix;

	Matrix3D * projectionMatrixInvert;

	float fov, nearClip, farClip;

	int fnfDirty, isAttached, hasTarget;
}Camera;

Camera * newCamera( float fov, float nearClip, float farClip, Entity * eye )
{
	Camera * cam;

	if( ( cam = ( Camera * )malloc( sizeof( Camera ) ) ) == NULL )
	{
		exit( TRUE );
	}
	
	cam->fov = fov;
	cam->nearClip = nearClip;
	cam->farClip = farClip;

	cam->eye = eye;
	cam->eye->position->z = -nearClip;

	cam->projectionMatrix = newMatrix3D(NULL);
	cam->projectionMatrixInvert = newMatrix3D(NULL);

	cam->target = newVector3D( 0.0f, 0.0f, 0.0f, 1.0f );

	cam->fnfDirty = TRUE;
	cam->isAttached = FALSE;
	cam->hasTarget = FALSE;

	return cam;
}

void camera_setTarget( Camera * camera, Vector3D * target )
{
	camera->target = target;
	camera->hasTarget = TRUE;
}

void camera_setNullTarget( Camera * camera, int setFree )
{
	if ( TRUE == setFree ) free( camera->target );

	camera->hasTarget = FALSE;
}

Matrix3D * getProjectionMatrix( Matrix3D * output, float top, float bottom, float left, float right, float near, float far )
{
	float fn;

	fn = far / ( far - near );

	output->m11 = 2 * near / (right - left);
	output->m22 = 2 * near / (top - bottom);
	
	if (left == -right)
	{
		output->m31 = 0;
	}
	else
	{
		output->m31 = (left + right) / (left - right);
	}

	if (top == -bottom)
	{
		output->m32 = 0;
	}
	else
	{
		output->m32 = (bottom + top) / (bottom - top);
	}

	output->m33 = fn;
	output->m34 = 1;
	output->m43 = - near * fn;
	output->m12 = output->m13 = output->m14 = output->m21 = output->m23 = output->m24 = output->m41 = output->m42 = output->m44 = 0.0f;

	return output;
}

Matrix3D * getPerspectiveFovLH( Matrix3D * output, Camera * camera, float aspect )
{
	float top = camera->nearClip * tanf( DEG2RAD( camera->fov * 0.5 ) );
	float bottom = -top;
	float right = top * aspect;
	float left = -right;

	return getProjectionMatrix(output, top, bottom, left, right, camera->nearClip, camera->farClip);
}

void camera_updateTransform(Camera * camera)
{
	Entity * eye = camera->eye;

	if ( TRUE == eye->transformDirty )
	{
		if ( FALSE == camera->hasTarget )
		{
			//单位化
			matrix3D_identity( eye->transform );
			//缩放
			matrix3D_appendScale( eye->transform, eye->scale->x, - eye->scale->y, eye->scale->z );
			//旋转
			matrix3D_append( eye->transform, quaternoin_toMatrix( &quaMtr, quaternoin_setFromEuler( &qua, DEG2RAD( eye->direction->y ), DEG2RAD( eye->direction->x ), DEG2RAD( eye->direction->z ) ) ) );
			//位移
			matrix3D_appendTranslation( eye->transform, eye->position->x, eye->position->y, eye->position->z );

			matrix3D_copy( eye->world, eye->transform );

			matrix3D_copy( eye->worldInvert, eye->world );

			matrix3D_getPosition( eye->worldPosition, eye->world );

			matrix3D_fastInvert( eye->world );
		}
		else
		{
			lookAtLH( eye->world, eye->position, camera->target, INV_Y_AXIS );

			matrix3D_getPosition( eye->worldPosition, eye->world );
		}
	}
}

# endif
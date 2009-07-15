#ifndef CAMERA_H
#define CAMERA_H

#include "Matrix3D.h"
#include "Entity.h"

typedef struct Camera
{
	Entity * eye;

	Vector3D * target;

	Matrix3D * projectionMatrix;

	Matrix3D * projectionMatrixInvert;

	float fov, nearClip, farClip, left, right, top, bottom;

	int fnfDirty, isAttached;
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
	cam->left = cam->right = cam->bottom = cam->top = 0.0f;

	cam->eye = eye;
	cam->eye->position->z = -nearClip;

	cam->projectionMatrix = newMatrix3D(NULL);
	cam->projectionMatrixInvert = newMatrix3D(NULL);

	cam->target = NULL;

	cam->fnfDirty = TRUE;
	cam->isAttached = FALSE;

	return cam;
}

Matrix3D getProjectionMatrix( float top, float bottom, float left, float right, float near, float far )
{
	Matrix3D proj_matrix;
	float fn;

	fn = far - near;

	proj_matrix.m11 = 2 * near / (right - left);
	proj_matrix.m22 = 2 * near / (top - bottom);
	
	if (left == -right)
	{
		proj_matrix.m31 = 0;
	}
	else
	{
		proj_matrix.m31 = (left + right) / (left - right);
	}

	if (top == -bottom)
	{
		proj_matrix.m32 = 0;
	}
	else
	{
		proj_matrix.m32 = (bottom + top) / (bottom - top);
	}

	proj_matrix.m33 = far / fn;
	proj_matrix.m34 = 1;
	proj_matrix.m43 = - near * far / fn;
	proj_matrix.m12 = proj_matrix.m13 = proj_matrix.m14 = proj_matrix.m21 = proj_matrix.m23 = proj_matrix.m24 = proj_matrix.m41 = proj_matrix.m42 = proj_matrix.m44 = 0.0f;

	return proj_matrix;
}

Matrix3D getPerspectiveFovLH( Camera * camera, float aspect )
{
	float top = camera->nearClip * tanf( DEG2RAD( camera->fov * 0.5 ) );
	float bottom = -top;
	float right = top * aspect;
	float left = -right;

	camera->bottom = bottom;
	camera->top = top;
	camera->left = left;
	camera->right = right;

	return getProjectionMatrix(top, bottom, left, right, camera->nearClip, camera->farClip);
}

void camera_update(Camera * camera)
{
	Entity * eye = camera->eye;

	if ( TRUE == eye->transformDirty )
	{
		if ( NULL == camera->target )
		{
			entity_updateTransform(eye);
			matrix3D_fastInvert(eye->world);
		}
		else
		{
			lookAtLH( eye->world, eye->position, camera->target, Y_AXIS );
			matrix3D_getPosition( eye->worldPosition, eye->world );
		}
	}
}

# endif
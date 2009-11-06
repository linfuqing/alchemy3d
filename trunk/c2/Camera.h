#ifndef CAMERA_H
#define CAMERA_H

#include <malloc.h>

#include "Matrix3D.h"
#include "Entity.h"

typedef struct Camera
{
	Entity * eye;

	Vector3D * target;

	Matrix4x4 * projectionMatrix;

	float fov, near, far, n_top, n_bottom, n_left, n_right, f_top, f_bottom, f_right, f_left, fn;

	int fnfDirty, isUVN;
}Camera;

Camera * newCamera( float fov, float near, float far, Entity * eye )
{
	Camera * cam;

	if( ( cam = ( Camera * )malloc( sizeof( Camera ) ) ) == NULL )
	{
		exit( TRUE );
	}
	
	cam->fov = fov;
	cam->near = near;
	cam->far = far;

	cam->eye = eye;
	cam->eye->position->z = -near;

	cam->projectionMatrix = newMatrix4x4(NULL);

	cam->target = NULL;

	cam->fnfDirty = TRUE;
	cam->isUVN = FALSE;

	return cam;
}

void camera_dispose( Camera * camera )
{
	matrix4x4_dispose( camera->projectionMatrix );
	
	memset( camera, 0, sizeof( Camera ) );
	
	free( camera );
}

INLINE void camera_setTarget( Camera * camera, Vector3D * target )
{
	camera->target = target;
	camera->isUVN = TRUE;
}

INLINE void camera_setNullTarget( Camera * camera, int setFree )
{
	if ( TRUE == setFree )
	{
		memset( camera->target, 0, sizeof( Vector3D ) );

		free( camera->target );
	}

	camera->isUVN = FALSE;
}

//根据上下左右远近截面获得投影矩阵
INLINE Matrix4x4 * getProjectionMatrix( Matrix4x4 * output, float top, float bottom, float left, float right, float near, float far )
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

//根据远近截面、宽高比和视角获得投影矩阵
INLINE Matrix4x4 * getPerspectiveFovLH( Matrix4x4 * output, Camera * camera, float aspect )
{
	camera->n_top = camera->near * tanf( DEG2RAD( camera->fov * 0.5f ) );
	camera->n_bottom = -camera->n_top;
	camera->n_right = camera->n_top * aspect;
	camera->n_left = -camera->n_right;

	camera->f_top = camera->far * tanf( DEG2RAD( camera->fov * 0.5f ) );
	camera->f_bottom = -camera->f_top;
	camera->f_right = camera->f_top * aspect;
	camera->f_left = -camera->f_right;

	return getProjectionMatrix(output, camera->n_top, camera->n_bottom, camera->n_left, camera->n_right, camera->near, camera->far);
}

//更新摄像机矩阵
INLINE void camera_updateTransform(Camera * camera)
{
	Quaternion qua;
	Matrix4x4 quaMtr;

	Entity * eye = camera->eye;

	//如果没有目标
	if ( FALSE == camera->isUVN )
	{
		//如果摄像机属性被改变
		if ( TRUE == eye->transformDirty )
		{
			//单位化
			matrix4x4_identity( eye->transform );
			//缩放
			matrix4x4_appendScale( eye->transform, eye->scale->x, - eye->scale->y, eye->scale->z );
			//旋转
			matrix4x4_append_self( eye->transform, quaternoin_toMatrix( &quaMtr, quaternoin_setFromEuler( &qua, DEG2RAD( eye->direction->y ), DEG2RAD( eye->direction->x ), DEG2RAD( eye->direction->z ) ) ) );
			//位移
			matrix4x4_appendTranslation( eye->transform, eye->position->x, eye->position->y, eye->position->z );

			matrix4x4_copy( eye->world, eye->transform );

			//从世界矩阵获得世界位置
			matrix4x4_getPosition( eye->w_pos, eye->world );

			matrix4x4_fastInvert( eye->world );
		}
	}
	//如果有目标
	else
	{
		//获得指向目标位置的旋转矩阵
		lookAt( eye->world, eye->position, camera->target, Y_AXIS );
		//从世界矩阵获得世界位置
		matrix4x4_getPosition( eye->w_pos, eye->world );

		eye->w_pos->x *= -1;
		eye->w_pos->y *= -1;
		eye->w_pos->z *= -1;
	}
}

# endif
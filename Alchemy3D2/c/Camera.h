#ifndef CAMERA_H
#define CAMERA_H

#include <malloc.h>

#include "Matrix3D.h"
#include "Entity.h"

#define CAMERA_UVN_TYPE_NONE     0
#define CAMERA_UVN_TYPE_ABSOLUT  1
#define CAMERA_UVN_TYPE_RELATIVE 2

typedef struct Camera
{
	Entity * eye;

	Vector3D * target;

	Matrix4x4 * projectionMatrix, * viewProjectionMatrix, *invertViewProjectionMatrix;

	float fov, near, far, n_top, n_bottom, n_left, n_right, f_top, f_bottom, f_right, f_left, fn, rotateX, rotateY, rotateZ, distance, factor, step;

	int fnfDirty, UVNType, rotationDirty;
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

	cam->projectionMatrix           = newMatrix4x4(NULL);
	cam->viewProjectionMatrix       = newMatrix4x4(NULL);
	cam->invertViewProjectionMatrix = newMatrix4x4(NULL);

	cam->target = newVector3D(0, 0, 0, 1);

	cam->fnfDirty = TRUE;
	cam->UVNType = CAMERA_UVN_TYPE_NONE;

	cam->rotateX = cam->rotateY = cam->rotateZ = 0.0f;
	cam->distance = 0.0f;
	cam->factor = 1.0f;
	cam->step = 1.0f;
	cam->rotationDirty = FALSE;

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
}

INLINE void camera_setNullTarget( Camera * camera, int setFree )
{
	if ( TRUE == setFree )
	{
		memset( camera->target, 0, sizeof( Vector3D ) );

		free( camera->target );
	}
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
	Vector3D temp;
	float angle, s,c;

	Entity * eye = camera->eye;

	//如果没有目标
	if ( CAMERA_UVN_TYPE_NONE == camera->UVNType )
	{
		//如果摄像机属性被改变
		if ( eye->transformDirty )
		{
			if(eye->matrixDirty)
			{
				//单位化
				matrix4x4_identity( eye->transform );
				//缩放
				matrix4x4_appendScale( eye->transform, eye->scale->x, - eye->scale->y, eye->scale->z );
				//旋转
				matrix4x4_append_self( eye->transform, quaternoin_toMatrix( &quaMtr, quaternoin_setFromEuler( &qua, DEG2RAD( eye->direction->y ), DEG2RAD( eye->direction->x ), DEG2RAD( eye->direction->z ) ) ) );
				//位移
				matrix4x4_appendTranslation( eye->transform, eye->position->x, eye->position->y, eye->position->z );
			}

			matrix4x4_copy( eye->world, eye->transform );

			//从世界矩阵获得世界位置
			matrix4x4_getPosition( eye->w_pos, eye->world );

			matrix4x4_fastInvert( eye->world );
		}
	}
	//如果有目标
	else
	{
		if(!eye->matrixDirty)
		{
			matrix4x4_decompose(eye->transform, eye->position, eye->scale, eye->direction, NULL);

			eye->matrixDirty = TRUE;
		}
			//matrix4x4_getPosition(eye->position, eye->transform);

		if(camera->rotationDirty)
		{
			if(camera->UVNType == CAMERA_UVN_TYPE_ABSOLUT)
			{
				angle = DEG2RAD(camera->rotateX);
				s     = sinf(angle);
				c     = cosf(angle);
				angle = DEG2RAD(camera->rotateY);

				eye->position->x = camera->target->x + camera->distance * sin(angle) * c;
				eye->position->y = camera->target->y + camera->distance * s * camera->factor;
				eye->position->z = camera->target->z + camera->distance * cos(angle) * c;
			}
			else
			{
				vector3D_subtract(&temp, eye->position, camera->target);

				//matrix4x4_transformVector_self(quaternoin_toMatrix( &quaMtr, quaternoin_setFromEuler( &qua, DEG2RAD(camera->rotateX), DEG2RAD(camera->rotateY), DEG2RAD(camera->rotateZ) ) ), eye->position);
				if(camera->rotateZ)
				{
					angle = DEG2RAD(camera->rotateZ);
					s     = sinf(angle);
					c     = cosf(angle);

					eye->position->x = c * temp.x - s * temp.y;
					eye->position->y = c * temp.y + s * temp.x;

					temp.x = eye->position->x;
					temp.y = eye->position->y;
				}

				if(camera->rotateY)
				{
					angle = DEG2RAD(camera->rotateY);
					s     = sinf(angle);
					c     = cosf(angle);

					eye->position->x = c * temp.x - s * temp.z;
					eye->position->z = c * temp.z + s * temp.x;

					temp.x = eye->position->x;
					temp.z = eye->position->z;
				}

				if(camera->rotateX)
				{
					angle = DEG2RAD(camera->rotateX);
					s     = sinf(angle);
					c     = cosf(angle);

					eye->position->y = c * temp.y - s * temp.z;
					eye->position->z = c * temp.z + s * temp.y;

					temp.y = eye->position->y;
					temp.z = eye->position->z;
				}

				vector3D_add(eye->position, &temp, camera->target);

				camera->rotateX = camera->rotateY = camera->rotateZ = 0.0f;
			}

			camera->rotationDirty = FALSE;
		}

		//获得指向目标位置的旋转矩阵
		/*lookAt( eye->world, eye->position, camera->target, Y_AXIS );

		matrix4x4_copy(eye->transform,  eye->world);

		matrix4x4_fastInvert(eye->transform);*/
		//lookAt( eye->transform, eye->position, camera->target, Y_AXIS );
		vector3D_subtract(&temp, camera->target, camera->eye->position);

		temp.x = DEG2RAD(temp.x);
		temp.y = DEG2RAD(temp.y);
		temp.z = DEG2RAD(temp.z);

		//vector3D_directionToRotation(&eye->direction->x, &eye->direction->y, &temp);
		vector3D_directionToRotation(&s, &c, &temp);

		eye->direction->x += (s - eye->direction->x) / camera->step;
		eye->direction->y += (c - eye->direction->y) / camera->step;

		/*if( temp.x == 0 && temp.z == 0 && temp.y == 0 )
		{
			eye->direction->x = 0;
			eye->direction->y = 0;
		}
		else if( temp.x == 0 && temp.z == 0  )
		{
			eye->direction->x = - RAD2DEG( asin( temp.y / vector3D_length(&temp) ) );
			eye->direction->y =   0;
		}
		else if( temp.z < 0 )
		{
			eye->direction->x = - RAD2DEG( asin( temp.y / vector3D_length(&temp) ) );
			eye->direction->y =   RAD2DEG( PI - asin( temp.x / sqrt(temp.x * temp.x + temp.z * temp.z) ) );
		}
		else
		{
			eye->direction->x = - RAD2DEG( asin( temp.y / vector3D_length(&temp) ) );
			eye->direction->y =   RAD2DEG( asin( temp.x / sqrt(temp.x * temp.x + temp.z * temp.z) ) );
		}*/

		//单位化
		matrix4x4_identity( eye->transform );
		//缩放
		matrix4x4_appendScale( eye->transform, eye->scale->x, - eye->scale->y, eye->scale->z );
		//旋转
		matrix4x4_append_self( eye->transform, quaternoin_toMatrix( &quaMtr, quaternoin_setFromEuler( &qua, DEG2RAD( eye->direction->y ), DEG2RAD( eye->direction->x ), DEG2RAD( eye->direction->z ) ) ) );
		//位移
		matrix4x4_appendTranslation( eye->transform, eye->position->x, eye->position->y, eye->position->z );

		matrix4x4_copy(eye->world,  eye->transform);

		//从世界矩阵获得世界位置
		matrix4x4_getPosition(eye->w_pos, eye->transform);

		matrix4x4_fastInvert(eye->world);

		//eye->matrixDirty = FALSE;
	}
}

# endif
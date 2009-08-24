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

	float fov, near, far, top, bottom, left, right;

	int fnfDirty, isAttached, hasTarget;
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

	cam->projectionMatrix = newMatrix3D(NULL);

	cam->target = NULL;

	cam->fnfDirty = TRUE;
	cam->isAttached = FALSE;
	cam->hasTarget = FALSE;

	return cam;
}

void camera_dispose( Camera * camera )
{
	matrix3D_dispose( camera->projectionMatrix );
	free( camera );
}

INLINE void camera_setTarget( Camera * camera, Vector3D * target )
{
	camera->target = target;
	camera->hasTarget = TRUE;
}

INLINE void camera_setNullTarget( Camera * camera, int setFree )
{
	if ( TRUE == setFree ) free( camera->target );

	camera->hasTarget = FALSE;
}

//������������Զ��������ͶӰ����
INLINE Matrix3D * getProjectionMatrix( Matrix3D * output, float top, float bottom, float left, float right, float near, float far )
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

//����Զ�����桢��߱Ⱥ��ӽǻ��ͶӰ����
INLINE Matrix3D * getPerspectiveFovLH( Matrix3D * output, Camera * camera, float aspect )
{
	camera->top = camera->near * tanf( DEG2RAD( camera->fov * 0.5 ) );
	camera->bottom = -camera->top;
	camera->right = camera->top * aspect;
	camera->left = -camera->right;

	return getProjectionMatrix(output, camera->top, camera->bottom, camera->left, camera->right, camera->near, camera->far);
}

//�������������
INLINE void camera_updateTransform(Camera * camera)
{
	Quaternion qua;
	Matrix3D quaMtr;

	Entity * eye = camera->eye;

	//�����������Ա��ı�
	if ( TRUE == eye->transformDirty )
	{
		//���û��Ŀ��
		if ( FALSE == camera->hasTarget )
		{
			//��λ��
			matrix3D_identity( eye->transform );
			//����
			matrix3D_appendScale( eye->transform, eye->scale->x, - eye->scale->y, eye->scale->z );
			//��ת
			matrix3D_append_self( eye->transform, quaternoin_toMatrix( &quaMtr, quaternoin_setFromEuler( &qua, DEG2RAD( eye->direction->y ), DEG2RAD( eye->direction->x ), DEG2RAD( eye->direction->z ) ) ) );
			//λ��
			matrix3D_appendTranslation( eye->transform, eye->position->x, eye->position->y, eye->position->z );

			matrix3D_copy( eye->world, eye->transform );

			//���������������λ��
			matrix3D_getPosition( eye->worldPosition, eye->world );

			matrix3D_fastInvert( eye->world );
		}
		//�����Ŀ��
		else
		{
			//���ָ��Ŀ��λ�õ���ת����
			lookAt( eye->world, eye->position, camera->target, Y_AXIS );
			//���������������λ��
			matrix3D_getPosition( eye->worldPosition, eye->world );

			eye->worldPosition->x *= -1;
			eye->worldPosition->y *= -1;
			eye->worldPosition->z *= -1;
		}
	}
}

# endif
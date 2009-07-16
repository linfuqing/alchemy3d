#ifndef __LIGHT_H
#define __LIGHT_H

#include <malloc.h>

#include "Entity.h"
#include "Vector3D.h"
#include "Color.h"

#define POINT_LIGHT			0
#define DIRECTIONAL_LIGHT	1
#define SPOT_LIGHT			2

typedef struct Light
{
	int type;			//����ƹ����ͣ������ܹ�ʹ��������������֮һ��D3DLIGHT_POINT, D3DLIGHT_SPOT, D3DLIGHT_DIRECTIONAL

	Color * ambient;		//�˹�Դ�����Ļ�������ɫ
	Color * diffuse;		//�˹�Դ�������������ɫ
	Color * specular;	//�˹�Դ�����ľ������ɫ

	float range;		//�ƹ��ܹ����������Χ
	float falloff;		//���ֵֻ�����ھ۹���ϡ�������ƹ��ڴ���Բ׶����Բ׶֮���ǿ��˥��������ֵͨ������Ϊ1.0f
	float theta;		//ֻ���ھ۹�ƣ�ָ����Բ׶�ĽǶȣ���λ�ǻ���
	float phi;			//ֻ���ھ۹�ƣ�ָ����Բ׶�ĽǶȣ���λ�ǻ���
	float attenuation0, attenuation1, attenuation2;		//��Щ˥����������������ƹ�ǿ�ȵĴ�������˥��

	Entity * source;
}Light;

Light * newPointLight( int type, Entity * source, Color * color )
{
	Light * light;

	if( ( light = ( Light * )malloc( sizeof( Light ) ) ) == NULL )
	{
		exit( TRUE );
	}

	light->type = type;
	light->ambient = newColor( 1.0f, 1.0f, 1.0f, 1.0f );
	light->diffuse = color;
	light->specular = newColor( 1.0f, 1.0f, 1.0f, 1.0f );

	color_scaleBy( light->ambient, color, 0.4f, 0.4f, 0.4f, 1 );
	color_scaleBy( light->specular, color, 0.6f, 0.6f, 0.6f, 1 );

	light->source = source;

	return light;
}

//Light * newDirectionalLight( Vector3D * direction, Color * color )
//{
//	Light * light;
//
//	if( ( light = ( Light * )malloc( sizeof( Light ) ) ) == NULL )
//	{
//		exit( TRUE );
//	}
//
//	light->type = DIRECTIONAL_LIGHT;
//	color_scaleBy( light->ambient, color, 0.4f);
//	light->diffuse = color;
//	color_scaleBy( light->specular, color, 0.6f );
//	light->direction = direction;
//
//	return light;
//}
//
//Light * newSpotLight( Vector3D * position, Vector3D * direction, Color * color )
//{
//	Light * light;
//
//	if( ( light = ( Light * )malloc( sizeof( Light ) ) ) == NULL )
//	{
//		exit( TRUE );
//	}
//
//	light->type = SPOT_LIGHT;
//	color_scaleBy( light->ambient, color, 0.4f);
//	light->diffuse = color;
//	color_scaleBy( light->specular, color, 0.6f );
//	light->position = position;
//	light->direction = direction;
//
//	return light;
//}

void light_updateTransform(Light * light)
{
	Entity * source = light->source;
	//��λ��
	matrix3D_identity( source->transform );
	//��ת
	matrix3D_append( source->transform, quaternoin_toMatrix( &quaMtr, quaternoin_setFromEuler( &qua, DEG2RAD( source->direction->y ), DEG2RAD( source->direction->x ), DEG2RAD( source->direction->z ) ) ) );
	//λ��
	matrix3D_appendTranslation( source->transform, source->position->x, source->position->y, source->position->z );

	matrix3D_copy( source->world, source->transform );

	matrix3D_getPosition( source->worldPosition, source->world );
}

#endif
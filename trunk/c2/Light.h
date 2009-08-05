#ifndef __LIGHT_H
#define __LIGHT_H

#include <malloc.h>

#include "Entity.h"
#include "Vector3D.h"
#include "Color.h"

#define POINT_LIGHT			0
#define DIRECTIONAL_LIGHT	1
#define SPOT_LIGHT			2

#define EASY_MODE			0
#define MID_MODE			1
#define HIGH_MODE			2

//��ֲ��ӵ�
#define M_LOCAL_VIEWRT		0

typedef struct Light
{
	int mode;			//����ģʽ
	int type;			//����ƹ����ͣ������ܹ�ʹ��������������֮һ��D3DLIGHT_POINT, D3DLIGHT_SPOT, D3DLIGHT_DIRECTIONAL
	int bOnOff;			//�ƹ��Ƿ���

	Color * ambient;	//�˹�Դ�����Ļ�������ɫ
	Color * diffuse;	//�˹�Դ�������������ɫ
	Color * specular;	//�˹�Դ�����ľ������ɫ

	float range;		//�ƹ��ܹ����������Χ
	float falloff;		//���ֵֻ�����ھ۹���ϡ�������ƹ��ڴ���Բ׶����Բ׶֮���ǿ��˥��������ֵͨ������Ϊ1.0f
	float theta;		//ֻ���ھ۹�ƣ�ָ����Բ׶�ĽǶȣ���λ�ǻ���
	float phi;			//ֻ���ھ۹�ƣ�ָ����Բ׶�ĽǶȣ���λ�ǻ���
	float attenuation0, attenuation1, attenuation2;		//��Щ˥����������������ƹ�ǿ�ȵĴ�������˥��
	float spotExp;		//�۹�ָ��
	float spotCutoff;	//�۹ⷢɢ���

	Entity * source;
}Light;

typedef struct Lights
{
	Light * light;

	struct Lights * next;
}Lights;

Light * newPointLight( int type, Entity * source )
{
	Light * light;

	if( ( light = ( Light * )malloc( sizeof( Light ) ) ) == NULL )
	{
		exit( TRUE );
	}

	//ȱʡ��ԴΪһȫ���ɫ���Դ
	light->mode = EASY_MODE;
	light->type = type;
	light->bOnOff = FALSE;
	light->ambient = newColor( 0.0f, 0.0f, 0.0f, 1.0f );
	light->diffuse = newColor( 1.0f, 1.0f, 1.0f, 1.0f );
	light->specular = newColor( 1.0f, 1.0f, 1.0f, 1.0f );

	light->attenuation0 = 1.0f;
	light->attenuation1 = light->attenuation2 = 0.0f;
	light->spotExp = 1.0f;
	light->spotCutoff = cosf( 180.0f * PI );

	light->source = source;

	return light;
}

void light_dispose( Light * light )
{
	free( light );
}

INLINE void setLightOnOff( Light * light, int OnOff )
{
	light->bOnOff = OnOff;
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

//���¹�Դ����
void light_updateTransform(Light * light)
{
	Entity * source = light->source;

	matrix3D_identity( source->transform );

	//�����Դ��ƽ�й���߾۹�ƣ��������ת����
	if ( light->type == SPOT_LIGHT )
		matrix3D_append_self( source->transform, quaternoin_toMatrix( &quaMtr, quaternoin_setFromEuler( &qua, DEG2RAD( source->direction->y ), DEG2RAD( source->direction->x ), DEG2RAD( source->direction->z ) ) ) );

	//����λ��
	matrix3D_appendTranslation( source->transform, source->position->x, source->position->y, source->position->z );

	matrix3D_copy( source->world, source->transform );
	//���������������λ��
	matrix3D_getPosition( source->worldPosition, source->world );
}

#endif
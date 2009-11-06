#ifndef __LIGHT_H
#define __LIGHT_H

#include <malloc.h>

#include "Entity.h"
#include "Vector3D.h"
#include "ColorValue.h"

#define POINT_LIGHT			0
#define DIRECTIONAL_LIGHT	1
#define SPOT_LIGHT			2

#define LOW_MODE			0
#define MID_MODE			1
#define HIGH_MODE			2

//��ֲ��ӵ�
#define M_LOCAL_VIEWRT		0

typedef struct Light
{
	int mode;				//����ģʽ
	int type;				//����ƹ����ͣ������ܹ�ʹ��������������֮һ��POINT_LIGHT, SPOT_LIGHT, DIRECTIONAL_LIGHT
	int bOnOff;				//�ƹ��Ƿ���

	Color888 * ambient;		//�˹�Դ�����Ļ�������ɫ
	Color888 * diffuse;		//�˹�Դ�������������ɫ
	Color888 * specular;	//�˹�Դ�����ľ������ɫ

	float range;			//�ƹ��ܹ����������Χ
	float falloff;			//���ֵֻ�����ھ۹���ϡ�������ƹ��ڴ���Բ׶����Բ׶֮���ǿ��˥��������ֵͨ������Ϊ1.0f
	float theta;			//ֻ���ھ۹�ƣ�ָ����Բ׶�ĽǶȣ���λ�ǻ���
	float phi;				//ֻ���ھ۹�ƣ�ָ����Բ׶�ĽǶȣ���λ�ǻ���
	float attenuation0, attenuation1, attenuation2;		//��Щ˥����������������ƹ�ǿ�ȵĴ�������˥��
	float spotExp;			//�۹�ָ��
	float spotCutoff;		//�۹ⷢɢ���

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
	light->mode = HIGH_MODE;
	light->type = type;
	light->bOnOff = FALSE;
	light->ambient = newColor888( 0, 0, 0, 255 );
	light->diffuse = newColor888( 255, 255, 255, 255 );
	light->specular = newColor888( 255, 255, 255, 255 );

	light->attenuation0 = 1.0f;
	light->attenuation1 = light->attenuation2 = 0.0f;
	light->spotExp = 1.0f;
	light->spotCutoff = cosf( 180.0f * PI );

	light->source = source;

	return light;
}

void light_dispose( Light * light )
{
	memset( light, 0, sizeof( Light ) );
	
	free( light );
}

INLINE void setLightOnOff( Light * light, int OnOff )
{
	light->bOnOff = OnOff;
}

//Light * newDirectionalLight( Vector3D * direction, ColorValue * color )
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
//Light * newSpotLight( Vector3D * position, Vector3D * direction, ColorValue * color )
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

#endif
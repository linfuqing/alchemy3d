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

//光局部视点
#define M_LOCAL_VIEWRT		0

typedef struct Light
{
	int mode;				//光照模式
	int type;				//定义灯光类型，我们能够使用下面三种类型之一：POINT_LIGHT, SPOT_LIGHT, DIRECTIONAL_LIGHT
	int bOnOff;				//灯光是否开启

	Color888 * ambient;		//此光源发出的环境光颜色
	Color888 * diffuse;		//此光源发出的漫射光颜色
	Color888 * specular;	//此光源发出的镜面光颜色

	float range;			//灯光能够传播的最大范围
	float falloff;			//这个值只能用在聚光灯上。它定义灯光在从内圆锥到外圆锥之间的强度衰减。它的值通常设置为1.0f
	float theta;			//只用于聚光灯；指定内圆锥的角度，单位是弧度
	float phi;				//只用于聚光灯；指定外圆锥的角度，单位是弧度
	float attenuation0, attenuation1, attenuation2;		//这些衰减变量被用来定义灯光强度的传播距离衰减
	float spotExp;			//聚光指数
	float spotCutoff;		//聚光发散半角

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

	//缺省光源为一全向白色点光源
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
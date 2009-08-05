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

//光局部视点
#define M_LOCAL_VIEWRT		0

typedef struct Light
{
	int mode;			//光照模式
	int type;			//定义灯光类型，我们能够使用下面三种类型之一：D3DLIGHT_POINT, D3DLIGHT_SPOT, D3DLIGHT_DIRECTIONAL
	int bOnOff;			//灯光是否开启

	Color * ambient;	//此光源发出的环境光颜色
	Color * diffuse;	//此光源发出的漫射光颜色
	Color * specular;	//此光源发出的镜面光颜色

	float range;		//灯光能够传播的最大范围
	float falloff;		//这个值只能用在聚光灯上。它定义灯光在从内圆锥到外圆锥之间的强度衰减。它的值通常设置为1.0f
	float theta;		//只用于聚光灯；指定内圆锥的角度，单位是弧度
	float phi;			//只用于聚光灯；指定外圆锥的角度，单位是弧度
	float attenuation0, attenuation1, attenuation2;		//这些衰减变量被用来定义灯光强度的传播距离衰减
	float spotExp;		//聚光指数
	float spotCutoff;	//聚光发散半角

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

//更新光源矩阵
void light_updateTransform(Light * light)
{
	Entity * source = light->source;

	matrix3D_identity( source->transform );

	//如果光源是平行光或者聚光灯，则更新旋转矩阵
	if ( light->type == SPOT_LIGHT )
		matrix3D_append_self( source->transform, quaternoin_toMatrix( &quaMtr, quaternoin_setFromEuler( &qua, DEG2RAD( source->direction->y ), DEG2RAD( source->direction->x ), DEG2RAD( source->direction->z ) ) ) );

	//更新位置
	matrix3D_appendTranslation( source->transform, source->position->x, source->position->y, source->position->z );

	matrix3D_copy( source->world, source->transform );
	//从世界矩阵获得世界位置
	matrix3D_getPosition( source->worldPosition, source->world );
}

#endif
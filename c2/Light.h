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
	int type;			//定义灯光类型，我们能够使用下面三种类型之一：D3DLIGHT_POINT, D3DLIGHT_SPOT, D3DLIGHT_DIRECTIONAL

	Color * ambient;		//此光源发出的环境光颜色
	Color * diffuse;		//此光源发出的漫射光颜色
	Color * specular;	//此光源发出的镜面光颜色

	float range;		//灯光能够传播的最大范围
	float falloff;		//这个值只能用在聚光灯上。它定义灯光在从内圆锥到外圆锥之间的强度衰减。它的值通常设置为1.0f
	float theta;		//只用于聚光灯；指定内圆锥的角度，单位是弧度
	float phi;			//只用于聚光灯；指定外圆锥的角度，单位是弧度
	float attenuation0, attenuation1, attenuation2;		//这些衰减变量被用来定义灯光强度的传播距离衰减

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

//更新光源矩阵
void light_updateTransform(Light * light)
{
	Entity * source = light->source;

	matrix3D_identity( source->transform );

	//如果光源是方向光或者聚光灯，则更新旋转矩阵
	if ( light->type == DIRECTIONAL_LIGHT || light->type == SPOT_LIGHT )
		matrix3D_append_self( source->transform, quaternoin_toMatrix( &quaMtr, quaternoin_setFromEuler( &qua, DEG2RAD( source->direction->y ), DEG2RAD( source->direction->x ), DEG2RAD( source->direction->z ) ) ) );

	//如果光源是点光源或者聚光灯，则更新位置
	if ( light->type == POINT_LIGHT || light->type == SPOT_LIGHT )
		matrix3D_appendTranslation( source->transform, source->position->x, source->position->y, source->position->z );

	matrix3D_copy( source->world, source->transform );
	//从世界矩阵获得世界位置
	matrix3D_getPosition( source->worldPosition, source->world );
}

#endif
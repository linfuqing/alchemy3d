#ifndef __MATERIAL_H
#define __MATERIAL_H

#include <malloc.h>

#include "Color.h"

typedef struct Material
{
	Color * ambient;	//指定此表面反射的环境光数量
	Color * diffuse;	//指定此表面反射的漫射光数量
	Color * specular;	//指定此表面反射的镜面光数量
	Color * emissive;	//这个是被用来给表面添加颜色，它使得物体看起来就象是它自己发出的光一样
	float power;		//指定锐利的镜面高光；它的值是高光的锐利值
}Material;

Material * newMaterial( Color * ambient, Color * diffuse, Color * specular, Color * emissive, float power )
{
	Material * m;

	if( ( m = ( Material * )malloc( sizeof( Material ) ) ) == NULL )
	{
		exit( TRUE );
	}

	m->ambient	= ambient;
	m->diffuse	= diffuse;
	m->specular	= specular;
	m->emissive	= emissive;
	m->power	= power;

	return m;
}

#endif
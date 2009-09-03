#ifndef __MATERIAL_H
#define __MATERIAL_H

#include <malloc.h>

#include "FloatColor.h"

typedef struct Material
{
	char * name;
	FloatColor * ambient;	//指定此表面反射的环境光数量
	FloatColor * diffuse;	//指定此表面反射的漫射光数量
	FloatColor * specular;	//指定此表面反射的镜面光数量
	FloatColor * emissive;	//这个是被用来给表面添加颜色，它使得物体看起来就象是它自己发出的光一样
	float power;		//指定锐利的镜面高光；它的值是高光的锐利值
}Material;

Material * newMaterial( FloatColor * ambient, FloatColor * diffuse, FloatColor * specular, FloatColor * emissive, float power )
{
	Material * m;

	if( ( m = ( Material * )malloc( sizeof( Material ) ) ) == NULL )
	{
		exit( TRUE );
	}

	ambient = NULL != ambient ? ambient : newFloatColor( 1.0f, 1.0f, 1.0f, 1.0f );
	diffuse = NULL != diffuse ? diffuse : newFloatColor( 0.6f, 0.6f, 0.6f, 0.6f );
	specular = NULL != specular ? specular : newFloatColor( 0.8f, 0.8f, 0.8f, 0.8f );
	emissive = NULL != emissive ? emissive : newFloatColor( 0.0f, 0.0f, 0.0f, 0.0f );

	m->name		= NULL;
	m->ambient	= ambient;
	m->diffuse	= diffuse;
	m->specular	= specular;
	m->emissive	= emissive;
	m->power	= power;

	return m;
}

void material_dispose( Material * m )
{
	m->name		= NULL;
	m->ambient	= NULL;
	m->diffuse	= NULL;
	m->emissive	= NULL;
	m->specular	= NULL;
	free( m );
	m = NULL;
}

#endif
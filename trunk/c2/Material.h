#ifndef __MATERIAL_H
#define __MATERIAL_H

#include <malloc.h>

#include "ColorValue.h"

typedef struct Material
{
	char * name;
	ColorValue * ambient;	//指定此表面反射的环境光数量
	ColorValue * diffuse;	//指定此表面反射的漫射光数量
	ColorValue * specular;	//指定此表面反射的镜面光数量
	ColorValue * emissive;	//这个是被用来给表面添加颜色，它使得物体看起来就象是它自己发出的光一样
	float power;		//指定锐利的镜面高光；它的值是高光的锐利值
}Material;

typedef struct MaterialList_TYP
{
	Material * material;

	struct MaterialList_TYP * next;

}MaterialList;

Material * newMaterial( ColorValue * ambient, ColorValue * diffuse, ColorValue * specular, ColorValue * emissive, float power )
{
	Material * m;

	if( ( m = ( Material * )malloc( sizeof( Material ) ) ) == NULL )
	{
		exit( TRUE );
	}

	ambient = NULL != ambient ? ambient : newColorValue( 1.0f, 1.0f, 1.0f, 1.0f );
	diffuse = NULL != diffuse ? diffuse : newColorValue( 0.6f, 0.6f, 0.6f, 0.6f );
	specular = NULL != specular ? specular : newColorValue( 0.8f, 0.8f, 0.8f, 0.8f );
	emissive = NULL != emissive ? emissive : newColorValue( 0.0f, 0.0f, 0.0f, 0.0f );

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
	memset( m, 0, sizeof( Material ) );
}

MaterialList * newMaterialList()
{
	MaterialList * materialList;

	if( ( materialList = ( MaterialList * )malloc( sizeof(MaterialList) ) ) == NULL ) exit( TRUE );

	materialList->next = NULL;

	return materialList;
}

void materialList_addMaterial( MaterialList * head, Material * t )
{
	MaterialList	* tl, * ntl;

	tl = head;

	while ( tl->next )
	{
		tl = tl->next;
	}

	if( ( ntl = ( MaterialList * )malloc( sizeof(MaterialList) ) ) == NULL ) exit( TRUE );

	ntl->material = t;
	ntl->next = NULL;
	tl->next = ntl;
}

#endif
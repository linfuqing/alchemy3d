#ifndef __MATERIAL_H
#define __MATERIAL_H

#include <malloc.h>

#include "Color888.h"

typedef struct Material
{
	char * name;
	Color888 * ambient;		//ָ���˱��淴��Ļ���������
	Color888 * diffuse;		//ָ���˱��淴������������
	Color888 * specular;	//ָ���˱��淴��ľ��������
	Color888 * emissive;	//����Ǳ����������������ɫ����ʹ�����忴�������������Լ������Ĺ�һ��
	float power;			//ָ�������ľ���߹⣻����ֵ�Ǹ߹������ֵ
	int doubleSide;
}Material;

typedef struct MaterialList_TYP
{
	Material * material;

	struct MaterialList_TYP * next;

}MaterialList;

Material * newMaterial( Color888 * ambient, Color888 * diffuse, Color888 * specular, Color888 * emissive, float power )
{
	Material * m;

	if( ( m = ( Material * )malloc( sizeof( Material ) ) ) == NULL )
	{
		exit( TRUE );
	}

	ambient = NULL != ambient ? ambient : newColor888( 255, 255, 255, 255 );
	diffuse = NULL != diffuse ? diffuse : newColor888( 153, 153, 153, 255 );
	specular = NULL != specular ? specular : newColor888( 204, 204, 204, 255 );
	emissive = NULL != emissive ? emissive : newColor888( 0, 0, 0, 0 );

	m->name		  = NULL;
	m->ambient	  = ambient;
	m->diffuse	  = diffuse;
	m->specular	  = specular;
	m->emissive	  = emissive;
	m->power	  = power;
	m->doubleSide = FALSE;

	return m;
}

void material_dispose( Material * m )
{
	m->name		= NULL;
	m->ambient	= NULL;
	m->diffuse	= NULL;
	m->emissive	= NULL;
	m->specular	= NULL;
	memset( m, 0, sizeof( Material ) );
	free( m );
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
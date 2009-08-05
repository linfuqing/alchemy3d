#ifndef __MATERIAL_H
#define __MATERIAL_H

#include <malloc.h>

#include "Color.h"

typedef struct Material
{
	Color * ambient;	//ָ���˱��淴��Ļ���������
	Color * diffuse;	//ָ���˱��淴������������
	Color * specular;	//ָ���˱��淴��ľ��������
	Color * emissive;	//����Ǳ����������������ɫ����ʹ�����忴�������������Լ������Ĺ�һ��
	float power;		//ָ�������ľ���߹⣻����ֵ�Ǹ߹������ֵ
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

void material_dispose( Material * m )
{
	m->ambient = NULL;
	m->diffuse = NULL;
	m->emissive = NULL;
	m->specular = NULL;
	free( m );
	m = NULL;
}

#endif
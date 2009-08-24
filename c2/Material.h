#ifndef __MATERIAL_H
#define __MATERIAL_H

#include <malloc.h>

#include "FloatColor.h"

typedef struct Material
{
	FloatColor * ambient;	//ָ���˱��淴��Ļ���������
	FloatColor * diffuse;	//ָ���˱��淴������������
	FloatColor * specular;	//ָ���˱��淴��ľ��������
	FloatColor * emissive;	//����Ǳ����������������ɫ����ʹ�����忴�������������Լ������Ĺ�һ��
	float power;		//ָ�������ľ���߹⣻����ֵ�Ǹ߹������ֵ
}Material;

Material * newMaterial( FloatColor * ambient, FloatColor * diffuse, FloatColor * specular, FloatColor * emissive, float power )
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
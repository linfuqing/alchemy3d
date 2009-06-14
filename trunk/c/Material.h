#ifndef __MATERIAL_H_INCLUDED__ 
#define __MATERIAL_H_INCLUDED__ 

typedef struct
{
	unsigned int color;
	Number       alpha;
}ColorMaterial;

typedef struct
{
	void * bitmap;
	int    smooth;
}BitmapMaterial;

typedef struct
{
	unsigned int color;
	Number       alpha;
	Number   thickness;
}LineMaterial;

#endif
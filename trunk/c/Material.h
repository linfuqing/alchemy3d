#ifndef MATERIAL_H
#define MATERIAL_H

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
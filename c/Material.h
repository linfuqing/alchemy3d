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
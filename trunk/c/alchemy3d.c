#include <math.h>
#include <AS3.h>

//位图缓冲区
int* gfxBuffer;

//深度缓冲区
double* zBuffer;

//深度/位图缓冲区长度
int nZBuffer;

//摄像机缓冲区
double* cameraBuffer;

//摄像机缓冲区长度
int nCameraBuffer;

///几何体数据缓冲区
double* geomBuffer;

//变换后顶点数据缓冲区
double* tGeomBuffer;

//几何体数据长度
int nGeomBuffer;

//材质缓冲区
unsigned int* matBuffer;

//材质缓冲区长度
int nMatBuffer;

//矩阵/几何体数量
int nMatrix;

int resX;
int resY;

double zoom;
double focus;
double fz;
double nearClip;
double farClip;

double gfxOffsetX;
double gfxOffsetY;

//初始化位图缓冲区
AS3_Val initializeGfx( void* self, AS3_Val args )
{
	AS3_ArrayValue(args, "IntType, IntType", &resX, &resY);

	nZBuffer = resX * resY;

	gfxBuffer = (int*)malloc( resX * resY * sizeof(int) );
	zBuffer = (double*)malloc( nZBuffer * sizeof(double) );

	gfxOffsetX = resX * 0.5;
	gfxOffsetY = resY * 0.5;

	return 0;
}

//初始化摄像机设置
AS3_Val initializeCamera( void* self, AS3_Val args )
{
	AS3_ArrayValue(args, "DoubleType, DoubleType, DoubleType, DoubleType, IntType", &zoom, &focus, &nearClip, &farClip, &nCameraBuffer);

	fz = zoom * focus;

	cameraBuffer = (double*)malloc(nCameraBuffer);

	return 0;
}

//初始化数据缓冲区
AS3_Val initializeGeom( void* self, AS3_Val args )
{
	AS3_ArrayValue(args, "IntType, IntType, IntType, IntType", &nMatrix, &nGeomBuffer);

	geomBuffer = (double*)malloc(nGeomBuffer);
	tGeomBuffer = (double*)malloc(nGeomBuffer);

	return 0;
}

//初始化材质缓冲区
AS3_Val initializeMaterials( void* self, AS3_Val args )
{
	AS3_ArrayValue(args, "IntType", &nMatBuffer);

	matBuffer = (unsigned int*)malloc( nMatBuffer * sizeof(unsigned int) );

	return 0;
}

//返回摄像机缓冲区起始偏移量
AS3_Val getCameraBufferPointer( void* self, AS3_Val args )
{
	return AS3_Ptr( cameraBuffer );
}

//返回位图缓冲区起始偏移量
AS3_Val getGfxBufferPointer( void* self, AS3_Val args )
{
	return AS3_Ptr( gfxBuffer );
}

//返回数据缓冲区起始偏移量
AS3_Val getGeomBufferPointer( void* self, AS3_Val args )
{
	return AS3_Ptr( geomBuffer );
}

//返回材质缓冲区起始偏移量
AS3_Val getMatBufferPointer( void* self, AS3_Val args )
{
	return AS3_Ptr( matBuffer );
}

//入口
int main()
{
	AS3_Val initializeCameraMethod = AS3_Function( NULL, initializeCamera );
	AS3_Val initializeGfxMethod = AS3_Function( NULL, initializeGfx );
	AS3_Val initializeGeomMethod = AS3_Function( NULL, initializeGeom );
	AS3_Val initializeMaterialsMethod = AS3_Function( NULL, initializeMaterials );

	AS3_Val getCameraBufferPointerMethod = AS3_Function( NULL, getCameraBufferPointer );
	AS3_Val getGfxBufferPointerMethod = AS3_Function( NULL, getGfxBufferPointer );
	AS3_Val getGeomBufferPointerMethod = AS3_Function( NULL, getGeomBufferPointer );
	AS3_Val getMatBufferPointerMethod = AS3_Function( NULL, getMatBufferPointer );



	AS3_Val result = AS3_Object( "initializeCamera:AS3ValType, initializeGfx:AS3ValType, initializeGeom:AS3ValType, initializeMaterials:AS3ValType,  getCameraBufferPointer:AS3ValType, getGeomBufferPointer:AS3ValType, getGfxBufferPointer:AS3ValType, getMatBufferPointer:AS3ValType",
								initializeCameraMethod, initializeGfxMethod, initializeGeomMethod, initializeMaterialsMethod, getCameraBufferPointerMethod, getGeomBufferPointerMethod, getGfxBufferPointerMethod, getMatBufferPointerMethod );



	AS3_Release( initializeCameraMethod );
	AS3_Release( initializeGfxMethod );
	AS3_Release( initializeGeomMethod );
	AS3_Release( initializeMaterialsMethod );
	
	AS3_Release( getCameraBufferPointerMethod );
	AS3_Release( getGfxBufferPointerMethod );
	AS3_Release( getGeomBufferPointerMethod );
	AS3_Release( getMatBufferPointerMethod );


	AS3_LibInit( result );

	return 0;
}
#include <stdlib.h>
#include <AS3.h>

#include "Vector3D.h"

//初始化位图缓冲区
AS3_Val initializeScene( void* self, AS3_Val args )
{
	Vector3D v = newVector3D(100, 200, 300, 1);

	Vector3D *ptrV = &v;

	return AS3_Ptr(ptrV);
}

//初始化摄像机设置
AS3_Val initializeCamera( void* self, AS3_Val args )
{
	int type;
	double zoom;
	double focus;
	double nearClip;
	double farClip;

	AS3_ArrayValue(args, "IntType, DoubleType, DoubleType, DoubleType, DoubleType", &type, &zoom, &focus, &nearClip, &farClip);

	double fz = zoom * focus;

	return 0;
}

//创建几何实体
AS3_Val createEntity( void* self, AS3_Val args )
{
	return 0;
}

//测试函数
AS3_Val test( void* self, AS3_Val args )
{
	Vector3D *ptr;

	AS3_ArrayValue(args, "PtrType", &ptr);

	AS3_Trace(AS3_Int(ptr->x));

	return 0;
}

//入口
int main()
{
	AS3_Val initializeSceneMethod = AS3_Function( NULL, initializeScene );
	AS3_Val initializeCameraMethod = AS3_Function( NULL, initializeCamera );
	AS3_Val createEntityMethod = AS3_Function( NULL, createEntity );
	AS3_Val testMethod = AS3_Function( NULL, test );



	AS3_Val result = AS3_Object( "initializeScene:AS3ValType, initializeCamera:AS3ValType, createEntity:AS3ValType, test:AS3ValType",
								initializeSceneMethod, initializeCameraMethod, createEntityMethod, testMethod );



	AS3_Release( initializeSceneMethod );
	AS3_Release( initializeCameraMethod );
	AS3_Release( createEntityMethod );
	AS3_Release( testMethod );


	AS3_LibInit( result );

	return 0;
}
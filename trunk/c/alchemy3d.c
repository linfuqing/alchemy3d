#include <stdlib.h>
#include <AS3.h>

#include "Base.h"
#include "DisplayObject3D.h"

DisplayObject3D do3d;

//初始化场景
//返回该对象的起始指针
AS3_Val initializeScene( void* self, AS3_Val args )
{
	return 0;
}

//初始化摄像机设置
//返回该对象的起始指针
AS3_Val initializeCamera( void* self, AS3_Val args )
{
	/*int type;
	double zoom;
	double focus;
	double nearClip;
	double farClip;

	AS3_ArrayValue(args, "IntType, DoubleType, DoubleType, DoubleType, DoubleType", &type, &zoom, &focus, &nearClip, &farClip);

	double fz = zoom * focus;*/

	return 0;
}

//创建几何实体
//返回该对象的起始指针
AS3_Val createEntity( void* self, AS3_Val args )
{
	 do3d = newDisplayObject3D();

	return AS3_Array("PtrType, PtrType, PtrType, PtrType", &do3d, &(do3d.position), &(do3d.direction), &(do3d.scale));
}

//测试函数
AS3_Val test( void* self, AS3_Val args )
{
	do3d_updateTransform(&do3d);

	AS3_Trace(AS3_Number(do3d.position.x));
	AS3_Trace(AS3_Number(do3d.position.y));
	AS3_Trace(AS3_Number(do3d.position.z));
	AS3_Trace(AS3_Number(do3d.scale.x));
	AS3_Trace(AS3_Number(do3d.scale.y));
	AS3_Trace(AS3_Number(do3d.scale.z));
	AS3_Trace(AS3_Number(do3d.transform.m11));
	AS3_Trace(AS3_Number(do3d.transform.m12));
	AS3_Trace(AS3_Number(do3d.transform.m13));
	AS3_Trace(AS3_Number(do3d.transform.m14));
	AS3_Trace(AS3_Number(do3d.transform.m21));
	AS3_Trace(AS3_Number(do3d.transform.m22));
	AS3_Trace(AS3_Number(do3d.transform.m23));
	AS3_Trace(AS3_Number(do3d.transform.m24));
	AS3_Trace(AS3_Number(do3d.transform.m31));
	AS3_Trace(AS3_Number(do3d.transform.m32));
	AS3_Trace(AS3_Number(do3d.transform.m33));
	AS3_Trace(AS3_Number(do3d.transform.m34));
	AS3_Trace(AS3_Number(do3d.transform.m41));
	AS3_Trace(AS3_Number(do3d.transform.m42));
	AS3_Trace(AS3_Number(do3d.transform.m43));
	AS3_Trace(AS3_Number(do3d.transform.m44));

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
#include <stdlib.h>
#include <AS3.h>

//��ʼ������
//���ظö������ʼָ��
AS3_Val initializeScene( void* self, AS3_Val args )
{
	return 0;
}

//��ʼ�����������
//���ظö������ʼָ��
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

//��������ʵ��
//���ظö������ʼָ��
AS3_Val createEntity( void* self, AS3_Val args )
{
	return 0;
}

//���Ժ���
AS3_Val test( void* self, AS3_Val args )
{
	int l;

	AS3_Val as3object;

	AS3_ArrayValue(args, "AS3ValType, IntType", &as3object, &l);
	
	int i = 0;
	for (; i < l; i ++)
	{
		AS3_CallTS("beginFill", as3object, "IntType, IntType", 0, 1);
		AS3_CallTS("drawRect", as3object, "DoubleType, DoubleType, DoubleType, DoubleType", 0.0, 0.0, 100.0, 100.0);
		AS3_CallTS("endFill", as3object, "");
	}

	AS3_Release(as3object);

	return 0;
}

//���
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
#include <stdlib.h>
#include <AS3.h>

#include "Render.h"

AS3_Val initializeVertex( void * self, AS3_Val args )
{
	Vertex * v;

	AS3_ArrayValue( args, "PtrType", v );

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", & ( v -> position -> x ), & ( v -> position -> y ), & ( v -> position -> z ), & ( v -> worldPosition -> x ), & ( v -> worldPosition -> y ), & ( v -> worldPosition -> z ) ); 
}

AS3_Val initializeVertices( void * self, AS3_Val args )
{
	Vertices * vs;

	Vertex   * v;

	Vector3D * v3d, * vp;

	Number   * vertices;

	int      length, i, vi;

	AS3_ArrayValue( args, "PtrType, IntType", vertices, & length );

	if( ( vs  = ( Vertices * )malloc( sizeof( Vertices ) * length + 1 ) ) == NULL
	 || ( v   = ( Vertex   * )malloc( sizeof( Vertex   ) * length     ) ) == NULL
	 || ( v3d = ( Vector3D * )malloc( sizeof( Vector3D ) * length * 2 ) ) == NULL )
	{
		exit( TRUE );
	}

	for( i = 1; i <= length; i ++ )
	{
		vi                                = i - 1;

		vp                                = v3d + 2 * i - 1;
		vp -> x                           = vertices[i];
		vp -> y                           = vertices[i + 1];
		vp -> z                           = vertices[i + 2];
		vp -> w                           = 1;

		* ( vp + 1 )                      = * vp;

		
		  ( v  + vi ) -> position      = vp;
		  ( v  + vi ) -> worldPosition = vp  + 1;
		  ( vs + i  ) -> vertex        = v   + vi;
		  ( vs + vi ) -> next          = vs  + i;
	}

	free( vertices );

	return AS3_Array( "PtrType, PtrType",vs, v );
}

AS3_Val initializeFaces( void * self, AS3_Val args )
{
}

//初始化场景
//返回该对象的起始指针
AS3_Val initializeViewport( void * self, AS3_Val args )
{
	Viewport * view;
	Scene    * s;

	Number left, right, top, bottom, far, near;

	AS3_ArrayValue( args, "DoubleType, DoubleType, DoubleType, DoubleType, DoubleType, DoubleType, PtrType", &left, &right, &top, &bottom, &far, &near, &s );

	view = newViewport( left, right, top, bottom, far, near, s );

	return AS3_Array( "PtrType", view );
}

//渲染
AS3_Val beginRender( void * self, AS3_Val args )
{
	RenderEngine * r;

	AS3_ArrayValue( args, "PtrType", &r );

	render( r, RENDER_MODE_DYNAMIC );

	return 0;
}

AS3_Val applyForTmpBuffer( void * self, AS3_Val args )
{
	int size;

	void * tmpBuffer;

	AS3_ArrayValue( args, "IntType", &size );

	if( ( tmpBuffer = malloc( size ) ) == NULL )
	{
		exit( TRUE );
	}

	return AS3_Ptr( tmpBuffer );
}

AS3_Val doubleTypeSize( void * self, AS3_Val args )
{
	return AS3_Int( sizeof( Double ) );
}

AS3_Val intTypeSize( void * self, AS3_Val args )
{
	return AS3_Int( sizeof( int ) );
}

//测试函数
AS3_Val test( void* self, AS3_Val args )
{
	/*entity_updateTransform(&entity);

	AS3_Trace(AS3_Number(entity.position.x));
	AS3_Trace(AS3_Number(entity.position.y));*/

	return 0;
}

//入口
int main()
{
	AS3_Val initializeDeviceMethod = AS3_Function( NULL, initializeDevice );
	AS3_Val initializeCameraMethod = AS3_Function( NULL, initializeCamera );
	AS3_Val initializeEngineMethod = AS3_Function( NULL, initializeEngine );
	AS3_Val initializeViewportMethod = AS3_Function( NULL, initializeViewport );
	AS3_Val initializeMaterialMethod = AS3_Function( NULL, initializeMaterial );
	AS3_Val initializeTextureMethod = AS3_Function( NULL, initializeTexture );
	AS3_Val initializeLightMethod = AS3_Function( NULL, initializeLight );
	AS3_Val initializeEntityMethod = AS3_Function( NULL, initializeEntity );
	AS3_Val applyForTmpBufferMethod = AS3_Function( NULL, applyForTmpBuffer );
	AS3_Val beginRenderMethod = AS3_Function( NULL, beginRender );
	AS3_Val testMethod = AS3_Function( NULL, test );



	AS3_Val result = AS3_Object( "initializeDevice:AS3ValType, initializeCamera:AS3ValType, initializeEngine:AS3ValType, initializeViewport:AS3ValType, initializeMaterial:AS3ValType, initializeTexture:AS3ValType, initializeLight:AS3ValType, initializeEntity:AS3ValType, applyForTmpBuffer:AS3ValType, beginRender:AS3ValType, test:AS3ValType",
		initializeDeviceMethod, initializeCameraMethod, initializeEngineMethod, initializeViewportMethod, initializeMaterialMethod, initializeTextureMethod, initializeLightMethod, initializeEntityMethod, applyForTmpBufferMethod, beginRenderMethod, testMethod );



	AS3_Release( initializeDeviceMethod );
	AS3_Release( initializeCameraMethod );
	AS3_Release( initializeEngineMethod );
	AS3_Release( initializeViewportMethod );
	AS3_Release( initializeMaterialMethod );
	AS3_Release( initializeTextureMethod );
	AS3_Release( initializeLightMethod );
	AS3_Release( initializeEntityMethod );
	AS3_Release( applyForTmpBufferMethod );
	AS3_Release( beginRenderMethod );
	AS3_Release( testMethod );


	AS3_LibInit( result );

	return 0;
}
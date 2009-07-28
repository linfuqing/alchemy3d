#include <stdlib.h>
#include <AS3.h>

#include "Render.h"


AS3_Val initializeDevice( void* self, AS3_Val args )
{
	return 0;
}

AS3_Val initializeCamera( void* self, AS3_Val args )
{
	Camera * camera;

	camera = newCamera( NULL, NULL, NULL );

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType", camera, camera->position, camera->direction, camera->scale );
}

AS3_Val initializeEngine( void* self, AS3_Val args )
{
	Viewport * view;
	RenderEngine * r;
	Number width, height;

	AS3_ArrayValue( args, "DoubleType, DoubleType, PtrType", &width, &height, &view );

	r    = newRenderEngine( 800, 600 );

	renderEngine_addViewport( r, view );

	return AS3_Array( "PtrType", r );
}

//初始化场景
//返回该对象的起始指针
AS3_Val initializeViewport( void* self, AS3_Val args )
{
	Viewport * view;
	Scene    * s;

	Number left, right, top, bottom, far, near;

	AS3_ArrayValue( args, "DoubleType, DoubleType, DoubleType, DoubleType, DoubleType, DoubleType, PtrType", &left, &right, &top, &bottom, &far, &near, &s );

	view = newViewport( left, right, top, bottom, far, near, s );

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", view, &view->left, &view->right, &view->top, &view->bottom, &view->far, &view->near, &view->scene );
}

AS3_Val initializeMaterial( void* self, AS3_Val args )
{
	return 0;
}

AS3_Val initializeTexture( void* self, AS3_Val args )
{
	return 0;
}

//初始化光源
//返回该对象起始指针
AS3_Val initializeLight( void* self, AS3_Val args )
{
	return 0;
}

//创建几何实体
//返回该对象的起始指针
AS3_Val initializeEntity( void* self, AS3_Val args )
{
	Number * tmpBuff;
	Polygon * p;
	Faces    * f;
	Vertices * v;
	Mesh     * m;
	Scene    * s;

	int vNum, fNum, vLen, i, j, k;

	AS3_ArrayValue( args, "PtrType, IntType, IntType", &tmpBuff, &vNum, &fNum );

	if (vNum != 0 && fNum != 0)
	{
		faces_initiate( & f );
		vertices_initiate( & v );

		vLen = vNum * VERTEX_SIZE;

		i = 0;
		j = 0;
		k = vLen;

		for ( ; i < vNum; i ++, k, j+= VERTEX_SIZE * 3 )
		{
			faces_push( f, ( p = newTriangle3D(	newVertex( newVector3D( tmpBuff[j], tmpBuff[j+1], tmpBuff[j+2], tmpBuff[j+3] ) ),
												newVertex( newVector3D( tmpBuff[j+4], tmpBuff[j+5], tmpBuff[j+6], tmpBuff[j+7] ) ),
												newVertex( newVector3D( tmpBuff[j+8], tmpBuff[j+9], tmpBuff[j+10], tmpBuff[j+11] ) ),
												newVector( tmpBuff[k + 3 + FACE_SIZE * i], tmpBuff[k + 4 + FACE_SIZE * i] ),
												newVector( tmpBuff[k + 5 + FACE_SIZE * i], tmpBuff[k + 6 + FACE_SIZE * i] ),
												newVector( tmpBuff[k + 7 + FACE_SIZE * i], tmpBuff[k + 8 + FACE_SIZE * i] ) ) ) );
		}

		vertices_push( v, p -> next -> vertex );
		vertices_push( v, p -> next -> next -> vertex );
		vertices_push( v, p -> next -> next -> next -> vertex );

		m = newMesh( f, v );

		s    = newScene( newMesh( f, v ) );

		free( tmpBuff );
	}

	tmpBuff = NULL;

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType", s, &s->visible, s->mesh->vertices, s->mesh->faces );
}

AS3_Val applyForTmpBuffer( void* self, AS3_Val args )
{
	int len, size;

	void * tmpBuff;

	AS3_ArrayValue( args, "IntType, IntType", &size, &len );

	if( ( tmpBuff = calloc( len, size ) ) == NULL )
	{
		exit( TRUE );
	}

	return AS3_Ptr( tmpBuff );
}

//渲染
AS3_Val beginRender( void* self, AS3_Val args )
{
	RenderEngine * r;

	AS3_ArrayValue( args, "PtrType", &r );

	render( r, RENDER_MODE_DYNAMIC );

	return 0;
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
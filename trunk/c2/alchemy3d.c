#include <stdlib.h>
#include <AS3.h>

#include "Base.h"
#include "Device.h"
#include "Screen.h"
#include "Entity.h"
#include "Color.h"
#include "Material.h"
#include "Light.h"
#include "Scene.h"
#include "Viewport.h"
#include "Polygon.h"
#include "Vertices.h"
#include "Faces.h"
#include "Mesh.h"
#include "Entity.h"
#include "Vector3D.h"
#include "Render.h"


AS3_Val initializeDevice( void* self, AS3_Val args )
{
	Device * device;

	device = newDevice();

	return AS3_Ptr( device );
}

AS3_Val initializeCamera( void* self, AS3_Val args )
{
	Device * device;

	Camera * camera;

	Entity * eye;

	double fov, nearClip, farClip;

	AS3_ArrayValue( args, "PtrType, PtrType, DoubleType, DoubleType, DoubleType", & device, & eye, & fov, & nearClip, & farClip );

	camera = newCamera( (float)fov, (float)nearClip, (float)farClip, eye );

	device_addCamera( device, camera );

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType",
					camera, camera->target, & camera->fov, & camera->nearClip, & camera->farClip, & camera->fnfDirty, & camera->hasTarget );
}

AS3_Val initializeScene( void* self, AS3_Val args )
{
	Device * device;

	Scene * scene;

	AS3_ArrayValue( args, "PtrType", &device );

	scene =  newScene();

	device_addScene(device, scene);

	return AS3_Ptr( scene );
}

//初始化场景
//返回该对象的起始指针
AS3_Val initializeViewport( void* self, AS3_Val args )
{
	Device * device;
	Viewport * view;
	Scene * scene;
	Camera * camera;

	double width, height;

	AS3_ArrayValue( args, "PtrType, DoubleType, DoubleType, PtrType, PtrType", &device, &width, &height, &scene, &camera );

	view = newViewport( (float)width, (float)height, scene, camera );

	device_addViewport(device, view);

	return AS3_Array( "PtrType, PtrType", view, view->gfxBuffer );
}

AS3_Val initializeMaterial( void* self, AS3_Val args )
{
	Material * material;

	material = newMaterial( newColor( 0.0f, 0.0f, 0.0f, 1.0f ),
							newColor( 0.0f, 0.0f, 0.0f, 1.0f ),
							newColor( 0.0f, 0.0f, 0.0f, 1.0f ),
							newColor( 0.0f, 0.0f, 0.0f, 1.0f ),
							0.0f );

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", material, material->ambient, material->diffuse, material->specular, material->emissive, &material->power );
}

//初始化光源
//返回该对象起始指针
AS3_Val initializeLight( void* self, AS3_Val args )
{
	Scene * scene;
	Entity * source;
	int type;
	Light * light;

	AS3_ArrayValue( args, "PtrType, PtrType, IntType", &scene, &source, &type );

	light = newPointLight( type, source, newColor( 1.0f, 1.0f, 1.0f, 1.0f ) );
	scene_addLight( scene, light );

	LIGHT_ENABLE = 1;

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType",
		light, light->ambient, light->diffuse, light->specular,
		&(light->range), &(light->falloff), &(light->theta), &(light->phi), &(light->attenuation0), &(light->attenuation1), &(light->attenuation2) );
}

//创建几何实体
//返回该对象的起始指针
AS3_Val initializeEntity( void* self, AS3_Val args )
{
	Scene * scene;
	Entity * entity, * parent;
	Faces * faces;
	Vertices * vertices;
	Material * material;
	float * tmpBuff;

	int vNum, fNum, vLen, i, j;

	AS3_ArrayValue( args, "PtrType, PtrType, PtrType, PtrType, IntType, IntType", &scene, &parent, &material, &tmpBuff, &vNum, &fNum );

	Vertex * vArr[vNum];

	entity = newEntity();

	if (vNum != 0 && fNum != 0)
	{
		vLen = vNum * VERTEX_SIZE;

		i = 0;
		j = 0;

		vertices = newVertices();
		faces = newFaces();

		for ( ; i < vNum; i ++, j += VERTEX_SIZE )
			vertices_push( vertices, ( vArr[i] = newVertex( tmpBuff[j], tmpBuff[j + 1], tmpBuff[j + 2] ) ) );

		for ( i = 0, j = vLen; i < fNum; i ++, j += FACE_SIZE )
			faces_push(faces, newTriangle3D( vArr[(int)tmpBuff[j]], vArr[(int)tmpBuff[j+1]], vArr[(int)tmpBuff[j+2]], newVector(tmpBuff[j + 3], tmpBuff[j + 4]), newVector(tmpBuff[j + 5], tmpBuff[j + 6]), newVector(tmpBuff[j + 7], tmpBuff[j + 8]) ));

		entity_setMesh( entity, newMesh( faces, vertices ) );

		free( tmpBuff );
	}

	tmpBuff = NULL;

	if ( parent == FALSE ) parent = NULL;

	if ( material != FALSE ) entity_setMaterial( entity, material );

	if ( scene != FALSE ) scene_addEntity(scene, entity, parent);

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", entity, &entity->material, entity->position, entity->direction, entity->scale, entity->worldPosition, entity->material, entity->texture, entity->mesh->vertices, entity->mesh->faces );
}

AS3_Val applyForTmpBuffer( void* self, AS3_Val args )
{
	int len = 0, size = 0;

	float * tmpBuff;

	AS3_ArrayValue( args, "IntType", &len );

	size = sizeof( float ) * len;

	if( ( tmpBuff = ( float * )malloc( size ) ) == NULL )
	{
		exit( TRUE );
	}

	memset( tmpBuff, 10.0f, size );

	return AS3_Ptr( tmpBuff );
}

//渲染
AS3_Val render( void* self, AS3_Val args )
{
	Device * device;

	AS3_ArrayValue( args, "PtrType", &device );

	device_render(device);

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
	AS3_Val initializeSceneMethod = AS3_Function( NULL, initializeScene );
	AS3_Val initializeViewportMethod = AS3_Function( NULL, initializeViewport );
	AS3_Val initializeMaterialMethod = AS3_Function( NULL, initializeMaterial );
	AS3_Val initializeLightMethod = AS3_Function( NULL, initializeLight );
	AS3_Val initializeEntityMethod = AS3_Function( NULL, initializeEntity );
	AS3_Val applyForTmpBufferMethod = AS3_Function( NULL, applyForTmpBuffer );
	AS3_Val renderMethod = AS3_Function( NULL, render );
	AS3_Val testMethod = AS3_Function( NULL, test );



	AS3_Val result = AS3_Object( "initializeDevice:AS3ValType, initializeCamera:AS3ValType, initializeScene:AS3ValType, initializeViewport:AS3ValType, initializeMaterial:AS3ValType, initializeLight:AS3ValType, initializeEntity:AS3ValType, applyForTmpBuffer:AS3ValType, render:AS3ValType, test:AS3ValType",
		initializeDeviceMethod, initializeCameraMethod, initializeSceneMethod, initializeViewportMethod, initializeMaterialMethod, initializeLightMethod, initializeEntityMethod, applyForTmpBufferMethod, renderMethod, testMethod );



	AS3_Release( initializeDeviceMethod );
	AS3_Release( initializeCameraMethod );
	AS3_Release( initializeSceneMethod );
	AS3_Release( initializeViewportMethod );
	AS3_Release( initializeMaterialMethod );
	AS3_Release( initializeLightMethod );
	AS3_Release( initializeEntityMethod );
	AS3_Release( applyForTmpBufferMethod );
	AS3_Release( renderMethod );
	AS3_Release( testMethod );


	AS3_LibInit( result );

	return 0;
}
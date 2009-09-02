#define INLINE inline
#define __AS3__

#include <stdlib.h>
#include <AS3.h>

#include "Base.h"
#include "Math.h"
#include "Device.h"
#include "Entity.h"
#include "Material.h"
#include "Texture.h"
#include "Light.h"
#include "Scene.h"
#include "Viewport.h"
#include "Triangle.h"
#include "Mesh.h"
#include "3DSLoader.h"
#include "Md2.h"

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
					camera, camera->target, & camera->fov, & camera->near, & camera->far, & camera->fnfDirty, & camera->hasTarget );
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

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", view, view->videoBuffer, & view->camera, & view->scene, & view->nRenderList, & view->nCullList, & view->nClippList );
}

AS3_Val initializeMaterial( void* self, AS3_Val args )
{
	Material * material;

	AS3_Val ambient, diffuse, specular, emissive;

	double a_a, a_r, a_g, a_b, d_a, d_r, d_g, d_b, s_a, s_r, s_g, s_b, e_a, e_r, e_g, e_b, power;

	AS3_ArrayValue( args, "AS3ValType, AS3ValType, AS3ValType, AS3ValType, DoubleType", &ambient, &diffuse, &specular, &emissive, &power );

	a_r = AS3_NumberValue( AS3_GetS( ambient, "redMultiplier" ) );
	a_g = AS3_NumberValue( AS3_GetS( ambient, "greenMultiplier" ) );
	a_b = AS3_NumberValue( AS3_GetS( ambient, "blueMultiplier" ) );
	a_a = AS3_NumberValue( AS3_GetS( ambient, "alphaMultiplier" ) );

	d_r = AS3_NumberValue( AS3_GetS( diffuse, "redMultiplier" ) );
	d_g = AS3_NumberValue( AS3_GetS( diffuse, "greenMultiplier" ) );
	d_b = AS3_NumberValue( AS3_GetS( diffuse, "blueMultiplier" ) );
	d_a = AS3_NumberValue( AS3_GetS( diffuse, "alphaMultiplier" ) );

	s_r = AS3_NumberValue( AS3_GetS( specular, "redMultiplier" ) );
	s_g = AS3_NumberValue( AS3_GetS( specular, "greenMultiplier" ) );
	s_b = AS3_NumberValue( AS3_GetS( specular, "blueMultiplier" ) );
	s_a = AS3_NumberValue( AS3_GetS( specular, "alphaMultiplier" ) );

	e_r = AS3_NumberValue( AS3_GetS( emissive, "redMultiplier" ) );
	e_g = AS3_NumberValue( AS3_GetS( emissive, "greenMultiplier" ) );
	e_b = AS3_NumberValue( AS3_GetS( emissive, "blueMultiplier" ) );
	e_a = AS3_NumberValue( AS3_GetS( emissive, "alphaMultiplier" ) );

	material = newMaterial( newFloatColor( (float)a_r, (float)a_g, (float)a_b, (float)a_a ),
							newFloatColor( (float)d_r, (float)d_g, (float)d_b, (float)d_a ),
							newFloatColor( (float)s_r, (float)s_g, (float)s_b, (float)s_a ),
							newFloatColor( (float)e_r, (float)e_g, (float)e_b, (float)e_a ),
							power );

	AS3_Release( ambient );
	AS3_Release( diffuse );
	AS3_Release( specular );
	AS3_Release( emissive );

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", material, material->ambient, material->diffuse, material->specular, material->emissive, &material->power );
}

AS3_Val initializeTexture( void* self, AS3_Val args )
{
	Texture * texture;
	int width, height;
	LPBYTE pRGBABuffer;

	AS3_ArrayValue( args, "IntType, IntType, IntType", &width, &height, &pRGBABuffer );

	texture = newTexture( width, height, pRGBABuffer );

	return AS3_Array( "PtrType", texture );
}

AS3_Val initializeLight( void* self, AS3_Val args )
{
	Scene * scene;
	Entity * source;
	int type;
	Light * light;

	AS3_ArrayValue( args, "PtrType, PtrType, IntType", &scene, &source, &type );

	light = newPointLight( type, source );
	scene_addLight( scene, light );

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType",
		light, &light->mode, &light->bOnOff, &light->type, light->ambient, light->diffuse, light->specular,
		&light->range, &light->falloff, &light->theta, &light->phi, &light->attenuation0, &light->attenuation1, &light->attenuation2 );
}

AS3_Val initializeEntity( void* self, AS3_Val args )
{
	Entity * entity = NULL, * parent = NULL;
	Mesh * mesh = NULL;
	Texture * texture = NULL;
	Material * material = NULL;
	float * meshBuffer = NULL;
	char * name = NULL;
	DWORD ** p_meshBuffer = NULL, *** pp_meshBuffer = NULL;

	int vNum, fNum, vLen, i, j;

	AS3_ArrayValue( args, "PtrType, PtrType, StrType, PtrType, IntType, IntType", &material, &texture, &name, &meshBuffer, &vNum, &fNum );

	p_meshBuffer = ( DWORD ** )meshBuffer;
	pp_meshBuffer = ( DWORD *** )meshBuffer;

	entity = newEntity();

	* entity->name = * name;
	entity->parent = parent;

	free( name );
	name = NULL;

	if ( vNum != 0 && fNum != 0 && meshBuffer!= 0 )
	{
		mesh = newMesh( vNum, fNum, meshBuffer );

		vLen = vNum * VERTEX_SIZE;

		i = 0;
		j = 0;

		for ( ; i < vNum; i ++, j += VERTEX_SIZE )
		{
			p_meshBuffer[j + 4] = ( DWORD * )mesh_push_vertex( mesh, meshBuffer[j + 0], meshBuffer[j + 1], meshBuffer[j + 2] );
		}

		for ( i = 0, j = vLen; i < fNum; i ++, j += FACE_SIZE )
		{
			mesh_push_triangle( mesh,
								(Vertex * )( * p_meshBuffer[j] ),
								(Vertex * )( * p_meshBuffer[j + 1] ),
								(Vertex * )( * p_meshBuffer[j + 2] ),
								newVector( meshBuffer[j + 3], meshBuffer[j + 4] ),
								newVector( meshBuffer[j + 5], meshBuffer[j + 6] ),
								newVector( meshBuffer[j + 7], meshBuffer[j + 8] ),
								NULL );
		}

		if ( material ) mesh_setMaterial( mesh, material );

		if ( texture ) mesh_setTexture( mesh, texture );

		entity_setMesh( entity, mesh );
	}

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType",
		entity, entity->position, entity->direction, entity->scale, entity->w_pos, & entity->lightEnable, & entity->mesh->render_mode, & entity->mesh->dirty, & entity->mesh->material, & entity->mesh->texture );
}

AS3_Val addEntity( void* self, AS3_Val args )
{
	Scene * scene = NULL;

	Entity * entity = NULL, * parent = NULL;

	AS3_ArrayValue( args, "PtrType, PtrType, PtrType", &scene, &entity, &parent );

	//如果有父亲，加入父亲的孩子列表
	if ( parent ) entity_addChild( parent, entity );

	//加入场景列表
	if ( parent ) scene_addEntity( scene, entity, parent );
	else scene_addEntity( scene, entity, NULL );

	return 0;
}

AS3_Val initialize3DS( void* self, AS3_Val args )
{
	Entity * entity = NULL;

	UCHAR * buffer = NULL;

	UINT length = 0;

	AS3_ArrayValue( args, "PtrType, PtrType, IntType", &entity, & buffer, & length );

	A3DS_LoadData( entity, & buffer, length );

	return 0;
}

AS3_Val initializeMD2( void* self, AS3_Val args )
{
	MD2 * md2;
	Entity * entity;
	UCHAR * buffer;

	AS3_ArrayValue( args, "PtrType, PtrType", & entity, & buffer );

	md2 = newMD2( entity );

	md2_read( & buffer, md2 );

	return 0;
}

AS3_Val applyForTmpBuffer( void* self, AS3_Val args )
{
	int len;

	void * tmpBuff;

	AS3_ArrayValue( args, "IntType", &len );

	if( ( tmpBuff = calloc( len, 1 ) ) == NULL )
	{
		exit( TRUE );
	}

	return AS3_Ptr( tmpBuff );
}

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
	AS3_Val initializeTextureMethod = AS3_Function( NULL, initializeTexture );
	AS3_Val initializeLightMethod = AS3_Function( NULL, initializeLight );
	AS3_Val initializeEntityMethod = AS3_Function( NULL, initializeEntity );
	AS3_Val addEntityMethod = AS3_Function( NULL, addEntity );
	AS3_Val initialize3DSMethod = AS3_Function( NULL, initialize3DS );
	AS3_Val applyForTmpBufferMethod = AS3_Function( NULL, applyForTmpBuffer );
	AS3_Val renderMethod = AS3_Function( NULL, render );
	AS3_Val initializeMD2Method = AS3_Function( NULL, initializeMD2 );
	AS3_Val testMethod = AS3_Function( NULL, test );



	AS3_Val result = AS3_Object( "initializeDevice:AS3ValType, initializeCamera:AS3ValType, initializeScene:AS3ValType, initializeViewport:AS3ValType, initializeMaterial:AS3ValType, initializeTexture:AS3ValType, initializeLight:AS3ValType, initializeEntity:AS3ValType, addEntity:AS3ValType, initialize3DS:AS3ValType, applyForTmpBuffer:AS3ValType, render:AS3ValType, initializeMD2:AS3ValType, test:AS3ValType",
		initializeDeviceMethod, initializeCameraMethod, initializeSceneMethod, initializeViewportMethod, initializeMaterialMethod, initializeTextureMethod, initializeLightMethod, initializeEntityMethod, addEntityMethod, initialize3DSMethod, applyForTmpBufferMethod, renderMethod, initializeMD2Method, testMethod );



	AS3_Release( initializeDeviceMethod );
	AS3_Release( initializeCameraMethod );
	AS3_Release( initializeSceneMethod );
	AS3_Release( initializeViewportMethod );
	AS3_Release( initializeMaterialMethod );
	AS3_Release( initializeTextureMethod );
	AS3_Release( initializeLightMethod );
	AS3_Release( initializeEntityMethod );
	AS3_Release( addEntityMethod );
	AS3_Release( initialize3DSMethod );
	AS3_Release( applyForTmpBufferMethod );
	AS3_Release( renderMethod );
	AS3_Release( initializeMD2Method );
	AS3_Release( testMethod );


	AS3_LibInit( result );

	return 0;
}
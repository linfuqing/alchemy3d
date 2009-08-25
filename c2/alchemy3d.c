#define INLINE inline
#define __AS3__

#include <stdlib.h>
#include <AS3.h>

#include "Base.h"
#include "Device.h"
#include "Entity.h"
#include "Material.h"
#include "Texture.h"
#include "Light.h"
#include "Scene.h"
#include "Viewport.h"
#include "Triangle.h"
#include "Mesh.h"
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

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType", view, view->videoBuffer, & view->nRenderList, & view->nCullList, & view->nClippList );
}

AS3_Val initializeMaterial( void* self, AS3_Val args )
{
	Material * material;

	material = newMaterial( newFloatColor( 0.0f, 0.0f, 0.0f, 0.0f ),
							newFloatColor( 0.0f, 0.0f, 0.0f, 0.0f ),
							newFloatColor( 0.0f, 0.0f, 0.0f, 0.0f ),
							newFloatColor( 0.0f, 0.0f, 0.0f, 0.0f ),
							0.0f );

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
	Scene * scene;
	Entity * entity, * parent;
	Mesh * mesh;
	Texture * texture;
	Material * material;
	float * meshBuffer;

	int vNum, fNum, vLen, i, j;

	AS3_ArrayValue( args, "PtrType, PtrType, PtrType, PtrType, PtrType, IntType, IntType", &scene, &parent, &material, &texture, &meshBuffer, &vNum, &fNum ); 

	Vertex * vArr[vNum];

	mesh = newMesh( vNum, fNum, meshBuffer );

	entity = newEntity();

	if (vNum != 0 && fNum != 0)
	{
		vLen = vNum * VERTEX_SIZE;

		i = 0;
		j = 0;

		for ( ; i < vNum; i ++, j += VERTEX_SIZE )
		{
			mesh_push_vertex( mesh, meshBuffer[j], meshBuffer[j + 1], meshBuffer[j + 2] );
			vArr[i] = & mesh->vertices[i];
		}

		for ( i = 0, j = vLen; i < fNum; i ++, j += FACE_SIZE )
			mesh_push_triangle( mesh, vArr[(int)meshBuffer[j]], vArr[(int)meshBuffer[j+1]], vArr[(int)meshBuffer[j+2]], newVector(meshBuffer[j + 3], meshBuffer[j + 4]), newVector(meshBuffer[j + 5], meshBuffer[j + 6]), newVector(meshBuffer[j + 7], meshBuffer[j + 8]), NULL );

		entity_setMesh( entity, mesh );
	}

	if ( texture != FALSE ) entity_setTexture( entity, texture );

	if ( material != FALSE ) entity_setMaterial( entity, material );

	if ( scene != FALSE ) scene_addEntity(scene, entity, parent);

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, IntType, IntType", entity, &entity->material, &entity->texture, entity->position, entity->direction, entity->scale, entity->worldPosition, entity->mesh->vertices, entity->mesh->faces, sizeof( Vertex ), sizeof( Triangle ) );
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

AS3_Val render( void* self, AS3_Val args )
{
	Device * device;

	AS3_ArrayValue( args, "PtrType", &device );

	device_render(device);

	return 0;
}

AS3_Val propertySetting( void* self, AS3_Val args )
{
	return 0;
}

//测试函数
AS3_Val test( void* self, AS3_Val args )
{
	/*WORD wa = RGBTo16(255, 255, 255);
	WORD wb = RGBTo16(125, 125, 125);
	DWORD walpha = 127;
	ULONG wc;*/

	/*int a = 10, b = 20, c;

	c = a + b;*/

	/*__asm__ __volatile__("movl $0x7e0f81f, %%edi;\n\t"
				"movl %%edi, %0;"
				:"=r"(wc)
				:"r"(wa), "r"(wb), "r"(walpha));*/

	//__asm__("__asm(push(0x7e0f81f), push(mstate.edi), op(0x3c))"); 

	/*push ebp ; ebp 压栈
		mov ebp,esp ; 保存 esp 到 ebp
		mov edi,0x7e0f81f ; dx=00000111111000001111100000011111
		add esp,8 ; esp 指向参数 c1

		pop eax ; 弹出 c1 到 ax
		pop ebx ; 弹出 c2 到 bx
		; 处理颜色
		mov cx,ax ; cx=r1..b1
		mov dx,bx ; dx=r2..b2

		sal eax,16 ; eax=r1g1b1......
		sal ebx,16 ; ebx=r2g2b2......
		mov ax,cx ; eax=r1g1b1r1g1b1
		mov bx,dx ; ebx=r2g2b2r2g2b2

		and eax,edi ; eax=..g1..r1..b1

		pop esi ; 弹出 alpha
		mul esi ; eax*=alpha
		neg esi ; -alpha
		and ebx,edi ; ebx=..g2..r2..b2
		add esi,0x20 ; 32-alpha
		xchg eax,ebx ; 交换 eax,ebx
		mul esi ; c2*=(32-alpha)
		add eax,ebx ; c1*alpha+c2*(32-alpha)
		mov esp,ebp
		sar eax,5 ；color=(c1*alpha+c2*(32-alpha))/32
		;还原成 RGB 形式
		pop ebp
		and eax,edi ; color=..g..r..b
		mov cx,ax ;
	sar eax,16 ;
	or ax,cx ; color=rgb (eax)
		ret*/

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
	AS3_Val applyForTmpBufferMethod = AS3_Function( NULL, applyForTmpBuffer );
	AS3_Val renderMethod = AS3_Function( NULL, render );
	AS3_Val propertySettingMethod = AS3_Function( NULL, propertySetting );
	AS3_Val testMethod = AS3_Function( NULL, test );



	AS3_Val result = AS3_Object( "initializeDevice:AS3ValType, initializeCamera:AS3ValType, initializeScene:AS3ValType, initializeViewport:AS3ValType, initializeMaterial:AS3ValType, initializeTexture:AS3ValType, initializeLight:AS3ValType, initializeEntity:AS3ValType, applyForTmpBuffer:AS3ValType, render:AS3ValType, propertySetting:AS3ValType, test:AS3ValType",
		initializeDeviceMethod, initializeCameraMethod, initializeSceneMethod, initializeViewportMethod, initializeMaterialMethod, initializeTextureMethod, initializeLightMethod, initializeEntityMethod, applyForTmpBufferMethod, renderMethod, propertySettingMethod, testMethod );



	AS3_Release( initializeDeviceMethod );
	AS3_Release( initializeCameraMethod );
	AS3_Release( initializeSceneMethod );
	AS3_Release( initializeViewportMethod );
	AS3_Release( initializeMaterialMethod );
	AS3_Release( initializeTextureMethod );
	AS3_Release( initializeLightMethod );
	AS3_Release( initializeEntityMethod );
	AS3_Release( applyForTmpBufferMethod );
	AS3_Release( renderMethod );
	AS3_Release( propertySettingMethod );
	AS3_Release( testMethod );


	AS3_LibInit( result );

	return 0;
}
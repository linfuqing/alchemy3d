#include <malloc.h>
#include <stdlib.h>

#define INLINE
#define __NOT_AS3__

#include "Base.h"
#include "Device.h"
#include "Scene.h"
#include "Entity.h"
#include "Viewport.h"
#include "Triangle.h"
#include "Mesh.h"
#include "Render.h"
#include "Vector3D.h"
#include "Matrix3D.h"
#include "FloatColor.h"
#include "Material.h"
#include "Texture.h"
#include "Light.h"
#include "Plane3D.h"

Device * device;

int main()
{
	Camera * camera;
	Viewport * view;
	Scene * scene;
	Entity * do3d, * do3d2;
	Vertex * vArr[4], * vArr2[9];
	Vector * point, * point2;
	Material * material, * material2;
	//Entity * lightSource;
	//Light * light;
	Texture * texture;
	Mesh * mesh1, * mesh2;

	float pos[16];
	float pos2[18];

	int i = 0;
	int j = 0;

	LPBYTE bitmapData;

	if( ( bitmapData = ( LPBYTE )calloc( 64*64*sizeof(DWORD), sizeof( BYTE ) ) ) == NULL )
	{
		exit( TRUE );
	}

	for ( i = 0; i < 64 * 64; i ++ )
	{
		bitmapData[i] = i;
	}

	texture = newTexture(64, 64, bitmapData);

	pos[0] = -10.0f;
	pos[1] = -10.0f;

	pos[2] = -10.0f;
	pos[3] = 10.0f;

	pos[4] = 10.0f;
	pos[5] = -10.0f;

	pos[6] = 10.0f;
	pos[7] = 10.0f;

	//========================
	pos2[0] = -50.0f;
	pos2[1] = -50.0f;

	pos2[2] = -50.0f;
	pos2[3] = 0.0f;

	pos2[4] = -50.0f;
	pos2[5] = 50.0f;

	pos2[6] = 0.0f;
	pos2[7] = -50.0f;

	pos2[8] = 0.0f;
	pos2[9] = 0.0f;

	pos2[10] = 0.0f;
	pos2[11] = 50.0f;

	pos2[12] = 50.0f;
	pos2[13] = -50.0f;

	pos2[14] = 50.0f;
	pos2[15] = 0.0f;

	pos2[16] = 50.0f;
	pos2[17] = 50.0f;

	mesh1 = newMesh( 4, 2 );
	mesh2 = newMesh( 18, 9 );

	mesh1->render_mode = RENDER_TEXTRUED_PERSPECTIVE_TRIANGLELP_FSINVZB_32;
	mesh2->render_mode = RENDER_TEXTRUED_TRIANGLE_GSINVZB_32;

	for ( i = 0; i < 8; i += 2)
	{
		mesh_push_vertex(mesh1, pos[i], pos[i+1], 0.0f);
		vArr[j] = & mesh1->vertices[j];
		j++;
	}

	point = newVector(1.0f, 1.0f);

	mesh_push_triangle(mesh1, vArr[0], vArr[2], vArr[1], newVector(0.0f, 0.0f), newVector(1.0f, 0.0f), newVector(0.0f, 1.0f), NULL);
	mesh_push_triangle(mesh1, vArr[3], vArr[1], vArr[2], newVector(1.0f, 1.0f), newVector(0.0f, 1.0f), newVector(1.0f, 0.0f), NULL);

	i = 0;
	j = 0;

	for (; i < 18; i += 2)
	{
		mesh_push_vertex(mesh2, pos2[i], pos2[i+1], 0.0f);
		vArr2[j] = & mesh2->vertices[j];
		j++;
	}

	point2 = newVector(.5f, .5f);

	mesh_push_triangle(mesh2, vArr2[0], vArr2[3], vArr2[1], point2, point2, point2, NULL);
	mesh_push_triangle(mesh2, vArr2[4], vArr2[1], vArr2[3], point2, point2, point2, NULL);
	mesh_push_triangle(mesh2, vArr2[1], vArr2[4], vArr2[2], point2, point2, point2, NULL);
	mesh_push_triangle(mesh2, vArr2[5], vArr2[2], vArr2[4], point2, point2, point2, NULL);

	mesh_push_triangle(mesh2, vArr2[3], vArr2[6], vArr2[4], point2, point2, point2, NULL);
	mesh_push_triangle(mesh2, vArr2[7], vArr2[4], vArr2[6], point2, point2, point2, NULL);
	mesh_push_triangle(mesh2, vArr2[4], vArr2[7], vArr2[5], point2, point2, point2, NULL);
	mesh_push_triangle(mesh2, vArr2[8], vArr2[5], vArr2[7], point2, point2, point2, NULL);

	material = newMaterial( newFloatColor( 0.05f, 0.05f, 0.05f, 1.0f ),
							newFloatColor( 0.3f, 0.8f, 0.6f, 1.0f ),
							newFloatColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							newFloatColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							4.0f );

	material2 = newMaterial( newFloatColor( 0.0f, 1.0f, 0.0f, 1.0f ),
							newFloatColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							newFloatColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							newFloatColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							4.0f );
	
	mesh_setMaterial( mesh1, material );
	mesh_setTexture( mesh1, texture );

	mesh_setMaterial( mesh2, material2 );
	mesh_setTexture( mesh2, texture );

	do3d = newEntity();
	//entity_setRotationX(do3d, 45.0f);
	//entity_setRotationZ(do3d, 134.0f);
	//entity_setY(do3d, -20.0f);
	entity_setZ(do3d, 200.0f);
	entity_setMesh( do3d, mesh1 );

	do3d2 = newEntity();
	entity_setRotationX(do3d2, 90.0f);
	entity_setZ(do3d2, 40.0f);
	entity_setMesh( do3d2, mesh2 );

	camera = newCamera( 90.0f, 100.0f, 5000.0f, newEntity() );
	camera_setTarget( camera, do3d->w_pos );

	//lightSource = newEntity();
	//entity_setZ(lightSource, 300.0f);
	//light = newPointLight( POINT_LIGHT, lightSource );
	//setLightOnOff( light, TRUE );
	//light->mode = HIGH_MODE;
	//light->ambient = newFloatColor( 0.0f, 0.0f, 0.0f, 1.0f );
	//light->diffuse = newFloatColor( 1.0f, 1.0f, 1.0f, 1.0f );
	//light->attenuation1 = .001f;
	//light->attenuation2 = .0000001f;

	scene = newScene();
	//scene_addLight(scene, light);
	scene_addEntity(scene, do3d, NULL);
	//scene_addEntity(scene, do3d2, NULL);
	//scene_addEntity(scene, lightSource, NULL);

	view = newViewport( 600, 400, scene, camera );

	device = newDevice();
	device_addViewport(device, view);
	device_addCamera(device, camera);
	device_addScene(device, scene);

	/*do3d1 = newEntity();
	do3d1->direction->x = 45;
	scene_addChild(scene, do3d1, NULL);*/

	device_render(device);
	//entity_setX(camera->eye, 10.0f);
	//device_render(device);
	/*device_render(device);
	device_render(device);
	device_render(device);*/

	//printf("%x", view->gfxBuffer[189206]);

	return 0;
}
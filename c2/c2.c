#include <malloc.h>
#include <stdlib.h>

#define INLINE

#include "Base.h"
#include "Device.h"
#include "Scene.h"
#include "Entity.h"
#include "Viewport.h"
#include "Polygon.h"
#include "Mesh.h"
#include "Render.h"
#include "Vector3D.h"
#include "Matrix3D.h"
#include "Color.h"
#include "Material.h"
#include "Texture.h"
#include "Light.h"

Device * device;

extern ULONG alpha(ULONG c1, ULONG c2, ULONG calpha); 

int main()
{
	Camera * camera;
	Viewport * view;
	Scene * scene;
	Entity * do3d, * do3d2;
	Entity * lightSource;
	Vertex * vArr[4], * vArr2[4];
	Vector * point, * point2;
	Material * material, * material2;
	Light * light;
	Texture * texture;
	Mesh * mesh1, * mesh2;

	float pos[16];
	float pos2[16];

	int i = 0;
	int j = 0;

	UINT32 * bitmapData;

	//WORD wd;

	WORD wa = RGBTo16(255, 255, 255);
	WORD wb = RGBTo16(125, 125, 125);
	DWORD walpha = 10;
	ULONG wc = alpha((ULONG)wa, (ULONG)wb, walpha);

	//DWORD d1 = (((((((wa << 16) | wa) & 0x7e0f81f) - (((wb << 16) | wb) & 0x7e0f81f)) * walpha) >> 5) + (((wb << 16) | wb) & 0x7e0f81f)) & 0x7e0f81f;
	//wa = (d1 & 0xffff0000)>>16; // g...r...b => ..g..
	//wb = d1 & 0xffff; // g...r...b => r...b
	//wd = wa | wb; // rgb

	if( ( bitmapData = ( UINT32 * )calloc( 400, sizeof( UINT32 ) ) ) == NULL )
	{
		exit( TRUE );
	}
	bitmapData[0] = 0xFFFFFFFF;
	bitmapData[1] = 0xFFFF0000;
	bitmapData[2] = 0xFF00FF00;
	bitmapData[3] = 0xFF0000FF;

	texture = newTexture(10, 10, bitmapData);

	pos[0] = -150.0f;
	pos[1] = 150.0f;

	pos[2] = 150.0f;
	pos[3] = 150.0f;

	pos[4] = -150.0f;
	pos[5] = -150.0f;

	pos[6] = 150.0f;
	pos[7] = -150.0f;

	pos2[0] = -150.0f;
	pos2[1] = -150.0f;
	pos2[2] = 150.0f;
	pos2[3] = -150.0f;
	pos2[4] = -150.0f;
	pos2[5] = 150.0f;
	pos2[6] = 150.0f;
	pos2[7] = 150.0f;

	mesh1 = newMesh( 4, 2 );

	for (; i < 8; i += 2)
	{
		mesh_push_vertices(mesh1, pos[i], pos[i+1], 0.0f);
		vArr[j] = & mesh1->vertices[j];
		j++;
	}

	point = newVector(1.0f, 1.0f);

	mesh_push_faces(mesh1, vArr[0], vArr[1], vArr[2], point, point, point);

	mesh_push_faces(mesh1, vArr[3], vArr[2], vArr[1], point, point, point);

	i = 0;
	j = 0;
	mesh2 = newMesh( 4, 2 );

	for (; i < 8; i += 2)
	{
		mesh_push_vertices(mesh2, pos2[i], pos2[i+1], 0.0f);
		vArr2[j] = & mesh2->vertices[j];
		j++;
	}

	point2 = newVector(.5f, .5f);

	mesh_push_faces(mesh2, vArr2[0], vArr2[1], vArr2[2], point2, point2, point2);

	mesh_push_faces(mesh2, vArr2[3], vArr2[2], vArr2[1], point2, point2, point2);

	material = newMaterial( newColor( 0.05f, 0.05f, 0.05f, 1.0f ),
							newColor( 0.3f, 0.8f, 0.6f, 1.0f ),
							newColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							newColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							4.0f );

	material2 = newMaterial( newColor( 0.0f, 1.0f, 0.0f, 1.0f ),
							newColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							newColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							newColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							4.0f );

	do3d = newEntity();
	//entity_setRotationX(do3d, 180.0f);
	entity_setZ(do3d, 500.0f);
	entity_setMesh( do3d, mesh1 );
	entity_setMaterial( do3d, material );
	entity_setTexture( do3d, texture );

	do3d2 = newEntity();
	entity_setZ(do3d2, 400.0f);
	entity_setMesh( do3d2, mesh2 );
	entity_setMaterial( do3d2, material2 );
	entity_setTexture( do3d2, texture );

	camera = newCamera( 90.0f, 100.0f, 5000.0f, newEntity() );
	//camera_setTarget( camera, do3d->worldPosition );

	lightSource = newEntity();
	entity_setZ(lightSource, 300.0f);
	light = newPointLight( POINT_LIGHT, lightSource );
	setLightOnOff( light, TRUE );
	light->mode = HIGH_MODE;
	light->ambient = newColor( 0.0f, 0.0f, 0.0f, 1.0f );
	light->diffuse = newColor( 1.0f, 1.0f, 1.0f, 1.0f );
	light->attenuation1 = .001f;
	light->attenuation2 = .0000001f;

	scene = newScene();
	scene_addLight(scene, light);
	scene_addEntity(scene, do3d, NULL);
	scene_addEntity(scene, do3d2, NULL);
	scene_addEntity(scene, lightSource, NULL);

	view = newViewport( 600.0f, 400.0f, scene, camera );

	device = newDevice();
	device_addViewport(device, view);
	device_addCamera(device, camera);
	device_addScene(device, scene);

	/*do3d1 = newEntity();
	do3d1->direction->x = 45;
	scene_addChild(scene, do3d1, NULL);*/

	device_render(device);
	//device_render(device);
	/*device_render(device);
	device_render(device);
	device_render(device);*/

	//printf("%x", view->gfxBuffer[189206]);

	return 0;
}
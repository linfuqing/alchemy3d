#include <malloc.h>
#include <stdlib.h>

#define INLINE
#define __NOT_AS3__

#include "Base.h"
#include "Math3D.h"
#include "Scene.h"
#include "Entity.h"
#include "Viewport.h"
#include "Triangle.h"
#include "Mesh.h"
#include "Vector3D.h"
#include "Matrix3D.h"
#include "FloatColor.h"
#include "Material.h"
#include "Texture.h"
#include "Light.h"
#include "Plane3D.h"
#include "3DS.h"
#include "MD2.h"
#include "Primitives.h"
#include "Terrain.h"

long GetFileSize(FILE* f)
{
	long pos, len;

    pos = ftell(f);

    fseek(f,0,SEEK_END);

    len = ftell(f);

    fseek(f,pos,SEEK_SET);

    return len;
}

int main()
{
	Camera * camera;
	Viewport * view;
	Scene * scene;
	Entity * do3d3;
	Material * material, * material2;
	//Entity * lightSource;
	//Light * light;
	Texture * texture;
	Mesh * mesh4;

	int i = 0;
	int j = 0;

	LPDWORD bitmapData;

	//MD2 * md2;

	//A3DS * a3ds;
	//FILE *fp;

	/////////////////////////////////////////////////////////////////////////////////////

	material = newMaterial( newFloatColor( 1.0f, 1.0f, 1.0f, 1.0f ),
							newFloatColor( 0.3f, 0.8f, 0.6f, 1.0f ),
							newFloatColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							newFloatColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							4.0f );

	material2 = newMaterial( newFloatColor( 0.0f, 1.0f, 0.0f, 1.0f ),
							newFloatColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							newFloatColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							newFloatColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							4.0f );

	if( ( bitmapData = ( LPDWORD )calloc( 2 * 2, sizeof( DWORD ) ) ) == NULL )
	{
		exit( TRUE );
	}

	for ( i = 0; i < 2 * 2; i ++ )
	{
		bitmapData[i] = 0xffffffff;
	}

	texture = newTexture( "default_tex" );
	texture_setData( texture, 2, 2, (LPBYTE)bitmapData );

	/////////////////////////////////////////////////////////////////////////////////////

	//mesh4 = newTerrain( NULL, texture, 4000, 4000, 2000, 2, material, NULL, RENDER_WIREFRAME_TRIANGLE_32 );
	mesh4 = newPlane( NULL, material, texture, 150.0f, 150.0f, 1, 1, RENDER_WIREFRAME_TRIANGLE_32 );
	mesh4->octree_depth = 0;

	/////////////////////////////////////////////////////////////////////////////////////
	
	scene = newScene();

	camera = newCamera( 90.0f, 10.0f, 10000.0f, newEntity() );
	entity_setZ(camera->eye, -10.0f);
	//entity_setX(camera->eye, 2000.0f);

	view = newViewport( 640, 480, scene, camera );

	do3d3 = newEntity();
	do3d3->name = "root";
	entity_setRotationX( do3d3, 90.0f);
	entity_setZ(do3d3, 500.0f);
	//entity_setY(do3d3, -1000.0f);
	entity_setMesh( do3d3, mesh4 );

	scene_addEntity(scene, do3d3, NULL);

	//camera_setTarget( camera, do3d3->w_pos );

	/////////////////////////////////////////////////////////////////////////////////////

	/*fopen_s( & fp, "D:\\3Dmodel\\3ds\\scene.jpg","rb");

	a3ds = A3DS_Create( fp, do3d3, RENDER_TEXTRUED_TRIANGLE_GSINVZB_32 );

	A3DS_Dispose( a3ds );

	for ( i = 0; i < do3d3->children->entity->mesh->nFaces; i ++ )
	{
		do3d3->children->entity->mesh->faces[i].texture = texture;
	}*/

	/*md2 = newMD2( do3d3 );

	md2_read( & buffer, md2, material, texture, 1 );*/

	/////////////////////////////////////////////////////////////////////////////////////

	//lightSource = newEntity();
	//entity_setZ(lightSource, 300.0f);
	//light = newPointLight( POINT_LIGHT, lightSource );
	//setLightOnOff( light, TRUE );
	//light->mode = HIGH_MODE;
	//light->ambient = newFloatColor( 0.0f, 0.0f, 0.0f, 1.0f );
	//light->diffuse = newFloatColor( 1.0f, 1.0f, 1.0f, 1.0f );
	//light->attenuation1 = .001f;
	//light->attenuation2 = .0000001f;
	//scene_addLight(scene, light);
	//scene_addEntity(scene, lightSource, NULL);

	for ( i = 0; i < 20; i ++ )
	{
		viewport_updateBeforeRender( view );

		viewport_project( view, 0 );

		viewport_render( view );

		viewport_updateAfterRender( view );
	}
	return 0;
}
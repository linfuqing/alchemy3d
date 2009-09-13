#include <malloc.h>
#include <stdlib.h>

#define INLINE
#define __NOT_AS3__

#include "Base.h"
#include "Math.h"
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
#include "3DSLoader.h"
#include "MD2.h"
#include "Primitives.h"

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
	/*FILE *fp;
	UCHAR * buffer;*/
	long length=0;

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

	if( ( bitmapData = ( LPDWORD )calloc( 256*256, sizeof( DWORD ) ) ) == NULL )
	{
		exit( TRUE );
	}

	for ( i = 0; i < 256 * 256; i ++ )
	{
		bitmapData[i] = 0xffffffff;
	}

	texture = newTexture( "default_tex" );
	texture_setData( texture, 256, 256, (LPBYTE)bitmapData );

	/////////////////////////////////////////////////////////////////////////////////////

	mesh4 = newPlane( NULL, material, texture, 100, 100, 1, 1 );

	/////////////////////////////////////////////////////////////////////////////////////
	
	scene = newScene();

	camera = newCamera( 90.0f, 100.0f, 5000.0f, newEntity() );

	view = newViewport( 600, 400, scene, camera );

	do3d3 = newEntity();
	do3d3->name = "root";
	entity_setZ(do3d3, 400.0f);
	entity_setMesh( do3d3, mesh4 );

	scene_addEntity(scene, do3d3, NULL);

	camera_setTarget( camera, do3d3->w_pos );

	/////////////////////////////////////////////////////////////////////////////////////

	/*fp = fopen("D:\\3Dmodel\\3ds\\plane.3ds","rb");

	length = GetFileSize(fp);

	buffer = (UCHAR * )malloc(length * sizeof(UCHAR));

	fread(buffer,1,length,fp);

	a3ds = A3DS_Create( do3d3, & buffer, length );

	A3DS_Dispose( a3ds );*/

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
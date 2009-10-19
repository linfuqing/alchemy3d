#include <malloc.h>
#include <stdlib.h>

#define INLINE
#define __NOT_AS3__
//#define RGB565

#include "Base.h"
#include "Math3D.h"

BYTE alpha_table[NUM_ALPHA_LEVELS][256];

#include "Scene.h"
#include "Entity.h"
#include "Viewport.h"
#include "Triangle.h"
#include "Mesh.h"
#include "Vector3D.h"
#include "Matrix3D.h"
#include "ColorValue.h"
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
	Bitmap * bitmap, * bitmap2;
	//Mesh * mesh4;
	Fog * fog;

	int i = 0;
	int j = 0;

	LPDWORD bitmapData;

	//MD2 * md2;

	//A3DS * a3ds;
	//FILE *fp;
	//UCHAR * buffer;
	//long length=0;

	alpha_Table_Builder(NUM_ALPHA_LEVELS, alpha_table);

	/////////////////////////////////////////////////////////////////////////////////////

	material = newMaterial( newColorValue( 1.0f, 1.0f, 1.0f, 1.0f ),
							newColorValue( 0.3f, 0.8f, 0.6f, 0.5f ),
							newColorValue( 1.0f, 0.0f, 0.0f, 1.0f ),
							newColorValue( 1.0f, 0.0f, 0.0f, 1.0f ),
							4.0f );

	material2 = newMaterial( newColorValue( 0.0f, 1.0f, 0.0f, 1.0f ),
							newColorValue( 1.0f, 0.0f, 0.0f, 1.0f ),
							newColorValue( 1.0f, 0.0f, 0.0f, 1.0f ),
							newColorValue( 1.0f, 0.0f, 0.0f, 1.0f ),
							4.0f );
	if( ( bitmapData = ( LPDWORD )calloc( 3 * 3, sizeof( DWORD ) ) ) == NULL )
	{
		exit( TRUE );
	}

	//¸ß³ÌÍ¼
	for ( i = 0; i < 3 * 3; i ++ )
	{
		bitmapData[i] = 0xffffffff;
	}

	bitmap2 = newBitmap( 3, 3, (LPBYTE)bitmapData );

	//ÎÆÀí
	if( ( bitmapData = ( LPDWORD )calloc( 4 * 4, sizeof( DWORD ) ) ) == NULL )
	{
		exit( TRUE );
	}

	for ( i = 0; i < 4 * 4; i ++ )
	{
		bitmapData[i] = 0xffffffff;
	}

	bitmap = newBitmap( 4, 4, (LPBYTE)bitmapData );
	texture = newTexture( "default_tex" );
	texture_setMipmap( texture, bitmap );
	texture->perspective_dist = 5000.0f;
	texture->addressMode = 1;

	fog = newFog( newColorValue( 0.0f, 0.0f, 0.0f, 1.0f ), 1000.0f, 4800.0f );
	fog->ready = TRUE;
	
	scene = newScene();

	scene_addFog( scene, fog );

	camera = newCamera( 90.0f, 10.0f, 5000.0f, newEntity() );
	entity_setZ(camera->eye, -910.0f);

	view = newViewport( 600, 400, scene, camera );


	do3d3 = newEntity();
	do3d3->name = "root";
	entity_setRotationX( do3d3, -25.0f);
	//entity_setZ(do3d3, -20.0f);
	entity_setY(do3d3, 30.0f);

	//do3d = newEntity();
	//do3d->name = "md2";
	//mesh4 = newPlane( NULL, material, texture, 200.0f, 200.0f, 2, 2, RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_FOG_GSINVZB_32 );
	//mesh4 = newPlane( NULL, material, texture, 150.0f, 150.0f, 1, 1, RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_INVZB_32 );
	//mesh4->terrainTrace = TRUE;
	//entity_setMesh( do3d, mesh4 );
	//entity_setX(do3d, 300.0f);
	//entity_setY(do3d, 1000.0f);

	//scene_addEntity(scene, do3d, NULL);
	scene_addEntity(scene, do3d3, NULL);

	/////////////////////////////////////////////////////////////////////////////////////

	do3d3 = newTerrain( do3d3, bitmap2, 600, 600, 0, material, texture, RENDER_TEXTRUED_TRIANGLE_GSINVZB_ALPHA_32, 1 );
	do3d3->mesh->octree_depth = 0;
	//do3d3->mesh->useMipmap = TRUE;
	//do3d3->mesh->mip_dist = 2000.0f;
	do3d3->mesh->fogEnable = TRUE;

	/////////////////////////////////////////////////////////////////////////////////////

	//camera_setTarget( camera, do3d3->w_pos );

	/////////////////////////////////////////////////////////////////////////////////////

	//fopen_s( & fp, "D:\\Inetpub\\wwwroot2\\engine\\alchemy3d\\as3\\src\\asset\\md2\\tris.MD2","rb");

	//length = GetFileSize(fp);

	//buffer = (UCHAR * )malloc(length * sizeof(UCHAR));

	//fread(buffer,1,length,fp);

	/*a3ds = A3DS_Create( fp, do3d3, RENDER_TEXTRUED_TRIANGLE_GSINVZB_32 );

	A3DS_Dispose( a3ds );

	for ( i = 0; i < do3d3->children->entity->mesh->nFaces; i ++ )
	{
		do3d3->children->entity->mesh->faces[i].texture = texture;
	}*/

	//md2 = newMD2( do3d );

	//md2_read( & buffer, md2, material, texture, 1, 24 );

	//md2->entity->mesh->terrainTrace = TRUE;

	/////////////////////////////////////////////////////////////////////////////////////

	//lightSource = newEntity();
	//entity_setZ(lightSource, 300.0f);
	//light = newPointLight( POINT_LIGHT, lightSource );
	//setLightOnOff( light, TRUE );
	//light->mode = HIGH_MODE;
	//light->ambient = newColorValue( 0.0f, 0.0f, 0.0f, 1.0f );
	//light->diffuse = newColorValue( 1.0f, 1.0f, 1.0f, 1.0f );
	//light->attenuation1 = .001f;
	//light->attenuation2 = .0000001f;
	//scene_addLight(scene, light);
	//scene_addEntity(scene, lightSource, NULL);

	for ( i = 0; i < 1; i ++ )
	{
		viewport_updateBeforeRender( view );

		viewport_project( view, 0 );

		viewport_render( view );

		viewport_updateAfterRender( view );
	}
	return 0;
}
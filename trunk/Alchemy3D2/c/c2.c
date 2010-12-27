#include <malloc.h>
#include <stdlib.h>

#define INLINE
#define __NOT_AS3__
//#define RGB565

#include "Base.h"
#include "Math3D.h"

BYTE logbase2ofx[2048];
BYTE alpha_table[NUM_ALPHA_LEVELS][256];
float dot5miplevel_table[11];
DWORD multiply256_table[256][256];
DWORD multiply256FIXP8_table[256][256];

#include "Scene.h"
#include "Entity.h"
#include "Triangle.h"
#include "Mesh.h"
#include "Viewport.h"
#include "Vector3D.h"
#include "Matrix3D.h"
#include "Color888.h"
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
	Entity * do3d3, * do3d;
	Material * material, * material2;
	//Entity * lightSource;
	//Light * light;
	Texture * texture;
	Bitmap * bitmap, * bitmap2;
	Mesh * mesh4, * mesh5;
	Fog * fog;

	int i = 0;
	int j = 0;

	LPDWORD bitmapData;

	//MD2 * md2;

	A3DS * a3ds;
	FILE *fp;
	UCHAR * buffer;
	long length=0;

	//test
	DWORD k;
	i = 255 << 22;
	j = ((i << 6) / (440058 >> 6))<<16;
	k = i<<6;
	//test

	log2base_Table_Builder(logbase2ofx);
	alpha_Table_Builder(NUM_ALPHA_LEVELS, alpha_table);
	multiply256_Table_Builder(multiply256_table);
	multiply256FIXP8_Table_Builder(multiply256FIXP8_table);
	dot5miplevel_Table_Builder(dot5miplevel_table);

	/////////////////////////////////////////////////////////////////////////////////////

	material = newMaterial( newColor888( 0, 0, 0, 255 ),
							newColor888( 100, 200, 0, 255 ),
							newColor888( 255, 0, 0, 255 ),
							newColor888( 255, 0, 0, 255 ),
							4.0f );

	material2 = newMaterial( newColor888( 255, 255, 255, 255 ),
							newColor888( 100, 200, 0, 255 ),
							newColor888( 255, 0, 0, 255 ),
							newColor888( 255, 0, 0, 255 ),
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
	if( ( bitmapData = ( LPDWORD )calloc( 256 * 256, sizeof( DWORD ) ) ) == NULL )
	{
		exit( TRUE );
	}

	for ( i = 0; i < 256 * 256; i ++ )
	{
		bitmapData[i] = 0xffffffff;
	}

	bitmap = newBitmap( 256, 256, (LPBYTE)bitmapData );
	texture = newTexture( "default_tex" );
	texture_setMipmap( texture, bitmap );
	texture->perspective_dist = 3000.0f;

	fog = newFog( newColorValue( 0.0f, 0.0f, 0.0f, 1.0f ), 1000.0f, 4000.0f, 1.0f, FOG_EXP2 );
	fog->ready = TRUE;
	
	scene = newScene();

	scene_addFog( scene, fog );

	camera = newCamera( 90.0f, 20.0f, 20000.0f, newEntity() );
	//entity_setZ(camera->eye, -910.0f);

	view = newViewport( 600, 400, scene, camera );

	do3d = newEntity();
	do3d->name = "root2";
	//entity_setRotationY( do3d, 45.0f);
	entity_setZ(do3d, 200.0f);
	//entity_setX(do3d, 200.0f);
	//entity_setY(do3d, -100.0f);

	do3d3 = newEntity();
	do3d3->name = "root";
	//entity_setRotationX( do3d3, 45.0f);
	//entity_setRotationY( do3d3, 45.0f);
	//entity_setZ(do3d3, 15.0f);
	//entity_setY(do3d3, 400.0f);
	//entity_setX(do3d3, -100.0f);

	mesh4 = newPlane( NULL, material, texture, 1024.0f, 1024.0f, 1, 1, RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_INVZB_32 );
	mesh5 = newPlane( NULL, material, texture, 320.0f, 320.0f, 1, 1, RENDER_TEXTRUED_PERSPECTIVE_TRIANGLE_INVZB_32 );
	mesh5->addressMode = ADDRESS_MODE_WRAP;
	mesh_setTexScale( mesh5, 4.0f, 4.0f );
	//mesh_setTexRotation( mesh5, 120.0f );
	//mesh5->useMipmap = TRUE;
	//mesh5->mip_dist = 500.0f;
	//mesh5->lightEnable = TRUE;
	//mesh4->terrainTrace = TRUE;
	//mesh5->fogEnable = TRUE;
	//entity_setMesh( do3d, mesh4 );
	//entity_setMesh( do3d3, mesh5 );

	//scene_addEntity(scene, do3d, NULL);
	scene_addEntity(scene, do3d3, NULL);

	/////////////////////////////////////////////////////////////////////////////////////

	//do3d3 = newTerrain( do3d3, bitmap2, 600, 600, 0, material, texture, RENDER_TEXTRUED_TRIANGLE_GSINVZB_ALPHA_32, 1 );
	//do3d3->mesh->octree_depth = 0;
	//do3d3->mesh->useMipmap = TRUE;
	//do3d3->mesh->mip_dist = 2000.0f;
	//do3d3->mesh->fogEnable = TRUE;

	/////////////////////////////////////////////////////////////////////////////////////

	//camera_setTarget( camera, do3d3->w_pos );

	/////////////////////////////////////////////////////////////////////////////////////

	fopen_s( & fp, "D:\\3Dmodel\\3ds\\plane.3ds","rb");

	length = GetFileSize(fp);

	buffer = (UCHAR * )malloc(length * sizeof(UCHAR));

	fread(buffer,1,length,fp);

	a3ds = A3DS_Create( fp, do3d3, RENDER_TEXTRUED_TRIANGLE_GSINVZB_32 );

	A3DS_Dispose( a3ds );

	//for ( i = 0; i < do3d3->children->entity->mesh->nFaces; i ++ )
	//{
	//	do3d3->children->entity->mesh->faces[i].texture = texture;
	//}

	//md2 = newMD2( do3d );

	//md2_read( & buffer, md2, material, texture, 1, 24 );

	//md2->entity->mesh->terrainTrace = TRUE;

	/////////////////////////////////////////////////////////////////////////////////////

	//lightSource = newEntity();
	//entity_setY(lightSource, 300.0f);
	//light = newPointLight( POINT_LIGHT, lightSource );
	//setLightOnOff( light, TRUE );
	//light->mode = HIGH_MODE;
	//light->ambient = newColor888( 0, 0, 0, 255 );
	//light->diffuse = newColor888( 255, 255, 255, 255 );
	//light->attenuation1 = .001f;
	//light->attenuation2 = .0000001f;
	//scene_addLight(scene, light);
	//scene_addEntity(scene, lightSource, NULL);

	for ( i = 0; i < 1; i ++ )
	{
		viewport_updateBeforeRender( view );

		viewport_project( view, 0 );

		printf( "%d", viewport_mouseOn( view, 250.0f, 250.0f ) );

		viewport_render( view );

		viewport_updateAfterRender( view );
	}

	return 0;
}
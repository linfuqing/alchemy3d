#define INLINE inline
#define __AS3__

#include <stdlib.h>
#include <AS3.h>

#include "Base.h"
#include "Math3D.h"
#include "Entity.h"
#include "Material.h"
#include "Texture.h"
#include "Bitmap.h"
#include "Light.h"
#include "Scene.h"
#include "Viewport.h"
#include "Triangle.h"
#include "Mesh.h"
#include "3DS.h"
#include "Md2.h"
#include "Primitives.h"
#include "AS3File.h"
#include "Terrain.h"

UCHAR * testBuff;

AS3_Val initializeCamera( void* self, AS3_Val args )
{
	Camera * camera;

	Entity * eye;

	double fov, nearClip, farClip;

	AS3_ArrayValue( args, "PtrType, DoubleType, DoubleType, DoubleType", & eye, & fov, & nearClip, & farClip );

	nearClip = nearClip < 5.0f ? 5.0f : nearClip;

	camera = newCamera( (float)fov, (float)nearClip, (float)farClip, eye );

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType",
					camera, camera->target, & camera->fov, & camera->near, & camera->far, & camera->fnfDirty, & camera->isUVN );
}

AS3_Val attachCamera( void* self, AS3_Val args )
{
	Camera * camera;

	Viewport * viewport;

	AS3_ArrayValue( args, "PtrType, PtrType", & camera, & viewport );

	viewport->camera = camera;

	return 0;
}

AS3_Val initializeScene( void* self, AS3_Val args )
{
	Scene * scene;

	scene =  newScene();

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", scene, & scene->nNodes, & scene->nVertices, & scene->nFaces, & scene->nRenderList, & scene->nCullList, & scene->nClippList );
}

AS3_Val attachScene( void* self, AS3_Val args )
{
	Scene * scene;

	//Viewport * viewport;

	SceneNode * entityList, * ep;

	Lights * lights;

	AS3_ArrayValue( args, "PtrType, PtrType, PtrType", & scene, & entityList, & lights );

	//viewport->scene = scene;

	//修改
	ep = entityList;

	while( ep != NULL )
	{
		ep->entity->scene = scene;
		ep = ep->next;
	}

	scene -> nodes  = entityList;
	scene -> lights = lights;
	scene -> dirty  = TRUE;

	return 0;
}

AS3_Val initializeViewport( void* self, AS3_Val args )
{
	Viewport * view;
	Scene * scene;
	Camera * camera;

	double width, height;

	AS3_ArrayValue( args, "DoubleType, DoubleType, PtrType, PtrType", &width, &height, &camera, &scene );


	view = newViewport( (float)width, (float)height, scene, camera );

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType", view, view->videoBuffer, view->zBuffer, & view->camera, & view->scene );
}

AS3_Val initializeLight( void* self, AS3_Val args )
{
	//Scene * scene;
	Entity * source;
	int type;
	Light * light;
	Lights * node;

	AS3_ArrayValue( args, "PtrType, IntType", &source, &type );

	light = newPointLight( type, source );
	//scene_addLight( scene, light );

	if( ( node = ( Lights * )malloc( sizeof( Lights ) ) ) == NULL )
	{
		exit( TRUE );
	}

	node -> light = light;
	node -> next  = NULL;

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType",
		light, &light->mode, &light->bOnOff, &light->type, light->ambient, light->diffuse, light->specular,
		&light->range, &light->falloff, &light->theta, &light->phi, &light->attenuation0, &light->attenuation1, &light->attenuation2, node );
}

AS3_Val addLight( void * self, AS3_Val args )
{
	//Scene * scene;
	Lights * lights, * node, * lp;

	AS3_ArrayValue( args, "PtrType, PtrType", & lights, & node );

	lp = lights;

	if( lp == NULL )
	{
		return 0;
	}
	//scene_addLight( scene, light );

	while( lp -> next != NULL )
	{
		lp = lp -> next;
	}

	lp -> next = node;


	return 0;
}

AS3_Val removeLight( void * self, AS3_Val args )
{
	Lights * lights, * node, * lp;

	unsigned int single;

	AS3_ArrayValue( args, "PtrType, PtrType, IntType", & lights, & node, & single );

	lp = lights;

	while( lp != NULL && lp -> next == node )
	{
		lp = lp -> next;
	}

	if( lp -> next != NULL )
	{
		lp   -> next = single ? node -> next : NULL;
		node -> next = single ? NULL : node -> next;
	}

	return 0;
}

AS3_Val initializeFog( void* self, AS3_Val args )
{
	Fog * fog;

	AS3_Val color;

	double a, r, g, b, distance, depth;

	AS3_ArrayValue( args, "AS3ValType, DoubleType, DoubleType", & color, & distance, & depth );

	r = AS3_NumberValue( AS3_GetS( color, "redMultiplier" ) );
	g = AS3_NumberValue( AS3_GetS( color, "greenMultiplier" ) );
	b = AS3_NumberValue( AS3_GetS( color, "blueMultiplier" ) );
	a = AS3_NumberValue( AS3_GetS( color, "alphaMultiplier" ) );

	fog = newFog( newColorValue( (float)r, (float)g, (float)b, (float)a ), (float)distance, (float)depth );

	AS3_Release( color );

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType", fog, fog->global, & fog->distance, & fog->depth );
}

AS3_Val addFog( void * self, AS3_Val args )
{
	Scene * scene;

	Fog * fog;

	AS3_ArrayValue( args, "PtrType, PtrType", & scene, & fog );

	scene_addFog( scene, fog );

	return 0;
}

AS3_Val removeFog( void * self, AS3_Val args )
{
	Scene * scene;

	Fog * fog;

	AS3_ArrayValue( args, "PtrType, PtrType", & scene, & fog );

	scene_removeFog( scene, fog );

	return 0;
}

AS3_Val initializePrimitives( void * self, AS3_Val args )
{
	Mesh     * base;
	Material * material;
	Texture  * texture;
	double width, height; 
	DWORD segments_width, segments_height;
	DWORD render_mode;

	AS3_ArrayValue( args, "PtrType, PtrType, PtrType, DoubleType, DoubleType, IntType, IntType, IntType", & base, & material, & texture, & width, & height, & segments_width, & segments_height, & render_mode );

	newPlane( base, material, texture, (float)width, (float)height, segments_width, segments_height, render_mode );

	return 0;
}

AS3_Val initializeTerrain( void * self, AS3_Val args )
{
	Entity * entity;
	Material * material;
	Texture * texture;
	Bitmap * map;
	double width, height, maxHeight;
	int addressMode;
	DWORD render_mode;

	AS3_ArrayValue( args, "PtrType, PtrType, PtrType, PtrType, DoubleType, DoubleType, DoubleType, IntType, IntType", 
		& entity, & map, & material, & texture, & width, & height, & maxHeight, & render_mode, & addressMode );

	newTerrain( entity, map, ( float )width, ( float )height, ( float )maxHeight, material, texture, render_mode, addressMode );

	return 0;
}

AS3_Val initializeMesh( void * self, AS3_Val args )
{
	Mesh         * mesh;

	Texture      * * tp;
	Material     * * mp;

	unsigned int   vl, fl, i, * fs;

	int          * rmp;

	float        * vs;

	Vector3D     * * vp;

	Vector       * uvp;

	AS3_ArrayValue( args, "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, IntType, IntType",& vs, & fs, & uvp, & mp, & tp, & rmp, & vl, & fl );

	mesh = newMesh( vl, fl );

	for( i = 0; i < vl; i ++ )
	{
		mesh_push_vertex( mesh, vs[i * 3], vs[i * 3 + 1], vs[i * 3 + 2] );
	}

	free( vs );

	for( i = 0; i < fl; i ++ )
	{
		mesh_push_triangle( 
			mesh,
			(* mesh -> vertices) + fs[i * 3], 
			(* mesh -> vertices) + fs[i * 3 + 1], 
			(* mesh -> vertices) + fs[i * 3 + 2],
			uvp + i * 3    ,
			uvp + i * 3 + 1,
			uvp + i * 3 + 2,
			mp[i],
			tp[i],
			rmp[i] );
	}

	free( fs );

	free( rmp );

	if( ( vp = ( Vector3D * *   )malloc( sizeof( Vector3D * ) * mesh -> nVertices ) ) == NULL )
	{
		return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", mesh, & mesh->lightEnable, & mesh->fogEnable, & mesh->useMipmap, & mesh->terrainTrace, & mesh->mip_dist, & mesh->v_dirty, & mesh->octree_depth );
	}

	for( i = 0; i < mesh -> nVertices; i ++ )
	{
		vp[i] = mesh -> vertices[i]->position;
	}

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", mesh, & mesh->lightEnable, & mesh->fogEnable, & mesh->useMipmap, & mesh->terrainTrace, & mesh->mip_dist, & mesh->v_dirty, & mesh->octree_depth, & vp );
}

AS3_Val setMeshAttribute( void* self, AS3_Val args )
{
	Mesh * mesh;

	int key;

	void * value;

	AS3_ArrayValue( args, "PtrType, IntType, PtrType", & mesh, & key, & value );

	switch ( key )
	{
		case MATERIAL_KEY:

			mesh_setMaterial( mesh, ( Material * )value );

			break;
			
		case TEXTURE_KEY:

			mesh_setTexture( mesh, ( Texture * )value );

			break;
			
		case RENDERMODE_KEY:

			mesh_setRenderMode( mesh, ( DWORD )value );

			break;
			
		case FOG_KEY:

			mesh->fogEnable = ( DWORD )value;

			break;
			
		case LIGHT_KEY:

			mesh->lightEnable = ( DWORD )value;

			break;
	}

	return 0;
}

AS3_Val initializeEntity( void* self, AS3_Val args )
{
	Entity * entity;

	SceneNode * node;

	Mesh * mesh;

	char * name;

	AS3_ArrayValue( args, "StrType, PtrType",  &name, &mesh );

	entity = newEntity();

	entity->name = name;

	if ( mesh ) entity->mesh = mesh;

	if( ( node = ( SceneNode * )malloc( sizeof( SceneNode ) ) ) == NULL )
	{
		exit( TRUE );
	}

	node -> entity = entity;
	node -> next   = NULL;

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType",
		entity, entity->position, entity->direction, entity->scale, entity->w_pos, & entity->mesh, & entity->transformDirty, node );
}

AS3_Val addEntity( void* self, AS3_Val args )
{
	SceneNode * parent, * node, * ep;

	unsigned int isChild;

	AS3_ArrayValue( args, "PtrType, PtrType, IntType",  & parent, & node, & isChild );

	if( parent == NULL )
	{
		return 0;
	}

	ep = isChild ? node : parent;

	if( ep == NULL )
	{
		return 0;
	}

	while( ep -> next != NULL )
	{
		ep = ep -> next;
		ep -> entity -> scene = parent -> entity -> scene;
	}

	if( isChild )
	{
		ep     -> next           = parent -> next;

		parent -> next           = node;

		node -> entity -> parent = parent -> entity;

		( parent -> entity -> nChildren ) ++;
	}
	else
	{
		ep   -> next             = node;

		node -> entity -> parent = NULL;
	}

	node -> entity -> scene = parent -> entity -> scene;

	return 0;
}

AS3_Val removeEntity(  void* self, AS3_Val args )
{
	SceneNode * parent, * node, * ep;

	AS3_ArrayValue( args, "PtrType, PtrType", & parent, & node );

	while( ep != NULL && ep -> next != node )
	{
		ep = ep -> next;
	}

	if( ep != NULL )
	{
		ep -> next   = node -> next;
		node -> next = NULL;
	}

	return 0;
}

AS3_Val initialize3DS( void* self, AS3_Val args )
{
	FILE * file;
	void * dest;
	Entity * entity;
	A3DS * a3ds;
	DWORD render_mode;

	AS3_ArrayValue(args, "AS3ValType, PtrType, IntType", &dest, &entity, &render_mode);

	file = funopen((void *)dest, readByteArray, writeByteArray, seekByteArray, closeByteArray);

	a3ds = A3DS_Create( file, entity, render_mode );

	return AS3_Array( "PtrType, IntType, IntType, PtrType, IntType, IntType, IntType", a3ds, a3ds->mNum, a3ds->tNum, a3ds->a3d_materialList->next, FPOS( A3DS_MaterialList, next ), FPOS( A3DS_MaterialList, texture ), FPOS( Texture, name ) );
}

AS3_Val loadComplete3DS( void* self, AS3_Val args )
{
	A3DS * a3ds = NULL;

	AS3_ArrayValue( args, "PtrType", &a3ds );

	A3DS_Dispose( a3ds );

	return 0;
}


AS3_Val initializeMD2( void* self, AS3_Val args )
{
	MD2 * md2;
	Entity * entity;
	Material * material;
	Texture * texture;
	UCHAR * buffer;
	int render_mode;
	unsigned int fps;

	AS3_ArrayValue( args, "PtrType, PtrType, PtrType, PtrType, IntType, IntType", & entity, & buffer, & material, & texture, & render_mode, & fps );

	md2 = newMD2(entity);

	md2_read( & buffer, md2, material, texture, render_mode, fps );

	//return AS3_Array( "PtrType, PtrType, PtrType, PtrType", 
	//	& entity->mesh->render_mode, & entity->mesh->dirty, & entity->mesh->material, & entity->mesh->texture );
	return AS3_Array( "PtrType, PtrType, PtrType, PtrType", 
		& md2 -> entity -> animation -> isPlay, & md2 -> entity -> animation -> loop, & md2 -> entity -> animation -> dirty, md2 -> entity -> animation -> currentFrameName );
}

///////////////////////////////////////////////////////////////////////////////////
//
//                                     Material
//
///////////////////////////////////////////////////////////////////////////////////

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

	material = newMaterial( newColorValue( (float)a_r, (float)a_g, (float)a_b, (float)a_a ),
							newColorValue( (float)d_r, (float)d_g, (float)d_b, (float)d_a ),
							newColorValue( (float)s_r, (float)s_g, (float)s_b, (float)s_a ),
							newColorValue( (float)e_r, (float)e_g, (float)e_b, (float)e_a ),
							power );

	AS3_Release( ambient );
	AS3_Release( diffuse );
	AS3_Release( specular );
	AS3_Release( emissive );

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", material, material->ambient, material->diffuse, material->specular, material->emissive, &material->power );
}

///////////////////////////////////////////////////////////////////////////////////
//
//                                     Texture
//
///////////////////////////////////////////////////////////////////////////////////

AS3_Val initializeBitmap( void* self, AS3_Val args )
{
	Bitmap * bitmap;
	int width, height;
	LPBYTE data;

	AS3_ArrayValue( args, "IntType, IntType, PtrType", &width, &height, &data );

	bitmap = newBitmap( width, height, data );

	return AS3_Ptr( bitmap );
}

AS3_Val initializeTexture( void* self, AS3_Val args )
{
	char * name;

	Texture * texture;

	AS3_ArrayValue( args, "StrType", &name );

	texture = newTexture( name );

	return AS3_Array( "PtrType, PtrType, PtrType", texture, & texture->perspective_dist, & texture -> addressMode );
}

AS3_Val setMipmap( void* self, AS3_Val args )
{
	Texture * texture;
	Bitmap * bitmap;

	AS3_ArrayValue( args, "PtrType, PtrType", &texture, &bitmap );

	texture_setMipmap( texture, bitmap );

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

AS3_Val freeTmpBuffer( void * self, AS3_Val args )
{
	void * pointer;

	AS3_ArrayValue( args, "PtrType", & pointer );

	free( pointer );

	return 0;
}

AS3_Val render( void* self, AS3_Val args )
{
	Viewport * viewport;

	int time;

	AS3_ArrayValue( args, "PtrType, IntType", &viewport, &time );

	viewport_updateBeforeRender( viewport );

	viewport_project( viewport, time );

	viewport_render( viewport );

	viewport_updateAfterRender( viewport );

	return 0;
}

//测试函数
AS3_Val test( void* self, AS3_Val args )
{
	return 0;
}

AS3_Val test2( void* self, AS3_Val args )
{
	//FILE * file;
	//long fileSize;
	//char * buffer;
	//void * dest;

	//AS3_ArrayValue(args, "AS3ValType", &dest);

	//file = funopen((void *)dest, readByteArray, writeByteArray, seekByteArray, closeByteArray);

	//fseek (file, 0, SEEK_END);
	//fileSize = ftell(file);
	//rewind (file);

	return 0;
}

//入口
int main()
{
	AS3_Val initializeCameraMethod = AS3_Function( NULL, initializeCamera );
	AS3_Val attachCameraMethod = AS3_Function( NULL, attachCamera );
	AS3_Val initializeSceneMethod = AS3_Function( NULL, initializeScene );
	AS3_Val attachSceneMethod = AS3_Function( NULL, attachScene );
	AS3_Val initializeViewportMethod = AS3_Function( NULL, initializeViewport );
	AS3_Val initializeMaterialMethod = AS3_Function( NULL, initializeMaterial );
	AS3_Val initializeBitmapMethod = AS3_Function( NULL, initializeBitmap );
	AS3_Val initializeTextureMethod = AS3_Function( NULL, initializeTexture );
	AS3_Val setMipmapMethod = AS3_Function( NULL, setMipmap );
	AS3_Val initializeLightMethod = AS3_Function( NULL, initializeLight );
	AS3_Val addLightMethod = AS3_Function( NULL, addLight );
	AS3_Val removeLightMethod = AS3_Function( NULL, removeLight );
	AS3_Val initializeFogMethod = AS3_Function( NULL, initializeFog );
	AS3_Val addFogMethod = AS3_Function( NULL, addFog );
	AS3_Val removeFogMethod = AS3_Function( NULL, removeFog );
	AS3_Val initializePrimitivesMethod = AS3_Function( NULL, initializePrimitives );
	AS3_Val initializeTerrainMethod = AS3_Function( NULL, initializeTerrain );
	AS3_Val initializeMeshMethod = AS3_Function( NULL, initializeMesh );
	AS3_Val setMeshAttributeMethod = AS3_Function( NULL, setMeshAttribute );
	AS3_Val initializeEntityMethod = AS3_Function( NULL, initializeEntity );
	AS3_Val addEntityMethod = AS3_Function( NULL, addEntity );
	AS3_Val removeEntityMethod = AS3_Function( NULL, removeEntity );
	AS3_Val initialize3DSMethod = AS3_Function( NULL, initialize3DS );
	AS3_Val loadComplete3DSMethod = AS3_Function( NULL, loadComplete3DS );
	AS3_Val applyForTmpBufferMethod = AS3_Function( NULL, applyForTmpBuffer );
	AS3_Val freeTmpBufferMethod = AS3_Function( NULL, freeTmpBuffer );
	AS3_Val renderMethod = AS3_Function( NULL, render );
	AS3_Val initializeMD2Method = AS3_Function( NULL, initializeMD2 );
	AS3_Val testMethod = AS3_Function( NULL, test );
	AS3_Val test2Method = AS3_Function( NULL, test2 );



	AS3_Val result = AS3_Object( "initializeCamera:AS3ValType,\
								 attachCamera:AS3ValType,\
								 initializeScene:AS3ValType,\
								 attachScene:AS3ValType,\
								 initializeViewport:AS3ValType,\
								 initializeMaterial:AS3ValType,\
								 initializeBitmap:AS3ValType,\
								 initializeTexture:AS3ValType,\
								 setMipmap:AS3ValType,\
								 initializeLight:AS3ValType,\
								 addLight:AS3ValType,\
								 removeLight:AS3ValType,\
								 initializeFog:AS3ValType,\
								 addFog:AS3ValType,\
								 removeFog:AS3ValType,\
								 initializePrimitives:AS3ValType,\
								 initializeTerrain:AS3ValType,\
								 initializeMesh:AS3ValType,\
								 setMeshAttribute:AS3ValType,\
								 initializeEntity:AS3ValType,\
								 addEntity:AS3ValType,\
								 removeEntity:AS3ValType,\
								 initialize3DS:AS3ValType,\
								 loadComplete3DS:AS3ValType,\
								 applyForTmpBuffer:AS3ValType,\
								 freeTmpBuffer:AS3ValType,\
								 render:AS3ValType,\
								 initializeMD2:AS3ValType,\
								 test:AS3ValType,\
								 test2:AS3ValType",
								initializeCameraMethod,
								attachCameraMethod,
								initializeSceneMethod,
								attachSceneMethod,
								initializeViewportMethod,
								initializeMaterialMethod,
								initializeBitmapMethod,
								initializeTextureMethod,
								setMipmapMethod,
								initializeLightMethod,
								addLightMethod,
								removeLightMethod,
								initializeFogMethod,
								addFogMethod,
								removeFogMethod,
								initializePrimitivesMethod,
								initializeTerrainMethod,
								initializeMeshMethod,
								setMeshAttributeMethod,
								initializeEntityMethod,
								addEntityMethod,
								removeEntityMethod,
								initialize3DSMethod,
								loadComplete3DSMethod,
								applyForTmpBufferMethod,
								freeTmpBufferMethod,
								renderMethod,
								initializeMD2Method,
								testMethod,
								test2Method );


	AS3_Release( initializeCameraMethod );
	AS3_Release( attachCameraMethod );
	AS3_Release( initializeSceneMethod );
	AS3_Release( attachSceneMethod );
	AS3_Release( initializeViewportMethod );
	AS3_Release( initializeMaterialMethod );
	AS3_Release( initializeBitmapMethod );
	AS3_Release( initializeTextureMethod );
	AS3_Release( setMipmapMethod );
	AS3_Release( initializeLightMethod );
	AS3_Release( addLightMethod );
	AS3_Release( removeLightMethod );
	AS3_Release( initializeFogMethod );
	AS3_Release( addFogMethod );
	AS3_Release( removeFogMethod );
	AS3_Release( initializePrimitivesMethod );
	AS3_Release( initializeTerrainMethod );
	AS3_Release( initializeMeshMethod );
	AS3_Release( setMeshAttributeMethod );
	AS3_Release( initializeEntityMethod );
	AS3_Release( addEntityMethod );
	AS3_Release( removeEntityMethod );
	AS3_Release( initialize3DSMethod );
	AS3_Release( loadComplete3DSMethod );
	AS3_Release( applyForTmpBufferMethod );
	AS3_Release( freeTmpBufferMethod );
	AS3_Release( renderMethod );
	AS3_Release( initializeMD2Method );
	AS3_Release( testMethod );
	AS3_Release( test2Method );


	AS3_LibInit( result );

	return 0;
}
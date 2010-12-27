#define INLINE inline
#define __AS3__
//#define RGB565

//#include <GL/gl.h>
#include <stdlib.h>
#include <AS3.h>

#include "Base.h"
#include "Math3D.h"

BYTE logbase2ofx[2048];
//BYTE alpha_table[NUM_ALPHA_LEVELS][256];
float dot5miplevel_table[11];
//DWORD multiply256_table[256][256];
//DWORD multiply256FIXP8_table[256][256];
//BYTE alphaTable[256][65536];
BYTE *alphaTable;

#include "Entity.h"
#include "Material.h"
#include "Texture.h"
#include "Bitmap.h"
#include "Light.h"
#include "Scene.h"
#include "Viewport.h"
#include "Triangle.h"
#include "Mesh.h"
#include "Color888.h"
#include "3DS.h"
#include "Md2.h"
#include "Primitives.h"
#include "AS3File.h"
#include "Terrain.h"

AS3_Val initializeCamera( void* self, AS3_Val args )
{
	Camera * camera;

	Entity * eye;

	double fov, nearClip, farClip;

	AS3_ArrayValue( args, "PtrType, DoubleType, DoubleType, DoubleType", & eye, & fov, & nearClip, & farClip );

	nearClip = nearClip < 5.0f ? 5.0f : nearClip;

	camera = newCamera( (float)fov, (float)nearClip, (float)farClip, eye );

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType",
					camera, camera->target, & camera->fov, & camera->near, & camera->far, & camera->fnfDirty, & camera->UVNType, &camera->rotationDirty, &camera->rotateX, &camera->rotateY, &camera->rotateZ, &camera->distance, &camera->factor, &camera->step);
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

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", scene, & scene->nNodes, & scene->nVertices, & scene->nFaces, & scene->nRenderList, & scene->nCullList, & scene->nClippList, &scene->terrain );
}

AS3_Val attachScene( void* self, AS3_Val args )
{
	Scene * scene;

	//Viewport * viewport;

	SceneNode * entityList, * ep;

	Lights * lights;

	//int ID = 0;

	AS3_ArrayValue( args, "PtrType, PtrType, PtrType", & scene, & entityList, & lights );

	//viewport->scene = scene;

	//修改
	ep = entityList;

	while( ep != NULL )
	{
		//ep -> ID = ++ ID;
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

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", view, view->videoBuffer, view->zBuffer, & view->camera, & view->scene, &view->sortTransList );
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

	if( lights == node )
	{
		node -> next = NULL;

		return 0;
	}

	lp = lights;

	while( lp != NULL && lp -> next != node )
	{
		lp = lp -> next;
	}

	if( lp != NULL )
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

	double a, r, g, b, density, start, end;

	int mode;

	AS3_ArrayValue( args, "AS3ValType, DoubleType, DoubleType, DoubleType, IntType", & color, & start, & end, & density, & mode );

	r = AS3_NumberValue( AS3_GetS( color, "redMultiplier" ) );
	g = AS3_NumberValue( AS3_GetS( color, "greenMultiplier" ) );
	b = AS3_NumberValue( AS3_GetS( color, "blueMultiplier" ) );
	a = AS3_NumberValue( AS3_GetS( color, "alphaMultiplier" ) );

	fog = newFog( newColorValue( (float)r, (float)g, (float)b, (float)a ), (float)start, (float)end, (float)density, mode );

	AS3_Release( color );

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", fog, fog->global, & fog->start, & fog->end, & fog->density, & fog->mode );
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
	Terrain* terrain;

	AS3_ArrayValue( args, "PtrType, PtrType, PtrType, PtrType, DoubleType, DoubleType, DoubleType, IntType, IntType", 
		& entity, & map, & material, & texture, & width, & height, & maxHeight, & render_mode, & addressMode );

	terrain = newTerrain( entity, map, ( float )width, ( float )height, ( float )maxHeight, material, texture, render_mode, addressMode );

	return AS3_Ptr(terrain);
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

	//free( vs );

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

	//free( fs );

	//free( rmp );

	if( ( vp = ( Vector3D * *   )malloc( sizeof( Vector3D * ) * mesh -> nVertices ) ) == NULL )
	{
		return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType",
							mesh,
							& mesh->lightEnable,
							& mesh->fogEnable,
							& mesh->useMipmap,
							//& mesh->terrainTrace,
							& mesh->mip_dist,
							& mesh->v_dirty, 
							& mesh->octree_depth,
							& mesh->addressMode,
							& mesh->texTransform->rotation,
							mesh->texTransform->offset,
							mesh->texTransform->scale,
							& mesh->texTransformDirty,
							& mesh->hit,
							& mesh->skinMeshController);
	}

	for( i = 0; i < mesh -> nVertices; i ++ )
	{
		vp[i] = mesh -> vertices[i]->position;
	}

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType",
						mesh,
						& mesh->lightEnable,
						& mesh->fogEnable,
						& mesh->useMipmap,
						//& mesh->terrainTrace,
						& mesh->mip_dist,
						& mesh->v_dirty,
						& mesh->octree_depth,
						& mesh->addressMode,
						& mesh->texTransform->rotation, 
						mesh->texTransform->offset,
						mesh->texTransform->scale,
						& mesh->texTransformDirty,
						& mesh->hit,
						& mesh->skinMeshController,
						& vp);
}

AS3_Val destroyMesh( void* self, AS3_Val args )
{
	Mesh *mesh = AS3_PtrValue(args);

	mesh_destroy(&mesh);

	return 0;
}

AS3_Val cloneMesh( void * self, AS3_Val args )
{
	Mesh* src, *mesh;

	Material* material;

	Texture* texture;

	int render_mode, i;

	Vector3D** vp;

	AS3_ArrayValue( args, "PtrType, PtrType, PtrType, IntType",& src, & material, & texture, & render_mode);

	mesh = mesh_clone(src, material, texture, render_mode);

	if( ( vp = ( Vector3D * *   )malloc( sizeof( Vector3D * ) * mesh -> nVertices ) ) == NULL )
	{
		return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType",
							mesh,
							& mesh->lightEnable,
							& mesh->fogEnable,
							& mesh->useMipmap,
							//& mesh->terrainTrace,
							& mesh->mip_dist,
							& mesh->v_dirty, 
							& mesh->octree_depth,
							& mesh->addressMode,
							& mesh->texTransform->rotation,
							mesh->texTransform->offset,
							mesh->texTransform->scale,
							& mesh->texTransformDirty,
							& mesh->hit,
							& mesh->skinMeshController);
	}

	for( i = 0; i < mesh -> nVertices; i ++ )
	{
		vp[i] = mesh -> vertices[i]->position;
	}

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType",
						mesh,
						& mesh->lightEnable,
						& mesh->fogEnable,
						& mesh->useMipmap,
						//& mesh->terrainTrace,
						& mesh->mip_dist,
						& mesh->v_dirty,
						& mesh->octree_depth,
						& mesh->addressMode,
						& mesh->texTransform->rotation, 
						mesh->texTransform->offset,
						mesh->texTransform->scale,
						& mesh->texTransformDirty,
						& mesh->hit,
						& mesh->skinMeshController,
						& vp );
}

AS3_Val initializeSkinMeshController( void* self, AS3_Val args )
{
	int numBones;
	Weight** weights;
	int* weightsLength;
	Vector3D* vertices;
	Matrix4x4* boneOffsetMatrices;
	Entity** bones;

	SkinMeshController* controller;

	AS3_ArrayValue( args, "IntType, PtrType, PtrType, PtrType, PtrType, PtrType",& numBones, & weights, &weightsLength, & vertices, & boneOffsetMatrices, & bones);

	controller = newSkinMeshController(numBones, weights, weightsLength, vertices, boneOffsetMatrices, bones);

	return AS3_Array("PtrType, PtrType", controller, &controller->numBones);
}

AS3_Val destroySkinMeshController( void* self, AS3_Val args )
{
	SkinMeshController *skinMeshController = AS3_PtrValue(args);

	skinMeshController_destroy(&skinMeshController);

	return 0;
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
			
		case ALPHA_KEY:

			mesh_setAlpha( mesh, ( int )value );

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
	node -> ID     = 0;
	node -> next   = NULL;

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType",
		entity, 
		entity->position, 
		entity->direction, 
		entity->scale, 
		entity->w_pos, 
		& entity->mesh, 
		& entity->transformDirty, 
		node, 
		&node->ID, 
		&entity->isBone, 
		entity->transform, 
		&entity->matrixDirty, 
		&entity->morphChannel, 
		&entity->skeletalChannel,
		&entity->terrainTrace,
		entity->aabb,
		entity->worldAABB,
		&entity->visible);
}

AS3_Val destroyEntity( void* self, AS3_Val args )
{
	SceneNode * node;

	node = AS3_PtrValue(args);

	entity_destroy(&node->entity);

	sceneNode_destroy(&node);

	return 0;
}

AS3_Val decomposeEntity( void* self, AS3_Val args )
{
	Entity * entity;

	entity = AS3_PtrValue(args);

	matrix4x4_decompose(entity->transform, entity->position, entity->scale, entity->direction, NULL);

	entity->matrixDirty = TRUE;

	return 0;
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

		//node -> ID = parent -> entity -> nChildren + parent -> ID;
	}
	else
	{
		//int ID = ep -> ID;

		ep   -> next             = node;

		node -> entity -> parent = NULL;

		while( ep != NULL )
		{
			//ep -> ID = ID ++;
			ep       = ep -> next;
		}
	}

	node -> entity -> scene = parent -> entity -> scene;

	return 0;
}

AS3_Val removeEntity(  void* self, AS3_Val args )
{
	SceneNode * parent, * child;

	AS3_ArrayValue( args, "PtrType, PtrType", & parent, & child );

	//AS3_ArrayValue( args, "PtrType, PtrType", & parent, & node );

	entity_removeChild(parent, child);
	/*if( node == NULL )
	{
		return 0;
	}*/

	/*if( node == parent )
	{
		while( node -> next != NULL && node -> next -> entity -> parent == parent -> entity )
		{
			node = node -> next;
		}

		//ep = node -> next;

		node -> next = NULL;

		return 0;
	}

	ep = parent;

	while( ep != NULL && ep -> next != node )
	{
		ep = ep -> next;
	}

	if( ep != NULL )
	{
		int ID = ep -> ID;

		while( node -> next != NULL && node -> next -> entity -> parent == ep -> next -> entity )
		{
			node = node -> next;
		}

		ep -> next   = node -> next;

		while( ep != NULL )
		{
			ep -> ID = ID ++;
			ep       = ep -> next;
		}

		node -> next = NULL;
	}*/

	return 0;
}

AS3_Val initializeAnimation( void* self, AS3_Val args )
{
	Animation* a;
	Entity* parent;
	int isPlay, loop;
	int maxTime, minTime;

	AS3_ArrayValue(args, "PtrType, IntType, IntType, IntType, IntType", &parent, &isPlay, &loop, &maxTime, &minTime);

	a = newAnimation(isPlay, loop, maxTime, minTime);

	parent->animation = a;

	return AS3_Array("PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", a, &a->isPlay, &a->loop, &a->minTime, &a->maxTime, &a->durationTime, &a->time, &a->timeMode);
}

AS3_Val destroyAnimation( void* self, AS3_Val args )
{
	Animation *animation;

	animation = AS3_PtrValue(args);

	animation_destroy(&animation);

	return 0;
}

AS3_Val initializeSkeletalChannel( void* self, AS3_Val args )
{
	int channelType;
	int length;
	Animation* animation;
	float* times;
	float** param;
	Matrix4x4* matrices;
	float* interpolations;
	Vector* inTangent;
	Vector* outTangent;
	SkeletalChannel* channel;

	AS3_ArrayValue(args, "IntType, IntType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", 
		&channelType, &length, &animation, &times, &param, &matrices,&interpolations, &inTangent, &outTangent);

	channel = newSkeletalChannel(animation, length, channelType, times, param, matrices, interpolations, inTangent, outTangent);

	return AS3_Ptr(channel);
}

AS3_Val destroySkeletalChannel( void* self, AS3_Val args )
{
	SkeletalChannel *channel;

	channel = AS3_PtrValue(args);

	skeletalChannel_destroy(&channel);

	return 0;
}

AS3_Val initializeMorphChannel( void* self, AS3_Val args )
{
	Animation* animation;
	int length, i, duration = 0;
	char* name;
	float* times;
	Vector3D** vertices;
	int* verticesLength;
	MorphChannel* channel;
	Frame* frames;

	AS3_ArrayValue(args, "PtrType, IntType, PtrType, PtrType, PtrType, PtrType", &animation, &length, &name, &times, &vertices, &verticesLength);

	if(length)
	{
		if( ( frames = ( Frame * )malloc(sizeof(Frame) * length) ) == NULL ) 
			exit( TRUE );

		for(i = 0; i < length; i ++)
		{
			frames[i].length   = verticesLength[i];
			frames[i].vertices = vertices[i];
			frames[i].time     = times[i];

			memcpy(frames[i].name, name + i * FRAME_NAME_LENGTH, FRAME_NAME_LENGTH);

			duration += frames[i].time;
		}
	}

	channel = newMorphChannel(animation, frames, length, duration);

	return AS3_Array("PtrType, PtrType, PtrType, PtrType", channel, &channel->dirty, channel->currentFrameName, &channel->nameLength);
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
	Bitmap * pBitmapData;
	Texture * pTexture;

	AS3_ArrayValue( args, "PtrType, PtrType, PtrType", &a3ds, &pTexture, &pBitmapData );

	texture_setMipmap( pTexture, pBitmapData, OFF );

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
	return 0;
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

	material = newMaterial( newColor888( (BYTE)(a_r * 255), (BYTE)(a_g * 255), (BYTE)(a_b * 255), (BYTE)(a_a * 255) ),
							newColor888( (BYTE)(d_r * 255), (BYTE)(d_g * 255), (BYTE)(d_b * 255), (BYTE)(d_a * 255) ),
							newColor888( (BYTE)(s_r * 255), (BYTE)(s_g * 255), (BYTE)(s_b * 255), (BYTE)(s_a * 255) ),
							newColor888( (BYTE)(e_r * 255), (BYTE)(e_g * 255), (BYTE)(e_b * 255), (BYTE)(e_a * 255) ),
							power );

	AS3_Release( ambient );
	AS3_Release( diffuse );
	AS3_Release( specular );
	AS3_Release( emissive );

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", material, material->ambient, material->diffuse, material->specular, material->emissive, &material->power, &material->doubleSide );
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

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType", texture, & texture->perspective_dist, &texture->perspectiveLP_dist, &texture->alphaTestRef, &texture->numMipLevels);
}

AS3_Val setMipmap( void* self, AS3_Val args )
{
	Texture * texture;
	Bitmap * bitmap;
	int numMipLevels;

	AS3_ArrayValue( args, "PtrType, PtrType, IntType", &texture, &bitmap, &numMipLevels );

	texture_setMipmap( texture, bitmap, numMipLevels );

	return 0;
}

AS3_Val getTextureLevelDesc( void* self, AS3_Val args )
{
	Texture* texture;
	int levels;
	
	AS3_ArrayValue( args, "PtrType, IntType", &texture, &levels);

	if(texture->mipmaps)
	{
		levels = MIN(MAX(levels, 0), texture->numMipLevels);

		return AS3_Array("IntType, IntType, PtrType", texture->mipmaps[levels]->width, texture->mipmaps[levels]->height, texture->mipmaps[levels]->pRGBABuffer);
	}

	return 0;
}

//DWORD g_TestColor[262144];
#define SWAP_ENDIAN(color) ( (color << 24) | ( (color << 8) & 0xff0000 ) | ( (color >> 8) & 0xff00 ) | (color >> 24) )

AS3_Val swapTextureEndian(void* self, AS3_Val args)
{
	int i, j, x, y, width, height, pitch, position;
	DWORD* data;
	DWORD color;

	AS3_ArrayValue(args, "IntType, IntType, IntType, IntType, PtrType, IntType", &x, &y, &width, &height, &data, &pitch);

	position = x + y * pitch;
	pitch = pitch - width;

	for(i = y; i < height; i ++)
	{
		for(j = x; j < width; j ++)
		{
			color = data[position];

			data[position] = SWAP_ENDIAN(color);

			position ++;
		}

		position += pitch;
	}

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
	
	double mouseX, mouseY;//, hit = 0;

	AS3_ArrayValue( args, "PtrType, IntType, DoubleType, DoubleType", &viewport, &time, & mouseX, & mouseY );

	viewport->mouseX = (int)mouseX;
	viewport->mouseY = (int)mouseY;

	//AS3_Trace( AS3_Int(viewport->mouseX) );
	//AS3_Trace( AS3_Int(viewport->mouseY) );

	viewport_updateBeforeRender( viewport );

	viewport_project( viewport, time );

	//hit = viewport_mouseOn( viewport, (float)mouseX, (float)mouseY );
	
	viewport_render( viewport );

	viewport_mouseOn( viewport, (int)mouseX, (int)mouseY );

	viewport_updateAfterRender( viewport );

	viewport->numFrames ++;

	return 0;//AS3_Int( hit );
}

//测试函数
AS3_Val test( void* self, AS3_Val args )
{
	//int i;
	//DWORD color;
	
	//for(i = 0; i < 262144; i++)
	//{
		//color = g_TestColor[i];

		//g_TestColor[i] = SWAP_ENDIAN(color);
	//}

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

	return 0;//AS3_Ptr(&g_TestColor);
}

//入口
int main()
{
	log2base_Table_Builder(logbase2ofx);
	//alpha_Table_Builder(NUM_ALPHA_LEVELS, alpha_table);
	//multiply256_Table_Builder(multiply256_table);
	//multiply256FIXP8_Table_Builder(multiply256FIXP8_table);
	dot5miplevel_Table_Builder(dot5miplevel_table);
	//buildAlphaTable(alphaTable);

	AS3_Val initializeCameraMethod = AS3_Function( NULL, initializeCamera );
	AS3_Val attachCameraMethod = AS3_Function( NULL, attachCamera );
	AS3_Val initializeSceneMethod = AS3_Function( NULL, initializeScene );
	AS3_Val attachSceneMethod = AS3_Function( NULL, attachScene );
	AS3_Val initializeViewportMethod = AS3_Function( NULL, initializeViewport );
	AS3_Val initializeMaterialMethod = AS3_Function( NULL, initializeMaterial );
	AS3_Val initializeBitmapMethod = AS3_Function( NULL, initializeBitmap );
	AS3_Val initializeTextureMethod = AS3_Function( NULL, initializeTexture );
	AS3_Val setMipmapMethod = AS3_Function( NULL, setMipmap );
	AS3_Val getTextureLevelDescMethod = AS3_Function( NULL, getTextureLevelDesc );
	AS3_Val swapTextureEndianMethod = AS3_Function( NULL, swapTextureEndian );
	AS3_Val initializeLightMethod = AS3_Function( NULL, initializeLight );
	AS3_Val addLightMethod = AS3_Function( NULL, addLight );
	AS3_Val removeLightMethod = AS3_Function( NULL, removeLight );
	AS3_Val initializeFogMethod = AS3_Function( NULL, initializeFog );
	AS3_Val addFogMethod = AS3_Function( NULL, addFog );
	AS3_Val removeFogMethod = AS3_Function( NULL, removeFog );
	AS3_Val initializePrimitivesMethod = AS3_Function( NULL, initializePrimitives );
	AS3_Val initializeTerrainMethod = AS3_Function( NULL, initializeTerrain );
	AS3_Val initializeMeshMethod = AS3_Function( NULL, initializeMesh );
	AS3_Val destroyMeshMethod = AS3_Function( NULL, destroyMesh );
	AS3_Val cloneMeshMethod = AS3_Function( NULL, cloneMesh );
	AS3_Val initializeSkinMeshControllerMethod = AS3_Function( NULL, initializeSkinMeshController );
	AS3_Val destroySkinMeshControllerMethod = AS3_Function( NULL, destroySkinMeshController );
	AS3_Val setMeshAttributeMethod = AS3_Function( NULL, setMeshAttribute );
	AS3_Val initializeEntityMethod = AS3_Function( NULL, initializeEntity );
	AS3_Val decomposeEntityMethod = AS3_Function( NULL, decomposeEntity );
	AS3_Val addEntityMethod = AS3_Function( NULL, addEntity );
	AS3_Val removeEntityMethod = AS3_Function( NULL, removeEntity );
	AS3_Val initializeAnimationMethod = AS3_Function( NULL, initializeAnimation);
	AS3_Val initializeSkeletalChannelMethod = AS3_Function( NULL, initializeSkeletalChannel );
	AS3_Val initializeMorphChannelMethod = AS3_Function( NULL, initializeMorphChannel );
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
								 getTextureLevelDesc:AS3ValType,\
								 swapTextureEndian:AS3ValType,\
								 initializeLight:AS3ValType,\
								 addLight:AS3ValType,\
								 removeLight:AS3ValType,\
								 initializeFog:AS3ValType,\
								 addFog:AS3ValType,\
								 removeFog:AS3ValType,\
								 initializePrimitives:AS3ValType,\
								 initializeTerrain:AS3ValType,\
								 initializeMesh:AS3ValType,\
								 destroyMesh:AS3ValType,\
								 cloneMesh:AS3ValType,\
								 initializeSkinMeshController:AS3ValType,\
								 destroySkinMeshController:AS3ValType,\
								 setMeshAttribute:AS3ValType,\
								 initializeEntity:AS3ValType,\
								 decomposeEntity:AS3ValType,\
								 addEntity:AS3ValType,\
								 removeEntity:AS3ValType,\
								 initializeAnimation:AS3ValType,\
								 initializeSkeletalChannel:AS3ValType,\
								 initializeMorphChannel:AS3ValType,\
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
								getTextureLevelDescMethod,
								swapTextureEndianMethod,
								initializeLightMethod,
								addLightMethod,
								removeLightMethod,
								initializeFogMethod,
								addFogMethod,
								removeFogMethod,
								initializePrimitivesMethod,
								initializeTerrainMethod,
								initializeMeshMethod,
								destroyMeshMethod,
								cloneMeshMethod,
								initializeSkinMeshControllerMethod,
								destroySkinMeshControllerMethod,
								setMeshAttributeMethod,
								initializeEntityMethod,
								decomposeEntityMethod,
								addEntityMethod,
								removeEntityMethod,
								initializeAnimationMethod,
								initializeSkeletalChannelMethod,
								initializeMorphChannelMethod,
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
	AS3_Release( getTextureLevelDescMethod );
	AS3_Release( swapTextureEndianMethod );
	AS3_Release( initializeLightMethod );
	AS3_Release( addLightMethod );
	AS3_Release( removeLightMethod );
	AS3_Release( initializeFogMethod );
	AS3_Release( addFogMethod );
	AS3_Release( removeFogMethod );
	AS3_Release( initializePrimitivesMethod );
	AS3_Release( initializeTerrainMethod );
	AS3_Release( initializeMeshMethod );
	AS3_Release( destroyMeshMethod );
	AS3_Release( cloneMeshMethod );
	AS3_Release( initializeSkinMeshControllerMethod );
	AS3_Release( destroySkinMeshControllerMethod );
	AS3_Release( setMeshAttributeMethod );
	AS3_Release( initializeEntityMethod );
	AS3_Release( decomposeEntityMethod );
	AS3_Release( addEntityMethod );
	AS3_Release( removeEntityMethod );
	AS3_Release( initializeAnimationMethod );
	AS3_Release( initializeSkeletalChannelMethod );
	AS3_Release( initializeMorphChannelMethod );
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
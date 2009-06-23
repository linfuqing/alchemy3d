#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "Scene.h"

typedef struct Viewport
{
	//RW
	Number   left;

	//RW
	Number   right;

	//RW
	Number   up;

	//RW
	Number   bottom;

	//R
	Scene  * scene;

	//N
	Scene  * Projection;

	//RW
	Camera * camera;
}Viewport;

void viewport_setScene( Viewport * v, Scene * s )
{
	if( v -> Projection != NULL )
	{
		scene_destroy( & ( v -> Projection ) );
	}

	v -> scene = s;

	if( ( v -> Projection = ( Scene * )malloc( sizeof( Scene ) ) ) == NULL )
	{
		exit( TRUE );
	}
}

Viewport * newViewport( Number left, Number right, Number up, Number bottom, Scene * scene )
{
	Viewport * viewport;

	if( ( viewport = ( Viewport * )malloc( sizeof( Viewport ) ) ) == NULL )
	{
		exit( TRUE );
	}

	viewport -> left   = left;
	viewport -> right  = right;
	viewport -> up     = up;
	viewport -> bottom = bottom;

	viewport -> camera = NULL;

	viewport_setScene( viewport, scene );

	return viewport;
}

void projectScene( Viewport * viewport, void render( Faces faces ) )
{

}

#endif
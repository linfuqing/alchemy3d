#ifndef VIEWPORT_H
#define VIEWPORT_H

//verson 1.2

#include "Scene.h"
#include "Projection.h"

typedef struct Viewport
{
	//R
	Number        left;

	//R
	Number        right;

	//R
	Number        top;

	//R
	Number        bottom;

	//R
	Number        near;

	//R
	Number        far;

	//R
	Scene       * scene;

	//N
	Projection  * projection;

	//RW
	Camera      * camera;

	//N
	Matrix3D    * project;

	//N
	Matrix3D    * transform;

	//N
	ScreenFaces * graphics;
}Viewport;

void viewport_setScene( Viewport * v, Scene * s )
{
	if( v -> projection != NULL )
	{
		projection_destroy( & v -> projection );  //________error!
	}

	v -> scene      = s;
	v -> projection = newProjection( s -> mesh, NULL );
}

void viewport_set( Viewport * v, Number left, Number right, Number top, Number bottom, Number far, Number near )
{
	v -> left   = left;
	v -> right  = right;
	v -> top    = top;
	v -> bottom = bottom;
	v -> far    = far;
	v -> near   = near;

	matrix3D_projectMatrix( v -> project, left, right, top, bottom, far, near );
}
	
Viewport * newViewport( Number left, Number right, Number top, Number bottom, Number far, Number near, Scene * scene )
{
	Viewport * viewport;

	if( ( viewport = ( Viewport * )malloc( sizeof( Viewport ) ) ) == NULL )
	{
		exit( TRUE );
	}

	viewport -> left       = left;
	viewport -> right      = right;
	viewport -> top        = top;
	viewport -> bottom     = bottom;
	viewport -> far        = far;
	viewport -> near       = near;

	viewport -> camera     = NULL;
	viewport -> projection = NULL;
	viewport -> project    = projectMatrix3D( top, bottom, left, right, near, far );
	viewport -> transform  = newMatrix3D( NULL );

	screenFaces_initiate( & ( viewport -> graphics ) );

	viewport_setScene( viewport, scene );

	return viewport;
}

int projectVertices( Matrix3D * transform, Vertices * vertices, ScreenVertices * screenVertices )
{
	Vertices * vs       = vertices       -> next;
	ScreenVertices * sv = screenVertices -> next;

	while( vs != NULL )
	{
		if( vs -> vertex != sv -> vertex -> parent )
		{
			return FALSE;
		}

		* ( sv -> vertex -> screen ) = * ( vs -> vertex -> worldPosition );
		matrix3D_projectVector( transform, sv -> vertex -> screen );

		vs = vs -> next;
		sv = sv -> next;
	}

	if( sv != NULL )
	{
		return FALSE;
	}

	return TRUE;
}

void projectSceneMesh( Matrix3D * project, Scene * s, Projection * p, ScreenFaces * graphics )
{
	Scene      * sp = s -> children;
	Projection * pp = p -> children;

	if( sp == NULL )
	{
		return;
	}

	if( pp == NULL || !projectVertices( project, sp -> mesh -> vertices, pp -> vertices ) )
	{
		p -> children = newProjection( sp -> mesh, & pp );
		pp            = p -> children;

		projectVertices( project, sp -> mesh -> vertices, pp -> vertices );
	}

	while( sp -> next != NULL )
	{

		if( pp -> next == NULL|| !projectVertices( project, sp-> next -> mesh -> vertices, pp-> next -> vertices ) )
		{
			pp -> next = newProjection( sp -> next -> mesh, & ( pp -> next ) );

			projectVertices( project, sp -> next -> mesh -> vertices, pp -> next -> vertices );
		}

		if( graphics -> rear != NULL )
		{
			graphics -> rear -> next = pp -> faces -> next;
		}
		else
		{
			graphics -> next         = pp -> faces -> next;
		}

		graphics -> rear             = pp -> faces -> rear;

		projectSceneMesh( project, sp, pp, graphics );

		sp = sp -> next;
		pp = pp -> next;
	}

	if( graphics -> rear != NULL )
	{
		graphics -> rear -> next = p -> faces -> next;
	}
	else
	{
		graphics -> next         = p -> faces -> next;
	}

	graphics -> rear             = p -> faces -> rear;

	projectSceneMesh( project, sp, pp, graphics );
}

void projectSceneChildren( Matrix3D * project, Scene * s, Projection * p, ScreenFaces * graphics  )
{
	Scene      * sp = s -> children;
	Projection * pp = p -> children;

	if( sp == NULL )
	{
		return;
	}

	if( pp == NULL )
	{
		p -> children = newProjection( sp -> mesh, NULL );

		pp            = p -> children;

		projectVertices( project, sp -> mesh -> vertices, pp -> vertices );

		if( graphics -> rear != NULL )
		{
			graphics -> rear -> next = pp -> faces -> next;
		}
		else
		{
			graphics -> next         = pp -> faces -> next;
		}

		graphics -> rear             = pp -> faces -> rear;

		projectSceneMesh( project, sp, pp, graphics );
	}
	else if( sp -> move )
	{	
		if( !projectVertices( project, sp -> mesh -> vertices, pp -> vertices ) )
		{
			p -> children = newProjection( sp -> mesh, & pp );

			pp            = p -> children;

			projectVertices( project, sp -> mesh -> vertices, pp -> vertices );
		}

		if( graphics -> rear != NULL )
		{
			graphics -> rear -> next = pp -> faces -> next;
		}
		else
		{
			graphics -> next         = pp -> faces -> next;
		}

		graphics -> rear             = pp -> faces -> rear;

		projectSceneMesh( project, sp, pp, graphics );
	}
	else
	{
		projectSceneChildren( project, sp, pp, graphics );
	}

	while( sp -> next != NULL )
	{
		if( pp -> next == NULL )
		{
			pp -> next = newProjection( sp -> next -> mesh, NULL );

			projectVertices( project, sp -> next -> mesh -> vertices, pp -> next -> vertices );

			if( graphics -> rear != NULL )
			{
				graphics -> rear -> next = pp -> next -> faces -> next;
			}
			else
			{
				graphics -> next         = pp -> next -> faces -> next;
			}

			graphics -> rear             = pp -> next -> faces -> rear;

			projectSceneMesh( project, sp -> next, pp -> next, graphics );
		}
		else if( sp -> next -> move )
		{
			if( !projectVertices( project, sp -> next -> mesh -> vertices, pp -> next -> vertices ) )
			{
				pp -> next = newProjection( sp -> next -> mesh, & ( pp -> next ) );

				projectVertices( project, sp -> next -> mesh -> vertices, pp -> next -> vertices );
			}

			if( graphics -> rear != NULL )
			{
				graphics -> rear -> next = pp -> next -> faces -> next;
			}
			else
			{
				graphics -> next         = pp -> next -> faces -> next;
			}

			graphics -> rear             = pp -> next -> faces -> rear;

			projectSceneMesh( project, sp -> next, pp -> next, graphics );
		}
		else
		{
			projectSceneChildren( project, sp -> next, pp -> next, graphics );
		}

		sp = sp -> next;
		pp = pp -> next;
	}
}

void viewport_updateProjectMatrix( Viewport * v )
{
	if( v -> camera == NULL )
	{
		* ( v -> transform ) = * ( v -> project );
	}
	else if( v -> camera -> move )
	{
		free( v -> transform );

		v -> transform = matrix3D_project( v -> project, matrix3D_multiply( camera_getTransform( v -> camera ), v -> camera -> world ) );
	}
}

void projectScene( Viewport * v, int mode )
{
	viewport_updateProjectMatrix( v );

	if( mode || v -> scene -> move )
	{
		if( !projectVertices( v -> transform, v -> scene -> mesh -> vertices, v -> projection -> vertices ) )
		{
			v -> projection = newProjection( v -> scene -> mesh, & ( v -> projection ) );

			projectVertices( v -> transform, v -> scene -> mesh -> vertices, v -> projection -> vertices );
		}

		projectSceneMesh( v -> transform, v -> scene, v -> projection, v -> graphics );
	}
	else
	{
		projectSceneChildren( v -> transform, v -> scene, v -> projection, v -> graphics );
	}
}

#endif
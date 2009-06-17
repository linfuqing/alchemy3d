#ifndef __VIEWPORT_H
#define __VIEWPORT_H

#include <malloc.h>

#include "DisplayObject3D.h"
#include "Scene.h"

typedef struct Viewport
{
	struct Scene * scene;
	struct DisplayObject3D * camera;
}Viewport;

Viewport * newViewport(Scene * scene, DisplayObject3D * camera)
{
	Viewport * view;

	if( (view = ( Viewport * )malloc( sizeof( Viewport ) ) ) == NULL)
	{
		exit( 1 );
	}

	view->camera = camera;
	view->scene = scene;

	return view;
}

void viewport_render(Viewport * viewport)
{
	Scene * scene, * p;

	DisplayObject3D * camera, * do3d;

	scene = viewport->scene;
	camera = viewport->camera;

	do3d_updateTransform(camera);
	matrix3D_invert(camera->view);

	p = scene->next;

	do
	{
		do3d = p->do3d;

		do3d_updateTransform(do3d);

		if( do3d->parent != NULL )
		{
			matrix3D_apprend(do3d->world, * (do3d->parent->world));
			matrix3D_apprend(do3d->view, * (do3d->parent->view));
		}
		else
		{
			matrix3D_apprend(do3d->view, * (camera->view));
		}

		p = p->next;
	}
	while( p!= NULL);
}

#endif
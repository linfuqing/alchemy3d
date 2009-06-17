#ifndef __VIEWPORT_H
#define __VIEWPORT_H

#include "DisplayObject3D.h"
#include "Scene.h"

typedef struct Viewport
{
	struct Scene * scene;
	struct DisplayObject3D * camera;
}Viewport;

void viewport_render(Viewport * viewport)
{
	Scene * scene, * p;
	Matrix3D * cloned;
	DisplayObject3D * camera, * do3d;

	scene = viewport->scene;
	camera = viewport->camera;

	do3d_updateTransform(camera);
	camera->view = matrix3D_clone(camera->transform);
	matrix3D_invert(camera->view);

	p = scene->next;

	do
	{
		do3d = p->do3d;

		do3d_updateTransform(do3d);

		if( do3d->parent != NULL )
		{
			
		}

		p = p->next;
	}
	while( p!= NULL);
}

#endif
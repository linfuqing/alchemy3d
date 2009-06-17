#include <malloc.h>

#include "Base.h"
#include "DisplayObject3D.h"
#include "Scene.h"
#include "Viewport.h"


int main()
{
	DisplayObject3D * do3d1;
	DisplayObject3D * do3d2;
	DisplayObject3D * do3d3;
	DisplayObject3D * do3d4;
	DisplayObject3D * do3d5;
	DisplayObject3D * camera;
	Viewport * view;
	Scene * scene;

	camera = newDisplayObject3D();
	camera->position->z = -1000;
	
	scene = newScene();

	view = newViewport(scene, camera);

	do3d1 = newDisplayObject3D();
	do3d1->position->x = 1;
	scene_addChild(scene, do3d1, NULL);

	do3d2 = newDisplayObject3D();
	do3d2->position->x = 2;
	scene_addChild(scene, do3d2, NULL);

	do3d3 = newDisplayObject3D();
	do3d3->position->x = 3;
	scene_addChild(scene, do3d3, do3d1);

	do3d4 = newDisplayObject3D();
	do3d4->position->x = 4;
	scene_addChild(scene, do3d4, NULL);

	do3d5 = newDisplayObject3D();
	do3d5->position->x = 5;
	scene_addChild(scene, do3d5, do3d2);

	//scene_project(scene);

	//printf("%f", do3d.transform.m14);

	viewport_render(view);

	return 0;
}
#include <malloc.h>

#include "DisplayObject3D.h"
#include "Scene.h"
#include "Base.h"

void test(DisplayObject3D * do3d);

int main()
{
	DisplayObject3D do3d1;
	DisplayObject3D do3d2;
	DisplayObject3D do3d3;

	Scene * sceneHead;
	
	sceneHead = scene_initiate();

	do3d1 = newDisplayObject3D();
	do3d1.position.x = 1;
	scene_addChild(sceneHead, &do3d1, NULL);
	
	do3d2 = newDisplayObject3D();
	do3d2.position.x = 2;
	scene_addChild(sceneHead, &do3d2, NULL);
	
	do3d3 = newDisplayObject3D();
	do3d3.position.x = 3;
	scene_addChild(sceneHead, &do3d3, &do3d1);

	//printf("%f", do3d.transform.m14);

	return 0;
}

void test(DisplayObject3D * do3d)
{

}
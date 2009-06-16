#include <malloc.h>

#include "DisplayObject3D.h"
#include "Scene.h"
#include "Base.h"
#include "SLNode.h"

void test(DisplayObject3D * do3d);

int main()
{
	DisplayObject3D do3d;
	DisplayObject3D * do3dPtr;
	Scene * sceneHead;
	int i = 0;
	
	sceneHead = scene_initiate();

	for (; i < 3; i ++)
	{
		do3d = newDisplayObject3D();
		do3dPtr = &do3d;
		do3d.position.x = i+1;
		//scene_push(sceneHead, 0, do3d);
		scene_addChild(sceneHead, do3dPtr, NULL);
	}

	//printf("%f", do3d.transform.m14);

	return 0;
}

void test(DisplayObject3D * do3d)
{

}
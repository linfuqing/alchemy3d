#include <malloc.h>

//#include "DisplayObject3D.h"
//#include "Scene.h"
#include "Base.h"
#include "Matrix3D.h"

void test(Matrix3D *m)
{
	Matrix3D * newM;

	Number transformData[16] = {4.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};

	newM = newMatrix3D(&transformData);

	* m = * newM;
}

int main()
{
	Matrix3D *m;

	Number transformData[16] = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};

	m = newMatrix3D(&transformData);

	test(m);

	/*DisplayObject3D * do3d1;
	DisplayObject3D * do3d2;
	DisplayObject3D * do3d3;
	DisplayObject3D * do3d4;
	DisplayObject3D * do3d5;

	Scene * scene;
	
	scene = scene_initiate();

	do3d1 = newDisplayObject3D();
	do3d1->position.x = 1;
	scene_addChild(scene, do3d1, NULL);

	do3d2 = newDisplayObject3D();
	do3d2->position.x = 2;
	scene_addChild(scene, do3d2, NULL);

	do3d3 = newDisplayObject3D();
	do3d3->position.x = 3;
	scene_addChild(scene, do3d3, do3d1);

	do3d4 = newDisplayObject3D();
	do3d4->position.x = 4;
	scene_addChild(scene, do3d4, NULL);

	do3d5 = newDisplayObject3D();
	do3d5->position.x = 5;
	scene_addChild(scene, do3d5, do3d2);*/

	//scene_project(scene);

	//printf("%f", do3d.transform.m14);

	return 0;
}
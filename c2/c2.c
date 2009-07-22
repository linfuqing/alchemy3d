#include <malloc.h>
#include <stdlib.h>

#include "Base.h"
#include "Math.h"
#include "Device.h"
#include "Entity.h"
#include "Scene.h"
#include "Viewport.h"
#include "Vertices.h"
#include "Polygon.h"
#include "Faces.h"
#include "Mesh.h"
#include "Render.h"
#include "Vector3D.h"
#include "Matrix3D.h"
#include "AABB.h"
#include "Color.h"
#include "Material.h"
#include "Texture.h"
#include "Light.h"

Device * device;

int main()
{
	//Matrix3D * m;
	//Matrix3D * cloned;
	//Quaternion * q;

	//Vector3D v1, v3;
	//Quaternion v2;

	//float transformData[16] = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};

	//m = newMatrix3D(&transformData);

	//cloned = matrix3D_clone(m);

	//matrix3D_appendTranslation(m, 200, 300, 400);
	//matrix3D_appendRotationX(m, 45);
	//matrix3D_appendRotationY(m, 45);
	//matrix3D_appendRotationZ(m, 45);
	//matrix3D_transpose(m);
	//matrix3D_invert(m);
	//matrix3D_appendScale(m, 3, 2, 1);

	//v = matrix3D_transformVector(m, newVector3D(100, 200, 300, 1));

	//q = quaternoin_setFromEuler(45 * PI / 180, 45 * PI / 180, 45 * PI / 180);
	//m = quaternoin_getMatrix(q);

	//matrix3D_decompose(m, &v1, &v2, &v3);

	Camera * camera;
	Viewport * view;
	Scene * scene;
	Faces * faces, * faces2;
	Vertices * vertices, * vertices2;
	Polygon * tri;
	Entity * do3d, * do3d2, * lightSource;
	Vertex * v;
	Vertex * vArr[4], * vArr2[4];
	Vector * point, * point2;
	Material * material, * material2;
	Light * light;
	Texture * texture;

	float pos[16];
	float pos2[16];

	int i = 0;
	int j = 0;

	texture = newTexture(120, 120);

	//初始化顶点链表
	vertices = newVertices();
	vertices2 = newVertices();
	//初始化面链表
	faces = newFaces();
	faces2 = newFaces();

	pos[0] = -150.0f;
	pos[1] = -150.0f;
	pos[2] = 150.0f;
	pos[3] = -150.0f;
	pos[4] = -150.0f;
	pos[5] = 150.0f;
	pos[6] = 150.0f;
	pos[7] = 150.0f;

	pos2[0] = -150.0f;
	pos2[1] = -150.0f;
	pos2[2] = 150.0f;
	pos2[3] = -150.0f;
	pos2[4] = -150.0f;
	pos2[5] = 150.0f;
	pos2[6] = 150.0f;
	pos2[7] = 150.0f;

	//构造顶点
	for (; i < 8; i += 2)
	{
		v = newVertex(pos[i], pos[i+1], 0.0f);
		vertices_push(vertices, v);
		vArr[j] = v;
		j++;
	}

	point = newVector(1.0f, 1.0f);
	tri = newTriangle3D(vArr[0], vArr[1], vArr[2], point, point, point);
	faces_push(faces, tri);

	tri = newTriangle3D(vArr[3], vArr[2], vArr[1], point, point, point);
	faces_push(faces, tri);

	i = 0;
	j = 0;

	for (; i < 8; i += 2)
	{
		v = newVertex(pos2[i], pos2[i+1], 0.0f);
		vertices_push(vertices2, v);
		vArr2[j] = v;
		j++;
	}

	point2 = newVector(.5f, .5f);
	tri = newTriangle3D(vArr2[0], vArr2[1], vArr2[2], point2, point2, point2);
	faces_push(faces2, tri);

	tri = newTriangle3D(vArr2[3], vArr2[2], vArr2[1], point2, point2, point2);
	faces_push(faces2, tri);

	material = newMaterial( newColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							newColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							newColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							newColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							0.0f );

	material2 = newMaterial( newColor( 0.0f, 1.0f, 0.0f, 1.0f ),
							newColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							newColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							newColor( 1.0f, 0.0f, 0.0f, 1.0f ),
							0.0f );

	do3d = newEntity();
	//entity_setRotationX( do3d, 90.0f );
	//entity_setY(do3d, -180.0f);
	entity_setZ(do3d, 500.0f);
	entity_setMesh( do3d, newMesh(faces, vertices) );
	entity_setMaterial( do3d, material );

	do3d2 = newEntity();
	entity_setZ(do3d2, 500);
	entity_setMesh( do3d2, newMesh(faces2, vertices2) );
	entity_setMaterial( do3d2, material2 );

	camera = newCamera( 90.0f, 100.0f, 5000.0f, newEntity() );
	camera_setTarget( camera, do3d->worldPosition );

	lightSource = newEntity();
	//lightSource->position->y = -500;
	//lightSource->position->z = 200;
	light = newPointLight( POINT_LIGHT, lightSource );
	setLightOnOff( light, TRUE );

	scene = newScene();
	scene_addLight(scene, light);
	scene_addEntity(scene, do3d, NULL);
	scene_addEntity(scene, do3d2, NULL);

	view = newViewport( 600.0f, 400.0f, scene, camera );

	device = newDevice();
	device_addViewport(device, view);
	device_addCamera(device, camera);
	device_addScene(device, scene);

	/*do3d1 = newEntity();
	do3d1->direction->x = 45;
	scene_addChild(scene, do3d1, NULL);*/

	device_render(device);
	device_render(device);
	device_render(device);
	device_render(device);
	device_render(device);

	//printf("%x", view->gfxBuffer[189206]);

	return 0;
}
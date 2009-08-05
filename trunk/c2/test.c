#include <malloc.h>
#include <stdlib.h>
#include <time.h>

#define INLINE

#include "Base.h"
#include "Math.h"
#include "Device.h"
#include "Entity.h"
#include "Scene.h"
#include "Viewport.h"
#include "Polygon.h"
#include "Mesh.h"
#include "Render.h"
#include "Vector3D.h"
#include "Matrix3D.h"
#include "AABB.h"
#include "Color.h"
#include "Material.h"
#include "Texture.h"
#include "Light.h"

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

	/*Camera * camera;
	Viewport * view;
	Scene * scene;
	Polygon * tri;
	Mesh * mesh;
	Vertex * v;
	Vertex * vArr[4];*/
	Vector * point;
	Vector3D * v3d;
	Entity * entity, entity1;
	uint32 d, d2;
	float a, r, g, b, a2, r2, g2, b2;

	Vector3D tv;

	//test***************************
	Matrix3D testM, testMP;
	float transformData[16] = {1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};

	time_t ts,te;
	double delT;

	float pos[8];
	int i = 0, j = 0;

	d = 0xff550000;
	d2 = 0xff55ff00;

	a = ((d >> 24) & 0xff) / 255.0f;
	r = ((d >> 16) & 0xff) / 255.0f;
	g = ((d >> 8) & 0xff) / 255.0f;
	b = (d & 0xff) / 255.0f;

	a2 = ((d2 >> 24) & 0xff) / 255.0f;
	r2 = ((d2 >> 16) & 0xff) / 255.0f;
	g2 = ((d2 >> 8) & 0xff) / 255.0f;
	b2 = (d2 & 0xff) / 255.0f;

	a *= a2;
	r *= r2;
	g *= g2;
	b *= b2;

	a *= 255;
	r *= 255;
	g *= 255;
	b *= 255;

	ts=clock();

	for (i = 0; i < 100000000; i ++)
	{
		//newVector3D(100, 200, 0, 1);
		//matrix3D_transformVector(&testM, v3d);
		//matrix3D_append4x4(&testM, entity->transform);
		//testM1 = matrix3D_clone(&testM);
		//matrix3D_copy(&testMP, &testM);
		//memcpy(&entity1, entity, sizeof( * entity));
		//matrix3D_invert4x4(&testM);
		//matrix3D_invert(testMP);
		//MAX(d, 1.0f);
		//if ( d > 1.0f ) d = 1.0f;
	}

	te=clock();

	delT = (double)(te-ts) / CLOCKS_PER_SEC;
	//printf("%f\n",delT);
	printf("%lx\n",d|d2);

	d2 = ((int)a << 24) + ((int)r << 16) + ((int)g << 8) + (int)b;
	printf("%lx\n",d2);

	return 0;
}
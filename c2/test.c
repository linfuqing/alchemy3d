#include <malloc.h>
#include <stdlib.h>
#include <time.h>

#define INLINE

#include "Base.h"
#include "Math.h"
#include "Entity.h"
#include "Scene.h"
#include "Viewport.h"
#include "Triangle.h"
#include "Mesh.h"
#include "Vector3D.h"
#include "Matrix3D.h"
#include "AABB.h"
#include "ARGBColor.h"
#include "Material.h"
#include "Texture.h"
#include "Light.h"

int main()
{
	time_t ts,te;
	double delT;
	int i = 0, k = 0;

	ts=clock();

	for (i = 0; i < 10000; i ++)
	{
		k ++;
	}

	te=clock();

	delT = (double)(te-ts) / CLOCKS_PER_SEC;
	printf("%f\n",delT);

	return 0;
}
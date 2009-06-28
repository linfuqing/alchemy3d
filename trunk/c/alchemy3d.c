#include <stdlib.h>
#include <AS3.h>

#include "Base.h"
#include "DisplayObject3D.h"
#include "Scene.h"
#include "Viewport.h"
#include "Polygon.h"
#include "Vertices.h"
#include "Faces.h"
#include "Mesh.h"
#include "DisplayObject3D.h"
#include "Vector3D.h"
#include "Render.h"

//初始化场景
//返回该对象的起始指针
AS3_Val initializeViewport( void* self, AS3_Val args )
{
	Camera * camera;
	Viewport * view;
	Scene * scene;

	Number width, height, fov, nearClip, farClip;

	AS3_ArrayValue(args, "DoubleType, DoubleType, DoubleType, DoubleType, DoubleType, DoubleType", &width, &height, &fov, &nearClip, &farClip);

	scene = newScene();
	camera = newCamera(fov, nearClip, farClip);
	view = newViewport(width, height, scene, camera);

	return AS3_Array("PtrType, PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", view, view->gfxBuffer, scene, camera, camera->fov, camera->nearClip, camera->farClip);
}

//渲染视口
AS3_Val renderViewport( void* self, AS3_Val args )
{
	Viewport * view;

	AS3_ArrayValue(args, "PtrType", &view);

	project(view);

	render(view);

	//AS3_Trace(AS3_Int(view->gfxBuffer[189206]));

	return 0;
}

//创建几何实体
//返回该对象的起始指针
AS3_Val createEntity( void* self, AS3_Val args )
{
	Scene * scene;
	Faces * faces;
	Vertices * vertices;
	Polygon * tri;
	DisplayObject3D * do3d, * parent;
	Mesh * mesh;

	int vNum, fNum, vLen, fLen, i, j;
	Number * vBuf, * fBuf;

	AS3_Val input_vertices, input_faces, input_vLength, input_fLength;

	AS3_ArrayValue(args, "PtrType, PtrType, AS3ValType, AS3ValType", &scene, &parent, &input_vertices, &input_faces);
	
	input_vLength = AS3_GetS(input_vertices, "length");
	input_fLength = AS3_GetS(input_faces, "length");

	vLen = AS3_IntValue(input_vLength);
	fLen = AS3_IntValue(input_fLength);

	vNum = vLen / VERTEX_SIZE_ELEMENT;
	fNum = fLen / FACE_SIZE_ELEMENT;

	Vertex * vArr[vNum];
	Vertex * v;

	do3d = newDisplayObject3D();

	if (vLen != 0 && fLen != 0)
	{
		vBuf = (Number *)calloc(vNum, VERTEX_SIZE_ELEMENT);
		fBuf = (Number *)calloc(fNum, FACE_SIZE_ELEMENT);
		
		AS3_ByteArray_seek(input_vertices, 0, SEEK_SET);
		AS3_ByteArray_seek(input_faces, 0, SEEK_SET);

		AS3_ByteArray_readBytes((void*)vBuf, input_vertices, vLen);
		AS3_ByteArray_readBytes((void*)fBuf, input_faces, fLen);

		AS3_Release(input_vertices);
		AS3_Release(input_faces);
		AS3_Release(input_vLength);
		AS3_Release(input_fLength);

		i = 0;
		j = 0;

		//初始化顶点链表
		vertices = newVertices();
		//初始化面链表
		faces = newFaces();

		for (; i < vNum; i ++)
		{
			j = i * VERTEX_SIZE;

			v = newVertex(newVector3D(vBuf[j], vBuf[j + 1], vBuf[j + 2], vBuf[j + 3]));

			vertices_push(vertices, v);

			vArr[i] = v;
		}

		for (i = 0; i < fNum; i ++)
		{
			j = i * FACE_SIZE;

			tri = newTriangle3D(vArr[(int)fBuf[j]], vArr[(int)fBuf[j + 1]], vArr[(int)fBuf[j + 2]], newVector(fBuf[j + 3], fBuf[j + 4]), newVector(fBuf[j + 5], fBuf[j + 6]), newVector(fBuf[j + 7], fBuf[j + 8]));

			faces_push(faces, tri);
		}

		mesh = newMesh(faces, vertices);
		do3d->mesh = mesh;
	}

	free(vBuf);
	free(fBuf);

	vBuf = NULL;
	fBuf = NULL;

	if (parent == 0)
	{
		parent = NULL;
	}

	scene_addChild(scene, do3d, parent);

	return AS3_Array("PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", do3d, do3d->position, do3d->direction, do3d->scale, do3d->mesh->vertices, do3d->mesh->faces);
}

//测试函数
AS3_Val test( void* self, AS3_Val args )
{
	/*do3d_updateTransform(&do3d);

	AS3_Trace(AS3_Number(do3d.position.x));
	AS3_Trace(AS3_Number(do3d.position.y));
	AS3_Trace(AS3_Number(do3d.position.z));
	AS3_Trace(AS3_Number(do3d.scale.x));
	AS3_Trace(AS3_Number(do3d.scale.y));
	AS3_Trace(AS3_Number(do3d.scale.z));
	AS3_Trace(AS3_Number(do3d.transform.m11));
	AS3_Trace(AS3_Number(do3d.transform.m12));
	AS3_Trace(AS3_Number(do3d.transform.m13));
	AS3_Trace(AS3_Number(do3d.transform.m14));
	AS3_Trace(AS3_Number(do3d.transform.m21));
	AS3_Trace(AS3_Number(do3d.transform.m22));
	AS3_Trace(AS3_Number(do3d.transform.m23));
	AS3_Trace(AS3_Number(do3d.transform.m24));
	AS3_Trace(AS3_Number(do3d.transform.m31));
	AS3_Trace(AS3_Number(do3d.transform.m32));
	AS3_Trace(AS3_Number(do3d.transform.m33));
	AS3_Trace(AS3_Number(do3d.transform.m34));
	AS3_Trace(AS3_Number(do3d.transform.m41));
	AS3_Trace(AS3_Number(do3d.transform.m42));
	AS3_Trace(AS3_Number(do3d.transform.m43));
	AS3_Trace(AS3_Number(do3d.transform.m44));*/

	return 0;
}

//入口
int main()
{
	AS3_Val initializeViewportMethod = AS3_Function( NULL, initializeViewport );
	AS3_Val renderViewportMethod = AS3_Function( NULL, renderViewport );
	AS3_Val createEntityMethod = AS3_Function( NULL, createEntity );
	AS3_Val testMethod = AS3_Function( NULL, test );



	AS3_Val result = AS3_Object( "initializeViewport:AS3ValType, renderViewport:AS3ValType, createEntity:AS3ValType, test:AS3ValType",
		initializeViewportMethod, renderViewportMethod, createEntityMethod, testMethod );



	AS3_Release( initializeViewportMethod );
	AS3_Release( renderViewportMethod );
	AS3_Release( createEntityMethod );
	AS3_Release( testMethod );


	AS3_LibInit( result );

	return 0;
}
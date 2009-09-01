#ifndef __SCREEN_H
#define __SCREEN_H

#include <malloc.h>

#include "Viewport.h"
#include "Scene.h"
#include "Camera.h"

typedef struct Viewports
{
	Viewport * viewport;

	struct Viewports * next;
}Viewports;

typedef struct Cameras
{
	Camera * camera;

	struct Cameras * next;
}Cameras;

typedef struct Scenes
{
	Scene * scene;

	struct Scenes * next;
}Scenes;

typedef struct Device
{
	int nViewports, nCameras, nScenes;

	struct Viewports * viewports;

	struct Cameras * cameras;

	struct Scenes * scenes;
}Device;

Device * newDevice()
{
	Device * d;

	if( ( d = ( Device * )malloc( sizeof( Device ) ) ) == NULL )
	{
		exit( TRUE );
	}

	d->nViewports = 0;
	d->viewports = NULL;
	d->cameras = NULL;
	d->scenes = NULL;

	return d;
}

//------------------------ViewPort----------------------
int device_addViewport(Device * device, Viewport * view)
{
	Viewports * p, * q;

	p = device->viewports;

	if (NULL != p)
	{
		while( NULL != p->next )
		{
			p = p->next;
		}
	}

	if( ( q = ( Viewports * )malloc( sizeof( Viewports ) ) ) == NULL )
	{
		exit( TRUE );
	}

	q->viewport = view;

	q->next = NULL;

	if (NULL == p)
	{
		device->viewports = q;
	}
	else
	{
		p->next = q;
	}

	device->nViewports ++;

	return TRUE;
}

int device_removeViewportAt( Device * device, int i )
{
	Viewports * p, * s;

	int j = 1;

	if( i > device->nViewports - 1)
	{
		//printf( "删除位置参数错误!" );
		return FALSE;
	}
	else if ( 0 == i)
	{
		s = device->viewports;

		device->viewports = s->next;
	}
	else
	{
		p = device->viewports;

		for (; j < i; j ++)
		{
			p = p->next;
		}

		s = p->next;

		p->next = s->next;
	}

	device->nViewports --;

	free(s);

	return TRUE;
}
//------------------------end ViewPort----------------------

//------------------------Camera----------------------
int device_addCamera(Device * device, Camera * camera)
{
	Cameras * p, * q;

	p = device->cameras;

	if (NULL != p)
	{
		while( NULL != p->next )
		{
			p = p->next;
		}
	}

	if( ( q = ( Cameras * )malloc( sizeof( Cameras ) ) ) == NULL )
	{
		exit( TRUE );
	}

	q->camera = camera;

	q->next = NULL;

	if (NULL == p)
	{
		device->cameras = q;
	}
	else
	{
		p->next = q;
	}

	device->nCameras ++;

	return TRUE;
}

int device_removeCameraAt( Device * device, int i )
{
	Cameras * p, * s;

	int j = 1;

	if( i > device->nCameras - 1)
	{
		//printf( "删除位置参数错误!" );
		return FALSE;
	}
	else if ( 0 == i)
	{
		s = device->cameras;

		device->cameras = s->next;
	}
	else
	{
		p = device->cameras;

		for (; j < i; j ++)
		{
			p = p->next;
		}

		s = p->next;

		p->next = s->next;
	}

	device->nCameras --;

	free(s);

	return TRUE;
}
//------------------------end Camera----------------------

//------------------------Scene----------------------
int device_addScene(Device * device, Scene * scene)
{
	Scenes * p, * q;

	p = device->scenes;

	if (NULL != p)
	{
		while( NULL != p->next )
		{
			p = p->next;
		}
	}

	if( ( q = ( Scenes * )malloc( sizeof( Scenes ) ) ) == NULL )
	{
		exit( TRUE );
	}

	q->scene = scene;

	q->next = NULL;

	if (NULL == p)
	{
		device->scenes = q;
	}
	else
	{
		p->next = q;
	}

	device->nScenes ++;

	return TRUE;
}

int device_removeSceneAt( Device * device, int i )
{
	Scenes * p, * s;

	int j = 1;

	if( i > device->nScenes - 1)
	{
		//printf( "删除位置参数错误!" );
		return FALSE;
	}
	else if ( 0 == i)
	{
		s = device->scenes;

		device->scenes = s->next;
	}
	else
	{
		p = device->scenes;

		for (; j < i; j ++)
		{
			p = p->next;
		}

		s = p->next;

		p->next = s->next;
	}

	device->nScenes --;

	free(s);

	return TRUE;
}
//------------------------end Scene----------------------

//----------------------Pipe Line-------------------
//设备负责保存场景，摄像机，视口
//这是里渲染入口
void device_render(Device * device)
{
	Viewports * viewports;
	Cameras * cameras;
	Scenes * scenes;

	Viewport * viewport;
	Scene * scene;
	Camera * camera;

	//更新所有摄像机
	cameras = device->cameras;

	while( NULL != cameras )
	{
		camera = cameras->camera;

		//如果相机已经连接到视口，则更新，反之不做任何处理
		if ( TRUE == camera->isAttached ) camera_updateTransform( camera );

		cameras = cameras->next;
	}

	//对所有视口进行投影和光栅化
	viewports = device->viewports;

	while( NULL != viewports )
	{
		viewport = viewports->viewport;

		viewport_updateBeforeRender( viewport );

		viewport_project( viewport );

		viewport_render( viewport );

		viewports = viewports->next;
	}

	//重置
	scenes = device->scenes;

	while( NULL != scenes )
	{
		scene = scenes->scene;
		
		scene_updateAfterRender( scene );

		scenes = scenes->next;
	}
}

#endif
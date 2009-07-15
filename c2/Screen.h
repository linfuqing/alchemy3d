#ifndef __SCREEN_H
#define __SCREEN_H

#include <malloc.h>

#include "Viewport.h"
#include "Scene.h"
#include "Render.h"

typedef struct ScreenNode
{
	Viewport * viewport;

	struct ScreenNode * next;
}ScreenNode;

typedef struct Screen
{
	int nViewports;

	struct ScreenNode * nodes;
}Screen;

Screen * newScreen()
{
	Screen * s;

	if( ( s = ( Screen * )malloc( sizeof( Screen ) ) ) == NULL )
	{
		exit( TRUE );
	}

	s->nViewports = 0;
	s->nodes = NULL;

	return s;
}

int screen_addChild(Screen * screen, Viewport * view)
{
	ScreenNode * p, * q;

	p = screen->nodes;

	if (NULL != p)
	{
		while( NULL != p->next )
		{
			p = p->next;
		}
	}

	if( ( q = ( ScreenNode * )malloc( sizeof( ScreenNode ) ) ) == NULL )
	{
		exit( TRUE );
	}

	q->viewport = view;

	q->next = NULL;

	if (NULL == p)
	{
		screen->nodes = q;
	}
	else
	{
		p->next = q;
	}

	screen->nViewports ++;

	return TRUE;
}

int screen_removeChildAt( Screen * screen, int i )
{
	ScreenNode * p, * s;

	int j = 1;

	if( i > screen->nViewports - 1)
	{
		//printf( "删除位置参数错误!" );
		return FALSE;
	}
	else if ( 0 == i)
	{
		s = screen->nodes;

		screen->nodes = s->next;
	}
	else
	{
		p = screen->nodes;

		for (; j < i; j ++)
		{
			p = p->next;
		}

		s = p->next;

		p->next = s->next;
	}

	screen->nViewports --;

	free(s);

	return TRUE;
}

void renderScreen(Screen * screen)
{
	ScreenNode * screenNode;
	Viewport * viewport;
	Scene * scene;

	//投影和光栅
	screenNode = screen->nodes;

	while( NULL != screenNode )
	{
		viewport = screenNode->viewport;

		viewport_project(viewport);

		rasterize(viewport);

		screenNode = screenNode->next;
	}

	//光栅化
	/*screenNode = screen->nodes;

	while( NULL != screenNode )
	{
		viewport = screenNode->viewport;

		screenNode = screenNode->next;
	}*/

	//渲染后重置
	screenNode = screen->nodes;

	while( NULL != screenNode )
	{
		scene = viewport->scene;

		scene->isUpdated = FALSE;

		screenNode = screenNode->next;
	}
}

#endif
#ifndef __SCENE_H
#define __SCENE_H

#include "DisplayObject3D.h"

//N
typedef struct Scene
{
	DisplayObject3D * do3d;

	struct Scene * next;
}Scene;

Scene * newScene()
{
	Scene * head;

	if( ( head = ( Scene * )malloc( sizeof( Scene ) ) ) == NULL )
	{
		exit( TRUE );
	}

	head->next = NULL;

	return head;
}

int scene_length( Scene * head )
{
	Scene * p = head;

	int size = 0;

	while( p->next != NULL )
	{
		p = p->next;

		size ++;
	}

	return size;
}

int scene_delete( Scene * head, int i, DisplayObject3D * do3d )
{
	Scene * p, * s;

	int j;

	p = head;

	j = - 1;

	while( p->next != NULL && j < i - 1 )
	{
		p = p->next;

		j ++;
	}

	if( j != i - 1 )
	{
		printf( "删除位置参数错误!" );
		return FALSE;
	}

	s = p->next;

	p->next = s->next;

	free( s );

	return TRUE;
}

Scene * scene_find( Scene * head, DisplayObject3D * do3d )
{   
	head = head->next;

	if ( head == NULL )
	{
		return NULL;
	}
	else
	{
		if ( do3d == head->do3d )
		{
			return head;
		}
		else
		{
			return scene_find( head->next, do3d );
		}
	}
}


void scene_addChild(Scene * head, DisplayObject3D * do3d, DisplayObject3D * parent)
{
	Scene * p, * n;

	int move = do3d -> camera -> move;

	//DisplayObject3D * ptr;

	if ( parent != NULL )
	{
		p = scene_find(head, do3d -> parent );

		if (p == NULL)
		{
			exit(FALSE);
		}

		do3d->parent = parent;
	}
	else
	{
		p = head -> next;

		while( p != NULL )
		{
			p = p->next;
		}
	}

	if( ( n = ( Scene * )malloc( sizeof( Scene ) ) ) == NULL )
	{
		exit( TRUE );
	}

	//加入场景后设置为移动.
	do3d -> camera -> move = move ? move : MOVE_TYPE_ADDED_SCENE;

	n->do3d = do3d;

	n->next = p->next;

	p->next = n;
}

//void scene_project(Scene * head)
//{
//	Scene * p;
//
//	p = head->next;
//
//	do
//	{
//		do3d_updateTransform(p->do3d);
//
//		p = p->next;
//	}
//	while( p!= NULL);
//}

void scene_destroy( Scene * * head )
{
	Scene * p, * p1;
	p = * head;

	while( p != NULL )
	{
		p1 = p;

		p = p->next;

		free( p1 );
	}

	* head = NULL;
}

#endif
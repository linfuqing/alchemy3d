#ifndef __SCENE_H
#define __SCENE_H

#include "DisplayObject3D.h"

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
		exit( 1 );
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
		printf( "É¾³ýÎ»ÖÃ²ÎÊý´íÎó!" );
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

	//DisplayObject3D * ptr;

	if (parent != NULL)
	{
		p = scene_find(head, parent);

		if (p == NULL)
		{
			exit(1);
		}

		do3d->parent = parent;
	}
	else
	{
		p = head;

		while(p->next != NULL)
		{
			p = p->next;
		}
	}

	if( ( n = ( Scene * )malloc( sizeof( Scene ) ) ) == NULL )
	{
		exit( 1 );
	}

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
#ifndef __SCENE_H_INCLUDED__ 
#define __SCENE_H_INCLUDED__ 

#include <string.h> 

#include "DisplayObject3D.h"

typedef struct SCENE
{
	DisplayObject3D * do3d;

	struct SCENE * next;
}Scene;

Scene * scene_initiate()
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

int scene_push( Scene * head, int i, DisplayObject3D * do3d )
{
	Scene * p, * q;
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
		printf( "插入位置出错!" );

		return FALSE;
	}

	if( ( q = ( Scene * )malloc( sizeof( Scene ) ) ) == NULL )
	{
		exit( 1 );
	}

	q->do3d = do3d;

	q->next = p->next;

	p->next = q;

	return TRUE;
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

int scene_get( Scene * head, int i, DisplayObject3D * do3d )
{
	Scene * p;

	int j;

	p = head;

	j = - 1;

	while( p->next != NULL && j < i )
	{
		p = p->next;

		j ++;
	}

	if( j != i )
	{
		printf( "取元素位置参数错!" );
		return FALSE;
	}

	do3d = p->do3d;

	return TRUE;
}

Scene * scene_find( Scene * head, DisplayObject3D * do3d )
{   
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
	DisplayObject3D * ptr;

	if (parent != NULL)
	{
		p = scene_find(head, parent);

		if (p == NULL)
		{
			exit(1);
		}
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

	if( ( n->do3d = ( DisplayObject3D * )malloc( sizeof( DisplayObject3D ) ) ) == NULL )
	{
		exit( 1 );
	}

	ptr = memcpy( n->do3d, do3d, sizeof( DisplayObject3D ) ); 
	
	if( ptr == NULL )
	{
		exit( 1 );
	}

	do3d = ptr;

	n->next = p->next;

	p->next = n;
}

void scene_project(Scene * head)
{
	Scene * p;

	p = head;

	while( p->next != NULL )
	{
		do3d_updateTransform(p->do3d);

		p = p->next;
	}
}

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
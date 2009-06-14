#ifndef __SCENE_H_INCLUDED__ 
#define __SCENE_H_INCLUDED__ 

#include "DisplayObject3D.h"

typedef struct Node
{
	DisplayObject3D * do3d;

	struct Node * next;
}Scene;

void scene_initiate( Scene * * head )
{
	if( ( * head = ( Scene * )malloc( sizeof( Scene ) ) ) == NULL )
	{
		exit( 1 );
	}

	( * head ) -> next = NULL;
}

int scene_length( Scene * head )
{
	Scene * p = head;
	
	int size = 0;

	while( p -> next != NULL )
	{
		p = p -> next;

		size ++;
	}

	return size;
}

int scene_push( Scene * head, int i, DisplayObject3D * x )
{
	Scene * p, * q;

	int j;

	p = head;
	j = - 1;

	while( p-> next != NULL && j< i - 1 )
	{
		p = p -> next;
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

	q -> do3d = x;

	q ->next = p -> next;

	p -> next = q;

	return TRUE;
}

int scene_pop( Scene * head, int i, DisplayObject3D * x )
{
	Scene * p, * s;

	int j;

	p = head;

	j = - 1;

	while( p -> next != NULL && p -> next -> next != NULL && j < i - 1 )
	{
		p = p -> next;

		j ++;
	}

	if( j != i - 1 )
	{
		printf( "删除位置参数错误!" );

		return FALSE;
	}

	s = p -> next;

	x = s -> do3d;

	p -> next = p -> next -> next;

	free( s );

	return TRUE;
}

int scene_get( Scene * head, int i, DisplayObject3D * x )
{
	Scene * p;

	int j;

	p = head;

	j = - 1;

	while( p -> next != NULL && j < i )
	{
		p = p -> next;

		j ++;
	}

	if( j != i )
	{
		printf( "取元素位置参数错!" );

		return FALSE;
	}

	x = p -> do3d;

	return TRUE;
}

Scene * scene_find( Scene * head, DisplayObject3D * item )
{   
	if ( head == NULL )
	{
		return FALSE;
	}
	else
	{
		if ( item == head->do3d )
		{
			return head;
		}
		else
		{
			return scene_find( head->next, item );
		}
	}
}

void scene_destroy( Scene * * head )
{
	Scene * p, * p1;

	p = * head;

	while( p != NULL )
	{
		p1 = p;

		p = p -> next;

		free( p1 );
	}

	* head = NULL;
}

#endif
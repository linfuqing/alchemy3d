#ifndef __FACES_H
#define __FACES_H

//verson 1.1
#include <malloc.h>

#include "Polygon.h"

//N
typedef struct FaceNode
{
	Polygon * face;

	struct FaceNode * next;
}FaceNode;

//N
typedef struct Faces
{
	int nFaces;

	struct FaceNode * nodes;
}Faces;

Faces * newFaces()
{
	Faces * f;

	if( ( f = ( Faces * )malloc( sizeof( Faces ) ) ) == NULL )
	{
		exit( TRUE );
	}

	f->nFaces = 0;
	f->nodes = NULL;

	return f;
}

int faces_push( Faces * head, Polygon * face )
{
	FaceNode * p, * q;

	p = head->nodes;

	if (NULL != p)
	{
		while( NULL != p->next )
		{
			p = p->next;
		}
	}

	if( ( q = ( FaceNode * )malloc( sizeof( FaceNode ) ) ) == NULL )
	{
		exit( TRUE );
	}

	q->face = face;

	q->next = NULL;

	if (NULL == p)
	{
		head->nodes = q;
	}
	else
	{
		p->next = q;
	}

	head->nFaces ++;

	return TRUE;
}

int faces_unshift( Faces * head, Polygon * face )
{
	FaceNode * p;

	if( ( p = ( FaceNode * )malloc( sizeof( FaceNode ) ) ) == NULL )
	{
		exit( TRUE );
	}

	p->face = face;

	p->next = head->nodes;

	head->nodes = p;

	head->nFaces ++;

	return TRUE;
}

void faces_destroy( Faces * * head )
{
	FaceNode * p, * p1;

	p = (* head)->nodes;

	while( p != NULL )
	{
		p1 = p;

		p = p->next;

		free( p1 );
	}

	free(* head);

	* head = NULL;
}

#endif
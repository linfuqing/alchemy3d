#ifndef __VERTICES_H
#define __VERTICES_H

//verson 1.1;
#include <malloc.h>

#include "Vertex.h"

//N
typedef struct VertexNode
{
	Vertex * vertex;

	struct VertexNode * next;
}VertexNode;

//N
typedef struct Vertices
{
	int nVertices;

	struct VertexNode * nodes;
}Vertices;

Vertices * newVertices()
{
	Vertices * v;

	if( ( v = ( Vertices * )malloc( sizeof( Vertices ) ) ) == NULL )
	{
		exit( TRUE );
	}

	v->nVertices = 0;
	v->nodes = NULL;

	return v;
}

void vertices_dispose( Vertices * v )
{
}

int vertices_push( Vertices * head, Vertex * vertex )
{
	VertexNode * p, * q;

	p = head->nodes;

	if (NULL != p)
	{
		while( NULL != p->next )
		{
			p = p->next;
		}
	}

	if( ( q = ( VertexNode * ) malloc( sizeof( VertexNode ) ) ) == NULL )
	{
		exit( TRUE );
	}

	q->vertex = vertex;

	q->next   = NULL;

	if (NULL == p)
	{
		head->nodes = q;
	}
	else
	{
		p->next = q;
	}

	head->nVertices ++;

	return TRUE;
}

//int vertices_unshift( Vertices * head, Vertex * vertex )
//{
//	VertexNode * p;
//
//	if( ( p = ( VertexNode * )malloc( sizeof( VertexNode ) ) ) == NULL )
//	{
//		exit( TRUE );
//	}
//
//	p->vertex = vertex;
//
//	p->next = head->nodes;
//
//	head->nodes = p;
//
//	head->nVertices ++;
//
//	return TRUE;
//}

void vertices_destroy( Vertices * * head )
{
	VertexNode * p, * p1;

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

//void vertices_order( Vertices * head, void visit( Vertex * vertex ) )
//{
//	Vertices * p = head -> next;
//
//	while( p != NULL )
//	{
//		visit(  p -> vertex );
//
//		p = p -> next;
//	}
//}

# endif
#ifndef FACES_H
#define FACES_H

//verson 1.1

# include"Polygon.h"

//N
typedef struct Faces
{
	Polygon      * face;

	struct Faces * next;
}Faces;

void faces_initiate( Faces * * head )
{
	if( ( ( * head ) = ( Faces * )malloc( sizeof( Faces ) ) ) == NULL )
	{
		exit( TRUE );
	}

	( * head ) -> next = NULL;
}

void faces_push( Faces * head, Polygon * face )
{
	Faces * p, * q;

	p = head;

	while( p -> next != NULL )
	{
		p = p -> next;
	}

	if( ( q = ( Faces * )malloc( sizeof( Faces ) ) ) == NULL )
	{
		exit( TRUE );
	}

	q -> face = face;

	q -> next = NULL;

	p -> next = q;
}

void faces_unshift( Faces * head, Polygon * face )
{
	Faces * p;

	if( ( p = ( Faces * )malloc( sizeof( Faces ) ) ) == NULL )
	{
		exit( TRUE );
	}

	p -> face = face;

	p -> next = head -> next;

	head -> next = p;
}

int faces_length( Faces * head )
{
	Faces * p;

	int  size = 0;

	p = head;

	while( p -> next != NULL )
	{
		p = p -> next;

		size ++;
	}

	return size;
}

int faces_verticesLength( Faces * head )
{
	Faces * p;

	int size = 0;

	p = head;

	while( p -> next != NULL )
	{
		p = p -> next;

		size += polygon_length( p -> face );
	}

	return size;
}

void faces_destroy( Faces * * head )
{
	Faces * p, * p1;

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
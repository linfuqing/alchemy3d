#ifndef FACES_H
#define FACES_H

//verson 1.1

# include"Polygon.h"

typedef struct Faces
{
	Polygon      * face;

	struct Faces * next;
}Faces;

void faces_initiate( Faces * * head )
{
	if( ( ( * head ) = ( Faces * )malloc( sizeof( Faces ) ) ) == NULL )
	{
		exit( 0 );
	}

	( * head ) -> next = NULL;
}

int faces_push( Faces * head, Polygon * face )
{
	Faces * p, * q;

	if( !polygon_check( face ) )
	{
		printf( "多边形未初始化!" );
		return FALSE;
	}

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

	return TRUE;
}

int faces_unshift( Faces * head, Polygon * face )
{
	Faces * p;

	if( !polygon_check( face ) )
	{
		printf( "多边形未初始化!" );
		return FALSE;
	}

	if( ( p = ( Faces * )malloc( sizeof( Faces ) ) ) == NULL )
	{
		exit( TRUE );
	}

	p -> face = face;

	p -> next = head -> next;

	head -> next = p;

	return TRUE;
}

int faces_length( Faces * head )
{
	Faces * p = head;

	int  size = 0;

	p = head;

	while( p -> next != NULL )
	{
		p = p -> next;

		size ++;
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
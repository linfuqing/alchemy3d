# ifndef VERTICES_H
# define VERTICES_H


//verson 1.1;

# include "Vertex.h"

//N
typedef struct Vertices
{
	Vertex * vertex;

	struct Vertices * next;
}Vertices;

void vertices_initiate( Vertices * * head )
{
	if( ( * head = ( Vertices * )malloc( sizeof( Vertices ) ) ) == NULL )
	{
		exit( TRUE );
	}

	( * head ) -> next = NULL;
}

int vertices_length( Vertices * head )
{
	Vertices * p = head;

	int     size = 0;

	p = head;

	while( p -> next != NULL )
	{
		p = p -> next;

		size ++;
	}

	return size;
}

int vertices_push( Vertices * head, Vertex * vertex )
{
	Vertices * p, * q;

	if( !vertex_check( vertex ) )
	{
		printf( "\n顶点未初始化!\n" );

		return FALSE;
	}

	p = head;

	while( p -> next != NULL )
	{
		p = p -> next;
	}

	if( ( q = ( Vertices * ) malloc( sizeof( Vertices ) ) ) == NULL )
	{
		exit( TRUE );
	}

	q -> vertex = vertex;

	q -> next   = NULL;

	p -> next = q;

	return TRUE;
}

int vertices_unshift( Vertices * head, Vertex * vertex )
{
	Vertices * p;

	if( !vertex_check( vertex ) )
	{
		printf( "\n顶点未初始化!\n" );
		return FALSE;
	}

	if( ( p = ( Vertices * )malloc( sizeof( Vertices ) ) ) == NULL )
	{
		exit( TRUE );
	}

	p -> vertex = vertex;

	p -> next = head -> next;

	head -> next = p;

	return TRUE;
}

int vertices_index( Vertices * head, Vertex * v )
{
	Vertices * p     = head -> next;
	int        index = 1;

	while( p != NULL )
	{
		if( p -> vertex == v )
		{
			return index;
		}

		index ++;

		p = p -> next;
	}

	return 0;
}

void vertices_destroy( Vertices * * head )
{
	Vertices * p, * p1;

	p = * head;

	while( p != NULL )
	{
		p1 = p;

		p = p -> next;

		free( p1 );
	}

	* head = NULL;
}

void vertices_order( Vertices * head, void visit( Vertex * vertex ) )
{
	Vertices * p = head -> next;

	while( p != NULL )
	{
		visit(  p -> vertex );

		p = p -> next;
	}
}

void transformVertices( Matrix3D * m, Vertices * head )
{
	Vertices * p = head -> next;

	while( p != NULL )
	{
		transformVertex( m, p -> vertex );

		p = p -> next;
	}
}

# endif
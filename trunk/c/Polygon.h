#ifndef POLYGON_H
#define POLYGON_H

/**
双堆栈结构。
__________________
verson 1.4
**/

# include <stdio.h>

# include "Vertex.h"
# include "Vector.h"

typedef struct Polygon
{
	Vertex * vertex;
	Vector * uv;

	Vector3D * normal;

	struct Polygon * next;
}Polygon;

int polygon_check( Polygon * node )
{
	return node && vertex_check( node -> vertex );
}

int isPolygon( Polygon * head )
{
	return polygon_check( head -> next ) && polygon_check( head -> next -> next ) && polygon_check( head -> next -> next -> next );
}

int polygon_isTriangle( Polygon * head )
{
	return isPolygon( head ) && ( head -> next -> next -> next -> next == NULL );
}

Vector3D polygon_normal( Vertex * v1, Vertex * v2, Vertex * v3 )
{	
	Vector3D * a, * b, * c, ca, bc,nor;

	if( !( vertex_check( v1 ) && vertex_check( v2 ) && vertex_check( v3 ) ) )
	{
		exit( 0 );
	}
	
	a = v1 -> worldPosition;
	b = v2 -> worldPosition;
	c = v3 -> worldPosition;

	ca = vector3D_subtract( * c, * a );
	bc = vector3D_subtract( * b, * c );

	nor = vector3D_crossProduct( ca, bc );

	vector3D_normalize( & nor );

	return nor;
}

Vector3D buildPolygon( Polygon * head )
{
	Polygon * p;

	if( !isPolygon( head ) )
	{
		exit( FALSE );
	}

	if( ( head -> normal = ( Vector3D * )malloc( sizeof( Vector3D ) ) ) == NULL )
	{
		exit( TRUE );
	}

	* ( head -> normal ) = polygon_normal( head -> vertex, head -> next -> vertex, head -> next -> next -> vertex );

	p = head -> next;

	while( p != NULL )
	{
		p -> normal = head -> normal;

		p = p -> next;
	}

	return * ( head -> normal );
}

void polygon_initiate( Polygon * * head )
{
	if( ( * head = ( Polygon * )malloc( sizeof( Polygon ) ) ) == NULL )
	{
		exit( TRUE );
	}

	( * head ) -> next = NULL;
	( * head ) -> normal = NULL;
}

int polygon_length( Polygon * head )
{
	Polygon * p = head;
	
	int size = 0;

	while( p -> next != NULL )
	{
		p = p -> next;

		size ++;
	}

	return size;
}

int polygon_push( Polygon * head, Vertex * vertex, Vector * uv )
{
	Polygon * p, * q;

	if( !vertex_check( vertex ) )
	{
		printf( "顶点未初始化!" );
		return FALSE;
	}

	p = head;

	if( isPolygon( head ) )
	{
		while(  p -> next -> next != NULL )
		{
			p = p -> next;
		}

		if( vector3D_equals( head -> normal ? (  * ( head -> normal ) ) : buildPolygon( head ), polygon_normal( p -> vertex, p -> next -> vertex, vertex ),FALSE ) )
		{
			p = p -> next;
		}
		else
		{
			printf( "法向量不对，无法插入!" );
			return FALSE;
		}
	}
	else
	{
		while(  p -> next != NULL )
		{
			p = p -> next;
		}
	}

	if( ( q = ( Polygon * )malloc( sizeof( Polygon ) ) ) == NULL )
	{
		exit( TRUE );
	}

	q -> vertex = vertex;
	q -> uv     = uv;

	q -> normal = head -> normal;

	q -> next   = NULL;

	p -> next   = q;

	return TRUE;
}

int polygon_unshift( Polygon * head, Vertex * vertex, Vector * uv )
{
	Polygon * p;

	if( !vertex_check( vertex ) )
	{
		printf( "顶点未初始化!" );
		return FALSE;
	}

	if( isPolygon( head ) && vector3D_equals( head -> normal ? (  * ( head -> normal ) ) : buildPolygon( head ), polygon_normal( head -> next -> vertex, head -> next -> next -> vertex, vertex ),FALSE ) )
	{
		printf( "法向量不对，无法插入!" );
		return FALSE;
	}

	if( ( p = ( Polygon * )malloc( sizeof( Polygon ) ) ) == NULL )
	{
		exit( TRUE );
	}

	p -> vertex = vertex;
	p -> uv     = uv;

	p -> normal = head -> normal;
	p -> next   = head -> next;

	head -> next = p;

	return TRUE;
}

int polygon_pop( Polygon * head, Vertex * * vertex, Vector * * uv )
{
	Polygon * p, * s;

	if( head -> next == NULL )
	{
		printf( "无元素可出栈!" );
		return FALSE;
	}

	p = head;

	while( p -> next -> next != NULL )
	{
		p = p -> next;
	}

	s = p -> next;

	if( vertex )
	{
		* vertex = s -> vertex;
	}

	if( uv )
	{
		* uv     = s -> uv;
	}

	p -> next = NULL;

	free( s );

	return TRUE;
}

int polygon_shift( Polygon * head, Vertex * * vertex, Vector * * uv )
{
	Polygon * s;

	if( ( s = head -> next ) == NULL )
	{
		printf( "无元素可出栈!" );
		return FALSE;
	}

	if( vertex )
	{
		* vertex = s -> vertex;
	}

	if( uv )
	{
		* uv     = s -> uv;
	}

	head -> next = head -> next -> next;

	free( s );

	return TRUE;
}

void polygon_destroy( Polygon * * head )
{
	Polygon * p, * p1;

	p = * head;

	while( p != NULL )
	{
		p1 = p;

		p = p -> next;

		free( p1 );
	}

	* head = NULL;
}

Polygon * newTriangle3D( Vertex * va, Vertex * vb, Vertex * vc, Vector *uva, Vector * uvb, Vector * uvc )
{
	Polygon * p;

	polygon_initiate( & p );

	polygon_push( p, va, uva );
	polygon_push( p, vb, uvb );
	polygon_push( p, vc, uvc );

	buildPolygon( p );

	return p;
}

void polygon_order( Polygon * head, void visit( Vertex vertex, Vector uv ) )
{
	Polygon * p = head -> next;

	while( p != NULL )
	{
		visit( * ( p -> vertex ), * ( p -> uv ) );

		p = p -> next;
	}
}

#endif
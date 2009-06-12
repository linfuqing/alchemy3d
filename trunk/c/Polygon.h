/**
可查找双链堆栈结构。
**/

# include <stdio.h>

# include "Vertex.h"
# include "Vector.h"

typedef struct Node
{
	Vertex * vertex;
	Vector * uv;

	Vector3D * normal;

	struct Node * next;
}Polygon;

int polygon_check( Polygon * node )
{
	return node && vertex_check( node -> vertex );
}

int isPolygon( Polygon * head )
{
	return polygon_check( head ) && polygon_check( head -> next ) && polygon_check( head -> next -> next );
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

Vector3D buildPolygon( Polygon * * head )
{
	Polygon * p = * head;

	if( !isPolygon( * head ) )
	{
		exit( 0 );
	}

	if( ( ( * head ) -> normal = ( Vector3D * )malloc( sizeof( Vector3D ) ) ) == NULL )
	{
		exit( 0 );
	}

	* ( ( * head ) -> normal ) = polygon_normal( ( * head ) -> vertex, ( * head ) -> next -> vertex, ( * head ) -> next -> next -> vertex );

	while( p -> next != NULL )
	{
		p -> normal = ( * head ) -> normal;
		p = p -> next;
	}

	return * ( ( * head ) -> normal );
}

void polygon_initiate( Polygon * * head )
{
	if( ( * head = ( Polygon * )malloc( sizeof( Polygon ) ) ) == NULL )
	{
		exit( 1 );
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

	Vector3D normal;

	p = head;

	if( isPolygon( head ) )
	{
		while(  p -> next -> next != NULL )
		{
			p = p -> next;
		}

		normal = head -> normal ? (  * ( head -> normal ) ) : buildPolygon( & head );

		if( vector3D_equals( normal, polygon_normal( p -> vertex, p -> next -> vertex, vertex ),FALSE ) )
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
		exit( 1 );
	}

	q -> vertex = vertex;
	q -> uv     = uv;

	q -> next = p -> next;

	p -> next = q;

	return TRUE;
}

void polygon_pop( Polygon * head, Vertex * vertex, Vector * uv )
{
	Polygon * p, * s;

	p = head;

	while( p -> next != NULL && p -> next -> next != NULL )
	{
		p = p -> next;
	}

	s = p -> next;

	if( vertex )
	{
		vertex = s -> vertex;
	}

	if( uv )
	{
		uv     = s -> uv;
	}

	p -> next = p -> next -> next;

	free( s );
}

int polygon_get( Polygon * head, int i, Vertex * vertex, Vector * uv )
{
	Polygon * p;

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

	vertex = p -> vertex;
	uv     = p -> uv;

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

	return p;
}

void polygon_order( Polygon * head, void visit( Vertex vertex, Vector uv ) )
{
	Polygon * p;

	p = head;

	while( p -> next != NULL )
	{
		visit( * ( p -> vertex ), * ( p -> uv ) );

		p = p -> next;
	}
}
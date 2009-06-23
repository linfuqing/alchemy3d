#include "Viewport.h"

//��Ⱦ��
void render( Faces face )
{
}

void main( void )
{
	Faces    * f;
	Vertices * v;
	Polygon  * p;

	Scene    * s;
	Viewport * view;

	//�½������.
	p = newTriangle3D( newVertex( newVector3D( 1, 2, 3, 1 ) ), newVertex( newVector3D( 1, 2, 3, 1 ) ), newVertex( newVector3D( 1, 2, 3, 1 ) ), NULL, NULL, NULL );

	//��ʼ��.
	faces_initiate( & f );
	vertices_initiate( & v );

	//������Ͷ���.
	faces_push( f, p );
	//....
	vertices_push( v, p -> next -> vertex );
	vertices_push( v, p -> next -> next -> vertex );
	vertices_push( v, p -> next -> next -> next -> vertex );
	//...

	s    = newScene( newMesh( f, v ) );

	view = newViewport( 0, 0, 500, 500, s );

	//�Գ������任.
	transformScene( s );

	//ͶӰ�������Գ�������Ⱦ.
	projectScene( view, render );
} 

#include "Viewport.h"

//渲染器
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

	//新建多边形.
	p = newTriangle3D( newVertex( newVector3D( 1, 2, 3, 1 ) ), newVertex( newVector3D( 1, 2, 3, 1 ) ), newVertex( newVector3D( 1, 2, 3, 1 ) ), NULL, NULL, NULL );

	//初始化.
	faces_initiate( & f );
	vertices_initiate( & v );

	//加入面和顶点.
	faces_push( f, p );
	//....
	vertices_push( v, p -> next -> vertex );
	vertices_push( v, p -> next -> next -> vertex );
	vertices_push( v, p -> next -> next -> next -> vertex );
	//...

	s    = newScene( newMesh( f, v ) );

	view = newViewport( 0, 0, 500, 500, s );

	//对场景做变换.
	transformScene( s );

	//投影场景并对场景做渲染.
	projectScene( view, render );
} 

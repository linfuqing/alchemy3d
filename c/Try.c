#include "Render.h"


void main( void )
{
	Faces    * f;
	Vertices * v;
	Polygon  * p;

	Camera   * c;
	Scene    * s;
	Screen   * screen;
	World    * w;
	Viewport * view;

	Mesh     * m;

	RenderEngine * r;

	//新建多边形.
	p = newTriangle3D( newVertex( newVector3D( 3.6, 2, 3, 1 ) ), newVertex( newVector3D( 1, 8, 4, 1 ) ), newVertex( newVector3D( 6, 2, 1, 1 ) ), NULL, NULL, NULL );

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

	m = newMesh( f, v );

	//project = newProjection( m, NULL );

	//project -> vertices -> next -> vertex -> screen -> x =100;

	s      = newScene( newMesh( f, v ), NULL );

	c      = newCamera( 250, 0, 250, NULL );

	view   = newViewport( 0, 0, 500, 500, c );

	screen = newScreen( view );

	w      = newWorld( s );

	//scene_addViewport( s, view );
	world_addViewport( w, screen, OFF );

	r    = newRenderEngine( 800, 600 );

	//renderEngine_addScene( r, s );

	renderEngine_addScene( r, w );

	render( r, RENDER_MODE_DYNAMIC );
}

typedef struct Node
{
	Vertex vertex;

	Vector uv;

	struct Node * parent;
	struct Node * next;
}Polygon;

Polygon newTriangle3D( Vertex * va, Vertex * vb, Vertex * vc, Vector *uva, Vector * uvb, Vector * uvc )
{
	Polygon * * p;

	polygon_initiate( p );

	polygon_push( * p, va, uva );polygon_push( * p, va, uva );polygon_push( * p, va, uva );

}
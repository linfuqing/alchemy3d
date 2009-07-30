#include <stdlib.h>
#include <AS3.h>

#include "Render.h"

AS3_Val initializeVector( void * self, AS3_Val args )
{
	Vector * v;

	AS3_ArrayValue( args, "PtrType", v );

	return AS3_Array( "PtrType, PtrType", & ( v -> x ), & ( v -> y ) );
}

AS3_Val initializeVertex( void * self, AS3_Val args )
{
	Vertex * v;

	AS3_ArrayValue( args, "PtrType", v );

	return AS3_Array( "PtrType, PtrType, PtrType, PtrType, PtrType, PtrType", & ( v -> position -> x ), & ( v -> position -> y ), & ( v -> position -> z ), & ( v -> worldPosition -> x ), & ( v -> worldPosition -> y ), & ( v -> worldPosition -> z ) ); 
}

AS3_Val initializeVertices( void * self, AS3_Val args )
{
	Vertices * vs;

	Vertex   * v;

	Vector3D * v3d, * vp;

	Number   * vertices;

	unsigned int length, i, vi;

	AS3_ArrayValue( args, "PtrType, IntType", vertices, & length );

	if( ( vs  = ( Vertices * )malloc( sizeof( Vertices ) * length + 1 ) ) == NULL
	 || ( v   = ( Vertex   * )malloc( sizeof( Vertex   ) * length     ) ) == NULL
	 || ( v3d = ( Vector3D * )malloc( sizeof( Vector3D ) * length * 2 ) ) == NULL )
	{
		exit( TRUE );
	}

	for( i = 1; i <= length; i ++ )
	{
		vi                             = i - 1;

		vp                             = v3d + 2 * vi;
		vp -> x                        = vertices[vi];
		vp -> y                        = vertices[vi + 1];
		vp -> z                        = vertices[vi + 2];
		vp -> w                        = 1;

		* ( vp + 1 )                   = * vp;

		
		  ( v  + vi ) -> position      = vp;
		  ( v  + vi ) -> worldPosition = vp  + 1;
		  ( vs + i  ) -> vertex        = v   + vi;
		  ( vs + vi ) -> next          = vs  + i;
	}

	( vs + i - 1 ) -> next = NULL;

	free( vertices );

	return AS3_Array( "PtrType, PtrType",vs, v );
}

AS3_Val initializeFaces( void * self, AS3_Val args )
{
	Vertex * * vp;
	Number * uvp;
	unsigned int * lp, fl, vl, i, j, total = 0;

	Polygon * pp, * phead;
	Faces   * fp;
	Vector  * uvs;

	AS3_ArrayValue( args, "PtrType, PtrType, PtrType, IntType, IntType", vp, uvp, lp, &fl, & vl );

	if( ( pp  = ( Polygon * )malloc( sizeof( Polygon ) * vl + fl ) ) == NULL
	 || ( fp  = ( Faces   * )malloc( sizeof( Faces   ) * fl + 1  ) ) == NULL
	 || ( uvs = ( Vector  * )malloc( sizeof( Vector  ) * vl      ) ) == NULL )
	{
		exit( TRUE );
	}

	for( i = 0; i < fl; i ++ )
	{
		phead = pp;

		for( j = 0; j < lp[i]; j ++ )
		{
			pp ++;

			( uvs + total ) -> x      = * ( uvp + 2 * total );
			( uvs + total ) -> y      = * ( uvp + 2 * total + 1 );

			pp         -> vertex = vp[total];
			pp         -> uv     = uvs + total;
			( pp - 1 ) -> next   = pp;

			total ++;
		}

		( pp - 1 ) -> next = NULL;

		buildPolygon( phead );

		( fp + i + 1 ) -> face = pp;

		( fp + i     ) -> next = fp + i + 1;
	}

	( fp + i ) -> next = NULL;

	free( vp  );
	free( uvp );
	free( lp  );

	return AS3_Array( "PtrType, PtrType, PtrType", fp, fp + 1, uvs );
}

AS3_Val initializeMesh( void * self, AS3_Val args )
{
	Vertices * vertices;
	Faces    * faces;

	AS3_ArrayValue( args, "PtrType, PtrType", vertices, faces );

	return AS3_Ptr( newMesh( faces, vertices ) );
}

AS3_Val initializeCamera( void * self, AS3_Val args )
{
	return 0;
}

AS3_Val initializeScene( void * self, AS3_Val args )
{
	Scene * s;

	Mesh * m;

	AS3_ArrayValue( args, "PtrType", m );

	s = newScene( m );

	return AS3_Array( "PtrType, PtrType, PtrType", s, & ( s -> visible ), s -> camera );
}

//初始化场景
//返回该对象的起始指针
AS3_Val initializeViewport( void * self, AS3_Val args )
{
	Viewport * view;
	Scene    * s;

	Number left, right, top, bottom, far, near;

	AS3_ArrayValue( args, "DoubleType, DoubleType, DoubleType, DoubleType, DoubleType, DoubleType, PtrType", &left, &right, &top, &bottom, &far, &near, &s );

	view = newViewport( left, right, top, bottom, far, near, s );

	return AS3_Ptr( view );
}

//渲染
AS3_Val beginRender( void * self, AS3_Val args )
{
	RenderEngine * r;

	AS3_ArrayValue( args, "PtrType", &r );

	render( r, RENDER_MODE_DYNAMIC );

	return 0;
}

AS3_Val applyForTmpBuffer( void * self, AS3_Val args )
{
	int size;

	void * tmpBuffer;

	AS3_ArrayValue( args, "IntType", &size );

	if( ( tmpBuffer = malloc( size ) ) == NULL )
	{
		exit( TRUE );
	}

	return AS3_Ptr( tmpBuffer );
}

AS3_Val doubleTypeSize( void * self, AS3_Val args )
{
	return AS3_Int( ( int )sizeof( double ) );
}

AS3_Val intTypeSize( void * self, AS3_Val args )
{
	return 0;//AS3_Int( sizeof( int ) );
}

//测试函数
AS3_Val test( void* self, AS3_Val args )
{
	/*entity_updateTransform(&entity);

	AS3_Trace(AS3_Number(entity.position.x));
	AS3_Trace(AS3_Number(entity.position.y));*/

	Number * x =( Number * )malloc( sizeof( Number ) * 2 );

	x[0] = 234.4447567657567;
	x[1] = 123.567555;

	return AS3_Ptr( x );
}

//入口
int main( void )
{
	AS3_Val initializeVectorMethod = AS3_Function( NULL, initializeVector );
	AS3_Val initializeVertexMethod = AS3_Function( NULL, initializeVertex );
	AS3_Val initializeFacesMethod = AS3_Function( NULL, initializeFaces );
	AS3_Val initializeViewportMethod = AS3_Function( NULL, initializeViewport );
	AS3_Val initializeVerticesMethod = AS3_Function( NULL, initializeVertices );
	AS3_Val initializeMeshMethod = AS3_Function( NULL, initializeMesh );
	AS3_Val doubleTypeSizeMethod = AS3_Function( NULL, doubleTypeSize );
	AS3_Val intTypeSizeMethod = AS3_Function( NULL, intTypeSize );
	AS3_Val applyForTmpBufferMethod = AS3_Function( NULL, applyForTmpBuffer );
	AS3_Val beginRenderMethod = AS3_Function( NULL, beginRender );
	AS3_Val testMethod = AS3_Function( NULL, test );



	AS3_Val result = AS3_Object( "initializeVector:AS3ValType, initializeVertex:AS3ValType, initializeFaces:AS3ValType, initializeViewport:AS3ValType, initializeVertices:AS3ValType, initializeMesh:AS3ValType, doubleTypeSize:AS3ValType, intTypeSize:AS3ValType, applyForTmpBuffer:AS3ValType, beginRender:AS3ValType, test:AS3ValType",
		initializeVectorMethod, initializeVertexMethod, initializeFacesMethod, initializeViewportMethod, initializeVerticesMethod, initializeMeshMethod, doubleTypeSizeMethod, intTypeSizeMethod, applyForTmpBufferMethod, beginRenderMethod, testMethod );



	AS3_Release( initializeVectorMethod );
	AS3_Release( initializeVertexMethod );
	AS3_Release( initializeFacesMethod );
	AS3_Release( initializeViewportMethod );
	AS3_Release( initializeVerticesMethod );
	AS3_Release( initializeMeshMethod );
	AS3_Release( doubleTypeSizeMethod );
	AS3_Release( intTypeSizeMethod );
	AS3_Release( applyForTmpBufferMethod );
	AS3_Release( beginRenderMethod );
	AS3_Release( testMethod );


	AS3_LibInit( result );

	return 0;
}
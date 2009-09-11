# include "Mesh.h"

Mesh * newPlane( 
				Mesh     * base, 
				Material * material, 
				Texture  * texture, 
				float width, 
				float height, 
				unsigned int segments_width, 
				unsigned int segments_height )
{
	float gridX    = segments_width + 1,     gridY    = segments_height + 1, 
		  textureX = width * .5f,            textureY = height * .5f, 
		  iW       = width / segments_width, iH       = height / segments_height,
		  x,y;

	Vector * uvA, * uvB, * uvC;

	Vector3D * a, * b, * c;

	unsigned int ix, iy, aIndex, cIndex, bIndex;
	
	mesh_reBuild( base, gridX, segments_width * segments_height, NULL );

	// Vertices
	for( ix = 0; ix < gridX; ix++ )
	{
		for( iy = 0; iy < gridY; iy++ )
		{
			x = ix * iW - textureX;
			y = iy * iH - textureY;
	
			mesh_push_vertex( base, x, y, 0 );
		}
	}
			
	// Faces	
	for(  ix = 0; ix < segments_width; ix++ )
	{
		for(  iy = 0; iy < segments_height; iy++ )
		{
			// Triangle A
			aIndex = ix     * gridY + iy;
			cIndex = ix     * gridY + (iy+1);
			bIndex = (ix+1) * gridY + iy;
					
			a = m -> vertices[aIndex];
			c = m -> vertices[cIndex];
			b = m -> vertices[bIndex];
	
			uvA =  newVector( ix     / segments_width, iy     / segments_height );
			uvC =  newVector( ix     / segments_width, (iy+1) / segments_height );
			uvB =  newVector( (ix+1) / segments_width, iy     / segments_height );
	
			mesh_push_triangle( base, a, b, c, uvA, uvB, uvC, material, texture );
	
			// Triangle B
			aIndex = (ix+1) * gridY + (iy+1);
			cIndex = (ix+1) * gridY + iy;
			bIndex = ix     * gridY + (iy+1);
					
			a = m -> vertices[aIndex];
			c = m -> vertices[cIndex];
			b = m -> vertices[bIndex];
	
			uvA =  newVector( (ix+1) / segments_width, (iy+1)  / segments_height );
			uvC =  newVector( (ix+1) / segments_width, iy      / segments_height );
			uvB =  newVector( ix      / segments_width, (iy+1) / segments_height );
					
			mesh_push_triangle( base, a, b, c, uvA, uvB, uvC, material, texture );
		}
	}
}

buildPlane(Mesh * base, Material * material, Texture * texture, int u, int v, float width, float height, float depth, int reverse):void
{
	// Find w depth axis
	int w = 0, count = 0;
	int iu = 0, iv = 0, i = 0;
	float gridU, gridV, gridU1, gridV1;
	float textureU, textureV, incU, incV;

	Vertex * vertex, * planeVerts;
	Vertex * a, * b, * c;
	float vx, vy, vz;

	Vector uvA;
	Vector uvC;
	Vector uvB;

	int aIndex;
	int cIndex;
	int bIndex;

	if( (u == 0 && v == 1) || (u == 1 && v == 0) ) w = 2;
	else if( (u == 0 && v == 2) || (u ==2 && v == 0) ) w = 1;
	else if( (u == 2 && v == 1) || (u == 1 && v == 2) ) w = 0;

	// Build plane
	gridU	= u;
	gridV	= v;
	gridU1	= gridU + 1;
	gridV1	= gridV + 1;

	if( ( planeVerts = ( Vertex * )calloc( gridU1 * gridV1, sizeof( Vertex ) ) ) == NULL ) exit( TRUE );

	textureU  = width * 0.5f;
	textureV  = height * 0.5f;
	incU      = width / gridU;
	incV      = height / gridV;

	mesh_reBuild( base, gridU1 * gridV1, gridU * gridV, NULL );

	// Vertices
	for( iu = 0; iu < gridU1; iu++ )
	{
		for( iv = 0; iv < gridV1; iv++ )
		{
			if (u == 0)			vx = (iu * incU - textureU) * reverse;
			else if (u == 1)	vy = (iu * incU - textureU) * reverse;
			else if (u == 2)	vz = (iu * incU - textureU) * reverse;

			if (v == 0)			vx = iv * incV - textureV;
			else if (v == 1)	vy = iv * incV - textureV;
			else if (v == 2)	vz = iv * incV - textureV;

			if (w == 0)			vx = depth;
			else if (w == 1)	vy = depth;
			else if (w == 2)	vz = depth;

			planeVerts[i] = mesh_push_vertex( base, vx, vy, vz );

			i ++;
		}
	}

	for(  iu = 0; iu < gridU; iu++ )
	{
		for(  iv= 0; iv < gridV; iv++ )
		{
			// Triangle A
			a = planeVerts[ iu     * gridV1 + iv     ];
			c = planeVerts[ iu     * gridV1 + (iv+1) ];
			b = planeVerts[ (iu+1) * gridV1 + iv     ];

			uvA =  newVector( iu     / gridU, iv     / gridV );
			uvC =  newVector( iu     / gridU, (iv+1) / gridV );
			uvB =  newVector( (iu+1) / gridU, iv     / gridV );

			mesh_push_triangle( base, a, b, c, uvA, uvB, uvC, material, texture );

			free( uvA );
			free( uvC );
			free( uvB );

			// Triangle B
			a = planeVerts[ (iu+1) * gridV1 + (iv+1) ];
			c = planeVerts[ (iu+1) * gridV1 + iv     ];
			b = planeVerts[ iu     * gridV1 + (iv+1) ];

			uvA =  newVector( (iu+1) / gridU, (iv+1) / gridV );
			uvC =  newVector( (iu+1) / gridU, iv     / gridV );
			uvB =  newVector( iu     / gridU, (iv+1) / gridV );

			mesh_push_triangle( base, c, a, b, uvC, uvA, uvB, material, texture );

			free( uvA );
			free( uvC );
			free( uvB );

			count += 2;
		}
	}
}

Mesh * newCube( 
			   Mesh     * base, 
			   Material * material,
			   Texture  * texture,
			   float width, 
			   float depth, 
			   float height, 
			   unsigned int segmentsS, 
			   unsigned int segmentsT, 
			   unsigned int segmentsH, 
			   unsigned int insideFaces, 
			   unsigned int excludeFaces )
{
	
}
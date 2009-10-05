#ifndef __PRIMITIVES_H
#define __PRIMITIVES_H

# include "Mesh.h"

Mesh * newPlane( 
				Mesh     * base, 
				Material * material, 
				Texture  * texture, 
				float width, 
				float height, 
				int segments_width, 
				int segments_height,
				DWORD render_mode )
{
	int gridX    = segments_width + 1,     gridY    = segments_height + 1;
	float textureX = width * .5f,            textureY = height * .5f, 
		  iW       = width / segments_width, iH       = height / segments_height,
		  x,y;

	Vector * uvA, * uvB, * uvC;

	Vertex * a, * b, * c;

	int ix, iy, aIndex, cIndex, bIndex;
	
	base = mesh_reBuild( base, gridX * gridY, segments_width * segments_height * 2 );

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
					
			a = base -> vertices[aIndex];
			c = base -> vertices[cIndex];
			b = base -> vertices[bIndex];

			uvA =  newVector( (float)(ix)   / (float)(segments_width), (float)(iy+1) / (float)(segments_height) );
			uvC =  newVector( (float)(ix)   / (float)(segments_width), (float)(iy)   / (float)(segments_height) );
			uvB =  newVector( (float)(ix+1) / (float)(segments_width), (float)(iy+1) / (float)(segments_height) );
	
			mesh_push_triangle( base, a, b, c, uvA, uvB, uvC, material, texture, render_mode );
	
			// Triangle B
			aIndex = (ix+1) * gridY + (iy+1);
			cIndex = (ix+1) * gridY + iy;
			bIndex = ix     * gridY + (iy+1);
					
			a = base -> vertices[aIndex];
			c = base -> vertices[cIndex];
			b = base -> vertices[bIndex];
	
			uvA =  newVector( (float)(ix+1) / (float)(segments_width), (float)(iy)   / (float)(segments_height) );
			uvC =  newVector( (float)(ix+1) / (float)(segments_width), (float)(iy+1) / (float)(segments_height) );
			uvB =  newVector( (float)(ix)   / (float)(segments_width), (float)(iy)   / (float)(segments_height) );
					
			mesh_push_triangle( base, a, b, c, uvA, uvB, uvC, material, texture, render_mode );
		}
	}

	return base;
}

void buildPlane(Mesh * base, Material * material, Texture * texture, int u, int v, float width, float height, float depth, int reverse)
{
	// Find w depth axis
	int w = 0, count = 0;
	int iu = 0, iv = 0, i = 0;
	int gridU, gridV, gridU1, gridV1;
	float textureU, textureV, incU, incV;

	Vertex ** planeVerts;
	Vertex * a, * b, * c;
	float vx, vy, vz;

	Vector * uvA, * uvB, * uvC;

	if( (u == 0 && v == 1) || (u == 1 && v == 0) ) w = 2;
	else if( (u == 0 && v == 2) || (u ==2 && v == 0) ) w = 1;
	else if( (u == 2 && v == 1) || (u == 1 && v == 2) ) w = 0;

	// Build plane
	gridU	= u;
	gridV	= v;
	gridU1	= gridU + 1;
	gridV1	= gridV + 1;

	if( ( planeVerts = ( Vertex ** )calloc( gridU1 * gridV1, sizeof( Vertex * ) ) ) == NULL ) exit( TRUE );

	textureU  = width * 0.5f;
	textureV  = height * 0.5f;
	incU      = width / gridU;
	incV      = height / gridV;

	base = mesh_reBuild( base, gridU1 * gridV1, gridU * gridV );

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

			uvA =  newVector( (float)(iu     / gridU), (float)(iv     / gridV) );
			uvC =  newVector( (float)(iu     / gridU), (float)((iv+1) / gridV) );
			uvB =  newVector( (float)((iu+1) / gridU), (float)(iv     / gridV) );

			mesh_push_triangle( base, a, b, c, uvA, uvB, uvC, material, texture, RENDER_WIREFRAME_TRIANGLE_32 );

			free( uvA );
			free( uvC );
			free( uvB );

			// Triangle B
			a = planeVerts[ (iu+1) * gridV1 + (iv+1) ];
			c = planeVerts[ (iu+1) * gridV1 + iv     ];
			b = planeVerts[ iu     * gridV1 + (iv+1) ];

			uvA =  newVector( (float)((iu+1) / gridU), (float)((iv+1) / gridV) );
			uvC =  newVector( (float)((iu+1) / gridU), (float)(iv     / gridV) );
			uvB =  newVector( (float)(iu     / gridU), (float)((iv+1) / gridV) );

			mesh_push_triangle( base, c, a, b, uvC, uvA, uvB, material, texture, RENDER_WIREFRAME_TRIANGLE_32 );

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
	return base;
}

#endif
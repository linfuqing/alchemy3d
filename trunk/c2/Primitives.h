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
		  textureX = width * .5,             textureY = height * .5, 
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
	
			mesh_push_vertex( m, x, y, 0 );
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
	
			mesh_push_triangle( m, a, b, c, uvA, uvB, uvC, material, texture );
	
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
					
			mesh_push_triangle( m, a, b, c, uvA, uvB, uvC, material, texture );
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
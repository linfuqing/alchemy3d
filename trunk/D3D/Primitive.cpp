#include "StdAfx.h"
#include "Primitive.h"
#include "Vertex.h"

LPMESH CreatePlane(
				   FLOAT  fWidth, 
				   FLOAT  fHeight, 
				   int    nSgmentsWidth, 
				   int    nSegmentsHeight,
				   LPDEVICE pDevice)
{
	/*int   nGridX    = nSgmentsWidth + 1,      nGridY    = nSegmentsHeight + 1;
	FLOAT fTextureX = fWidth * .5f,           fTextureY = fHeight * .5f, 
		  fIW       = fWidth / nSgmentsWidth, fIH       = fHeight / nSegmentsHeight,
		  x,y;

	LPMESH pMesh;

	Vector * uvA, * uvB, * uvC, * uvs;

	Vertex * a, * b, * c;

	int ix, iy, aIndex, cIndex, bIndex;

	if(pMesh)
		pMesh->Release();

	D3DXCreateMeshFVF(nSgmentsWidth * nSegmentsHeight * 2, nGridX * nGridY, D3DPOOL_MANAGED, D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_NORMAL, pDevice, &pMesh);


	// Vertices
	for( ix = 0; ix < gridX; ix++ )
	{
		for( iy = 0; iy < gridY; iy++ )
		{
			x = ix * iW - textureX;
			y = textureY - iy * iH;
	
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

			uvA =  newVector( (float)(ix)   / (float)(segments_width), (float)(iy) / (float)(segments_height) );
			uvC =  newVector( (float)(ix)   / (float)(segments_width), (float)(iy + 1)   / (float)(segments_height) );
			uvB =  newVector( (float)(ix + 1) / (float)(segments_width), (float)(iy) / (float)(segments_height) );
	
			mesh_push_triangle( base, a, b, c, uvA, uvB, uvC, material, texture, render_mode );
	
			// Triangle B
			aIndex = (ix+1) * gridY + (iy+1);
			cIndex = (ix+1) * gridY + iy;
			bIndex = ix     * gridY + (iy+1);
					
			a = base -> vertices[aIndex];
			c = base -> vertices[cIndex];
			b = base -> vertices[bIndex];
	
			uvA =  newVector( (float)(ix + 1) / (float)(segments_width), (float)(iy + 1)   / (float)(segments_height) );
			uvC =  newVector( (float)(ix + 1) / (float)(segments_width), (float)(iy) / (float)(segments_height) );
			uvB =  newVector( (float)(ix)   / (float)(segments_width), (float)(iy + 1)   / (float)(segments_height) );
					
			mesh_push_triangle( base, a, b, c, uvA, uvB, uvC, material, texture, render_mode );
		}
	}

	return pMesh;*/
	return NULL;
}
#ifndef __TEXTURE_H
#define __TEXTURE_H

#include <malloc.h>

typedef struct Texture
{
	int width, height, * bitmapDataP;
	
	float * bitmapData;
}Texture;

Texture * newTexture( int width, int height, float * bitmapData )
{
	Texture * texture;
	int i, j, len, * bitmapDataP;

	if( ( texture = ( Texture * )malloc( sizeof( Texture ) ) ) == NULL )
	{
		exit( TRUE );
	}

	texture->width = width;
	texture->height = height;
	texture->bitmapData = bitmapData;

	if( ( bitmapDataP = ( int * )calloc( width * height, sizeof( int ) ) ) == NULL )
	{
		exit( TRUE );
	}

	i = j = 0;
	len = width * height * 4;

	for (; i < len; i += 4)
	{
		bitmapDataP[j++] = i;
	}

	texture->bitmapDataP = bitmapDataP;

	return texture;
}

void texture_dispose( Texture * texture )
{
	free( texture->bitmapData );
	texture->bitmapData = NULL;

	free( texture->bitmapDataP );
	texture->bitmapDataP = NULL;
}

#endif
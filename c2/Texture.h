#ifndef __TEXTURE_H
#define __TEXTURE_H

#include <malloc.h>

typedef struct Texture
{
	int width, height;
	
	uint32 * datas;
}Texture;

Texture * newTexture( int width, int height, uint32 * datas )
{
	Texture * texture;

	if( ( texture = ( Texture * )malloc( sizeof( Texture ) ) ) == NULL )
	{
		exit( TRUE );
	}

	texture->width = width;
	texture->height = height;
	texture->datas = datas;

	return texture;
}

#endif
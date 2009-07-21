#ifndef __TEXTURE_H
#define __TEXTURE_H

#include <malloc.h>

typedef struct Texture
{
	int width, height;
	
	uint32 * datas;
}Texture;

Texture * newTexture( int width, int height )
{
	Texture * texture;

	if( ( texture = ( Texture * )malloc( sizeof( Texture ) ) ) == NULL )
	{
		exit( TRUE );
	}

	texture->width = width;
	texture->height = height;

	if( ( texture->datas = ( uint32 * )malloc( sizeof( uint32 ) * width * height ) ) == NULL )
	{
		exit( TRUE );
	}

	return texture;
}

void dispose( Texture * texture )
{
	free( texture->datas );
	texture->datas = NULL;
}

#endif
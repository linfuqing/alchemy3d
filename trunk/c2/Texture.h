#ifndef __TEXTURE_H
#define __TEXTURE_H

#include <malloc.h>

typedef struct Texture
{
	int width, height, wh;
	
	LPBYTE pRGBABuffer;

	BYTE * ARGBBuffer;
}Texture;

Texture * newTexture( int width, int height, LPBYTE pRGBABuffer )
{
	Texture * texture;
	BYTE * ARGBBuffer;
	int lSize = 0, i = 0, j = 0;
	DWORD dwPixel;
	DWORD a,r,g,b;

	if( ( texture = ( Texture * )malloc( sizeof( Texture ) ) ) == NULL )
	{
		exit( TRUE );
	}

	lSize = width * height;

	texture->width = width;
	texture->height = height;
	texture->wh = lSize;
	texture->pRGBABuffer = pRGBABuffer;

	if( ( ARGBBuffer = ( BYTE * )malloc( sizeof( BYTE ) * lSize * 4 ) ) == NULL ) exit( TRUE );

	for ( ; i < lSize; i ++, j += 4 )
	{
		dwPixel = pRGBABuffer[i];

		a = ( ( dwPixel >> 24 ) & 0xff );
		r = ( ( dwPixel >> 16 ) & 0xff );
		g = ( ( dwPixel >> 8 ) & 0xff );
		b = ( ( dwPixel) & 0xff);

		ARGBBuffer[j] = a;
		ARGBBuffer[j+1] = r;
		ARGBBuffer[j+2] = g;
		ARGBBuffer[j+3] = b;
	}

	texture->ARGBBuffer = ARGBBuffer;

	return texture;
}

void texture_dispose( Texture * texture )
{
	free( texture->pRGBABuffer );
	texture->pRGBABuffer = NULL;
}

#endif
#ifndef __TEXTURE_H
#define __TEXTURE_H

#include <malloc.h>

typedef struct Texture
{
	char * name;

	int width;
	int height;
	int wh;
	
	LPBYTE pRGBABuffer;

}Texture;

typedef struct TextureList_TYP
{
	Texture * texture;

	struct TextureList_TYP * next;

}TextureList;

Texture * newTexture( char * name )
{
	Texture * texture;

	if( ( texture = ( Texture * )malloc( sizeof( Texture ) ) ) == NULL ) exit( TRUE );

	texture->name = name;
	texture->width = 0;
	texture->height = 0;
	texture->wh = 0;
	texture->pRGBABuffer = NULL;

	return texture;
}

void texture_setData( Texture * texture, int width, int height, LPBYTE data )
{
	texture->width = width;
	texture->height = height;
	texture->wh = width * height;
	texture->pRGBABuffer = data;
}

void texture_dispose( Texture * texture )
{
	free( texture->pRGBABuffer );
	texture->pRGBABuffer = NULL;
}

TextureList * newTextureList()
{
	TextureList * textureList;

	if( ( textureList = ( TextureList * )malloc( sizeof(TextureList) ) ) == NULL ) exit( TRUE );

	textureList->next = NULL;

	return textureList;
}

void textureList_addTexture( TextureList * head, Texture * t )
{
	TextureList	* tl, * ntl;

	tl = head;

	while ( tl->next )
	{
		tl = tl->next;
	}

	if( ( ntl = ( TextureList * )malloc( sizeof(TextureList) ) ) == NULL ) exit( TRUE );

	ntl->texture = t;
	ntl->next = NULL;
	tl->next = ntl;
}

#endif
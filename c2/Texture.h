#ifndef __TEXTURE_H
#define __TEXTURE_H

#include <malloc.h>
#include "Bitmap.h"

typedef struct Texture
{
	char * name;

	float perspective_dist;

	Bitmap ** mipmaps;

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

	texture->perspective_dist = 5000.0f;

	texture->mipmaps = NULL;

	return texture;
}

void texture_setMipmap( Texture * t, Bitmap * b )
{
	bitmap_generateMipmaps( b, & t->mipmaps, 1.01f );
}

int texture_dispose( Texture * texture )
{
	int num_mip_levels;

	if ( texture && texture->mipmaps )
	{
		num_mip_levels = logbase2ofx[texture->mipmaps[0]->width] + 1;

		bitmap_deleteMipmaps( texture->mipmaps , num_mip_levels );

		memset( texture, 0, sizeof( Texture ) );

		return 1;
	}
	else
	{
		return 0;
	}
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
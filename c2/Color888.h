#ifndef __COLOR888_H
#define __COLOR888_H

#include <malloc.h>

typedef struct Color888
{
	union
	{
		struct
		{
			BYTE red;
			BYTE green;
			BYTE blue;
			BYTE alpha;
		};

		BYTE color[4];
	};
}Color888;

INLINE void color888_normalize( Color888 * c )
{
	c->red = MAX( c->red, 0 );
	c->green = MAX( c->green, 0 );
	c->blue = MAX( c->blue, 0 );
	c->alpha = MAX( c->alpha, 0 );
	
	c->red = MIN( c->red, 255 );
	c->green = MIN( c->green, 255 );
	c->blue = MIN( c->blue, 255 );
	c->alpha = MIN( c->alpha, 255 );
}

INLINE void color888_zero( Color888 * c )
{
	c->red = c->green = c->blue = c->alpha = 0;
}

INLINE void color888_identity( Color888 * c )
{
	c->red = c->green = c->blue = c->alpha = 255;
}

Color888 * newColor888( BYTE r, BYTE g, BYTE b, BYTE a )
{
	Color888 * c;

	if( ( c = ( Color888 * )malloc( sizeof( Color888 ) ) ) == NULL )
	{
		exit( TRUE );
	}

	c->red = r;
	c->green = g;
	c->blue = b;
	c->alpha = a;

	color888_normalize( c );

	return c;
}

INLINE void color888_dispose( Color888 * color )
{
	memset( color, 0, sizeof( Color888 ) );
	
	free( color );
}

INLINE DWORD color888_toUint32( Color888 * c )
{
	return ( ( (int)(c->alpha) << 24 ) + ( (int)(c->red) << 16 ) + ( (int)(c->green) << 8 ) + (int)(c->blue) );
}

INLINE void color888_copy( Color888 * c, Color888 * src )
{
	c->red = src->red;
	c->green = src->green;
	c->blue = src->blue;
	c->alpha = src->alpha;
}

INLINE Color888 * color888_clone( Color888 * src )
{
	return newColor888( src->red, src->green, src->blue, src->alpha );
}

#endif
#ifndef __COLOR565_H
#define __COLOR565_H

typedef struct Color565
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

		BYTE color[3];
	};
}Color565;

INLINE void color565_normalize( Color565 * c )
{
	c->red = MAX( c->red, 0 );
	c->green = MAX( c->green, 0 );
	c->blue = MAX( c->blue, 0 );
	
	c->red = MIN( c->red, 31 );
	c->green = MIN( c->green, 63 );
	c->blue = MIN( c->blue, 31 );
}

INLINE void color565_zero( Color565 * c )
{
	c->red = c->green = c->blue = 0;
}

INLINE void color565_identity( Color565 * c )
{
	c->red = c->blue = 31;
	c->green = 63;
}

Color565 * newColor565( BYTE r, BYTE g, BYTE b )
{
	Color565 * c;

	if( ( c = ( Color565 * )malloc( sizeof( Color565 ) ) ) == NULL )
	{
		exit( TRUE );
	}

	c->red = r;
	c->green = g;
	c->blue = b;
	c->alpha = 0;

	color565_normalize( c );

	return c;
}

INLINE void color565_dispose( Color565 * color )
{
	memset( color, 0, sizeof( Color565 ) );
	
	free( color );
}

INLINE void color565_copy( Color565 * c, Color565 * src )
{
	c->red = src->red;
	c->green = src->green;
	c->blue = src->blue;
}

INLINE Color565 * color565_clone( Color565 * src )
{
	return newColor565( src->red, src->green, src->blue );
}

#endif
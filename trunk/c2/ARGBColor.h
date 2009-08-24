#ifndef __ARGBCOLOR_H
#define __ARGBCOLOR_H

#include <malloc.h>

typedef struct ARGBColor
{
	union
	{
		struct
		{
			WORD red;
			WORD green;
			WORD blue;
			WORD alpha;
		};

		WORD color[4];
	};
}ARGBColor;

INLINE void argbColor_normalize( ARGBColor * c )
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

INLINE void argbColor_zero( ARGBColor * c )
{
	c->red = c->green = c->blue = c->alpha = 0;
}

INLINE void argbColor_identity( ARGBColor * c )
{
	c->red = c->green = c->blue = c->alpha = 255;
}

ARGBColor * newARGBColor( WORD r, WORD g, WORD b, WORD a )
{
	ARGBColor * c;

	if( ( c = ( ARGBColor * )malloc( sizeof( ARGBColor ) ) ) == NULL )
	{
		exit( TRUE );
	}

	c->red = r;
	c->green = g;
	c->blue = b;
	c->alpha = a;

	argbColor_normalize( c );

	return c;
}

INLINE void argbColor_dispose( ARGBColor * color )
{
	free( color );
}

INLINE DWORD argbColor_toUint32( ARGBColor * c )
{
	return ( ( (int)(c->alpha) << 24 ) + ( (int)(c->red) << 16 ) + ( (int)(c->green) << 8 ) + (int)(c->blue) );
}

INLINE void argbColor_copy( ARGBColor * c, ARGBColor * src )
{
	c->red = src->red;
	c->green = src->green;
	c->blue = src->blue;
	c->alpha = src->alpha;
}

INLINE ARGBColor * argbColor_clone( ARGBColor * src )
{
	return newARGBColor( src->red, src->green, src->blue, src->alpha );
}

#endif
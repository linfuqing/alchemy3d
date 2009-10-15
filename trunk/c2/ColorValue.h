#ifndef __COLORVALUE_H
#define __COLORVALUE_H

#include <malloc.h>

#include "Color565.h"
#include "Color888.h"

#define DARK	0xff000000
#define WHITE	0xffffffff
#define RED		0xffff0000
#define GREEN	0xff00ff00
#define BLUE	0xff0000ff

#define RGBTo16(r, g, b) ( ( ( r >> 3 ) << 11 ) | ( ( g >> 2 ) << 5 ) | ( b >> 3 ) )

typedef struct ColorValue
{
	union
	{
		struct
		{
			float red;
			float green;
			float blue;
			float alpha;
		};

		float color[4];
	};
}ColorValue;

INLINE void colorValue_normalize( ColorValue * c )
{
	c->red = MAX( c->red, 0.0f );
	c->green = MAX( c->green, 0.0f );
	c->blue = MAX( c->blue, 0.0f );
	c->alpha = MAX( c->alpha, 0.0f );
	
	c->red = MIN( c->red, 1.0f );
	c->green = MIN( c->green, 1.0f );
	c->blue = MIN( c->blue, 1.0f );
	c->alpha = MIN( c->alpha, 1.0f );
}

INLINE void colorValue_zero( ColorValue * c )
{
	c->red = c->green = c->blue = c->alpha = 0.0f;
}

INLINE void colorValue_identity( ColorValue * c )
{
	c->red = c->green = c->blue = c->alpha = 1.0f;
}

ColorValue * newColorValue( float r, float g, float b, float a )
{
	ColorValue * c;

	if( ( c = ( ColorValue * )malloc( sizeof( ColorValue ) ) ) == NULL )
	{
		exit( TRUE );
	}

	c->red = r;
	c->green = g;
	c->blue = b;
	c->alpha = a;

	colorValue_normalize( c );

	return c;
}

INLINE void colorValue_dispose( ColorValue * color )
{
	free( color );

	memset( color, 0, sizeof( ColorValue ) );
}

INLINE Color565 * colorValueTo565( ColorValue * c )
{
	return newColor565( (int)(c->red * 31), (int)(c->green * 63), (int)(c->blue * 31 ) );
}

INLINE Color888 * colorValueTo888( ColorValue * c )
{
	return newColor888( (int)(c->alpha * 255), (int)(c->red * 255), (int)(c->green * 255), (int)(c->blue * 255 ) );
}

INLINE void colorValue_copy( ColorValue * c, ColorValue * src )
{
	c->red = src->red;
	c->green = src->green;
	c->blue = src->blue;
	c->alpha = src->alpha;
}

INLINE ColorValue * colorValue_add( ColorValue * output, ColorValue * c1, ColorValue * c2 )
{
	output->alpha = c1->alpha + c2->alpha;
	output->red = c1->red + c2->red;
	output->green = c1->green + c2->green;
	output->blue = c1->blue + c2->blue;

	return output;
}

INLINE ColorValue * colorValue_subtract( ColorValue * output, ColorValue * c1, ColorValue * c2 )
{
	output->alpha = c1->alpha - c2->alpha;
	output->red = c1->red - c2->red;
	output->green = c1->green - c2->green;
	output->blue = c1->blue - c2->blue;

	return output;
}

INLINE ColorValue * colorValue_scaleBy( ColorValue * output, ColorValue * c, float r, float g, float b, float a )
{
	output->alpha = c->alpha * a;
	output->red = c->red * r;
	output->green = c->green * g;
	output->blue = c->blue * b;

	return output;
}

INLINE ColorValue * colorValue_append( ColorValue * output, ColorValue * c1, ColorValue * c2 )
{
	output->alpha = c1->alpha * c2->alpha;
	output->red = c1->red * c2->red;
	output->green = c1->green * c2->green;
	output->blue = c1->blue * c2->blue;

	return output;
}

//================================================================================================

INLINE ColorValue * colorValue_add_self( ColorValue * c1, ColorValue * c2 )
{
	c1->alpha += c2->alpha;
	c1->red += c2->red;
	c1->green += c2->green;
	c1->blue += c2->blue;

	return c1;
}

INLINE ColorValue * colorValue_subtract_self( ColorValue * c1, ColorValue * c2 )
{
	c1->alpha -= c2->alpha;
	c1->red -= c2->red;
	c1->green -= c2->green;
	c1->blue -= c2->blue;

	return c1;
}

INLINE ColorValue * colorValue_scaleBy_self( ColorValue * c, float r, float g, float b, float a )
{
	c->alpha *= a;
	c->red *= r;
	c->green *= g;
	c->blue *= b;

	return c;
}

INLINE ColorValue * colorValue_append_self( ColorValue * c1, ColorValue * c2 )
{
	c1->alpha *= c2->alpha;
	c1->red *= c2->red;
	c1->green *= c2->green;
	c1->blue *= c2->blue;

	return c1;
}

#endif
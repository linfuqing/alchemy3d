#ifndef __FLOATCOLOR_H
#define __FLOATCOLOR_H

#include <malloc.h>

#include "ARGBColor.h"

#define DARK	0xff000000
#define WHITE	0xffffffff
#define RED		0xffff0000
#define GREEN	0xff00ff00
#define BLUE	0xff0000ff

#define RGBTo16(r, g, b) ( ( ( r >> 3 ) << 11 ) | ( ( g >> 2 ) << 5 ) | ( b >> 3 ) )

typedef struct FloatColor
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
}FloatColor;

INLINE void floatColor_normalize( FloatColor * c )
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

INLINE void floatColor_zero( FloatColor * c )
{
	c->red = c->green = c->blue = c->alpha = 0.0f;
}

INLINE void floatColor_identity( FloatColor * c )
{
	c->red = c->green = c->blue = c->alpha = 1.0f;
}

FloatColor * newFloatColor( float r, float g, float b, float a )
{
	FloatColor * c;

	if( ( c = ( FloatColor * )malloc( sizeof( FloatColor ) ) ) == NULL )
	{
		exit( TRUE );
	}

	c->red = r;
	c->green = g;
	c->blue = b;
	c->alpha = a;

	floatColor_normalize( c );

	return c;
}

INLINE void floatColor_dispose( FloatColor * color )
{
	free( color );
}

INLINE FloatColor * argbColor_toFloatColor( ARGBColor * c )
{
	float d = 1.0f / 255.0f;

	return newFloatColor( c->alpha * d, c->red * d, c->green * d, c->blue * d );
}

INLINE ARGBColor * floatColor_toARGBColor( FloatColor * c )
{
	return newARGBColor( (int)(c->alpha * 255), (int)(c->red * 255), (int)(c->green * 255), (int)(c->blue * 255 ) );
}

INLINE DWORD floatColor_toUint32( FloatColor * c )
{
	return ( (int)(c->alpha * 255) << 24 ) + ( (int)(c->red * 255) << 16 ) + ( (int)(c->green * 255) << 8 ) + (int)(c->blue * 255);
}

INLINE void floatColor_copy( FloatColor * c, FloatColor * src )
{
	c->red = src->red;
	c->green = src->green;
	c->blue = src->blue;
	c->alpha = src->alpha;
}

INLINE FloatColor * floatColor_add( FloatColor * output, FloatColor * c1, FloatColor * c2 )
{
	output->alpha = c1->alpha + c2->alpha;
	output->red = c1->red + c2->red;
	output->green = c1->green + c2->green;
	output->blue = c1->blue + c2->blue;

	return output;
}

INLINE FloatColor * floatColor_subtract( FloatColor * output, FloatColor * c1, FloatColor * c2 )
{
	output->alpha = c1->alpha - c2->alpha;
	output->red = c1->red - c2->red;
	output->green = c1->green - c2->green;
	output->blue = c1->blue - c2->blue;

	return output;
}

INLINE FloatColor * floatColor_scaleBy( FloatColor * output, FloatColor * c, float r, float g, float b, float a )
{
	output->alpha = c->alpha * a;
	output->red = c->red * r;
	output->green = c->green * g;
	output->blue = c->blue * b;

	return output;
}

INLINE FloatColor * floatColor_append( FloatColor * output, FloatColor * c1, FloatColor * c2 )
{
	output->alpha = c1->alpha * c2->alpha;
	output->red = c1->red * c2->red;
	output->green = c1->green * c2->green;
	output->blue = c1->blue * c2->blue;

	return output;
}

//================================================================================================

INLINE FloatColor * floatColor_add_self( FloatColor * c1, FloatColor * c2 )
{
	c1->alpha += c2->alpha;
	c1->red += c2->red;
	c1->green += c2->green;
	c1->blue += c2->blue;

	return c1;
}

INLINE FloatColor * floatColor_subtract_self( FloatColor * c1, FloatColor * c2 )
{
	c1->alpha -= c2->alpha;
	c1->red -= c2->red;
	c1->green -= c2->green;
	c1->blue -= c2->blue;

	return c1;
}

INLINE FloatColor * floatColor_scaleBy_self( FloatColor * c, float r, float g, float b, float a )
{
	c->alpha *= a;
	c->red *= r;
	c->green *= g;
	c->blue *= b;

	return c;
}

INLINE FloatColor * floatColor_append_self( FloatColor * c1, FloatColor * c2 )
{
	c1->alpha *= c2->alpha;
	c1->red *= c2->red;
	c1->green *= c2->green;
	c1->blue *= c2->blue;

	return c1;
}

#endif
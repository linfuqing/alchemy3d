#ifndef __COLOR_H
#define __COLOR_H

#include <malloc.h>

#define DARK	0xff000000
#define WHITE	0xffffffff
#define RED		0xffff0000
#define GREEN	0xff00ff00
#define BLUE	0xff0000ff

typedef struct Color
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
}Color;

INLINE void color_normalize( Color * c )
{
	c->red = MIN( MAX( c->red, 0 ), 1.0f );
	c->green = MIN( MAX( c->green, 0 ), 1.0f );
	c->blue = MIN( MAX( c->blue, 0 ), 1.0f );
	c->alpha = MIN( MAX( c->alpha, 0 ), 1.0f );
}

Color * newColor( float r, float g, float b, float a )
{
	Color * c;

	if( ( c = ( Color * )malloc( sizeof( Color ) ) ) == NULL )
	{
		exit( TRUE );
	}

	c->red = r;
	c->green = g;
	c->blue = b;
	c->alpha = a;

	color_normalize( c );

	return c;
}

void color_dispose( Color * color )
{
	free( color );
}

INLINE uint32 colorToUint32( Color * c )
{
	return ( (int)(c->alpha * 255) << 24 ) + ( (int)(c->red * 255) << 16 ) + ( (int)(c->green * 255) << 8 ) + (int)(c->blue * 255);
}

INLINE void color_copy( Color * c, Color * src )
{
	c->red = src->red;
	c->green = src->green;
	c->blue = src->blue;
	c->alpha = src->alpha;
}

INLINE Color * color_add( Color * output, Color * c1, Color * c2 )
{
	output->alpha = c1->alpha + c2->alpha;
	output->red = c1->red + c2->red;
	output->green = c1->green + c2->green;
	output->blue = c1->blue + c2->blue;

	return output;
}

INLINE Color * color_scaleBy( Color * output, Color * c, float r, float g, float b, float a )
{
	output->alpha = c->alpha * a;
	output->red = c->red * r;
	output->green = c->green * g;
	output->blue = c->blue * b;

	return output;
}

INLINE Color * color_append( Color * output, Color * c1, Color * c2 )
{
	output->alpha = c1->alpha * c2->alpha;
	output->red = c1->red * c2->red;
	output->green = c1->green * c2->green;
	output->blue = c1->blue * c2->blue;

	return output;
}

//================================================================================================

INLINE Color * color_add_self( Color * c1, Color * c2 )
{
	c1->alpha += c2->alpha;
	c1->red += c2->red;
	c1->green += c2->green;
	c1->blue += c2->blue;

	return c1;
}

INLINE Color * color_scaleBy_self( Color * c, float r, float g, float b, float a )
{
	c->alpha *= a;
	c->red *= r;
	c->green *= g;
	c->blue *= b;

	return c;
}

INLINE Color * color_append_self( Color * c1, Color * c2 )
{
	c1->alpha *= c2->alpha;
	c1->red *= c2->red;
	c1->green *= c2->green;
	c1->blue *= c2->blue;

	return c1;
}

INLINE void color_zero( Color * c )
{
	c->red = c->green = c->blue = c->alpha = 0.0f;
}

#endif
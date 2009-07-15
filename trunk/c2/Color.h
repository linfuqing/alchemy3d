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

Color * newColor( float r, float g, float b, float a )
{
	Color * c;

	if( ( c = ( Color * )malloc( sizeof( Color ) ) ) == NULL )
	{
		exit( TRUE );
	}
	
	r = r > 1.0f ? 1.0f : r;
	r = r < 0.0f ? 0.0f : r;

	g = g > 1.0f ? 1.0f : g;
	g = g < 0.0f ? 0.0f : g;

	b = b > 1.0f ? 1.0f : b;
	b = b < 0.0f ? 0.0f : b;

	a = a > 1.0f ? 1.0f : a;
	a = a < 0.0f ? 0.0f : a;

	c->red = r;
	c->green = r;
	c->blue = r;
	c->alpha = r;

	return c;
}

uint32 colorToUint32( Color * c )
{
	return ( (int)(c->alpha * 255) << 24 ) + ( (int)(c->red * 255) << 16 ) + ( (int)(c->green * 255) << 8 ) + (int)(c->blue * 255);
}

Color * color_add( Color * output, Color * c1, Color * c2 )
{
	output->red = c1->red + c2->red;
	output->green = c1->green + c2->green;
	output->blue = c1->blue + c2->blue;

	output->red = output->red > 1.0f ? 1.0f : output->red;
	output->green = output->green > 1.0f ? 1.0f : output->green;
	output->blue = output->blue > 1.0f ? 1.0f : output->blue;

	return output;
}

Color * color_scaleBy( Color * output, Color * c, float value )
{
	value = value > 1.0f ? 1.0f : value;
	value = value < 0.0f ? 0.0f : value;

	output->red = c->red * value;
	output->green = c->green * value;
	output->blue = c->blue * value;

	return output;
}

Color * color_append( Color * output, Color * c1, Color * c2 )
{
	output->red = c1->red * c2->red;
	output->green = c1->green * c2->green;
	output->blue = c1->blue * c2->blue;

	return output;
}

//================================================================================================

Color * color_add_self( Color * c1, Color * c2 )
{
	c1->red += c2->red;
	c1->green += c2->green;
	c1->blue += c2->blue;

	c1->red = c1->red > 1.0f ? 1.0f : c1->red;
	c1->green = c1->green > 1.0f ? 1.0f : c1->green;
	c1->blue = c1->blue > 1.0f ? 1.0f : c1->blue;

	return c1;
}

Color * color_scaleBy_self( Color * c, float value )
{
	value = value > 1.0f ? 1.0f : value;
	value = value < 0.0f ? 0.0f : value;

	c->red *= value;
	c->green *= value;
	c->blue *= value;

	return c;
}

Color * color_append_self( Color * c1, Color * c2 )
{
	c1->red *= c2->red;
	c1->green *= c2->green;
	c1->blue *= c2->blue;

	return c1;
}

#endif
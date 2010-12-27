#ifndef __FOG_H
#define __FOG_H

#include <malloc.h>

#include "Color888.h"

#define EXTEND_SCENE_LENGTH 100
#define FOG_EXP				1
#define FOG_EXP2			2
#define FOG_LINEAR			3

typedef struct Fog
{
	float density, start, end;

	int ready, mode;

	BYTE * fog_table;

#ifdef RGB565
	Color565 * global;
#else
	Color888 * global;
#endif

}Fog;

void fog_buildTable( Fog * f, float screenDepth )
{
	float ex;

	int i, j;

	int sd = (int)( screenDepth + 0.5f ) + 1;
	int _start = (int)( f->start + 0.5f );
	int _end = (int)( f->end > screenDepth ? screenDepth : f->end + 0.5f );
	int _len = _end - _start + 1;

	if ( f->fog_table ) free( memset( f->fog_table, 0, sizeof(BYTE) * sd ) );

	if( ( f->fog_table = ( BYTE * )malloc( sizeof( BYTE ) * sd ) ) == NULL ) exit( TRUE );

	i = 0, j = 0;

	for ( i = 0; i < _start; i ++ )
	{
		f->fog_table[i] = 255;
	}

	switch ( f->mode )
	{
		case FOG_EXP:

			for ( ; i <= _end; i ++, j ++ )
			{
				ex = j * f->density / _len;

				f->fog_table[i] = (BYTE)( 255.0f / powf( 2.71828, ex ) );
			}

			break;

		case FOG_EXP2:

			for ( ; i <= _end; i ++, j ++ )
			{
				ex = j * f->density / _len;

				f->fog_table[i] = (BYTE)( 255.0f / powf( 2.71828, ex * ex ) );
			}

			break;

		case FOG_LINEAR:

			for ( ; i <= _end; i ++ )
			{
				f->fog_table[i] = ( _end - i ) * 255 / _len;
			}

			break;
	}

	for ( ; i < sd; i ++ )
	{
		f->fog_table[i] = f->fog_table[i-1];
	}
}

Fog * newFog( ColorValue * color, float start, float end, float density, int mode )
{
	Fog * f;

	if( ( f = ( Fog * )malloc( sizeof( Fog ) ) ) == NULL ) exit( TRUE );

	f->start = start < 0.0f ? 0.0f : start;
	f->end = end < start ? start : end;

	f->density = density < 0.0f ? 0.0f : density;
	f->density = f->density > 1.0f ? 1.0f : f->density;

	f->mode = mode;

	f->ready = FALSE;

#ifdef RGB565
	f->global = colorValueTo565( color );
#else
	f->global = colorValueTo888( color );
#endif

	f->fog_table = NULL;

	return f;
}

void fog_render(Fog* fog, LPBYTE videoBuffer, LPBYTE zbuffer, int length, DWORD frame)
{
	DWORD* screen_ptr = (DWORD*)videoBuffer,
		 * z_ptr      = (DWORD*)zbuffer,
		 color,
		 z;

	int i;

	DWORD fogColor,
		  f,
		  r = fog->global->red,
		  g = fog->global->green,
		  b = fog->global->blue;

	BYTE f_concentration;

	fogColor = RGB32BIT(0, r, g, b);

	r <<= FIXP8_SHIFT;
	g <<= FIXP8_SHIFT;
	b <<= FIXP8_SHIFT;

	frame = (frame & 7) << FIXP28_SHIFT;

	for(i = 0; i < length; i ++)
	{
		if( (z = *z_ptr) > frame)
		{
			f_concentration = fog->fog_table[0x10000000 / (z & 0xfffffff)];

			if ( f_concentration == 0 )
				*screen_ptr = (*screen_ptr & 0xff000000) | fogColor;
			else if(f_concentration < 255)
			{
				color = *screen_ptr;

				f = f_concentration << FIXP16_SHIFT;

				*screen_ptr = RGB32BIT(*screen_ptr & 0xff000000, alphaTable[f | r | ( (color >> FIXP16_SHIFT) & 0xff )], alphaTable[f | g | ( (color >> FIXP8_SHIFT) & 0xff )], alphaTable[f | b | (color & 0xff)]);
			}
		}

		screen_ptr ++;

		z_ptr ++;
	}
}

#endif
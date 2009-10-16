#ifndef __FOG_H
#define __FOG_H

#include <malloc.h>

#include "Color888.h"

#define EXTEND_SCENE_LENGTH 10

typedef struct Fog
{
	float depth, distance;

	int ready, length;

	float * fog_table;

#ifdef RGB565
	Color565 * global;
#else
	Color888 * global;
#endif

}Fog;

Fog * newFog( ColorValue * color, float distance, float depth )
{
	Fog * f;

	if( ( f = ( Fog * )malloc( sizeof( Fog ) ) ) == NULL ) exit( TRUE );

	f->depth = depth < 0.0f ? 0.0f : depth;
	f->distance = distance < 0.0f ? 0.0f : distance;

	f->ready = FALSE;

#ifdef RGB565
	f->global = colorValueTo565( color );
#else
	f->global = colorValueTo888( color );
#endif

	f->fog_table = NULL;

	return f;
}

int buildFogTable( Fog * f, float sceneDepth )
{
	if ( f->distance > sceneDepth )
		return 0;
	else
	{
		float len;

		int i, j, end;

		int sd = (int)( sceneDepth + 0.5f ) + EXTEND_SCENE_LENGTH;
		int distance = (int)( f->distance + 0.0f );
		int depth = (int)( f->depth + 0.5f );

		if ( f->fog_table )
		{
			memset( f->fog_table, 0, sizeof(float) * f->length );
			
			free( f->fog_table );
		}

		if( ( f->fog_table = ( float * )calloc( sd, sizeof( float ) ) ) == NULL ) exit( TRUE );

		end = MIN( distance + depth, sd );

		len = 1.0f / ( end - distance + 1 );

		j = 1;

		for ( i = distance; i <= end; i ++, j ++ )
		{
			f->fog_table[i] = (float)j * len;
		}

		for ( ; i <= sd; i ++ )
		{
			f->fog_table[i] = 1.0f;
		}

		f->length = sd;
	}

	return 1;
}

#endif
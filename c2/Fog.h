#ifndef __FOG_H
#define __FOG_H

#include <malloc.h>

#include "ARGBColor.h"

#define EXTEND_SCENE_LENGTH 10

typedef struct Fog
{
	float depth, distance;

	int ready, length;

	float * fog_table;

	ARGBColor * global;

}Fog;

Fog * newFog( FloatColor * color, float distance, float depth )
{
	Fog * f;

	if( ( f = ( Fog * )malloc( sizeof( Fog ) ) ) == NULL ) exit( TRUE );

	f->depth = depth < 0.0f ? 0.0f : depth;
	f->distance = distance < 0.0f ? 0.0f : distance;

	f->ready = FALSE;

	f->global = floatColor_toARGBColor( color );

	f->fog_table = NULL;

	return f;
}

int buildFogTable( Fog * f, float sceneDepth )
{
	if ( f->distance < EPSILON_E3 )
		return 0;
	else
	{
		int i, j, end, len;

		int sd = (int)( sceneDepth + 0.5f ) + EXTEND_SCENE_LENGTH;
		int distance = (int)( f->distance + 0.0f );
		int depth = (int)( f->depth + 0.5f );

		if ( f->fog_table )
		{
			free( f->fog_table );

			memset( f->fog_table, 0, sizeof(float) * f->length );
		}

		if( ( f->fog_table = ( float * )calloc( sd, sizeof( float ) ) ) == NULL ) exit( TRUE );

		end = MIN( distance + depth, sd );

		len = end - distance;

		j = 0;

		for ( i = distance; i <= end; i ++, j ++ )
		{
			f->fog_table[i] = (float)j / len;
		}

		for ( ; i <= sd; i ++, j ++ )
		{
			f->fog_table[i] = (float)j / len;
		}

		f->length = sd;
	}

	return 1;
}

#endif
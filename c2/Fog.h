#ifndef __FOG_H
#define __FOG_H

#include <malloc.h>

typedef struct Fog_TYP
{
	float min_dist, max_dist;

	BYTE * fog_table;

}Fog;

Fog * newFog( float min_d, float max_d )
{
	Fog * f;

	if( ( f = ( Fog * )malloc( sizeof( Fog ) ) ) == NULL ) exit( TRUE );

	if ( min_d > max_d )
	{
		float tmp;

		SWAP( min_d, max_d, tmp );
	}

	f->min_dist = min_d < 0.0f ? 0.0f : min_d;
	f->max_dist = max_d < 0.0f ? 0.0f : max_d;

	f->fog_table = NULL;

	return f;
}

int buildFogTable( Fog * f, float near, float far )
{
	if ( ! FCMP( f->min_dist, f->max_dist ) )
		return 0;
	else
	{
		/*DWORD i;

		if( ( f->fog_table = ( BYTE * )calloc( far - near, sizeof( BYTE ) ) ) == NULL ) exit( TRUE );

		if ( f->min_dist > near )
		{
			for ( i = 0; i < f->min_dist; i ++ )
			{
				f->fog_table[i] = 0;
			}
		}

		if ( f->max_dist < far )
		{
			for ( i = f->max_dist; i < far; i ++ )
			{
				f->fog_table[i] = 255;
			}
		}*/
	}

	return 1;
}

#endif
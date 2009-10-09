#ifndef __FOG_H
#define __FOG_H

#include <malloc.h>

typedef struct Fog
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

int buildFogTable( Fog * f, float distance )
{
	if ( FCMP( f->min_dist, f->max_dist ) )
		return 0;
	else
	{
		int i, j, end, len;

		int d = (int)( distance + 0.5f );
		int nd = (int)( f->min_dist + 0.0f );
		int md = (int)( f->max_dist + 0.5f );

		if( ( f->fog_table = ( BYTE * )calloc( d, sizeof( BYTE ) ) ) == NULL ) exit( TRUE );

		if ( nd > 0 )
		{
			for ( i = 0; i < nd; i ++ )
			{
				f->fog_table[i] = 0;
			}
		}

		if ( md < d )
		{
			for ( i = md; i < d; i ++ )
			{
				f->fog_table[i] = 255;
			}
		}

		end = MIN( md, d );

		len = end - nd;

		j = 0;

		for ( i = nd; i < end; i ++, j ++ )
		{
			f->fog_table[i] = (int)( 255 * j / len );
		}
	}

	return 1;
}

#endif
#ifndef VECTOR_H
#define VECTOR_H

# include "Base.h"

//RW
typedef struct
{
	union
	{
		struct
		{
			float x;
			float y;
		};

		struct
		{
			float u;
			float v;
		};
	};
}Vector;

INLINE float vector_lengthSquared( Vector * v )
{
	return v->x * v->x + v->y * v->y;
}

INLINE int vector_fast_lengthSquared( int x, int y )
{
	int mn;

	x = abs(x);
	y = abs(y);

	mn = MIN(x,y);

	return(x+y-(mn>>1)-(mn>>2)+(mn>>4));
}

Vector * newVector( float x, float y)
{
	Vector * v;

	if( ( v = ( Vector * )malloc( sizeof( Vector ) ) ) == NULL )
	{
		exit( TRUE );
	}

	v->x = x;
	v->y = y;

	return v;
}

INLINE Vector * vector_clone( Vector * src )
{
	return newVector( src->x, src->y );
}

INLINE void vector_copy( Vector * dest, Vector * src )
{
	dest->x = src->x;
	dest->y = src->y;
}

INLINE void vector_dispose( Vector * v )
{
	free( v );
}

#endif
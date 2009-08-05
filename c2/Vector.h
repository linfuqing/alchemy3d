#ifndef VECTOR_H
#define VECTOR_H

# include "Base.h"

//RW
typedef struct
{
	float x;
	float y;
}Vector;

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

void vector_dispose( Vector * v )
{
	free( v );
}

#endif
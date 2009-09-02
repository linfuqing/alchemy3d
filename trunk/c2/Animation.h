#pragma once

# define FRAME_NAME_LENGTH 16

# include "entity.h"

typedef struct
{
	char       name[FRAME_NAME_LENGTH];
	Vector3D * scale;
	Vector3D * translate;
	Vector3D * vertex;
}Frame;

typedef struct
{
	Frame      * frames;
	unsigned int length;
}Animation;

typedef struct
{
	Entity    * entity;
	Animation * animation;
}Movie;

Animation   * newAnimation( Frame * frames, unsigned int length )
{
	Animation * a;

	if( ( a = ( Animation * ) malloc( sizeof( Animation ) ) ) == NULL )
	{
		exit( TRUE );
	}

	a -> frames = frames;
	a -> length = length;
}

MovieObject * newMovie( Animation * animation )
{
	Movie * m;

	if( ( m = ( Movie * )malloc( sizeof( Moive ) ) ) == NULL )
	{
		exit( TRUE );
	}

	m -> entity    = newEntity();
	m -> animation = animation;
}
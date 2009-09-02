#pragma once

# define FRAME_NAME_LENGTH 16

typedef struct
{
	char         name[FRAME_NAME_LENGTH];

	Vector3D   * vertices;

	unsigned int length;
}Frame;

typedef struct
{
	Frame        * frames;
	unsigned int   length;
}Animation;

typedef struct
{
	Animation  * animation;
	unsigned int length;
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

	return a;
}

Movie * newMovie( Animation * animation, unsigned int length )
{
	Movie * m;

	if( ( m = ( Movie * )malloc( sizeof( Movie ) ) ) == NULL )
	{
		exit( TRUE );
	}

	m -> animation = animation;
	m -> length    = length;

	return m;
}
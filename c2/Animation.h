#pragma once

# include "Mesh.h"
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
	Mesh         * parent;
	int            isPlay;
	unsigned int   currentFrameIndex;
	unsigned int   length;
}Animation;

typedef struct
{
	Animation  * animation;
	unsigned int length;
}Movie;

Animation   * newAnimation( Mesh * parent, Frame * frames, unsigned int length )
{
	Animation * a;

	if( ( a = ( Animation * ) malloc( sizeof( Animation ) ) ) == NULL )
	{
		exit( TRUE );
	}

	a -> parent            = parent;
	a -> frames            = frames;
	a -> length            = length;
	a -> isPlay            = TRUE;
	a -> currentFrameIndex = 0;

	return a;
}


void animation_updateToFrame( Animation * animation, unsigned int keyFrame )
{
	int i;

	if( animation -> length == 0 || animation -> length < keyFrame || animation -> parent == NULL )
	{
		return;
	}

	for( i = 0; i < ( animation -> parent -> nVertices ); i ++ )
	{
		animation -> parent -> vertices[i].position -> x = animation -> frames[keyFrame].vertices[i].x;
		animation -> parent -> vertices[i].position -> y = animation -> frames[keyFrame].vertices[i].y;
		animation -> parent -> vertices[i].position -> z = animation -> frames[keyFrame].vertices[i].z;
	}
}

void animation_update( Animation * animation )
{
	if( animation -> isPlay )
	{
		animation -> currentFrameIndex = ( animation -> currentFrameIndex ) > ( animation -> length ) ? 0 : ( animation -> currentFrameIndex );

		animation_updateToFrame( animation, animation -> currentFrameIndex );

		animation -> currentFrameIndex ++;
	}
}
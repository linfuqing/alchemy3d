#pragma once

# include <time.h>

# include "Mesh.h"

# define FRAME_NAME_LENGTH 16

typedef struct
{
	char         name[FRAME_NAME_LENGTH];

	clock_t      time;

	Vector3D   * vertices;

	unsigned int length;
}Frame;

typedef struct
{
	Frame        * frames;
	Mesh         * parent;
	int            isPlay;
	clock_t        durationTime;
	clock_t        startTime;
	unsigned int   length;
}Animation;

typedef struct
{
	Animation  * animation;
	unsigned int length;
}Movie;

Animation   * newAnimation( Mesh * parent, Frame * frames, unsigned int length, clock_t duration )
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
	a -> startTime         = 0;
	a -> durationTime      = duration;
	//a -> currentFrameIndex = 0;

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

void animation_updateToTime( Animation * animation, float timeAlpha )
{
	float frameAlpha;
	unsigned int   currentFrameIndex = ( unsigned int )floor( ( animation -> length - 1 ) * timeAlpha );
	int i;

	if( animation -> length == 0 || animation -> parent == NULL  )
	{
		return;
	}

	currentFrameIndex = currentFrameIndex < ( animation -> length - 1 ) ? currentFrameIndex : 0;

	frameAlpha        = ( timeAlpha *   ( animation -> durationTime )     - animation -> frames[currentFrameIndex].time ) / 
					    ( animation -> frames[currentFrameIndex + 1].time - animation -> frames[currentFrameIndex].time );

	frameAlpha        = frameAlpha > 1 ? 1 : ( frameAlpha < 0 ? 0 : frameAlpha );

	for( i = 0; i < ( animation -> parent -> nVertices ); i ++ )
	{
		animation -> parent -> vertices[i].position -> x = animation -> frames[currentFrameIndex].vertices[i].x + frameAlpha * ( animation -> frames[currentFrameIndex + 1].vertices[i].x - animation -> frames[currentFrameIndex].vertices[i].x );
		animation -> parent -> vertices[i].position -> y = animation -> frames[currentFrameIndex].vertices[i].y + frameAlpha * ( animation -> frames[currentFrameIndex + 1].vertices[i].y - animation -> frames[currentFrameIndex].vertices[i].y );
		animation -> parent -> vertices[i].position -> z = animation -> frames[currentFrameIndex].vertices[i].z + frameAlpha * ( animation -> frames[currentFrameIndex + 1].vertices[i].z - animation -> frames[currentFrameIndex].vertices[i].z );
	}
}

void animation_update( Animation * animation )
{
	if( animation -> isPlay )
	{
		//animation -> currentFrameIndex = ( animation -> currentFrameIndex ) > ( animation -> length ) ? 0 : ( animation -> currentFrameIndex );

		//animation_updateToFrame( animation, animation -> currentFrameIndex );

		//animation -> currentFrameIndex ++;

		clock_t elapsed  = clock() - animation -> startTime;
				
		if( elapsed > animation -> durationTime )
		{
			animation -> startTime = clock();

			elapsed                = 0;
		}

		AS3_Trace( AS3_Number( ( float )( elapsed * 1.0 / ( animation -> durationTime ) ) ) );
		AS3_Trace( AS3_Number( animation -> durationTime ) );
		AS3_Trace( AS3_Int( clock() ) );

		animation_updateToTime( animation, ( float )( elapsed * 1.0 / ( animation -> durationTime ) ) );
	}
}
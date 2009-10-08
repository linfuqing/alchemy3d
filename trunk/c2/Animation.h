#ifndef __ANIMATION_H
#define __ANIMATION_H

//# include <time.h>
# include <ctype.h>

#include "Mesh.h"

#define FRAME_NAME_LENGTH 16

typedef struct
{
	char         name[FRAME_NAME_LENGTH];

	int          time;

	Vector3D   * vertices;

	unsigned int length;
}Frame;

typedef struct Animation
{
	Frame        * frames;
	Mesh         * parent;
	char           currentFrameName[FRAME_NAME_LENGTH];
	int            dirty;
	int            maxTime;
	int            minTime;
	int            loop;
	int            isPlay;
	int            durationTime;
	int            startTime;
	unsigned int   length;
}Animation;

typedef struct
{
	Animation  * animation;
	unsigned int length;
}Movie;

Animation   * newAnimation( Mesh * parent, Frame * frames, unsigned int length, int duration )
{
	Animation * a;

	if( ( a = ( Animation * ) malloc( sizeof( Animation ) ) ) == NULL )
	{
		exit( TRUE );
	}

	a -> parent              = parent;
	a -> frames              = frames;
	a -> dirty               = FALSE;
	a -> maxTime             = duration;
	a -> minTime             = 0;
	a -> length              = length;
	a -> loop                = TRUE;
	a -> isPlay              = TRUE;
	a -> startTime           = 0;
	a -> durationTime        = duration;
	//a -> currentFrameIndex = 0;

	return a;
}


void animation_updateToFrame( Animation * animation, unsigned int keyFrame )
{
	DWORD i;

	if( animation -> length == 0 || animation -> length < keyFrame || animation -> parent == NULL )
	{
		return;
	}

	for( i = 0; i < ( animation -> parent -> nVertices ); i ++ )
	{
		animation -> parent -> vertices[i]->position -> x = animation -> frames[keyFrame].vertices[i].x;
		animation -> parent -> vertices[i]->position -> y = animation -> frames[keyFrame].vertices[i].y;
		animation -> parent -> vertices[i]->position -> z = animation -> frames[keyFrame].vertices[i].z;
	}
}

void animation_updateToTime( Animation * animation, float timeAlpha )
{
	float frameAlpha;
	unsigned int   currentFrameIndex = ( unsigned int )floor( ( animation -> length - 1 ) * timeAlpha );
	DWORD i;

	if( animation -> length == 0 || animation -> parent == NULL  )
	{
		return;
	}

	currentFrameIndex = currentFrameIndex < ( animation -> length - 1 ) ? currentFrameIndex : 0;

	frameAlpha        = ( timeAlpha *   ( animation -> durationTime )     - animation -> frames[currentFrameIndex].time ) / 
					    ( animation -> frames[currentFrameIndex + 1].time - animation -> frames[currentFrameIndex].time );

	frameAlpha        = frameAlpha > 1 ? 1 : ( frameAlpha < 0 ? 0 : frameAlpha );

	//AS3_Trace( AS3_Number( timeAlpha ) );
	//AS3_Trace( AS3_Number( frameAlpha ) );

	for( i = 0; i < ( animation -> parent -> nVertices ); i ++ )
	{
		animation -> parent -> vertices[i]->position -> x = animation -> frames[currentFrameIndex].vertices[i].x + frameAlpha * ( animation -> frames[currentFrameIndex + 1].vertices[i].x - animation -> frames[currentFrameIndex].vertices[i].x );
		animation -> parent -> vertices[i]->position -> y = animation -> frames[currentFrameIndex].vertices[i].y + frameAlpha * ( animation -> frames[currentFrameIndex + 1].vertices[i].y - animation -> frames[currentFrameIndex].vertices[i].y );
		animation -> parent -> vertices[i]->position -> z = animation -> frames[currentFrameIndex].vertices[i].z + frameAlpha * ( animation -> frames[currentFrameIndex + 1].vertices[i].z - animation -> frames[currentFrameIndex].vertices[i].z );
	}

	animation->parent->v_dirty = TRUE;
}

//可优化函数,将动作指令存储到内存中
int animation_updateToName( Animation * animation, char name[FRAME_NAME_LENGTH] )
{
	DWORD i;
	int min = OFF, max = OFF, match;
	char * ch = NULL;

	if( !strcmp( name, "all" ) )
	{
		min = 0;
		max = animation -> durationTime;
	}
	else
	{
		for( i = 0; i < animation -> length; i ++ )
		{
			if( ( ch = strstr( animation -> frames[i].name, name ) ) == animation -> frames[i].name )
			{
				ch += strlen( name );

				match = TRUE;
			
				while( * ch != '\0' )
				{
					if( !isdigit( ( int )( * ch ++ ) ) )
					{
						match = FALSE;
						break;
					}
				}

				if( match )
				{
					min = min == OFF ? i : min;
					max = i;
				}
			}
			else if( min != OFF )
			{
				break;
			}
		}
	}

	if( min != OFF )
	{
		animation -> minTime = animation -> frames[min].time;
		animation -> maxTime = animation -> frames[max].time;

		return TRUE;
	}

	return FALSE;
}

void animation_update( Animation * animation, int time )
{
	if( animation -> dirty )
	{
		animation -> dirty = FALSE;

		animation_updateToName( animation, animation -> currentFrameName );
	}

	if( animation -> isPlay )
	{
		//animation -> currentFrameIndex = ( animation -> currentFrameIndex ) > ( animation -> length ) ? 0 : ( animation -> currentFrameIndex );

		//animation_updateToFrame( animation, animation -> currentFrameIndex );

		//animation -> currentFrameIndex ++;

		int elapsed  = time - animation -> startTime + animation -> minTime;

		if( elapsed > animation -> maxTime )
		{
			animation -> startTime = time;

			elapsed                = animation -> minTime;

			animation -> isPlay    = animation -> loop ? TRUE : FALSE;
		}

		//AS3_Trace( AS3_Number( ( float )( elapsed * 1.0 / ( animation -> durationTime ) ) ) );
		//AS3_Trace( AS3_Number( animation -> durationTime ) );
		//AS3_Trace( AS3_Int( clock() ) );

		animation_updateToTime( animation, elapsed * 1.0f / animation -> durationTime );
	}
}

#endif

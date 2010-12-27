#ifndef __ANIMATION_H
#define __ANIMATION_H

//# include <time.h>
# include <ctype.h>

#include "Entity.h"

#define ANIMATION_TIME_MODE_GROBAL   0
#define ANIMATION_TIME_MODE_NARMAL   1
#define ANIMATION_TIME_MODE_ACCURATE 2

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
	//Vertex
	int            loop;
	int            isPlay;
	int            timeMode;

	int            time;
	int            maxTime;
	int            minTime;
	int            durationTime;
	int            startTime;
	int            elapsedTime;
}Animation;

#define SKELETAL_CHANNEL_TYPE_VISIBILITY    0

#define SKELETAL_CHANNEL_TYPE_TRANSFORM     1

#define SKELETAL_CHANNEL_TYPE_TRANSLATION   2
#define SKELETAL_CHANNEL_TYPE_ROTATION      3
#define SKELETAL_CHANNEL_TYPE_SCALE         4

#define SKELETAL_CHANNEL_TYPE_TRANSLATION_X 5
#define SKELETAL_CHANNEL_TYPE_TRANSLATION_Y 6
#define SKELETAL_CHANNEL_TYPE_TRANSLATION_Z 7

#define SKELETAL_CHANNEL_TYPE_ROTATION_X    8
#define SKELETAL_CHANNEL_TYPE_ROTATION_Y    9
#define SKELETAL_CHANNEL_TYPE_ROTATION_Z    10

#define SKELETAL_CHANNEL_TYPE_SCALE_X       11
#define SKELETAL_CHANNEL_TYPE_SCALE_Y       12
#define SKELETAL_CHANNEL_TYPE_SCALE_Z       13

typedef struct SkeletalChannel
{
	int length;
	int channelType;
	float* times;
	float** params;
	Matrix4x4* matrices;
	float* interpolations;
	Vector* inTangent;
	Vector* outTangent;

}SkeletalChannel;

typedef struct MorphChannel
{
	int length;
	Frame* frames;
	char currentFrameName[FRAME_NAME_LENGTH];
	int  nameLength;
	int  dirty;

}MorphChannel;

Animation *newAnimation(int isPlay, int loop, int maxTime, int minTime)
{
	Animation * a;

	if( ( a = ( Animation * ) malloc( sizeof( Animation ) ) ) == NULL )
	{
		exit( TRUE );
	}

	a->isPlay       = isPlay;
	a->loop         = loop;
	a->timeMode     = ANIMATION_TIME_MODE_GROBAL;
	a->time         = 0;
	a->maxTime      = maxTime;
	a->minTime      = minTime;
	a->durationTime = maxTime - minTime;
	a->startTime    = 0;
	a->elapsedTime  = 0;

	return a;
}

void animation_destroy(Animation **animation)
{
	free(*animation);

	*animation = NULL;
}

void animation_update( Animation * animation, int time )
{
	if(animation->timeMode != ANIMATION_TIME_MODE_GROBAL)
		time = animation->time;

	if( animation -> isPlay )
	{
		if(animation->timeMode == ANIMATION_TIME_MODE_ACCURATE)
		{
			animation->elapsedTime = MAX( MIN(time, animation->maxTime), animation->minTime );

			animation->startTime   = animation->elapsedTime;
		}
		else
		{
			animation->elapsedTime  = time - animation -> startTime;

			if(animation->elapsedTime > 0)
			{
				animation->elapsedTime +=  animation->minTime;

				if( animation->elapsedTime > animation -> maxTime )
				{
					animation -> startTime = time;

					animation->elapsedTime = animation -> minTime;

					animation -> isPlay    = animation -> loop ? TRUE : FALSE;
				}
			}
			else
			{
				animation->elapsedTime +=  animation->maxTime;

				if( animation->elapsedTime < animation -> minTime )
				{
					animation -> startTime = time;

					animation->elapsedTime = animation -> maxTime;

					animation -> isPlay    = animation -> loop ? TRUE : FALSE;
				}
			}
		}
	}
}

SkeletalChannel* newSkeletalChannel(
								   Animation* animation, 
								   int length, 
								   int channelType,
								   float* times,
								   float** params,
								   Matrix4x4* matrices,
								   float* interpolations,
								   Vector* inTangent,
								   Vector* outTangent)
{
	SkeletalChannel * channel;
	int maxTime = times[length - 1] * 1000;
	int minTime = times[0         ] * 1000;

	if( ( channel = (SkeletalChannel *) malloc( sizeof(SkeletalChannel) ) ) == NULL )
	{
		exit( TRUE );
	}

	if(animation)
	{
		animation->maxTime      = MAX(animation->maxTime, maxTime);
		animation->minTime      = MIN(animation->minTime, minTime);
		animation->durationTime = animation->maxTime - animation->minTime;
	}

	channel->length         = length;
	channel->channelType    = channelType;
	channel->times          = times;
	channel->params         = params;
	channel->matrices       = matrices;
	channel->interpolations = interpolations;
	channel->inTangent      = inTangent;
	channel->outTangent     = outTangent;

	return channel;
}

void skeletalChannel_destroy(SkeletalChannel** channel)
{
	free(*channel);

	*channel = NULL;
}

void skeletalChannel_update(SkeletalChannel * channel, Entity* parent, Animation* animation)
{
	if(!animation->isPlay)
		return;

	unsigned int currentFrameIndex = channel->length - 1, length, oldLength;
	float time = animation->elapsedTime * 0.001f;

	if(time < channel->times[0])
		currentFrameIndex = 0;
	else if(time < channel->times[currentFrameIndex])
	{
		length = oldLength = channel->length - 1;
		
		while (length > 1)
		{
			oldLength = length;
			length  >>= 1;
						
			if (channel->times[currentFrameIndex - length] > time)
			{
				currentFrameIndex -= length;
				length             = oldLength - length;
			}
		}
	}

	switch(channel->channelType)
	{
	case SKELETAL_CHANNEL_TYPE_VISIBILITY:
		parent->visible = channel->params[currentFrameIndex][0] > 0;
		break;
	case SKELETAL_CHANNEL_TYPE_TRANSFORM:
		matrix4x4_copy(parent->transform, &channel->matrices[currentFrameIndex]);
		parent->transformDirty = TRUE;
		parent->matrixDirty    = FALSE;
		break;
	case SKELETAL_CHANNEL_TYPE_TRANSLATION:
		if(!parent->matrixDirty)
		{
			matrix4x4_decompose(parent->transform, parent->position, parent->scale, parent->direction, NULL);
			parent->matrixDirty = TRUE;
		}

		parent->position->x = channel->params[currentFrameIndex][0];
		parent->position->y = channel->params[currentFrameIndex][1];
		parent->position->z = channel->params[currentFrameIndex][2];

		parent->transformDirty = TRUE;
		break;
	case SKELETAL_CHANNEL_TYPE_ROTATION:
		if(!parent->matrixDirty)
		{
			matrix4x4_decompose(parent->transform, parent->position, parent->scale, parent->direction, NULL);
			parent->matrixDirty = TRUE;
		}

		parent->direction->x = channel->params[currentFrameIndex][0];
		parent->direction->y = channel->params[currentFrameIndex][1];
		parent->direction->z = channel->params[currentFrameIndex][2];

		parent->transformDirty = TRUE;
		break;
	case SKELETAL_CHANNEL_TYPE_SCALE:
		if(!parent->matrixDirty)
		{
			matrix4x4_decompose(parent->transform, parent->position, parent->scale, parent->direction, NULL);
			parent->matrixDirty = TRUE;
		}

		parent->scale->x = channel->params[currentFrameIndex][0];
		parent->scale->y = channel->params[currentFrameIndex][1];
		parent->scale->z = channel->params[currentFrameIndex][2];

		parent->transformDirty = TRUE;
		break;
	case SKELETAL_CHANNEL_TYPE_TRANSLATION_X:
		if(!parent->matrixDirty)
		{
			matrix4x4_decompose(parent->transform, parent->position, parent->scale, parent->direction, NULL);
			parent->matrixDirty = TRUE;
		}

		parent->position->x = channel->params[currentFrameIndex][0];

		parent->transformDirty = TRUE;
		break;
	case SKELETAL_CHANNEL_TYPE_TRANSLATION_Y:
		if(!parent->matrixDirty)
		{
			matrix4x4_decompose(parent->transform, parent->position, parent->scale, parent->direction, NULL);
			parent->matrixDirty = TRUE;
		}

		parent->position->y = channel->params[currentFrameIndex][0];

		parent->transformDirty = TRUE;
		break;
	case SKELETAL_CHANNEL_TYPE_TRANSLATION_Z:
		if(!parent->matrixDirty)
		{
			matrix4x4_decompose(parent->transform, parent->position, parent->scale, parent->direction, NULL);
			parent->matrixDirty = TRUE;
		}

		parent->position->z = channel->params[currentFrameIndex][0];

		parent->transformDirty = TRUE;
		break;
	case SKELETAL_CHANNEL_TYPE_ROTATION_X:
		if(!parent->matrixDirty)
		{
			matrix4x4_decompose(parent->transform, parent->position, parent->scale, parent->direction, NULL);
			parent->matrixDirty = TRUE;
		}

		parent->direction->x = channel->params[currentFrameIndex][0];

		parent->transformDirty = TRUE;
		break;
	case SKELETAL_CHANNEL_TYPE_ROTATION_Y:
		if(!parent->matrixDirty)
		{
			matrix4x4_decompose(parent->transform, parent->position, parent->scale, parent->direction, NULL);
			parent->matrixDirty = TRUE;
		}

		parent->direction->y = channel->params[currentFrameIndex][0];

		parent->transformDirty = TRUE;
		break;
	case SKELETAL_CHANNEL_TYPE_ROTATION_Z:
		if(!parent->matrixDirty)
		{
			matrix4x4_decompose(parent->transform, parent->position, parent->scale, parent->direction, NULL);
			parent->matrixDirty = TRUE;
		}

		parent->direction->z = channel->params[currentFrameIndex][0];

		parent->transformDirty = TRUE;
		break;
	case SKELETAL_CHANNEL_TYPE_SCALE_X:
		if(!parent->matrixDirty)
		{
			matrix4x4_decompose(parent->transform, parent->position, parent->scale, parent->direction, NULL);
			parent->matrixDirty = TRUE;
		}

		parent->scale->x = channel->params[currentFrameIndex][0];

		parent->transformDirty = TRUE;
		break;
	case SKELETAL_CHANNEL_TYPE_SCALE_Y:
		if(!parent->matrixDirty)
		{
			matrix4x4_decompose(parent->transform, parent->position, parent->scale, parent->direction, NULL);
			parent->matrixDirty = TRUE;
		}

		parent->scale->y = channel->params[currentFrameIndex][0];

		parent->transformDirty = TRUE;
		break;
	case SKELETAL_CHANNEL_TYPE_SCALE_Z:
		if(!parent->matrixDirty)
		{
			matrix4x4_decompose(parent->transform, parent->position, parent->scale, parent->direction, NULL);
			parent->matrixDirty = TRUE;
		}

		parent->scale->z = channel->params[currentFrameIndex][0];

		parent->transformDirty = TRUE;
		break;
	}
}

void morphChannel_set(MorphChannel* channel, Animation *animation, Frame * frames, unsigned int length, int duration)
{
	if(animation)
	{
		animation->maxTime      = MAX(duration, animation->maxTime);
		animation->minTime      = 0;
		animation->durationTime = animation->maxTime;
	}

	channel->frames     = frames;
	channel->dirty      = FALSE;
	channel->length     = length;
	channel->nameLength = 0;
}

MorphChannel* newMorphChannel(Animation *animation, Frame * frames, unsigned int length, int duration)
{
	MorphChannel * channel;

	if( ( channel = (MorphChannel*) malloc( sizeof(MorphChannel) ) ) == NULL )
		exit( TRUE );

	morphChannel_set(channel, animation, frames, length, duration);

	return channel;
}

void morphChannel_destroy(MorphChannel **channel)
{
	free(*channel);

	*channel = NULL;
}

void morphChannel_clear(MorphChannel* channel)
{
	int i;
	for(i = 0; i < channel->length; i ++)
	{
		free(channel->frames[i].vertices);

		memset( &channel->frames[i], 0, sizeof( Frame ) );
	}

	free(channel->frames);

	memset( channel, 0, sizeof( MorphChannel ) );
}

void morphChannel_reBuild(MorphChannel* channel, Animation *animation, Frame * frames, unsigned int length, int duration)
{
	if(!channel)
		channel = newMorphChannel(animation, frames, length, duration);
	else
	{
		morphChannel_clear(channel);

		morphChannel_set(channel, animation, frames, length, duration);
	}
}

void morphChannel_updateToFrame(MorphChannel * channel, unsigned int keyFrame, Mesh* parent)
{
	DWORD i;

	float x, y, z;

	if(channel -> length == 0 || channel -> length < keyFrame || !parent)
		return;

	for(i = 0; i < parent->nVertices; i ++)
	{
		x = channel->frames[keyFrame].vertices[i].x;
		y = channel->frames[keyFrame].vertices[i].y;
		z = channel->frames[keyFrame].vertices[i].z;

		parent->vertices[i]->normal->x *= x / parent->vertices[i]->position->x;
		parent->vertices[i]->normal->y *= y / parent->vertices[i]->position->y;
		parent->vertices[i]->normal->z *= z / parent->vertices[i]->position->z;

		parent->vertices[i]->position->x = x;
		parent->vertices[i]->position->y = y;
		parent->vertices[i]->position->z = z;
	}

	mesh_updateFaces(parent);
}

void morphChannel_updateToTime(MorphChannel *channel, Animation* animation, Mesh* parent)
{
	float frameAlpha;
	unsigned int currentFrameIndex, nextFrameIndex;
	int currentTime, nextTime;
	DWORD i;
	float x, y, z;

	if(channel->length == 0)
		return;

	currentFrameIndex = (unsigned int)floor( (channel->length - 1) * animation->elapsedTime * 1.0f / animation->durationTime );

	currentFrameIndex = currentFrameIndex < (channel->length - 1) ? currentFrameIndex : 0;

	nextFrameIndex    = currentFrameIndex + 1;

	nextFrameIndex    = nextFrameIndex < (channel->length - 1) ? nextFrameIndex : 0;

	currentTime       = channel->frames[currentFrameIndex].time;

	nextTime          = nextFrameIndex ? channel->frames[nextFrameIndex].time : (animation->durationTime - currentTime);

	frameAlpha        = (animation->elapsedTime - currentTime) * 1.0f / (nextTime - currentTime);

	frameAlpha        = frameAlpha > 1 ? 1 : (frameAlpha < 0 ? 0 : frameAlpha);

	for(i = 0; i < parent -> nVertices; i ++)
	{
		x = channel->frames[currentFrameIndex].vertices[i].x + frameAlpha * (channel -> frames[nextFrameIndex].vertices[i].x - channel -> frames[currentFrameIndex].vertices[i].x);
		y = channel->frames[currentFrameIndex].vertices[i].y + frameAlpha * (channel -> frames[nextFrameIndex].vertices[i].y - channel -> frames[currentFrameIndex].vertices[i].y);
		z = channel->frames[currentFrameIndex].vertices[i].z + frameAlpha * (channel -> frames[nextFrameIndex].vertices[i].z - channel -> frames[currentFrameIndex].vertices[i].z);

		parent->vertices[i]->normal->x *= x / parent->vertices[i]->position->x;
		parent->vertices[i]->normal->y *= y / parent->vertices[i]->position->y;
		parent->vertices[i]->normal->z *= z / parent->vertices[i]->position->z;

		parent->vertices[i]->position->x = x;
		parent->vertices[i]->position->y = y;
		parent->vertices[i]->position->z = z;
	}

	mesh_updateFaces(parent);
}

//可优化函数,将动作指令存储到内存中
void morphChannel_updateToName(MorphChannel *channel, char name[FRAME_NAME_LENGTH], Animation* animation, Mesh* parent)
{
	DWORD i;
	int min = OFF, max = OFF, match;
	char * ch = NULL;

	if( !strcmp( name, "all" ) )
	{
		min = 0;
		max = channel->length;
	}
	else
	{
		for(i = 0; i < channel->length; i ++)
		{
			if( ( ch = strstr(channel->frames[i].name, name) ) == channel -> frames[i].name )
			{
				ch += strlen(name);

				match = TRUE;
			
				while(*ch != '\0')
				{
					if( !isdigit( (int)(*ch ++) ) )
					{
						match = FALSE;
						break;
					}
				}

				if(match)
				{
					min = min == OFF ? i : min;
					max = i;
				}
			}
			else if(min != OFF)
				break;
		}
	}

	if( min != OFF )
	{
		animation->startTime += animation->elapsedTime - animation->minTime;
		animation->minTime    = channel->frames[min].time;
		animation->maxTime    = channel->frames[max].time;

		morphChannel_updateToFrame(channel, min, parent);
	}
}

void morphChannel_update(MorphChannel *channel,  Mesh* parent, Animation* animation)
{
	if(channel->dirty)
	{
		channel->dirty = FALSE;

		channel->currentFrameName[channel->nameLength] = '\0';

		morphChannel_updateToName(channel, channel->currentFrameName, animation, parent);

		return;
	}

	if(!animation->isPlay)
		return;

	morphChannel_updateToTime(channel, animation, parent);
}

#endif

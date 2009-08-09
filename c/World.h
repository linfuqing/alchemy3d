# ifndef WORLD_H
# define WORLD_H

# include "Viewport.h"

typedef struct Screen
{
	Viewport * viewport;

	struct Screen   * next;
}Screen;

typedef struct World
{
	Scene        * scene;
	Screen       * screen;

	struct World * next;
}World;

World * newWorld( Scene * s )
{
	World * w;

	if( ( w = ( World * )malloc( sizeof( World ) ) ) == NULL )
	{
		exit( TRUE );
	}

	w -> scene  = s;
	w -> screen = NULL;
	w -> next   = NULL;

	return w;
}

Screen * newScreen( Viewport * v )
{
	Screen * s;

	if( ( s = ( Screen * )malloc( sizeof( Screen ) ) ) == NULL )
	{
		exit( TRUE );
	}

	s -> viewport = v;

	s -> next     = NULL;

	return s;
}

//void world_addViewport( World * w, Viewport * v )
void world_addViewport( World * w, Screen * s, int at )
{
	Screen * sp = w -> screen;

	if( sp == NULL || at == 0 )
	{
		w -> screen = s;
	}
	else
	{
		if( at == OFF )
		{
			while( sp -> next != NULL )
			{
				sp = sp -> next;
			}
		}
		else
		{
			int i = 1;

			while( sp -> next != NULL && i < at )
			{
				sp = sp -> next;

				i ++;
			}
		}

		sp -> next = s;
	}

	viewport_setScene( s -> viewport, w -> scene );
}

int world_removeViewport( World * w, Screen * s )
{
	Screen * sp = w -> screen;

	if( sp == NULL )
	{
		return FALSE;
	}
	else if( s == sp )
	{
		w -> screen = sp -> next;

		return TRUE;
	}
	else
	{
		while( sp -> next != NULL && sp -> next != s )
		{
			sp = sp -> next;
		}

		if( sp -> next == s )
		{
			sp -> next = s -> next;

			return TRUE;
		}
	}

	return FALSE;
}

# endif
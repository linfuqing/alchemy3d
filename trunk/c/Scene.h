#ifndef SCENE_H
#define SCENE_H

//verson 1.5

# include "Mesh.h"
# include "Object.h"
//# include "Viewport.h"

//typedef struct Viewport Viewport;

/*typedef struct Screen
{
	struct Viewport * viewport;

	struct Screen   * next;
}Screen;*/

typedef struct Scene
{
	//Screen       * screen;

	//R
	Object       * object;

	//RW
	Mesh         * mesh;

	//RW
	int            visible;

	//N
	int            move;

	//N
	struct Scene * next;

	//N
	struct Scene * children;
}Scene;

Scene * newScene( Mesh * m, Object * o )
{
	Scene * scene;

	if( ( scene = ( Scene * )malloc( sizeof( Scene ) ) ) == NULL )
	{
		exit( TRUE );
	}

	scene -> object         = o == NULL ? newObject( NULL ) : o;
	scene -> object -> move = MOVE_TYPE_ADDED_SCENE;
	scene -> mesh           = m;
	scene -> visible        = TRUE;
	//scene -> screen         = NULL;
	scene -> children       = NULL;
	scene -> next           = NULL;
	scene -> move           = FALSE;

	return scene;
}

//void viewport_setScene( struct Viewport * v, Scene * s );

/*void scene_addViewport( Scene * s, struct Viewport * v )
{
	Screen * screen;

	if( ( screen = ( Screen * )malloc( sizeof( Screen ) ) ) == NULL )
	{
		exit( TRUE );
	}

	screen -> viewport = v;

	screen -> next     = s -> screen;

	s      -> screen   = screen;

	viewport_setScene( v, s );
}*/

int scene_numChildren( Scene * s )
{
	Scene * p = s -> children;

	int size = 0;

	while( p -> next != NULL )
	{
		p = p -> next;

		size ++;
	}

	return size;
}

/**
����ǰ������.
**/
void scene_previousRootOrder( Scene * s,void visit( Scene * child ) )
{
	Scene * p = s -> children;

	while( p != NULL )
	{
		visit( p );

		scene_previousRootOrder( p, visit );

		p = p -> next;
	}
}

/**
����ǰ������.
**/
void scene_previousOrder( Scene * s, void visit( Scene * child ) )
{
	visit( s );

	scene_previousRootOrder( s, visit );
}

int scene_find( Scene * s, Scene * child )
{   
	Scene * p = s -> children;

	while( p != NULL )
	{
		if( p == child )
		{
			return TRUE;
		}

		if( scene_find( p, child ) )
		{
			return TRUE;
		}

		p = p -> next;
	}

	return FALSE;
}

int scene_contains( Scene * s, Scene * child )
{
	return s == child || scene_find( s, child );
}

/**
�������л�.
**/
/*
void scene_orderTree( Scene * s )
{
	Scene * p = s -> children;
	int    ID = scene_numChildren( s );

	while( p != NULL )
	{
		p -> ID = ID;

		scene_orderTree( p );

		ID --;

		p = p -> next;
	}
}*/

int scene_removeChild( Scene * s, Scene * child, int all )
{   
	Scene * p = s -> children;

	if( p == child )
	{
		s -> children = p -> next;

		free( p );

		return TRUE;
	}

	if( p != NULL )
	{
		while( p -> next != NULL )
		{
			if( p -> next == child )
			{
				p -> next = child -> next;

				free( child );

				return TRUE;
			}

			if( all )
			{
				if( scene_removeChild( p, child, TRUE ) )
				{
					return TRUE;
				}
			}

			p = p -> next;
		}

		if( all )
		{
			scene_removeChild( p, child, TRUE );
		}
	}

	return FALSE;
}

void scene_addChild( Scene * s, Scene * child )
{
	//child -> ID             = s -> children == NULL ? s -> ID + 1 : s -> ID + s -> children -> ID + 1;

	child -> object -> move = child -> object -> move ? child -> object -> move : MOVE_TYPE_ADDED_SCENE;

	child -> next           = s -> children;

	s -> children           = child;

	//scene_orderTree( child );
}

/**
�任��������.
�������жϵı任,Ҳ���Ǿ�̬��Ⱦ,���ܳ����Ƿ�䶯�����任.
**/
void transformSceneMesh( Scene * s )
{
	Scene * p = s -> children, * head = p;

	if( head == NULL )
	{
		object_getTransform( s -> object );

		return;
	}
	
	//��ʱS��P�ĸ���,�������������ǲ��������ر任��.
	* ( head -> object -> world ) = * object_getTransform( s -> object );
	matrix3D_apprend( head -> object -> world, s -> object -> world );

	while( p != NULL && p -> visible )
	{
		//����ʽ��ֵ.
		* ( p -> object -> world ) = * ( head -> object -> world );

		transformSceneMesh( p );

		if( mesh_check( p -> mesh ) )
		{
			//����任,���Ӽ�����ʱʹ���Ӽ��������, ������ʱ���㱾���������.
			transformVertices( p -> children ? p -> children -> object -> world : ( temp = * object_getTransform( p -> object ),matrix3D_apprend( & temp, p -> object -> world ), & temp ), p -> mesh -> vertices );
		}

		p = p -> next;
	}
}

/**
�����Ӽ��任,�����������ĺ��Ӽ���.
**/
void transformSceneChildren( Scene * s )
{
	/*Scene * previous = s -> children, * p;

	if( previous == NULL )
	{
		return;
	}
	else if( previous -> visible )
	{
		if( previous -> camera -> move )
		{
			//�������ƶ�,��ȫ�任�Ӽ�.
			previous -> move = TRUE;
			transformSceneMesh( previous );
		}
		else
		{
			//��������Ӽ��Ƿ�䶯.
			previous -> move = FALSE;
			transformSceneChildren( previous );
		}
	}

	p = previous -> next;

	while( p != NULL )
	{
		if( !( p -> visible ) )
		{
			previous = p;

			p = p -> next;

			continue;
		}
		else if( p -> camera -> move )
		{
			//�������ƶ�,��ȫ�任�Ӽ�.
			p -> move = TRUE;
			transformSceneMesh( p );

			//�ѱ䶯�ĺ����ƶ�����ǰ��
			//[
			previous -> next = p -> next;

			p -> next        = s -> children;

			s -> children    = p;

			p                = previous -> next;
			//]
		}
		else
		{
			//��������Ӽ��Ƿ�䶯.
			p -> move = FALSE;
			transformSceneChildren( p );

			previous = p;

			p = p -> next;

		}
	}*/

	Scene * p = s -> children;

	while( p != NULL )
	{
		if( !( p -> visible ) )
		{
			p = p -> next;
			continue;
		}
		else if( p -> object -> move )
		{
			//�������ƶ�,��ȫ�任�Ӽ�.
			p -> move = TRUE;
			transformSceneMesh( p );
		}
		else
		{
			//��������Ӽ��Ƿ�䶯.
			p -> move = FALSE;
			transformSceneChildren( p );
		}

		p = p -> next;
	}
}

/**
�������ı任.
**/
void transformScene( Scene * s, int mode )
{
	if( mode || ( s -> visible && s -> object -> move ) )
	{
		s -> move = TRUE;
		transformSceneMesh( s );
	}
	else if( s -> visible )
	{
		s -> move = FALSE;
		transformSceneChildren( s );
	}
}

void scene_free( Scene * s )
{
	object_destroy( & ( s -> object ) );

	mesh_destroy( & ( s -> mesh ) );

	free( s );
}

void scene_destroy( Scene * * s )
{
	scene_previousOrder( * s, scene_free );

	* s = NULL;
}

#endif
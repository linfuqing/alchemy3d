#ifndef SCENE_H
#define SCENE_H

# include "Mesh.h"
# include "Camera.h"

typedef struct Scene
{
	
	//R
	Camera       * camera;

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

Scene * newScene( Mesh * mesh )
{
	Scene * scene;

	if( ( scene = ( Scene * )malloc( sizeof( Scene ) ) ) == NULL )
	{
		exit( TRUE );
	}

	scene -> camera   = newCamera( NULL, NULL, NULL );
	scene -> mesh     = mesh;
	scene -> visible  = TRUE;
	scene -> children = NULL;
	scene -> next     = NULL;
	scene -> move     = FALSE;

	return scene;
}

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
void scene_preRootOrder( Scene * s,void visit( Scene * child ) )
{
	Scene * p = s -> children;

	while( p != NULL )
	{
		visit( p );

		scene_preRootOrder( p, visit );

		p = p -> next;
	}
}

/**
����ǰ������.
**/
void scene_preOrder( Scene * s, void visit( Scene * child ) )
{
	visit( s );

	scene_preRootOrder( s, visit );
}

int scene_removeChild( Scene * s, Scene * child )
{   
	Scene * p = s -> children;

	if( p == child )
	{
		s -> children = p -> next;

		free( p );
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

			p = p -> next;
		}

	}

	return FALSE;
}

void scene_addChild( Scene * s, Scene * child )
{
	Scene * p = s -> children;

	if( p == NULL )
	{
		s -> children = child;

		return;
	}

	while( p -> next != NULL )
	{
		p = p -> next;
	}

	p -> children = child;
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
�任��������.
�������жϵı任,Ҳ���Ǿ�̬��Ⱦ,���ܳ����Ƿ�䶯�����任.
**/
void transformSceneMesh( Scene * s )
{
	Scene * p = s -> children, * head = p;
	
	//��ʱS��P�ĸ���,�������������ǲ��������ر任��.
	* ( head -> camera -> world ) = matrix3D_multiply( camera_getPosition( s -> camera ), * ( s -> camera -> world ) );

	while( p != NULL && p -> visible )
	{
		//����ʽ��ֵ.
		* ( p -> camera -> world ) = * ( head -> camera -> world );

		scene_transformSceneMesh( p );

		if( mesh_check( p -> mesh ) )
		{
			//����任,���Ӽ�����ʱʹ���Ӽ��������, ������ʱ���㱾���������.
			transformVertices( p -> children ? p -> children -> camera -> world : matrix3D_multiply( camera_getPosition( p ->camera ), * ( p -> camera -> world ) ), p -> mesh -> vertices );
		}

		p = p -> next;
	}
}

/**
�����Ӽ��任,�����������ĺ��Ӽ���.
**/
void transformSceneChildren( Scene * s )
{
	Scene * p = s -> children;

	while( p != NULL )
	{
		if( !( p -> visible ) )
		{
			continue;
		}
		else if( p -> camera -> move )
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
void transformScene( Scene * s )
{
	if( s -> visible && s -> camera -> move )
	{
		p -> move = TRUE;
		transformSceneMesh( p );
	}
	else
	{
		p -> move = FALSE;
		transformSceneChildren( p );
	}
}

void scene_destroy( Scene * * s )
{
	scene_preOrder( * s, free );

	* s = NULL;
}

#endif
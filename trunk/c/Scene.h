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
场景前根遍历.
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
场景前续遍历.
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
变换场景网格.
这是无判断的变换,也就是静态渲染,不管场景是否变动都做变换.
**/
void transformSceneMesh( Scene * s )
{
	Scene * p = s -> children, * head = p;
	
	//此时S是P的父级,这里的世界矩阵是不包含本地变换的.
	* ( head -> camera -> world ) = matrix3D_multiply( camera_getPosition( s -> camera ), * ( s -> camera -> world ) );

	while( p != NULL && p -> visible )
	{
		//复制式付值.
		* ( p -> camera -> world ) = * ( head -> camera -> world );

		scene_transformSceneMesh( p );

		if( mesh_check( p -> mesh ) )
		{
			//网格变换,当子集存在时使用子集世界矩阵, 不存在时计算本地世界矩阵.
			transformVertices( p -> children ? p -> children -> camera -> world : matrix3D_multiply( camera_getPosition( p ->camera ), * ( p -> camera -> world ) ), p -> mesh -> vertices );
		}

		p = p -> next;
	}
}

/**
场景子集变换,即不包含根的孩子计算.
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
			//如果相机移动,完全变换子集.
			p -> move = TRUE;
			transformSceneMesh( p );
		}
		else
		{
			//继续监测子集是否变动.
			p -> move = FALSE;
			transformSceneChildren( p );
		}

		p = p -> next;
	}
}

/**
包含根的变换.
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
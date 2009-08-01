
#include "Mesh.h"

typedef struct ScreenVertex
{
	Vertex   * parent;

	Vector3D * screen;
}ScreenVertex;

//N
typedef struct ScreenVertices
{
	/*Vertex                * parent;

	Vector3D              * screen;*/

	ScreenVertex          * vertex;

	struct ScreenVertices * next;
}ScreenVertices;

typedef struct ScreenPolygon
{
	ScreenVertex         * vertex;

	Polygon              * parent;

	struct ScreenPolygon * next;
}ScreenPolygon;

typedef struct ScreenFaces
{
	ScreenPolygon      * face;

	Faces              * parent;

	struct ScreenFaces * rear;

	struct ScreenFaces * next;
}ScreenFaces;

typedef struct Projection
{
	ScreenVertices    * vertices;

	ScreenFaces       * faces;

	struct Projection * next;

	struct Projection * children;
}Projection;

void screenFaces_initiate( ScreenFaces * * head )
{
	if( ( * head = ( ScreenFaces * )malloc( sizeof( ScreenFaces ) ) ) == NULL )
	{
		exit( TRUE );
	}

	( * head ) -> next = NULL;
	( * head ) -> rear = NULL;
}

void projection_destroy( Projection * * p )
{
}

/**
产生一个结构跟网格相同的投影.
**/
Projection * newProjection( Mesh * m, Projection * * from )
{
	//投影
	Projection     * projection;

	ScreenVertex   * sv;

	//屏幕顶点集,作为数组指针用
	ScreenVertices * svs;

	//屏幕多边形,作为数组指针用
	ScreenPolygon  * sp;

	//屏幕面集,作为数组指针用
	ScreenFaces    * sfs;

	//网格面指针
	Faces          * fp;

	//网格顶点指针
	Vertices       * vp;

	//网格多边形指针
	Polygon        * pp;

	//向量集,关联到的所有向量,作为数组指针用
	Vector3D       * vs;

	//vi作为i索引
	int              numVertices = vertices_length( m -> vertices ), numFaces = faces_length( m -> faces ), i, vi;

	//一次性申请所有空间
	if( ( projection = ( Projection     * )malloc( sizeof( Projection     )                                                     ) ) == NULL
	||  ( svs        = ( ScreenVertices * )malloc( sizeof( ScreenVertices ) * ( numVertices                        + 1        ) ) ) == NULL      //+1为头指针结点
	||  ( sfs        = ( ScreenFaces    * )malloc( sizeof( ScreenFaces    ) * ( numFaces                           + 1        ) ) ) == NULL
	||  ( vs         = ( Vector3D       * )malloc( sizeof( Vector3D       ) * ( numVertices                                   ) ) ) == NULL      //包括screen和view
	||  ( sv         = ( ScreenVertex   * )malloc( sizeof( ScreenVertex   ) * ( numVertices                                   ) ) ) == NULL
	||  ( sp         = ( ScreenPolygon  * )malloc( sizeof( ScreenPolygon  ) * ( faces_verticesLength( m -> faces ) + numFaces ) ) ) == NULL )    //面所包含的所有顶点加上所有的头结点
	{
		exit( TRUE );
	}

	if( from != NULL )
	{
		projection -> children = ( * from ) -> children;
		projection -> next     = ( * from ) -> next;

		projection_destroy( from );
	}
	else
	{
		projection -> children = NULL;
		projection -> next     = NULL;
	}

	vp = m -> vertices;

	//接入第一个父级
	sv -> parent = vp -> vertex;

	//2D顶点装载向量,从第一个结点开始
	for( i = 1; i <= numVertices; i ++ )
	{
		vi = i - 1;

		( sv + i   ) -> parent = ( vp = vp -> next ) -> vertex;
		( sv + i   ) -> screen = vs  + vi;
		( svs + i  ) -> vertex = sv  + i;
		( svs + vi ) -> next   = svs + i;  
	}

	//置空尾指针
	( svs + i - 1 ) -> next    = NULL;

	//接入头指针
	projection -> vertices = svs;

	//如果网格是粒子集,则返回无面投影
	if( m -> faces == NULL )
	{
		projection -> faces = NULL;

		return projection;
	}

	//面指针指向第一个网格面结点
	fp = m -> faces -> next;

	//对接父级
	sfs -> parent = m -> faces;

	//装载面
	for( i = 1; i <= numFaces; i ++ )
	{
		//对接父级
		( sfs + i ) -> parent = fp;

		//无面则不投影
		if( fp -> face == NULL )
		{
			fp = fp -> next;

			( sfs + i - 1 ) -> next = sfs + i;

			continue;
		}

		//投影面链表对接
		( sfs + i ) -> face         = sp;

		//初始化多边形结点
		pp                          = fp -> face -> next;

		//投影面
		while( pp != NULL )
		{
			//如果获取不到面顶点的索引值,则判断网格为不匹配网格,无法投影
			if( ( vi = vertices_index( m -> vertices, pp -> vertex ) ) != 0 )
			{
				//依照网格顺序对接投影面构造,并链接面链表
				++ sp;

				sp -> vertex       = ( svs + vi ) -> vertex;
				sp -> parent       = pp;

				( sp - 1 ) -> next = sp;
			
				pp                 = pp  -> next;
			}
			else
			{
				printf( "不匹配网格!" );

				exit( FALSE );
			}
		}

		//置空链表尾
		sp -> next = NULL;

		//转到下一个头结点
		sp ++;

		//连接面集
		fp = fp -> next;

		( sfs + i - 1 ) -> next = sfs + i;
	}

	//置空链表尾
	( sfs + i - 1 ) -> next  = NULL;

	//尾指针对接
	sfs             -> rear  = i ? sfs + i - 1 : NULL;

	//对接面头结点
	projection      -> faces = sfs;

	return projection;
}


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
����һ���ṹ��������ͬ��ͶӰ.
**/
Projection * newProjection( Mesh * m, Projection * * from )
{
	//ͶӰ
	Projection     * projection;

	ScreenVertex   * sv;

	//��Ļ���㼯,��Ϊ����ָ����
	ScreenVertices * svs;

	//��Ļ�����,��Ϊ����ָ����
	ScreenPolygon  * sp;

	//��Ļ�漯,��Ϊ����ָ����
	ScreenFaces    * sfs;

	//������ָ��
	Faces          * fp;

	//���񶥵�ָ��
	Vertices       * vp;

	//��������ָ��
	Polygon        * pp;

	//������,����������������,��Ϊ����ָ����
	Vector3D       * vs;

	//vi��Ϊi����
	int              numVertices = vertices_length( m -> vertices ), numFaces = faces_length( m -> faces ), i, vi;

	//һ�����������пռ�
	if( ( projection = ( Projection     * )malloc( sizeof( Projection     )                                                     ) ) == NULL
	||  ( svs        = ( ScreenVertices * )malloc( sizeof( ScreenVertices ) * ( numVertices                        + 1        ) ) ) == NULL      //+1Ϊͷָ����
	||  ( sfs        = ( ScreenFaces    * )malloc( sizeof( ScreenFaces    ) * ( numFaces                           + 1        ) ) ) == NULL
	||  ( vs         = ( Vector3D       * )malloc( sizeof( Vector3D       ) * ( numVertices                                   ) ) ) == NULL      //����screen��view
	||  ( sv         = ( ScreenVertex   * )malloc( sizeof( ScreenVertex   ) * ( numVertices                                   ) ) ) == NULL
	||  ( sp         = ( ScreenPolygon  * )malloc( sizeof( ScreenPolygon  ) * ( faces_verticesLength( m -> faces ) + numFaces ) ) ) == NULL )    //�������������ж���������е�ͷ���
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

	//�����һ������
	sv -> parent = vp -> vertex;

	//2D����װ������,�ӵ�һ����㿪ʼ
	for( i = 1; i <= numVertices; i ++ )
	{
		vi = i - 1;

		( sv + i   ) -> parent = ( vp = vp -> next ) -> vertex;
		( sv + i   ) -> screen = vs  + vi;
		( svs + i  ) -> vertex = sv  + i;
		( svs + vi ) -> next   = svs + i;  
	}

	//�ÿ�βָ��
	( svs + i - 1 ) -> next    = NULL;

	//����ͷָ��
	projection -> vertices = svs;

	//������������Ӽ�,�򷵻�����ͶӰ
	if( m -> faces == NULL )
	{
		projection -> faces = NULL;

		return projection;
	}

	//��ָ��ָ���һ����������
	fp = m -> faces -> next;

	//�ԽӸ���
	sfs -> parent = m -> faces;

	//װ����
	for( i = 1; i <= numFaces; i ++ )
	{
		//�ԽӸ���
		( sfs + i ) -> parent = fp;

		//������ͶӰ
		if( fp -> face == NULL )
		{
			fp = fp -> next;

			( sfs + i - 1 ) -> next = sfs + i;

			continue;
		}

		//ͶӰ������Խ�
		( sfs + i ) -> face         = sp;

		//��ʼ������ν��
		pp                          = fp -> face -> next;

		//ͶӰ��
		while( pp != NULL )
		{
			//�����ȡ�����涥�������ֵ,���ж�����Ϊ��ƥ������,�޷�ͶӰ
			if( ( vi = vertices_index( m -> vertices, pp -> vertex ) ) != 0 )
			{
				//��������˳��Խ�ͶӰ�湹��,������������
				++ sp;

				sp -> vertex       = ( svs + vi ) -> vertex;
				sp -> parent       = pp;

				( sp - 1 ) -> next = sp;
			
				pp                 = pp  -> next;
			}
			else
			{
				printf( "��ƥ������!" );

				exit( FALSE );
			}
		}

		//�ÿ�����β
		sp -> next = NULL;

		//ת����һ��ͷ���
		sp ++;

		//�����漯
		fp = fp -> next;

		( sfs + i - 1 ) -> next = sfs + i;
	}

	//�ÿ�����β
	( sfs + i - 1 ) -> next  = NULL;

	//βָ��Խ�
	sfs             -> rear  = i ? sfs + i - 1 : NULL;

	//�Խ���ͷ���
	projection      -> faces = sfs;

	return projection;
}

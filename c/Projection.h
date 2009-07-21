
#include "Mesh.h"

//N
typedef struct ScreenVertices
{
	Vertex                * parent;

	Vector3D              * screen;

	struct ScreenVertices * next;
}ScreenVertices;

typedef struct ScreenPolygon
{
	Vertex               * parent;

	Vector3D             * screen;

	struct ScreenPolygon * next;
}ScreenPolygon;

typedef struct ScreenFaces
{
	ScreenPolygon      * face;

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

	svs -> parent = vp -> vertex;

	//2D����װ������,�ӵ�һ����㿪ʼ
	for( i = 1; i <= numVertices; i ++ )
	{
		vi = i - 1;

		( svs + i  ) -> parent = ( vp = vp -> next ) -> vertex;
		( svs + i  ) -> screen = vs  + vi;
		( svs + vi ) -> next   = svs + i;  
	}

	//�ÿ�βָ��
	( svs + i ) -> next    = NULL;

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

	//װ����
	for( i = 1; i <= numFaces; i ++ )
	{
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
			if( vi = vertices_index( m -> vertices, pp -> vertex ) )
			{
				//��������˳��Խ�ͶӰ�湹��,������������
				++ sp;

				sp -> screen       = ( svs + vi ) -> screen;
				sp -> parent       = ( svs + vi ) -> parent;

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
	( sfs + i ) -> next  = NULL;

	//βָ��Խ�
	sfs         -> rear  = i ? sfs + i : NULL;

	//�Խ���ͷ���
	projection  -> faces = sfs;

	return projection;
}

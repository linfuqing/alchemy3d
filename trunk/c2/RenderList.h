#ifndef __RENDERLIST_H
#define __RENDERLIST_H

#define NUM_PER_RL_INIT 200

typedef struct RenderList
{
	int k;

	Triangle * polygon;

	struct RenderList * next;
	struct RenderList * pre;

}RenderList;

//���Ż�
RenderList * initializeRenderList( int number )
{
	int i = 0;

	RenderList * renderList, * lastRenderList;

	if( ( renderList = ( RenderList * )malloc( sizeof( RenderList ) * number ) ) == NULL ) exit( TRUE );

	lastRenderList = renderList;

	for ( ; i < number - 1; i ++ )
	{
		lastRenderList->polygon = NULL;

		lastRenderList->k = 0;

		lastRenderList->next = lastRenderList + 1;

		lastRenderList->pre = lastRenderList - 1;

		lastRenderList ++;
	}

	lastRenderList->polygon = NULL;

	lastRenderList->next = NULL;

	lastRenderList->pre = lastRenderList - 1;

	return renderList;
}

INLINE void renderList_extendRenderList( RenderList ** rl_ptr, int length )
{
	RenderList * newRL;

	newRL = initializeRenderList( length );

	newRL->pre = * rl_ptr;

	newRL->k = (* rl_ptr)->k + 1;

	(* rl_ptr)->next = newRL;
}

INLINE void renderList_push( RenderList ** rl_ptr, Triangle * face)
{
	( * rl_ptr )->k = ( * rl_ptr )->pre->k + 1;

	//���������Ⱦ�б�β���������NUM_PER_RL_INIT���µĽ��(ע�Ᵽ����β)
	if ( NULL == (* rl_ptr)->next ) renderList_extendRenderList( rl_ptr, NUM_PER_RL_INIT );

	( * rl_ptr )->polygon = face;
	( * rl_ptr ) = ( * rl_ptr )->next;
}

//Partition
RenderList * renderList_qPartition( RenderList ** L, RenderList * p , RenderList * q )
{
	float max1, max2;

	max1 = p->polygon->vertex[0]->v_pos->w;
	max1 = MAX( max1, p->polygon->vertex[1]->v_pos->w );
	max1 = MAX( max1, p->polygon->vertex[2]->v_pos->w );

	(*L)->polygon = p->polygon;

	while ( p->k < q->k )
	{
		max2 = q->polygon->vertex[0]->v_pos->w;
		max2 = MAX( max2, q->polygon->vertex[1]->v_pos->w );
		max2 = MAX( max2, q->polygon->vertex[2]->v_pos->w );

		while ((p->k < q->k) && max2 <= max1 )  q=q->pre;//�Ӻ���ǰ�ң��ҵ����ڻ�׼��¼
		p->polygon = q->polygon;
		while ( (p->k < q->k) && max2 >= max1 )  p=p->next;//��ǰ����ң��ҵ�С�ڻ�׼��¼��Ԫ��
		q->polygon = p->polygon;                              //������Ԫ�ؽ���
	}

	p->polygon=(*L)->polygon;                         //����׼��¼�浽�ҵ���λ��

	return p;
}

//QSort����
void renderList_qSort( RenderList ** L, RenderList * p, RenderList * q )
{
	RenderList * pivotloc;

	if ( p->k && q->k && p->k < q->k )
	{
		pivotloc = renderList_qPartition(L, p, q) ;   //Partition���� �ҵ���׼��¼ ��ָ��
		renderList_qSort (L, p, pivotloc->pre) ;               //���ζԻ�׼��¼ǰ�ļ�¼��������
		renderList_qSort (L, pivotloc->next, q);               //���ζԻ�׼��¼��ļ�¼����
	}
}

//����������
void renderList_quickSort( RenderList ** L, RenderList ** tail )
{
	RenderList * p , * q;
	p=(*L)->next;
	q=(*tail);

	renderList_qSort(L, p ,q );
}

#endif
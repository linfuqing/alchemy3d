#ifndef __RENDERLIST_H
#define __RENDERLIST_H

# include "Triangle.h"
# include "Entity.h"

#define NUM_PER_RL_INIT 200

typedef struct RenderList
{
	Triangle * polygon;

	struct SceneNode  * parent;

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

		lastRenderList->parent  = NULL;

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

	(* rl_ptr)->next = newRL;
}

INLINE void renderList_push( RenderList ** rl_ptr, Triangle * face, struct SceneNode * parent )
{
	//���������Ⱦ�б�β���������NUM_PER_RL_INIT���µĽ��(ע�Ᵽ����β)
	if ( NULL == (* rl_ptr)->next ) renderList_extendRenderList( rl_ptr, NUM_PER_RL_INIT );

	( * rl_ptr )->polygon = face;
	( * rl_ptr )->parent  = parent;
	( * rl_ptr ) = ( * rl_ptr )->next;
}

#define ORDER_SORT 0
#define REVERSE_SORT 1

//Partition
RenderList * renderList_qPartition( RenderList * pstHead, RenderList * pstLow, RenderList * pstHigh, int order )
{
	Triangle * iTmp;
	int pivot = 0;

	if ( !pstHead )
	{
		exit(1);
	}
	if ( !pstHead->next )
	{
		return pstHead->next;//��һ��Ԫ��
	}

	pivot = pstLow->polygon->depth;

	if ( order == ORDER_SORT )
	{
		while ( pstLow != pstHigh )
		{
			//�ɽ���Զ����
			//�Ӻ�����ǰ��
			while ( pstLow != pstHigh && pstHigh->polygon->depth >= pivot )
			{
				pstHigh = pstHigh->pre;
			}
			//����high low
			iTmp = pstLow->polygon;
			pstLow->polygon = pstHigh->polygon;
			pstHigh->polygon = iTmp;
			//��ǰ����
			while ( pstLow != pstHigh && pstLow->polygon->depth <= pivot )
			{
				pstLow = pstLow->next;
			}
			//����high low
			iTmp = pstLow->polygon;
			pstLow->polygon = pstHigh->polygon;
			pstHigh->polygon = iTmp;
		}
	}
	else
	{
		while ( pstLow != pstHigh )
		{
			//�ɽ���Զ����
			//�Ӻ�����ǰ��
			while ( pstLow != pstHigh && pstHigh->polygon->depth <= pivot )
			{
				pstHigh = pstHigh->pre;
			}
			//����high low
			iTmp = pstLow->polygon;
			pstLow->polygon = pstHigh->polygon;
			pstHigh->polygon = iTmp;
			//��ǰ����
			while ( pstLow != pstHigh && pstLow->polygon->depth >= pivot )
			{
				pstLow = pstLow->next;
			}
			//����high low
			iTmp = pstLow->polygon;
			pstLow->polygon = pstHigh->polygon;
			pstHigh->polygon = iTmp;
		}
	}

	return pstLow;
}

//QSort����
void renderList_qSort( RenderList * pstHead, RenderList * pstLow, RenderList * pstHigh, int order )
{
	RenderList * pstTmp = NULL;

	pstTmp = renderList_qPartition(pstHead, pstLow, pstHigh, order);

	if ( pstLow != pstTmp )
	{
		renderList_qSort(pstHead, pstLow, pstTmp->pre, order);
	}
	if ( pstHigh != pstTmp )
	{
		renderList_qSort(pstHead, pstTmp->next, pstHigh, order);
	}
}

//����������
void renderList_quickSort( RenderList * pstHead, RenderList * pstHigh, int order )
{
	renderList_qSort( pstHead, pstHead->next, pstHigh, order );
}

#endif
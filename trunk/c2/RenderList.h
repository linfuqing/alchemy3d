#ifndef __RENDERLIST_H
#define __RENDERLIST_H

# include "Triangle.h"
# include "Entity.h"

#define NUM_PER_RL_INIT 200

typedef struct RenderList
{
	int k;

	Triangle * polygon;
	struct SceneNode  * parent;

	struct RenderList * next;
	struct RenderList * pre;

}RenderList;

//已优化
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

INLINE void renderList_push( RenderList ** rl_ptr, Triangle * face, struct SceneNode * parent )
{
	( * rl_ptr )->k = ( * rl_ptr )->pre->k + 1;

	//如果到达渲染列表尾部，则插入NUM_PER_RL_INIT个新的结点(注意保留表尾)
	if ( NULL == (* rl_ptr)->next ) renderList_extendRenderList( rl_ptr, NUM_PER_RL_INIT );

	( * rl_ptr )->polygon = face;
	( * rl_ptr )->parent  = parent;
	( * rl_ptr ) = ( * rl_ptr )->next;
}

//Partition
RenderList * renderList_qPartition( RenderList ** L, RenderList * p , RenderList * q )
{
	float pivotkey = p->polygon->depth;

	(*L)->polygon = p->polygon;

	while ( p->k < q->k )
	{
		while ((p->k < q->k) && q->polygon->depth <= pivotkey )  q=q->pre;//从后向前找，找到大于基准记录
		p->polygon = q->polygon;
		while ( (p->k < q->k) && q->polygon->depth >= pivotkey )  p=p->next;//从前向后找，找到小于基准记录的元素
		q->polygon = p->polygon;                              //上述两元素交换
	}

	p->polygon=(*L)->polygon;                         //将基准记录存到找到的位置

	return p;
}

//QSort函数
void renderList_qSort( RenderList ** L, RenderList * p, RenderList * q )
{
	RenderList * pivotloc;

	if ( p->k && q->k && p->k < q->k )
	{
		pivotloc = renderList_qPartition(L, p, q) ;   //Partition函数 找到基准记录 的指针
		renderList_qSort (L, p, pivotloc->pre) ;               //依次对基准记录前的记录快速排序
		renderList_qSort (L, pivotloc->next, q);               //依次对基准记录后的记录排序
	}
}

//快速排序函数
void renderList_quickSort( RenderList ** L, RenderList ** tail )
{
	RenderList * p , * q;
	p=(*L)->next;
	q=(*tail);

	renderList_qSort(L, p ,q );
}

#endif
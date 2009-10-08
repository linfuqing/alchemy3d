#ifndef __RENDERLIST_H
#define __RENDERLIST_H

#define NUM_PER_RL_INIT 200

typedef struct RenderList
{
	Triangle * polygon;

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

		lastRenderList->next = lastRenderList + 1;

		lastRenderList->pre = lastRenderList - 1;

		lastRenderList ++;
	}

	lastRenderList->polygon = NULL;

	lastRenderList->next = NULL;

	lastRenderList->pre = lastRenderList - 1;

	return renderList;
}



INLINE void extendRenderList( RenderList ** rl_ptr, int length )
{
	RenderList * newRL;

	newRL = initializeRenderList( length );

	newRL->pre = * rl_ptr;

	(* rl_ptr)->next = newRL;
}

INLINE void renderList_push( RenderList ** rl_ptr, Triangle * face)
{
	//如果到达渲染列表尾部，则插入NUM_PER_RL_INIT个新的结点(注意保留表尾)
	if ( NULL == (* rl_ptr)->next )
		extendRenderList( rl_ptr, NUM_PER_RL_INIT );

	( * rl_ptr )->polygon = face;
	( * rl_ptr ) = ( * rl_ptr )->next;
}

#endif
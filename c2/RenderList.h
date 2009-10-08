#ifndef __RENDERLIST_H
#define __RENDERLIST_H

#define NUM_PER_RL_INIT 200

typedef struct RenderList
{
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
	//���������Ⱦ�б�β���������NUM_PER_RL_INIT���µĽ��(ע�Ᵽ����β)
	if ( NULL == (* rl_ptr)->next )
		extendRenderList( rl_ptr, NUM_PER_RL_INIT );

	( * rl_ptr )->polygon = face;
	( * rl_ptr ) = ( * rl_ptr )->next;
}

#endif
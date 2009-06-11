/**************************************************************************************
 **----------------------------------------------------------------------------------**
 **                                     |SLNode|                                     **
 **__________________________________________________________________________________**
 **                         include< stdlib.h, malloc.h, Base.h >                    **
 **__________________________________________________________________________________**
 **                                  typedef DataType                                **
 **__________________________________________________________________________________**
 **************************************************************************************/


typedef struct Node
{
	DataType data;

	struct Node * next;
}SLNode;

void ListInitiate( SLNode * * head )
{
	if( ( * head = ( SLNode * )malloc( sizeof( SLNode ) ) ) == NULL )
	{
		exit( 1 );
	}

	( * head ) -> next = NULL;
}

int ListLength( SLNode * head )
{
	SLNode * p = head;
	
	int size = 0;

	while( p -> next != NULL )
	{
		p = p -> next;

		size ++;
	}

	return size;
}

int ListInsert( SLNode * head, int i, DataType x )
{
	SLNode * p, * q;

	int j;

	p = head;
	j = - 1;

	while( p-> next != NULL && j< i - 1 )
	{
		p = p -> next;
		j ++;
	}

	if( j != i - 1 )
	{
		printf( "插入位置出错!" );

		return FALUSE;
	}

	if( ( q = ( SLNode * )malloc( sizeof( SLNode ) ) ) == NULL )
	{
		exit( 1 );
	}

	q -> data = x;

	q ->next = p -> next;

	p -> next = q;

	return TRUE;
}

int ListDelete( SLNode * head, int i, DataType * x )
{
	SLNode * p, * s;

	int j;

	p = head;

	j = - 1;

	while( p -> next != NULL && p -> next -> next != NULL && j < i - 1 )
	{
		p = p -> next;

		j ++;
	}

	if( j != i - 1 )
	{
		printf( "删除位置参数错误!" );

		return FALSE;
	}

	s = p -> next;

	* x = s -> data;

	p -> next = p -> next -> next;

	free( s );

	return TRUE;
}

int ListGet( SLNode * head, int i, DataType * x )
{
	SLNode * p;

	int j;

	p = head;

	j = - 1;

	while( p -> next != NULL && j < i )
	{
		p = p -> next;

		j ++;
	}

	if( j != i )
	{
		printf( "取元素位置参数错!" );

		return FALUSE;
	}

	* x = p -> data;

	return TRUE;
}

void Destroy( SLNode * * head )
{
	SLNode * p, * p1;

	p = * head;

	while( p != NULL )
	{
		p1 = p;

		p = p -> next;

		free( p1 );
	}

	* head = NULL;
}
/********************************************************************
* File Name : quick_sort.c *
* Created : 2007/05/08 *
* Author : SunYonggao *
* Description : 对双向链表进行快速排序
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//定义类型 所有的排序例子中都是用的int作为data
typedef int elemType;

//返回值
#define RET_SUCCESS ( 1 )
#define RET_FAILED ( 0 )

//定义链表的长度
#define LIST_MAX_SIZE (6)

//定义链表申请内存不够时报错信息
#define NO_MEMORY printf("Error! Not enough memory!\n");exit(1)

//双向链表结构体定义
typedef struct tagDuNode_t
{
	elemType data;
	struct tagDuNode_t * pstNext;
	struct tagDuNode_t * pstPrior;
}duNode_t;

//初始化双向链表
int initDuList(duNode_t ** pstHead)
{
	int iRet = RET_SUCCESS;
	int iCir = 0;
	duNode_t * pstTmp1 = NULL;
	duNode_t * pstTmp2 = NULL;

	int num[6] = {749,749,449,599,599,449};

	//初始化头节点
	* pstHead = (duNode_t *)malloc(sizeof(duNode_t));
	(* pstHead)->pstPrior = NULL;
	(* pstHead)->pstNext = NULL;
	if ( !pstHead )
	{
		NO_MEMORY;
	}
	pstTmp1 = * pstHead;
	//链表初始化
	srand( time(NULL) );//随机数
	for( iCir = 0; iCir < LIST_MAX_SIZE; iCir++ )
	{
		pstTmp2 = (duNode_t *)malloc(sizeof(duNode_t));
		if ( !pstTmp2 )
		{
			NO_MEMORY;
		}
		//赋初值
		pstTmp2->data = num[iCir];
		pstTmp2->pstNext = NULL;
		pstTmp1->pstNext = pstTmp2;
		pstTmp2->pstPrior = pstTmp1;
		pstTmp1 = pstTmp2;
	}
	return iRet;
}

// 打印链表 链表的data元素是可打印的整形
int showDuList(duNode_t * pstHead)
{
	int iRet = RET_SUCCESS;
	duNode_t * pstTmp = pstHead->pstNext;
	if ( NULL == pstHead )
	{
		printf("链表的头节点是空\n");
		iRet = RET_FAILED;
	}
	else
	{
		while ( pstTmp )
		{
			printf("%d ", pstTmp->data);
			pstTmp = pstTmp->pstNext;
		}
		printf("\n");
	}
	return iRet;
}

/* 删除包括头节点在内的所有的节点. 07/04/28 */
int destroyList(duNode_t * pstHead)
{
	duNode_t * pstTmp = NULL; /* Temp pointer for circle */
	int iRet = RET_SUCCESS;
	if ( !pstHead )
	{
		printf("Error! pstHead is null\n");
		iRet = RET_FAILED;
	}
	else
	{
		while ( pstHead ) /* Free nodes */
		{
			pstTmp = pstHead;
			pstHead = pstHead->pstNext;
			free(pstTmp);
		}
		pstHead = NULL;
	}
	return iRet;
}/* End of destroyList----------------------------------------------*/

//正确的快速排序 2007/05/09
/*
一趟快速排序的具体做法是：附设两个指针low和high(即第一个和最后一个指针),
他们的初值分别为low和high设枢轴(一般为low的值pivot)记录的关键字
(即本例子中的整形data)为pivot，则首先从high所指位置
起向前搜索到第一个关键字小于pivot的记录和枢轴记录交换，然后从low所指位置起
向后搜索，找到第一个关键字大于pivot的记录和枢轴记录相互交换，重复这两步直
至low = high为止。
*/
duNode_t * partion(duNode_t * pstHead, duNode_t * pstLow, duNode_t * pstHigh)
{
	elemType iTmp = 0;
	elemType pivot = 0;
	if ( !pstHead )
	{
		printf("错误，头节点为空！\n");
		exit(1);
	}
	if ( !pstHead->pstNext )
	{
		return pstHead->pstNext;//就一个元素
	}
	pivot = pstLow->data;
	while ( pstLow != pstHigh )
	{
		//从后面往前换
		while ( pstLow != pstHigh && pstHigh->data <= pivot )
		{
			pstHigh = pstHigh->pstPrior;
		}
		//交换high low
		iTmp = pstLow->data;
		pstLow->data = pstHigh->data;
		pstHigh->data = iTmp;
		//从前往后换
		while ( pstLow != pstHigh && pstLow->data >= pivot )
		{
			pstLow = pstLow->pstNext;
		}
		//交换high low
		iTmp = pstLow->data;
		pstLow->data = pstHigh->data;
		pstHigh->data = iTmp;
	}
	return pstLow;
}
//快排
void quick_sort(duNode_t * pstHead, duNode_t * pstLow, duNode_t * pstHigh)
{
	duNode_t * pstTmp = NULL;

	pstTmp = partion(pstHead, pstLow, pstHigh);
	if ( pstLow != pstTmp )
	{
		quick_sort(pstHead, pstLow, pstTmp->pstPrior);
	}
	if ( pstHigh != pstTmp )
	{
		quick_sort(pstHead, pstTmp->pstNext, pstHigh);
	}

}
void main()
{
	duNode_t * pstHead = NULL;
	duNode_t * pstHigh = NULL;
	duNode_t * pstTmp = NULL;
	initDuList(&pstHead); //初始化
	printf("Before sorting:\n");
	showDuList(pstHead); //打印
	pstTmp = pstHead->pstNext;
	while ( pstTmp->pstNext )
	{
		pstTmp = pstTmp->pstNext;
	}
	pstHigh = pstTmp;//找到最后一个节点的指针用于快排时
	quick_sort(pstHead, pstHead->pstNext, pstHigh);//快排序
	printf("After sorting:\n");
	showDuList(pstHead);
	destroyList(pstHead);
	pstHead = NULL;
}
/* End of file ----------------------------------------------------*/
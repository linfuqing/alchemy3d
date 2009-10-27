/********************************************************************
* File Name : quick_sort.c *
* Created : 2007/05/08 *
* Author : SunYonggao *
* Description : ��˫��������п�������
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//�������� ���е����������ж����õ�int��Ϊdata
typedef int elemType;

//����ֵ
#define RET_SUCCESS ( 1 )
#define RET_FAILED ( 0 )

//��������ĳ���
#define LIST_MAX_SIZE (6)

//�������������ڴ治��ʱ������Ϣ
#define NO_MEMORY printf("Error! Not enough memory!\n");exit(1)

//˫������ṹ�嶨��
typedef struct tagDuNode_t
{
	elemType data;
	struct tagDuNode_t * pstNext;
	struct tagDuNode_t * pstPrior;
}duNode_t;

//��ʼ��˫������
int initDuList(duNode_t ** pstHead)
{
	int iRet = RET_SUCCESS;
	int iCir = 0;
	duNode_t * pstTmp1 = NULL;
	duNode_t * pstTmp2 = NULL;

	int num[6] = {749,749,449,599,599,449};

	//��ʼ��ͷ�ڵ�
	* pstHead = (duNode_t *)malloc(sizeof(duNode_t));
	(* pstHead)->pstPrior = NULL;
	(* pstHead)->pstNext = NULL;
	if ( !pstHead )
	{
		NO_MEMORY;
	}
	pstTmp1 = * pstHead;
	//�����ʼ��
	srand( time(NULL) );//�����
	for( iCir = 0; iCir < LIST_MAX_SIZE; iCir++ )
	{
		pstTmp2 = (duNode_t *)malloc(sizeof(duNode_t));
		if ( !pstTmp2 )
		{
			NO_MEMORY;
		}
		//����ֵ
		pstTmp2->data = num[iCir];
		pstTmp2->pstNext = NULL;
		pstTmp1->pstNext = pstTmp2;
		pstTmp2->pstPrior = pstTmp1;
		pstTmp1 = pstTmp2;
	}
	return iRet;
}

// ��ӡ���� �����dataԪ���ǿɴ�ӡ������
int showDuList(duNode_t * pstHead)
{
	int iRet = RET_SUCCESS;
	duNode_t * pstTmp = pstHead->pstNext;
	if ( NULL == pstHead )
	{
		printf("�����ͷ�ڵ��ǿ�\n");
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

/* ɾ������ͷ�ڵ����ڵ����еĽڵ�. 07/04/28 */
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

//��ȷ�Ŀ������� 2007/05/09
/*
һ�˿�������ľ��������ǣ���������ָ��low��high(����һ�������һ��ָ��),
���ǵĳ�ֵ�ֱ�Ϊlow��high������(һ��Ϊlow��ֵpivot)��¼�Ĺؼ���
(���������е�����data)Ϊpivot�������ȴ�high��ָλ��
����ǰ��������һ���ؼ���С��pivot�ļ�¼�������¼������Ȼ���low��ָλ����
����������ҵ���һ���ؼ��ִ���pivot�ļ�¼�������¼�໥�������ظ�������ֱ
��low = highΪֹ��
*/
duNode_t * partion(duNode_t * pstHead, duNode_t * pstLow, duNode_t * pstHigh)
{
	elemType iTmp = 0;
	elemType pivot = 0;
	if ( !pstHead )
	{
		printf("����ͷ�ڵ�Ϊ�գ�\n");
		exit(1);
	}
	if ( !pstHead->pstNext )
	{
		return pstHead->pstNext;//��һ��Ԫ��
	}
	pivot = pstLow->data;
	while ( pstLow != pstHigh )
	{
		//�Ӻ�����ǰ��
		while ( pstLow != pstHigh && pstHigh->data <= pivot )
		{
			pstHigh = pstHigh->pstPrior;
		}
		//����high low
		iTmp = pstLow->data;
		pstLow->data = pstHigh->data;
		pstHigh->data = iTmp;
		//��ǰ����
		while ( pstLow != pstHigh && pstLow->data >= pivot )
		{
			pstLow = pstLow->pstNext;
		}
		//����high low
		iTmp = pstLow->data;
		pstLow->data = pstHigh->data;
		pstHigh->data = iTmp;
	}
	return pstLow;
}
//����
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
	initDuList(&pstHead); //��ʼ��
	printf("Before sorting:\n");
	showDuList(pstHead); //��ӡ
	pstTmp = pstHead->pstNext;
	while ( pstTmp->pstNext )
	{
		pstTmp = pstTmp->pstNext;
	}
	pstHigh = pstTmp;//�ҵ����һ���ڵ��ָ�����ڿ���ʱ
	quick_sort(pstHead, pstHead->pstNext, pstHigh);//������
	printf("After sorting:\n");
	showDuList(pstHead);
	destroyList(pstHead);
	pstHead = NULL;
}
/* End of file ----------------------------------------------------*/
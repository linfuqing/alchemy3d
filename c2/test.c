//01-01-09 14:14
# include <stdio.h>
# include <stdlib.h>
# define status int
# define OK 1
# define FALSE 0
# define OVERFLOW -2

typedef struct node{
        int data , k;  //data存数据，k标记数据元素在链表中的相对位置
        struct node* pre;  //指向前结点
        struct node* next;  //指向后结点
}Node ,* NodePtr;    //结点定义


status CreateBiList(NodePtr* L,NodePtr* tail){
       int n  , e ,i;
       NodePtr p1 , p2 ;
       printf("Total Num\n") ;
             scanf("%d" , &n) ;
       (*L)=(NodePtr)malloc(sizeof(Node));    (*L)->next=NULL ;
       (*L)->data=n ;  (*L)->k=0;                             //头结点在建立双向链表时记录数据个数   ，在快速排序时用于记录基准记录

       p1=p2=(NodePtr)malloc(sizeof(Node));
       p1->pre=(*L);
       (*L)->next=p1;
       printf("Enter Num\n");
       scanf("%d",&e);     p1->data=e; p1->k=1   ;      //建立除头结点外第一个结点
          for( i=2 ; i<n+1 ; i++){
              p1=(NodePtr)malloc(sizeof(Node));   //依次建立结点
              scanf("%d",&e);   p1->data=e; p1->k=i;
              p2->next=p1;                //双向的建立
              p1->pre=p2;
              p2=p1;
              p1->next=NULL;
          }//for循环建立双向链表
          (*tail)=p1; //if(!L)printf("OK");
       return  OK;
}         //建立双向链表函数

status DestoryBilist(NodePtr* L){
       NodePtr p;
       int i;
       for(i=0 ; p!=NULL ;i++){
           p=(*L)->next;
           free(*L);            //销毁函数
           *L=p;
       }
       return OK;
}       //销毁双向链表

NodePtr Partition(NodePtr*L,NodePtr p , NodePtr q) {
   int pivotkey = p->data ;
   (*L)->data = p->data;
    while  (p->k < q->k) {
        while ( (p->k < q->k) && q->data >= pivotkey )  q=q->pre;//从后向前找，找到小于基准记录
        p->data = q->data;
        while ( (p->k < q->k) && p->data <= pivotkey )  p=p->next;//从前向后找，找到大于基准记录的元素
        q->data = p->data;                              //上述两元素交换
    }
    p->data=(*L)->data;                         //将基准记录存到找到的位置
    return p;            //本程序最关键部分，寻找基准记录
}//Partition

void QSort(NodePtr*L , NodePtr p,  NodePtr q ){
      if (p->k < q->k){
             NodePtr  pivotloc = Partition(L,p, q) ;   //Partition函数 找到基准记录 的指针
             QSort (L,p, pivotloc->pre) ;               //依次对基准记录前的记录快速排序
             QSort (L,pivotloc->next, q);               //依次对基准记录后的记录排序
      }
}  //QSort函数




void QuickSort(NodePtr*L,NodePtr* tail){
       NodePtr p , q  ;//if(!L)printf("Sorry");system("pause");
       p=(*L)->next;   //printf("%d",L);
       q=(*tail);

       QSort(L,p ,q );  //这里L->data 是待排数据元素的个数

} //快速排序函数

void Display(NodePtr* L){
     NodePtr p=*L;
     while(p->next){
          p=p->next;
          printf("%d ",p->data);
     }
}         //打印函数

main()
{
     NodePtr  L  , tail;
     CreateBiList(&L,&tail);//if(!L)printf("ZL");
     QuickSort(&L,&tail);

     Display(&L);
     //DestoryBilist(&L);
     system("pause");
     return 0;
}     //主函数

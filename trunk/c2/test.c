//01-01-09 14:14
# include <stdio.h>
# include <stdlib.h>
# define status int
# define OK 1
# define FALSE 0
# define OVERFLOW -2

typedef struct node{
        int data , k;  //data�����ݣ�k�������Ԫ���������е����λ��
        struct node* pre;  //ָ��ǰ���
        struct node* next;  //ָ�����
}Node ,* NodePtr;    //��㶨��


status CreateBiList(NodePtr* L,NodePtr* tail){
       int n  , e ,i;
       NodePtr p1 , p2 ;
       printf("Total Num\n") ;
             scanf("%d" , &n) ;
       (*L)=(NodePtr)malloc(sizeof(Node));    (*L)->next=NULL ;
       (*L)->data=n ;  (*L)->k=0;                             //ͷ����ڽ���˫������ʱ��¼���ݸ���   ���ڿ�������ʱ���ڼ�¼��׼��¼

       p1=p2=(NodePtr)malloc(sizeof(Node));
       p1->pre=(*L);
       (*L)->next=p1;
       printf("Enter Num\n");
       scanf("%d",&e);     p1->data=e; p1->k=1   ;      //������ͷ������һ�����
          for( i=2 ; i<n+1 ; i++){
              p1=(NodePtr)malloc(sizeof(Node));   //���ν������
              scanf("%d",&e);   p1->data=e; p1->k=i;
              p2->next=p1;                //˫��Ľ���
              p1->pre=p2;
              p2=p1;
              p1->next=NULL;
          }//forѭ������˫������
          (*tail)=p1; //if(!L)printf("OK");
       return  OK;
}         //����˫��������

status DestoryBilist(NodePtr* L){
       NodePtr p;
       int i;
       for(i=0 ; p!=NULL ;i++){
           p=(*L)->next;
           free(*L);            //���ٺ���
           *L=p;
       }
       return OK;
}       //����˫������

NodePtr Partition(NodePtr*L,NodePtr p , NodePtr q) {
   int pivotkey = p->data ;
   (*L)->data = p->data;
    while  (p->k < q->k) {
        while ( (p->k < q->k) && q->data >= pivotkey )  q=q->pre;//�Ӻ���ǰ�ң��ҵ�С�ڻ�׼��¼
        p->data = q->data;
        while ( (p->k < q->k) && p->data <= pivotkey )  p=p->next;//��ǰ����ң��ҵ����ڻ�׼��¼��Ԫ��
        q->data = p->data;                              //������Ԫ�ؽ���
    }
    p->data=(*L)->data;                         //����׼��¼�浽�ҵ���λ��
    return p;            //��������ؼ����֣�Ѱ�һ�׼��¼
}//Partition

void QSort(NodePtr*L , NodePtr p,  NodePtr q ){
      if (p->k < q->k){
             NodePtr  pivotloc = Partition(L,p, q) ;   //Partition���� �ҵ���׼��¼ ��ָ��
             QSort (L,p, pivotloc->pre) ;               //���ζԻ�׼��¼ǰ�ļ�¼��������
             QSort (L,pivotloc->next, q);               //���ζԻ�׼��¼��ļ�¼����
      }
}  //QSort����




void QuickSort(NodePtr*L,NodePtr* tail){
       NodePtr p , q  ;//if(!L)printf("Sorry");system("pause");
       p=(*L)->next;   //printf("%d",L);
       q=(*tail);

       QSort(L,p ,q );  //����L->data �Ǵ�������Ԫ�صĸ���

} //����������

void Display(NodePtr* L){
     NodePtr p=*L;
     while(p->next){
          p=p->next;
          printf("%d ",p->data);
     }
}         //��ӡ����

main()
{
     NodePtr  L  , tail;
     CreateBiList(&L,&tail);//if(!L)printf("ZL");
     QuickSort(&L,&tail);

     Display(&L);
     //DestoryBilist(&L);
     system("pause");
     return 0;
}     //������

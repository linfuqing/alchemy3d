#include <stdio.h>

typedef unsigned short		WORD;
typedef unsigned int		DWORD;
typedef unsigned long		ULONG;

#define RGBTo16(r, g, b) ( ( ( r >> 3 ) << 11 ) | ( ( g >> 2 ) << 5 ) | ( b >> 3 ) )

/** GCC -o ins  inlinemmx.c **/

int main(int argc,char *argv[])
{
	WORD wa = 10;
	WORD wb = 20;
	DWORD walpha = 127;
	ULONG wc;

	asm volatile("movl $0x7e0f81f, %%edi;\n\t"
				"movl %%edi, %0;"
				:"=r"(wc)
				:"r"(wa), "r"(wb), "r"(walpha)
				:
	);
	//int result, i;
	//short a[] = {1, 2, 3, 4, 5, 6, 7, 8};
	//short b[] = {1, 1, 1, 1, 1, 1, 1, 1};

	//printf("...with MMX instructions...\n");

	///*���ȣ���������ۻ��Ĵ������㣬ʵ��ȱʡ��Ϊ0��*/
	//asm("pandn %%mm5,%%mm5;"::);

	///*����a, b��ÿ�Ķ�����˺��������ӣ��γ������*/
	///*�����ѭ��������C����*/
	//for(i = 0; i < sizeof(a)/sizeof(short); i += 4){
	//	asm("movq %0,%%mm0;\
	//		movq %1,%%mm1;\
	//		pmaddwd %%mm1,%%mm0;\
	//		paddd %%mm0,%%mm5; #��˺���� "
	//		:
	//	: "m" (a[i]), "m" (b[i]));
	//}

	/*������ͷ��룬�����*/
	/*asm("movq %%mm5, %%mm0;\
		psrlq $32,%%mm5;\
		paddd %%mm0, %%mm5;\
		movd %%mm5,%0;\
		emms"
		:"=r" (result)
		:);*/

	//printf("result: 0x%x\n", result);
	////������Ϊ0x24

	printf("result: %x\n", wc);

	return(0);
}

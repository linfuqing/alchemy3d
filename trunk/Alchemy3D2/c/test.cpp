#include <stdio.h>
#include <malloc.h>

struct b
{
	int as;
	int sdf;
	int dsaf;
};
void main()
{
	//unsigned char *i = (unsigned char*)malloc(sizeof(int) * 2);
	//unsigned int *j = (unsigned int*)i, *k = j;

	
	
	//printf("%d\n", j);
	//printf("%d\n", ++j);
	//b *a = new b;
	__asm{

		.386
		.model flat, stdcall
		option casemap:none

		include windows.inc
		include user32.inc
		includelib user32.lib
		include kernel32.inc
		includelib kernel32.lib
	}
}
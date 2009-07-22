#include<stdio.h>
#include<pthread.h>
#include <AS3.h>

#define NUM 6

void print_msg()
{
	int i;
	for(i=0;i<NUM;i++)
	{
		AS3_Trace(AS3_String("test"));
		//printf("%s",m);
		fflush(stdout);
		//sleep(1);
	}
}

AS3_Val thread()
{
	pthread_t t1,t2;
	pthread_create(&t1,NULL,print_msg,NULL);
	pthread_create(&t2,NULL,print_msg,NULL);

	/*pthread_join(t1,NULL);
	pthread_join(t2,NULL);*/

	return 0;
}

int main()
{
	AS3_Val threadMethod = AS3_Function( NULL, thread );

	AS3_Val result = AS3_Object( "thread:AS3ValType", threadMethod );

	AS3_Release( threadMethod );

	AS3_LibInit( result );

	return 0;
}
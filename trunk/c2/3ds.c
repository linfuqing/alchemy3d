#define INLINE inline
#define __AS3__

#include <stdlib.h>
#include <AS3.h>
#include <string.h>

#include "Base.h"
#include "Math.h"
#include "3DSLoader.h"

AS3_Val load_from_bytes( void* self, AS3_Val args )
{
	UCHAR * buffer = NULL;

	UINT length = 0;

	AS3_ArrayValue( args, "PtrType, IntType", & buffer, & length );

	A3DS_LoadData( & buffer, length );

	return 0;
}

AS3_Val applyForTmpBuffer( void* self, AS3_Val args )
{
	int len;

	void * tmpBuff;

	AS3_ArrayValue( args, "IntType", &len );

	if( ( tmpBuff = calloc( len, 1 ) ) == NULL )
	{
		exit( TRUE );
	}

	return AS3_Ptr( tmpBuff );
}

int main()
{
	AS3_Val load_from_bytesMethod = AS3_Function( NULL, load_from_bytes );
	AS3_Val applyForTmpBufferMethod = AS3_Function( NULL, applyForTmpBuffer );



	AS3_Val result = AS3_Object( "load_from_bytes:AS3ValType, applyForTmpBuffer:AS3ValType",
									load_from_bytesMethod, applyForTmpBufferMethod );



	AS3_Release( load_from_bytesMethod );
	AS3_Release( applyForTmpBufferMethod );


	AS3_LibInit( result );

	return 0;
}
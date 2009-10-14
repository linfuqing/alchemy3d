#ifndef __FORCE_H
#define __FORCE_H

# include "Vector3D.h"

typedef struct Force
{
	Vector3D     * value;

	int            life;

	float          tautness;

	Vector3D     * anchor;

	struct Force * next;
}Force;

Force * newForce( float x, float y, float z, int life, Vector3D * anchor, float tautness )
{
	Force * f;

	if( ( f = ( Force * )malloc( sizeof( Force ) ) ) == NULL )
	{
		exit( TRUE );
	}

	f -> value    = newVector3D( x, y, z, 1 );
	
	f -> anchor   = anchor;

	f -> tautness = tautness;

	f -> life     = life;

	f -> next     = NULL;

	return f;
}

void force_update( Force * force, Vector3D * target )
{
	if( force -> anchor )
	{
		vector3D_subtract( force -> value, target, force -> anchor );
		force -> value -> w = force -> tautness;
		vector3D_project( force -> value );
	}
}

int force_live( Force * force )
{
	if ( force -> life != OFF && !( force -> life -- ) )
	{
		return FALSE;
	}

	return TRUE;
}

void force_dispose( Force * force )
{
	free( force -> value  );
	free( force -> anchor );
	free( force );

	memset( force, 0, sizeof( Force ) );
}

# endif
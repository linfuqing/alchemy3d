#ifndef __PHYSICS_H
#define __PHYSICS_H

# include "Force.h"

typedef struct
{
	Force    * forces;

	Vector3D * velocity;

	Vector3D * acceleration;

	float      mass;

	float      friction;
}Physics;

Physics * newPhysics( float mass, float friction )
{
	Physics * p;

	if( ( p = ( Physics * )malloc( sizeof( Physics ) ) ) == NULL )
	{
		exit( TRUE );
	}

	p -> forces       = NULL;
	p -> friction     = friction;
	p -> mass         = mass;
	p -> velocity     = newVector3D( 0, 0, 0, 1 );
	p -> acceleration = newVector3D( 0, 0, 0, 1 );

	return p;
}

void physics_update( Physics * p, Vector3D * target )
{
	Force * fp = p -> forces;

	vector3D_set( p -> acceleration, 0, 0, 0, 1 );

	while( fp )
	{
		if( force_live( fp ) )
		{
			force_update( fp, target );

			vector3D_add_self( p -> acceleration, fp -> value );
		}
		
		fp = fp -> next;
	}

	p -> acceleration -> w = p -> mass;

	vector3D_project( p -> acceleration );

	vector3D_add_self( p -> velocity, p -> acceleration );

	vector3D_scaleBy( p -> velocity, 1 - p -> friction );

	vector3D_add_self( target, p -> velocity );
}

# endif
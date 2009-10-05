#ifndef __OCTREE_H
#define __OCTREE_H

typedef struct OctreeData
{
	int nFaces;

	Triangle ** faces;

	AABB * aabb, * worldAABB, * CVVAABB;

}OctreeData;

typedef struct Octree
{
	OctreeData * data;

	struct Octree * tlb, * trb, * tlf, * trf, * blb, * brb, * blf, * brf;

}Octree;

OctreeData * newOctreeData()
{
	OctreeData * t;

	if( ( t = ( OctreeData * )malloc( sizeof( OctreeData ) ) ) == NULL ) exit( TRUE );

	t->nFaces		= 0;
	t->faces		= NULL;
	t->aabb			= newAABB();
	t->worldAABB	= newAABB();
	t->CVVAABB		= newAABB();

	return t;
}

Octree * newOctree()
{
	Octree * t;

	if( ( t = ( Octree * )malloc( sizeof( Octree ) ) ) == NULL ) exit( TRUE );

	t->tlb = t->trb = t->tlf = t->trf = NULL;
	t->blb = t->brb = t->blf = t->brf = NULL;

	t->data = newOctreeData();

	return t;
}

//构造四叉树
int buildOctree( Octree * root, int maxdepth )
{
	//级数递减
	if( -- maxdepth >= 0 )
	{
		int i = 0, i0 = 0, i1 = 0, i2 = 0, i3 = 0, i4 = 0, i5 = 0, i6 = 0, i7 = 0;
		float t0x, t0y, t0z, t1x, t1y, t1z, t2x, t2y, t2z, minX, maxX, minY, maxY, minZ, maxZ;
		Triangle * face, ** f0, ** f1, ** f2, ** f3, ** f4, ** f5, ** f6, ** f7;

		OctreeData * pr_data = ( ( OctreeData * )root->data );

		t0x = pr_data->aabb->min->x;
		t1x = ( pr_data->aabb->min->x + pr_data->aabb->max->x ) * .5f;
		t2x = pr_data->aabb->max->x;

		t0y = pr_data->aabb->min->y;
		t1y = ( pr_data->aabb->min->y + pr_data->aabb->max->y ) * .5f;
		t2y = pr_data->aabb->max->y;

		t0z = pr_data->aabb->min->z;
		t1z = ( pr_data->aabb->min->z + pr_data->aabb->max->z ) * .5f;
		t2z = pr_data->aabb->max->z;

		if( ( f0 = ( Triangle ** )malloc( sizeof( Triangle * ) * pr_data->nFaces ) ) == NULL ) exit( TRUE );
		if( ( f1 = ( Triangle ** )malloc( sizeof( Triangle * ) * pr_data->nFaces ) ) == NULL ) exit( TRUE );
		if( ( f2 = ( Triangle ** )malloc( sizeof( Triangle * ) * pr_data->nFaces ) ) == NULL ) exit( TRUE );
		if( ( f3 = ( Triangle ** )malloc( sizeof( Triangle * ) * pr_data->nFaces ) ) == NULL ) exit( TRUE );
		if( ( f4 = ( Triangle ** )malloc( sizeof( Triangle * ) * pr_data->nFaces ) ) == NULL ) exit( TRUE );
		if( ( f5 = ( Triangle ** )malloc( sizeof( Triangle * ) * pr_data->nFaces ) ) == NULL ) exit( TRUE );
		if( ( f6 = ( Triangle ** )malloc( sizeof( Triangle * ) * pr_data->nFaces ) ) == NULL ) exit( TRUE );
		if( ( f7 = ( Triangle ** )malloc( sizeof( Triangle * ) * pr_data->nFaces ) ) == NULL ) exit( TRUE );

		for ( ; i < pr_data->nFaces; i ++ )
		{
			face = pr_data->faces[i];

			minX = face->vertex[0]->position->x;
			minX = minX < face->vertex[1]->position->x ? minX : face->vertex[1]->position->x;
			minX = minX < face->vertex[2]->position->x ? minX : face->vertex[2]->position->x;

			maxX = face->vertex[0]->position->x;
			maxX = maxX > face->vertex[1]->position->x ? maxX : face->vertex[1]->position->x;
			maxX = maxX > face->vertex[2]->position->x ? maxX : face->vertex[2]->position->x;

			minY = face->vertex[0]->position->y;
			minY = minY < face->vertex[1]->position->y ? minY : face->vertex[1]->position->y;
			minY = minY < face->vertex[2]->position->y ? minY : face->vertex[2]->position->y;

			maxY = face->vertex[0]->position->y;
			maxY = maxY > face->vertex[1]->position->y ? maxY : face->vertex[1]->position->y;
			maxY = maxY > face->vertex[2]->position->y ? maxY : face->vertex[2]->position->y;

			minZ = face->vertex[0]->position->z;
			minZ = minZ < face->vertex[1]->position->z ? minZ : face->vertex[1]->position->z;
			minZ = minZ < face->vertex[2]->position->z ? minZ : face->vertex[2]->position->z;

			maxZ = face->vertex[0]->position->z;
			maxZ = maxZ > face->vertex[1]->position->z ? maxZ : face->vertex[1]->position->z;
			maxZ = maxZ > face->vertex[2]->position->z ? maxZ : face->vertex[2]->position->z;

			//先处理上半区间
			if ( minY < t1y )
			{
				if ( minX < t1x )
				{
					if ( minZ < t1z )
					{
						f0[i0] = face;

						i0 ++;
					}
					else
					{
						f1[i1] = face;

						i1 ++;
					}
				}
				else
				{
					if ( minZ < t1z )
					{
						f2[i2] = face;

						i2 ++;
					}
					else
					{
						f3[i3] = face;

						i3 ++;
					}
				}
			}
			//下半区间
			else
			{
				if ( minX < t1x )
				{
					if ( minZ < t1z )
					{
						f4[i4] = face;

						i4 ++;
					}
					else
					{
						f5[i5] = face;

						i5 ++;
					}
				}
				else
				{
					if ( minZ < t1z )
					{
						f6[i6] = face;

						i6 ++;
					}
					else
					{
						f7[i7] = face;

						i7 ++;
					}
				}
			}
		}

		if ( i0 )
		{
			root->tlb = newOctree();
			root->tlb->data = newOctreeData();

			if( ( root->tlb->data->faces = ( Triangle ** )malloc( sizeof( Triangle * ) * i0 ) ) == NULL ) exit( TRUE );

			root->tlb->data->nFaces = i0;

			for ( i = 0; i < i0; i ++ )
			{
				root->tlb->data->faces[i] = f0[i];

				aabb_add_3D( root->tlb->data->aabb, f0[i]->vertex[0]->position );
				aabb_add_3D( root->tlb->data->aabb, f0[i]->vertex[1]->position );
				aabb_add_3D( root->tlb->data->aabb, f0[i]->vertex[2]->position );
			}
		}

		if ( i1 )
		{
			root->trb = newOctree();
			root->trb->data = newOctreeData();

			if( ( root->trb->data->faces = ( Triangle ** )malloc( sizeof( Triangle * ) * i1 ) ) == NULL ) exit( TRUE );

			root->trb->data->nFaces = i1;

			for ( i = 0; i < i1; i ++ )
			{
				root->trb->data->faces[i] = f1[i];

				aabb_add_3D( root->trb->data->aabb, f1[i]->vertex[0]->position );
				aabb_add_3D( root->trb->data->aabb, f1[i]->vertex[1]->position );
				aabb_add_3D( root->trb->data->aabb, f1[i]->vertex[2]->position );
			}
		}

		if ( i2 )
		{
			root->tlf = newOctree();
			root->tlf->data = newOctreeData();

			if( ( root->tlf->data->faces = ( Triangle ** )malloc( sizeof( Triangle * ) * i2 ) ) == NULL ) exit( TRUE );
		
			root->tlf->data->nFaces = i2;

			for ( i = 0; i < i2; i ++ )
			{
				root->tlf->data->faces[i] = f2[i];

				aabb_add_3D( root->tlf->data->aabb, f2[i]->vertex[0]->position );
				aabb_add_3D( root->tlf->data->aabb, f2[i]->vertex[1]->position );
				aabb_add_3D( root->tlf->data->aabb, f2[i]->vertex[2]->position );
			}
		}

		if ( i3 )
		{
			root->trf = newOctree();
			root->trf->data = newOctreeData();

			if( ( root->trf->data->faces = ( Triangle ** )malloc( sizeof( Triangle * ) * i3 ) ) == NULL ) exit( TRUE );
		
			root->trf->data->nFaces = i3;

			for ( i = 0; i < i3; i ++ )
			{
				root->trf->data->faces[i] = f3[i];

				aabb_add_3D( root->trf->data->aabb, f3[i]->vertex[0]->position );
				aabb_add_3D( root->trf->data->aabb, f3[i]->vertex[1]->position );
				aabb_add_3D( root->trf->data->aabb, f3[i]->vertex[2]->position );
			}
		}

		if ( i4 )
		{
			root->blb = newOctree();
			root->blb->data = newOctreeData();

			if( ( root->blb->data->faces = ( Triangle ** )malloc( sizeof( Triangle * ) * i4 ) ) == NULL ) exit( TRUE );
		
			root->blb->data->nFaces = i4;

			for ( i = 0; i < i4; i ++ )
			{
				root->blb->data->faces[i] = f4[i];

				aabb_add_3D( root->blb->data->aabb, f4[i]->vertex[0]->position );
				aabb_add_3D( root->blb->data->aabb, f4[i]->vertex[1]->position );
				aabb_add_3D( root->blb->data->aabb, f4[i]->vertex[2]->position );
			}
		}

		if ( i5 )
		{
			root->brb = newOctree();
			root->brb->data = newOctreeData();

			if( ( root->brb->data->faces = ( Triangle ** )malloc( sizeof( Triangle * ) * i5 ) ) == NULL ) exit( TRUE );
		
			root->brb->data->nFaces = i5;

			for ( i = 0; i < i5; i ++ )
			{
				root->brb->data->faces[i] = f5[i];

				aabb_add_3D( root->brb->data->aabb, f5[i]->vertex[0]->position );
				aabb_add_3D( root->brb->data->aabb, f5[i]->vertex[1]->position );
				aabb_add_3D( root->brb->data->aabb, f5[i]->vertex[2]->position );
			}
		}

		if ( i6 )
		{
			root->blf = newOctree();
			root->blf->data = newOctreeData();

			if( ( root->blf->data->faces = ( Triangle ** )malloc( sizeof( Triangle * ) * i6 ) ) == NULL ) exit( TRUE );
		
			root->blf->data->nFaces = i6;

			for ( i = 0; i < i6; i ++ )
			{
				root->blf->data->faces[i] = f6[i];

				aabb_add_3D( root->blf->data->aabb, f6[i]->vertex[0]->position );
				aabb_add_3D( root->blf->data->aabb, f6[i]->vertex[1]->position );
				aabb_add_3D( root->blf->data->aabb, f6[i]->vertex[2]->position );
			}
		}

		if ( i7 )
		{
			root->brf = newOctree();
			root->brf->data = newOctreeData();

			if( ( root->brf->data->faces = ( Triangle ** )malloc( sizeof( Triangle * ) * i7 ) ) == NULL ) exit( TRUE );
		
			root->brf->data->nFaces = i7;

			for ( i = 0; i < i7; i ++ )
			{
				root->brf->data->faces[i] = f7[i];

				aabb_add_3D( root->brf->data->aabb, f7[i]->vertex[0]->position );
				aabb_add_3D( root->brf->data->aabb, f7[i]->vertex[1]->position );
				aabb_add_3D( root->brf->data->aabb, f7[i]->vertex[2]->position );
			}
		}

		free( f0 );
		free( f1 );
		free( f2 );
		free( f3 );
		free( f4 );
		free( f5 );
		free( f6 );
		free( f7 );

		buildOctree( root->tlb, maxdepth );
		buildOctree( root->trb, maxdepth );
		buildOctree( root->tlf, maxdepth );
		buildOctree( root->trf, maxdepth );
		buildOctree( root->blb, maxdepth );
		buildOctree( root->brb, maxdepth );
		buildOctree( root->blf, maxdepth );
		buildOctree( root->brf, maxdepth );
	}

	return TRUE;
}

#endif

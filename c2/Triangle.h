#ifndef __triangle_H
#define __triangle_H

#include "Vertex.h"
#include "Vector.h"
#include "Texture.h"
#include "Material.h"

//R
typedef struct Triangle
{
	int miplevel, fogEnable, lightEnable, depth, uvTransformed;

	DWORD render_mode;

	Texture * texture;

	Material * material;

	Vector3D * normal;

	Vertex * vertex[3];

	Vector * uv[3], *(* t_uv)[3],  * c_uv[3];

}Triangle;

INLINE Vector3D * triangle_normal( Vector3D * normal, Vertex * v0, Vertex * v1, Vertex * v2 )
{	
	Vector3D u, v;

	vector3D_crossProduct(normal, vector3D_subtract( &u, v0->position, v2->position ), vector3D_subtract( &v, v1->position, v0->position ) );

	vector3D_normalize(normal);

	return normal;
}

Triangle * newTriangle( Vertex * va, Vertex * vb, Vertex * vc, Vector * uva, Vector * uvb, Vector * uvc, Material * material, Texture * texture )
{
	Triangle * p;

	if( ( p = ( Triangle * )malloc( sizeof( Triangle ) ) ) == NULL ) exit( TRUE );

	p->vertex[0] = va;
	p->vertex[1] = vb;
	p->vertex[2] = vc;

	p->uv[0] = uva;
	p->uv[1] = uvb;
	p->uv[2] = uvc;

	p->c_uv[0] = vector_clone( uva );
	p->c_uv[1] = vector_clone( uvb );
	p->c_uv[2] = vector_clone( uvc );

	vertex_addContectedFaces( p, va );
	vertex_addContectedFaces( p, vb );
	vertex_addContectedFaces( p, vc );

	p->texture = texture;
	p->material = material;

	p->miplevel = 0;
	p->render_mode = RENDER_NONE;
	p->uvTransformed = FALSE;
	p->fogEnable = FALSE;
	p->lightEnable = FALSE;
	p->depth = 0;

	return p;
}

INLINE void triangle_copy( Triangle * dest, Triangle * src )
{
	vertex_copy( dest->vertex[0], src->vertex[0] );
	vertex_copy( dest->vertex[1], src->vertex[1] );
	vertex_copy( dest->vertex[2], src->vertex[2] );

	vector_copy( dest->uv[0], src->uv[0] );
	vector_copy( dest->uv[1], src->uv[1] );
	vector_copy( dest->uv[2], src->uv[2] );

	vector_copy( dest->c_uv[0], src->c_uv[0] );
	vector_copy( dest->c_uv[1], src->c_uv[1] );
	vector_copy( dest->c_uv[2], src->c_uv[2] );

	vector3D_copy( dest->normal, src->normal );

	dest->texture = src->texture;
	dest->material = src->material;
	dest->render_mode = src->render_mode;
	dest->miplevel = src->miplevel;
	dest->uvTransformed = src->uvTransformed;
	dest->depth = 0;
}

INLINE Triangle * triangle_clone( Triangle * src )
{
	Triangle * dest;

	if( ( dest = ( Triangle * )malloc( sizeof( Triangle ) ) ) == NULL ) exit( TRUE );

	dest->vertex[0] = vertex_clone( src->vertex[0] );
	dest->vertex[1] = vertex_clone( src->vertex[1] );
	dest->vertex[2] = vertex_clone( src->vertex[2] );

	dest->uv[0] = vector_clone( src->uv[0] );
	dest->uv[1] = vector_clone( src->uv[1] );
	dest->uv[2] = vector_clone( src->uv[2] );

	dest->c_uv[0] = vector_clone( src->c_uv[0] );
	dest->c_uv[1] = vector_clone( src->c_uv[1] );
	dest->c_uv[2] = vector_clone( src->c_uv[2] );

	vertex_addContectedFaces( dest, dest->vertex[0] );
	vertex_addContectedFaces( dest, dest->vertex[1] );
	vertex_addContectedFaces( dest, dest->vertex[2] );

	dest->texture = src->texture;
	dest->material = src->material;
	dest->render_mode = src->render_mode;
	dest->miplevel = src->miplevel;
	dest->uvTransformed = src->uvTransformed;
	dest->depth = 0;

	dest->normal = vector3D_clone( src->normal );

	return dest;
}

INLINE void triangle_dispose( Triangle * p )
{
	BYTE tmiplevels = logbase2ofx[p->texture->mipmaps[0]->width] + 1;

	memset( p->normal, 0, sizeof( Vector3D ) );
	memset( p->t_uv[0][0], 0, sizeof( Vector ) * tmiplevels * 3 );
	memset( p->t_uv, 0, sizeof( Vector * ) * tmiplevels * 3 );
	memset( p->c_uv, 0, sizeof( Vector ) );
	memset( p, 0, sizeof( Triangle ) );
	
	free( p->normal );
	free( p->t_uv[0][0] );
	free( p->t_uv );
	free( p->c_uv );
	free( p );
}

INLINE int triangle_isOnSameSide( float pX, float pY, float pAX, float pAY, float pBX, float pBY, float pCX, float pCY )
{
	Vector ba, pa, ca;

	ba.x = pBX - pAX;
	ba.y = pBY - pAY;

	pa.x = pX  - pAX;
	pa.y = pY  - pAY;

	ca.x = pCX - pAX;
	ca.y = pCY - pAY;

	return vector_crossProduct( & ba, & pa ) * vector_crossProduct( & ca, & pa ) > 0;
}

INLINE int triangle_hitTestPoint2D( Triangle * triangle, float x, float y )
{
	return ! ( triangle_isOnSameSide( 
		x, y, 
		triangle -> vertex[0] -> v_pos -> x, 
		triangle -> vertex[0] -> v_pos -> y, 
		triangle -> vertex[1] -> v_pos -> x,
		triangle -> vertex[1] -> v_pos -> y,
		triangle -> vertex[2] -> v_pos -> x,
		triangle -> vertex[2] -> v_pos -> y )
		||     triangle_isOnSameSide( 
		x, y, 
		triangle -> vertex[1] -> v_pos -> x, 
		triangle -> vertex[1] -> v_pos -> y, 
		triangle -> vertex[2] -> v_pos -> x,
		triangle -> vertex[2] -> v_pos -> y,
		triangle -> vertex[0] -> v_pos -> x,
		triangle -> vertex[0] -> v_pos -> y )
		||     triangle_isOnSameSide( 
		x, y, 
		triangle -> vertex[2] -> v_pos -> x, 
		triangle -> vertex[2] -> v_pos -> y, 
		triangle -> vertex[0] -> v_pos -> x,
		triangle -> vertex[0] -> v_pos -> y,
		triangle -> vertex[1] -> v_pos -> x,
		triangle -> vertex[1] -> v_pos -> y ) );
}

INLINE void triangle_transform( Matrix4x4 * world, Matrix4x4 * projection, Triangle * face )
{
	Vertex * vert;

	if ( face->vertex[0]->transformed == 0 )
	{
		face->vertex[0]->transformed = 1;

		vert = face->vertex[0];

		//matrix4x4_transformVector( vert->w_pos, world, vert->position );

		vert->v_pos->x = projection->m11 * vert->position->x + projection->m21 * vert->position->y + projection->m31 * vert->position->z + projection->m41;
		vert->v_pos->y = projection->m12 * vert->position->x + projection->m22 * vert->position->y + projection->m32 * vert->position->z + projection->m42;
		vert->v_pos->w = projection->m14 * vert->position->x + projection->m24 * vert->position->y + projection->m34 * vert->position->z + projection->m44;

		vert->s_pos->x = vert->v_pos->x / vert->v_pos->w;
		vert->s_pos->y = vert->v_pos->y / vert->v_pos->w;
	}

	if ( face->vertex[1]->transformed == 0 )
	{
		face->vertex[1]->transformed = 1;

		vert = face->vertex[1];

		//matrix4x4_transformVector( vert->w_pos, world, vert->position );

		vert->v_pos->x = projection->m11 * vert->position->x + projection->m21 * vert->position->y + projection->m31 * vert->position->z + projection->m41;
		vert->v_pos->y = projection->m12 * vert->position->x + projection->m22 * vert->position->y + projection->m32 * vert->position->z + projection->m42;
		vert->v_pos->w = projection->m14 * vert->position->x + projection->m24 * vert->position->y + projection->m34 * vert->position->z + projection->m44;

		vert->s_pos->x = vert->v_pos->x / vert->v_pos->w;
		vert->s_pos->y = vert->v_pos->y / vert->v_pos->w;
	}

	if ( face->vertex[2]->transformed == 0 )
	{
		face->vertex[2]->transformed = 1;

		vert = face->vertex[2];

		//matrix4x4_transformVector( vert->w_pos, world, vert->position );

		vert->v_pos->x = projection->m11 * vert->position->x + projection->m21 * vert->position->y + projection->m31 * vert->position->z + projection->m41;
		vert->v_pos->y = projection->m12 * vert->position->x + projection->m22 * vert->position->y + projection->m32 * vert->position->z + projection->m42;
		vert->v_pos->w = projection->m14 * vert->position->x + projection->m24 * vert->position->y + projection->m34 * vert->position->z + projection->m44;

		vert->s_pos->x = vert->v_pos->x / vert->v_pos->w;
		vert->s_pos->y = vert->v_pos->y / vert->v_pos->w;
	}
}

void triangle_setUV( Triangle * face, int miplevels )
{
	int i, texWidth, texHeight;

	for ( i = 0; i < miplevels; i ++ )
	{
		texWidth = face->texture->mipmaps[i]->width;
		texHeight = face->texture->mipmaps[i]->height;

		texWidth--;
		texHeight--;

		face->t_uv[i][0]->u = (face->uv[0]->u * texWidth);
		face->t_uv[i][0]->v = (face->uv[0]->v * texHeight);

		face->t_uv[i][1]->u = (face->uv[1]->u * texWidth);
		face->t_uv[i][1]->v = (face->uv[1]->v * texHeight);

		face->t_uv[i][2]->u = (face->uv[2]->u * texWidth);
		face->t_uv[i][2]->v = (face->uv[2]->v * texHeight);
	}

	/*if( addressMode == ADDRESS_MODE_CLAMP )
	{
		face->t_uv[0]->u = ( face->uv[0]->u > 1 ? 1 : face->uv[0]->u ) * texWidth ;
		face->t_uv[0]->v = ( face->uv[0]->v > 1 ? 1 : face->uv[0]->v ) * texHeight;

		face->t_uv[1]->u = ( face->uv[1]->u > 1 ? 1 : face->uv[1]->u ) * texWidth ;
		face->t_uv[1]->v = ( face->uv[1]->v > 1 ? 1 : face->uv[1]->v ) * texHeight;

		face->t_uv[2]->u = ( face->uv[2]->u > 1 ? 1 : face->uv[2]->u ) * texWidth ;
		face->t_uv[2]->v = ( face->uv[2]->v > 1 ? 1 : face->uv[2]->v ) * texHeight;
	}
	else
	{
		int u0 = ( int )( face->uv[0]->u ), v0 = ( int )( face->uv[0]->v ),
			u1 = ( int )( face->uv[1]->u ), v1 = ( int )( face->uv[1]->v ),
			u2 = ( int )( face->uv[2]->u ), v2 = ( int )( face->uv[2]->v ),
			maxU,maxV;

		float tu0 = face->uv[0]->u - u0, tv0 = face->uv[0]->v - v0,
			  tu1 = face->uv[1]->u - u1, tv1 = face->uv[1]->v - v1,
			  tu2 = face->uv[2]->u - u2, tv2 = face->uv[2]->v - v2;

		maxU = face->uv[0]->u > face->uv[1]->u ? ( face->uv[0]->u > face->uv[2]->u ? u0 : u2 ) : ( face->uv[2]->u > face->uv[1]->u ? u2 : u1 );
		maxV = face->uv[0]->v > face->uv[1]->v ? ( face->uv[0]->v > face->uv[2]->v ? v0 : v2 ) : ( face->uv[2]->v > face->uv[1]->v ? v2 : v1 );
	
		tu0 = ( tu0 == 0 ) ? ( u0 == maxU ? 1 : 0 ) : tu0, tv0 = ( tv0 == 0 ) ? ( v0 == maxV ? 1 : 0 ) : tv0,
		tu1 = ( tu1 == 0 ) ? ( u1 == maxU ? 1 : 0 ) : tu1, tv1 = ( tv1 == 0 ) ? ( v1 == maxV ? 1 : 0 ) : tv1,
		tu2 = ( tu2 == 0 ) ? ( u2 == maxU ? 1 : 0 ) : tu2, tv2 = ( tv2 == 0 ) ? ( v2 == maxV ? 1 : 0 ) : tv2;

		if( addressMode == ADDRESS_MODE_MIRROR )
		{
			int bu0 = u0 % 2, bu1 = u1 % 2, bu2 = u2 % 2,
				bv0 = v0 % 2, bv1 = v1 % 2, bv2 = v2 % 2,

				nu0 = tu0 == 1 && maxU == u0,
				nv0 = tv0 == 1 && maxV == v0,

				nu1 = tu1 == 1 && maxU == u1,
				nv1 = tv1 == 1 && maxV == v1,

				nu2 = tu2 == 1 && maxU == u2,
				nv2 = tv2 == 1 && maxV == v2;

			face->t_uv[0]->u = ( ( bu0 && ! nu0 ) || ( ! bu0 && nu0 ) ? ( 1 - tu0 ) : tu0 ) * texWidth ;
			face->t_uv[0]->v = ( ( bv0 && ! nv0 ) || ( ! bv0 && nv0 ) ? ( 1 - tv0 ) : tv0 ) * texHeight;

			face->t_uv[1]->u = ( ( bu1 && ! nu1 ) || ( ! bu1 && nu1 ) ? ( 1 - tu1 ) : tu1 ) * texWidth ;
			face->t_uv[1]->v = ( ( bv1 && ! nv1 ) || ( ! bv1 && nv1 ) ? ( 1 - tv1 ) : tv1 ) * texHeight;

			face->t_uv[2]->u = ( ( bu2 && ! nu2 ) || ( ! bu2 && nu2 ) ? ( 1 - tu2 ) : tu2 ) * texWidth ;
			face->t_uv[2]->v = ( ( bv2 && ! nv2 ) || ( ! bv2 && nv2 ) ? ( 1 - tv2 ) : tv2 ) * texHeight;
		}
		else
		{
			face->t_uv[0]->u = tu0 * texWidth ;
			face->t_uv[0]->v = tv0 * texHeight;

			face->t_uv[1]->u = tu1 * texWidth ;
			face->t_uv[1]->v = tv1 * texHeight;

			face->t_uv[2]->u = tu2 * texWidth ;
			face->t_uv[2]->v = tv2 * texHeight;
		}
	}*/
}

void triangle_transformUV( Triangle * face, TexTransform * transformation, int miplevels, int addressMode )
{
	int i, texWidth, texHeight;
	float x, y;

	Matrix3x3 * m = transformation->transform;

	switch ( addressMode )
	{
		case ADDRESS_MODE_WRAP:

			for ( i = 0; i < miplevels; i ++ )
			{
				texWidth = face->texture->mipmaps[i]->width;
				texHeight = face->texture->mipmaps[i]->height;

				texWidth--;
				texHeight--;

				face->t_uv[i][0]->u = (face->uv[0]->u * texWidth);
				face->t_uv[i][0]->v = (face->uv[0]->v * texHeight);

				x = (m->m11 * face->t_uv[i][0]->u + m->m21 * face->t_uv[i][0]->v + m->m31);
				y = (m->m12 * face->t_uv[i][0]->u + m->m22 * face->t_uv[i][0]->v + m->m32);

				face->t_uv[i][0]->u = x;
				face->t_uv[i][0]->v = y;

				face->t_uv[i][1]->u = (face->uv[1]->u * texWidth);
				face->t_uv[i][1]->v = (face->uv[1]->v * texHeight);
				
				x = (m->m11 * face->t_uv[i][1]->u + m->m21 * face->t_uv[i][1]->v + m->m31);
				y = (m->m12 * face->t_uv[i][1]->u + m->m22 * face->t_uv[i][1]->v + m->m32);

				face->t_uv[i][1]->u = x;
				face->t_uv[i][1]->v = y;

				face->t_uv[i][2]->u = (face->uv[2]->u * texWidth);
				face->t_uv[i][2]->v = (face->uv[2]->v * texHeight);
				
				x = (m->m11 * face->t_uv[i][2]->u + m->m21 * face->t_uv[i][2]->v + m->m31);
				y = (m->m12 * face->t_uv[i][2]->u + m->m22 * face->t_uv[i][2]->v + m->m32);

				face->t_uv[i][2]->u = x;
				face->t_uv[i][2]->v = y;
			}

			break;
	}
}

void triangle_createMipUVChain( Triangle * face, int tmiplevels )
{
	int i, j, k = 0;

	Vector * tmp;

	if ( ( tmp = ( Vector * )calloc( tmiplevels * 3, sizeof( Vector ) ) ) == NULL ) exit( TRUE );
	if ( ( face->t_uv = ( Vector *(*)[3] )calloc( tmiplevels, sizeof( Vector * ) * 3 ) ) == NULL ) exit( TRUE );

	for ( j = 0; j < tmiplevels; j ++ )
	{
		for ( i = 0; i < 3; i ++ )
		{
			face->t_uv[j][i] = tmp + k;

			k ++;
		}
	}
}

INLINE int triangle_backFaceCulling( Triangle * face, Vector3D * viewerToLocal, Vector3D * viewerPosition  )
{
	vector3D_subtract( viewerToLocal, viewerPosition, face->vertex[0]->position );

	vector3D_normalize( viewerToLocal );

	return ( vector3D_dotProduct( viewerToLocal, face->normal ) < 0.0f );
}

#endif
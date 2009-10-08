#ifndef __triangle_H
#define __triangle_H

#include "Vertex.h"
#include "Vector.h"
#include "Texture.h"
#include "Material.h"

//R
typedef struct Triangle
{
	int miplevel;

	DWORD render_mode;

	int uvTransformed;

	Texture * texture;

	Material * material;

	Vector3D * normal;

	Vertex * vertex[3];

	Vector * uv[3], * t_uv[3];

}Triangle;

INLINE Vector3D * triangle_normal( Vector3D * normal, Vertex * v0, Vertex * v1, Vertex * v2 )
{	
	Vector3D u, v;

	vector3D_crossProduct(normal, vector3D_subtract( &u, v1->position, v0->position ), vector3D_subtract( &v, v2->position, v0->position ) );

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

	p->t_uv[0] = vector_clone( uva );;
	p->t_uv[1] = vector_clone( uvb );;
	p->t_uv[2] = vector_clone( uvc );;

	vertex_addContectedFaces( p, va );
	vertex_addContectedFaces( p, vb );
	vertex_addContectedFaces( p, vc );

	p->texture = texture;
	p->material = material;

	p->miplevel = 0;
	p->render_mode = RENDER_NONE;
	p->uvTransformed = FALSE;

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

	vector_copy( dest->t_uv[0], src->t_uv[0] );
	vector_copy( dest->t_uv[1], src->t_uv[1] );
	vector_copy( dest->t_uv[2], src->t_uv[2] );

	vector3D_copy( dest->normal, src->normal );

	dest->texture = src->texture;
	dest->material = src->material;
	dest->render_mode = src->render_mode;
	dest->miplevel = src->miplevel;
	dest->uvTransformed = src->uvTransformed;
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

	dest->t_uv[0] = vector_clone( src->t_uv[0] );
	dest->t_uv[1] = vector_clone( src->t_uv[1] );
	dest->t_uv[2] = vector_clone( src->t_uv[2] );

	vertex_addContectedFaces( dest, dest->vertex[0] );
	vertex_addContectedFaces( dest, dest->vertex[1] );
	vertex_addContectedFaces( dest, dest->vertex[2] );

	dest->texture = src->texture;
	dest->material = src->material;
	dest->render_mode = src->render_mode;
	dest->miplevel = src->miplevel;
	dest->uvTransformed = src->uvTransformed;

	dest->normal = vector3D_clone( src->normal );

	return dest;
}

INLINE void triangle_dispose( Triangle * p)
{
	free( p->normal );
	free( p->t_uv );

	memset( p, 0, sizeof( Triangle ) );
}

INLINE void triangle_transform( Matrix3D * world, Matrix3D * projection, Triangle * face )
{
	Vertex * vert;

	if ( face->vertex[0]->transformed == 0 )
	{
		face->vertex[0]->transformed = 1;

		vert = face->vertex[0];

		matrix3D_transformVector( vert->w_pos, world, vert->position );

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

		matrix3D_transformVector( vert->w_pos, world, vert->position );

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

		matrix3D_transformVector( vert->w_pos, world, vert->position );

		vert->v_pos->x = projection->m11 * vert->position->x + projection->m21 * vert->position->y + projection->m31 * vert->position->z + projection->m41;
		vert->v_pos->y = projection->m12 * vert->position->x + projection->m22 * vert->position->y + projection->m32 * vert->position->z + projection->m42;
		vert->v_pos->w = projection->m14 * vert->position->x + projection->m24 * vert->position->y + projection->m34 * vert->position->z + projection->m44;

		vert->s_pos->x = vert->v_pos->x / vert->v_pos->w;
		vert->s_pos->y = vert->v_pos->y / vert->v_pos->w;
	}
}

INLINE void triangle_setUV( Triangle * face, int texWidth, int texHeight )
{
	texWidth--;

	texHeight--;

	face->t_uv[0]->u = face->uv[0]->u * texWidth;
	face->t_uv[0]->v = face->uv[0]->v * texHeight;

	face->t_uv[1]->u = face->uv[1]->u * texWidth;
	face->t_uv[1]->v = face->uv[1]->v * texHeight;

	face->t_uv[2]->u = face->uv[2]->u * texWidth;
	face->t_uv[2]->v = face->uv[2]->v * texHeight;
}

#endif
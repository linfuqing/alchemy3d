#ifndef __MESH_H
#define __MESH_H

#include "Triangle.h"
#include "Vertex.h"
#include "Vector.h"
#include "AABB.h"
#include "Material.h"
#include "Texture.h"

struct Animation;

typedef struct Mesh
{
	int nFaces, nVertices, v_dirty, f_dirty, textureReady, lightEnable;

	Triangle  * faces;

	Vertex * vertices;

	AABB * aabb, * worldAABB, * CVVAABB;

	struct Animation * animation;

}Mesh;


void mesh_build( Mesh * m, int nVertices, int nFaces, float * meshBuffer  )
{
	if( ( m->faces		= ( Triangle * )calloc( nFaces, sizeof( Triangle ) ) ) == NULL ) exit( TRUE );
	if( ( m->vertices	= ( Vertex * )calloc( nVertices, sizeof( Vertex ) ) ) == NULL ) exit( TRUE );
}

Mesh * newMesh( int nVertices, int nFaces, float * meshBuffer )
{
	Mesh * m;

	if( ( m	= ( Mesh * )calloc( 1, sizeof( Mesh ) ) ) == NULL) exit( TRUE );

	m->nFaces			= 0;
	m->nVertices		= 0;

	if( nVertices && nFaces )
	{
		mesh_build( m, nVertices, nFaces, meshBuffer );
	}

	m->aabb				= newAABB();
	m->worldAABB		= newAABB();
	m->CVVAABB			= newAABB();

	m->v_dirty			= TRUE;
	m->f_dirty			= FALSE;
	m->textureReady		= FALSE;
	m->lightEnable		= FALSE;

	m->animation        = NULL;

	return m;
}

Vertex * mesh_push_vertex( Mesh * m, float x, float y, float z )
{
	Vertex * v = & m->vertices[m->nVertices];

	v->position = newVector3D(x, y, z, 1.0f);
	v->w_pos = newVector3D(x, y, z, 1.0f);
	v->s_pos = newVector3D(x, y, z, 1.0f);

	v->color = newARGBColor( 255, 255, 255, 255 );

	v->normal = newVector3D( 0.0f, 0.0f, 0.0f, 1.0f );

	v->contectedFaces = NULL;
	v->nContectedFaces = 0;

	v->transformed = FALSE;

	m->nVertices ++;

	return v;
}

Triangle * mesh_push_triangle( Mesh * m, Vertex * va, Vertex * vb, Vertex * vc, Vector * uva, Vector * uvb, Vector * uvc, Material * material, Texture * texture, int render_mode )
{
	Triangle * p = & m->faces[m->nFaces];

	p->vertex[0] = va;
	p->vertex[1] = vb;
	p->vertex[2] = vc;

	p->uv[0] = vector_clone( uva );
	p->uv[1] = vector_clone( uvb );
	p->uv[2] = vector_clone( uvc );

	p->center = newVector3D( 0.0f, 0.0f, 0.0f, 1.0f );
	p->normal = newVector3D( 0.0f, 0.0f, 0.0f, 1.0f );

	p->texture = texture;
	p->material = material;

	p->render_mode = render_mode;

	vertex_addContectedFaces( p, va );
	vertex_addContectedFaces( p, vb );
	vertex_addContectedFaces( p, vc );

	m->nFaces ++;

	return p;
}

void computeFaceNormal( Mesh * m )
{
	int i = 0;

	Triangle * face;
	Vertex * v0, * v1, * v2;

	if ( ! m->faces || m->nFaces == 0) exit( TRUE );

	for( ; i < m->nFaces; i ++ )
	{
		face = & m->faces[i];

		v0 = face->vertex[0];
		v1 = face->vertex[1];
		v2 = face->vertex[2];

		face->center->x = ( v0->position->x + v1->position->x + v2->position->x ) * 0.33333333f;
		face->center->y = ( v0->position->y + v1->position->y + v2->position->y ) * 0.33333333f;
		face->center->z = ( v0->position->z + v1->position->z + v2->position->z ) * 0.33333333f;
		face->center->w = 1.0f;

		face->normal->x = 0.0f;
		face->normal->y = 0.0f;
		face->normal->z = 0.0f;
		face->normal->w = 1.0f;

		triangle_normal( face->normal, face->vertex[0], face->vertex[1], face->vertex[2] );
	}
}

void computeVerticesNormal( Mesh * m )
{
	int i = 0;
	float len, oneOverMag;
	ContectedFaces * cf;
	Vertex * vert;

	if ( ! m->vertices || m->nVertices == 0) exit( TRUE );

	//为提高效率，这么不使用任何内联函数，以降低函数调用开销
	for( ; i < m->nVertices; i ++ )
	{
		vert = & m->vertices[i];

		vert->transformed = FALSE;

		//重新计算AABB
		aabb_add( m->aabb, vert->position );

		if ( vert->nContectedFaces == 0 ) continue;

		vert->normal->x = 0.0f;
		vert->normal->y = 0.0f;
		vert->normal->z = 0.0f;
		vert->normal->w = 1.0f;

		//遍历关联面
		cf = m->vertices[i].contectedFaces;

		while ( NULL != cf )
		{
			vector3D_add_self( vert->normal, cf->face->normal );

			cf = cf->next;
		}

		len = sqrtf( vert->normal->x * vert->normal->x + vert->normal->y * vert->normal->y + vert->normal->z * vert->normal->z );

		oneOverMag = 1.0f / len;

		if ( !len ) vert->normal->x = vert->normal->y = vert->normal->z = 0.0f;
		else
		{
			vert->normal->x *= oneOverMag;
			vert->normal->y *= oneOverMag;
			vert->normal->z *= oneOverMag;
		}
	}
}

void mesh_setRenderMode(  Mesh * m, DWORD renderMode )
{
	int i = 0;

	for( ; i < m->nFaces; i ++ )
	{
		m->faces[i].render_mode = renderMode;
	}
}

void mesh_setMaterial( Mesh * m, Material * mat )
{
	int i = 0;

	for( ; i < m->nFaces; i ++ )
	{
		m->faces[i].material = mat;
	}
}

void mesh_computeUV( Mesh * m )
{
	int i = 0, j = 0;

	Texture * t;

	Triangle * face;

	for( i = 0; i < m->nFaces; i ++ )
	{
		face =  & m->faces[i];

		t = face->texture;

		if ( ! face->uvTransformed && t->pRGBABuffer && t->width != 0 && t->height != 0 )
		{
			face->uv[0]->u *= t->width - 1;
			face->uv[0]->v *= t->height - 1;

			face->uv[1]->u *= t->width - 1;
			face->uv[1]->v *= t->height - 1;

			face->uv[2]->u *= t->width - 1;
			face->uv[2]->v *= t->height - 1;

			face->uvTransformed = TRUE;
		}

		if ( face->uvTransformed ) j ++;
	}

	if ( i == j )
	{
		m->textureReady = TRUE;
	}
}

void mesh_setTexture( Mesh * m, Texture * t )
{
	int i = 0;

	for( ; i < m->nFaces; i ++ )
	{
		m->faces[i].texture = t;
	}
}

void mesh_updateMesh( Mesh * mesh )
{	
//#ifdef __AS3__
//	int j = 0, k = 0;
//	float * meshBuffer;
//	DWORD * p_meshBuffer;
//	Triangle * face;
//	Vertex * vs;
//
//	meshBuffer = mesh->meshBuffer;
//
//	if ( meshBuffer && mesh->f_dirty )
//	{
//		p_meshBuffer = ( DWORD * )(meshBuffer + mesh->nVertices * VERTEX_SIZE);
//
//		for( j = 0, k = 0; j < mesh->nFaces; j ++, k += FACE_SIZE)
//		{
//			face = & mesh->faces[j];
//
//			face->render_mode = ( DWORD )p_meshBuffer[k + 9];
//			face->material = ( Material * ) p_meshBuffer[k + 10];
//			face->texture = ( Texture * )p_meshBuffer[k + 11];
//		}
//	}
//
//	if ( meshBuffer && mesh->v_dirty )
//	{
//		for( j = 0, k = 0; j < mesh->nVertices; j ++, k += VERTEX_SIZE)
//		{
//			vs = & mesh->vertices[j];
//
//			//如果顶点局部坐标发生改变
//			vs->position->x = meshBuffer[k];
//			vs->position->y = meshBuffer[k + 1];
//			vs->position->z = meshBuffer[k + 2];
//		}
//	}
//#endif

	if ( mesh->v_dirty )
	{
		computeFaceNormal( mesh );
		computeVerticesNormal( mesh );
	}
	else
	{
		int i = 0;

		for( ; i < mesh->nVertices; i ++ )
		{
			mesh->vertices[i].transformed = FALSE;
		}
	}

	if ( ! mesh->textureReady ) mesh_computeUV( mesh );
}

INLINE AABB * mesh_transformNewAABB( AABB * output, Matrix3D * m, AABB * aabb )
{
	float invw;

	Vector3D T_L_F_V;
	Vector3D T_R_F_V;
	Vector3D T_L_B_V;
	Vector3D T_R_B_V;
	Vector3D B_L_F_V;
	Vector3D B_R_F_V;
	Vector3D B_L_B_V;
	Vector3D B_R_B_V;

	Vector3D * min = aabb->min, * max = aabb->max;

	T_L_F_V.x = min->x;	T_L_F_V.y = min->y;	T_L_F_V.z = min->z;	T_L_F_V.w = 1;
	T_R_F_V.x = max->x;	T_R_F_V.y = min->y;	T_R_F_V.z = min->z;	T_R_F_V.w = 1;
	T_L_B_V.x = min->x;	T_L_B_V.y = min->y;	T_L_B_V.z = max->z;	T_L_B_V.w = 1;
	T_R_B_V.x = max->x;	T_R_B_V.y = min->y;	T_R_B_V.z = max->z;	T_R_B_V.w = 1;
	B_L_F_V.x = min->x;	B_L_F_V.y = max->y;	B_L_F_V.z = min->z;	B_L_F_V.w = 1;
	B_R_F_V.x = max->x;	B_R_F_V.y = max->y;	B_R_F_V.z = min->z;	B_R_F_V.w = 1;
	B_L_B_V.x = min->x;	B_L_B_V.y = max->y;	B_L_B_V.z = max->z;	B_L_B_V.w = 1;
	B_R_B_V.x = max->x;	B_R_B_V.y = max->y;	B_R_B_V.z = max->z;	B_R_B_V.w = 1;

	matrix3D_transformVector_self( m, & T_L_F_V );
	invw = 1.0f / T_L_F_V.w;
	T_L_F_V.x *= invw;
	T_L_F_V.y *= invw;

	matrix3D_transformVector_self( m, & T_R_F_V );
	invw = 1.0f / T_R_F_V.w;
	T_R_F_V.x *= invw;
	T_R_F_V.y *= invw;

	matrix3D_transformVector_self( m, & T_L_B_V );
	invw = 1.0f / T_L_B_V.w;
	T_L_B_V.x *= invw;
	T_L_B_V.y *= invw;

	matrix3D_transformVector_self( m, & T_R_B_V );
	invw = 1.0f / T_R_B_V.w;
	T_R_B_V.x *= invw;
	T_R_B_V.y *= invw;

	matrix3D_transformVector_self( m, & B_L_F_V );
	invw = 1.0f / B_L_F_V.w;
	B_L_F_V.x *= invw;
	B_L_F_V.y *= invw;

	matrix3D_transformVector_self( m, & B_R_F_V );
	invw = 1.0f / B_R_F_V.w;
	B_R_F_V.x *= invw;
	B_R_F_V.y *= invw;

	matrix3D_transformVector_self( m, & B_L_B_V );
	invw = 1.0f / B_L_B_V.w;
	B_L_B_V.x *= invw;
	B_L_B_V.y *= invw;

	matrix3D_transformVector_self( m, & B_R_B_V );
	invw = 1.0f / B_R_B_V.w;
	B_R_B_V.x *= invw;
	B_R_B_V.y *= invw;

	aabb_add_4D( output, & T_L_F_V );
	aabb_add_4D( output, & T_R_F_V );
	aabb_add_4D( output, & T_L_B_V );
	aabb_add_4D( output, & T_R_B_V );
	aabb_add_4D( output, & B_L_F_V );
	aabb_add_4D( output, & B_R_F_V );
	aabb_add_4D( output, & B_L_B_V );
	aabb_add_4D( output, & B_R_B_V );

	return output;
}

void mesh_clear( Mesh * mesh )
{
	int i = 0;

	for ( ; i < mesh->nVertices; i ++ )
	{
		vertex_dispose( & mesh->vertices[i] );
	}
	for ( i = 0; i < mesh->nFaces; i ++ )
	{
		triangle_dispose( & mesh->faces[i] );
	}

	free( mesh->vertices );
	free( mesh->faces );
}

Mesh * mesh_reBuild(  Mesh * m, int nVertices, int nFaces, float * meshBuffer )
{
	if( m == NULL )
	{
		m = newMesh( nVertices, nFaces, meshBuffer );
	}
	else if( ! ( m -> nVertices ) || ! ( m -> nFaces ) )
	{
		mesh_build( m, nVertices, nFaces, meshBuffer );
	}
	else
	{
		mesh_clear( m );
		mesh_build( m, nVertices, nFaces, meshBuffer );
	}

	return m;
}

void mesh_dispose( Mesh * mesh )
{
	aabb_dispose( mesh->aabb );
	aabb_dispose( mesh->worldAABB );
	aabb_dispose( mesh->CVVAABB );

	mesh_clear( mesh );

	free( mesh );
}

#endif

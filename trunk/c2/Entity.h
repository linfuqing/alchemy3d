#ifndef __ENTITY_H
#define __ENTITY_H

#include <malloc.h>

#include "Vector3D.h"
#include "Matrix3D.h"
#include "Quaternion.h"
#include "Mesh.h"
#include "Material.h"
#include "Math.h"
#include "Texture.h"
#include "AABB.h"

typedef struct Entity
{
	//RW
	int visible, transformDirty, nChildren;
	//N
	struct SceneNode * children;
	//N
	struct Entity * parent;
	//N
	struct Scene * scene;
	//RW
	Vector3D * position, * direction, * scale, * worldPosition;
	//R
	Matrix3D * transform, * world, * worldInvert;
	//R
	Mesh * mesh;

	Material * material;

	Texture * texture;

}Entity;

typedef struct SceneNode
{
	Entity * entity;

	struct SceneNode * next;
}SceneNode;

Entity * newEntity()
{
	Entity * entity;

	if( ( entity = ( Entity * )malloc( sizeof( Entity ) ) ) == NULL )
	{
		exit( TRUE );
	}

	entity->position		= newVector3D(0.0f, 0.0f, 0.0f, 1.0f);
	entity->direction		= newVector3D(0.0f, 0.0f, 0.0f, 1.0f);
	entity->scale			= newVector3D(1.0f, 1.0f, 1.0f, 1.0f);
	entity->worldPosition	= newVector3D(0.0f, 0.0f, 0.0f, 1.0f);

	entity->transform		= newMatrix3D(NULL);
	entity->world			= newMatrix3D(NULL);
	entity->worldInvert		= newMatrix3D(NULL);

	entity->children		= NULL;
	entity->parent			= NULL;
	entity->mesh			= NULL;
	entity->material		= NULL;
	entity->texture			= NULL;
	entity->scene			= NULL;

	entity->visible = entity->transformDirty = TRUE;

	entity->nChildren = 0;

	return entity;
}

SceneNode * entity_findChild( SceneNode * childNode, Entity * child )
{
	if ( childNode == NULL )
	{
		return NULL;
	}
	else
	{
		if ( child == childNode->next->entity )
		{
			return childNode;
		}
		else
		{
			return entity_findChild( childNode->next, child );
		}
	}
}

void entity_addChild( Entity * parent, Entity * child )
{
	SceneNode * sceneNode, * n;

	sceneNode = parent->children;

	if (NULL != sceneNode)
	{
		while( NULL != sceneNode->next )
		{
			sceneNode = sceneNode->next;
		}
	}

	if( ( n = ( SceneNode * )malloc( sizeof( SceneNode ) ) ) == NULL )
	{
		exit( TRUE );
	}

	n->entity = child;

	if (NULL == sceneNode)
	{
		n->next = NULL;

		parent->children = n;
	}
	else
	{
		n->next = sceneNode->next;

		sceneNode->next = n;
	}

	n->entity->parent = parent;

	parent->nChildren ++;
}

void entity_removeChild( Entity * entity, Entity * child )
{
	SceneNode * sceneNode, * s;

	sceneNode = entity->children;

	if ( sceneNode->entity == child )
	{
		entity->children = entity->children->next;
	}
	else
	{
		s = entity_findChild( sceneNode, child );

		s->next = s->next->next;
	}

	entity->nChildren --;

	free( child );

	child =NULL;
}

void entity_dispose( Entity * entity )
{
	vector3D_dispose( entity->position );
	vector3D_dispose( entity->worldPosition );
	vector3D_dispose( entity->direction );
	vector3D_dispose( entity->scale );
	matrix3D_dispose( entity->transform );
	matrix3D_dispose( entity->world );
	matrix3D_dispose( entity->worldInvert );
	free( entity );
}

void entity_setMesh( Entity * entity, Mesh * m )
{
	ContectedFaces * cf;
	Vector3D * nv, posN;
	float d;
	int i = 0;

	m->aabb			= newAABB();
	m->worldAABB	= newAABB();

	for( ; i < m->nVertices; i ++ )
	{
		//AABB
		aabb_add(m->aabb, m->vertices[i].position);

		if ( m->vertices[i].nContectedFaces == 0 )
			continue;

		nv = newVector3D( 0.0f, 0.0f, 0.0f, 1.0f );

		cf = m->vertices[i].contectedFaces;

		while ( NULL != cf )
		{
			vector3D_add( nv, nv, cf->face->normal );

			cf = cf->next;
		}

		vector3D_normalize( nv );

		vector3D_copy( m->vertices[i].normal, nv );

		vector3D_copy( & posN, m->vertices[i].position );

		d = 1.0f / m->vertices[i].nContectedFaces;

		vector3D_scaleBy( & posN, d );

		vector3D_normalize( & posN );

		vector3D_add( nv, nv, & posN );

		vector3D_normalize( nv );

		m->vertices[i].normal2 = nv;
	}

	aabb_copy( m->worldAABB, m->aabb );

	entity->mesh = m;
}

INLINE void entity_setMaterial( Entity * entity, Material * m )
{
	entity->material = m;
}

INLINE void entity_setTexture( Entity * entity, Texture * t )
{
	entity->texture = t;
}

static Matrix3D quaMtr;
static Quaternion qua;

INLINE void entity_updateTransform(Entity * entity)
{
	//单位化
	matrix3D_identity( entity->transform );
	//缩放
	matrix3D_appendScale( entity->transform, entity->scale->x, - entity->scale->y, entity->scale->z );
	//旋转
	matrix3D_append_self( entity->transform, quaternoin_toMatrix( &quaMtr, quaternoin_setFromEuler( &qua, DEG2RAD( entity->direction->y ), DEG2RAD( entity->direction->x ), DEG2RAD( entity->direction->z ) ) ) );
	//位移
	matrix3D_appendTranslation( entity->transform, entity->position->x, entity->position->y, entity->position->z );

	matrix3D_copy( entity->world, entity->transform );
	
	//如果存在父结点，则连接父结点世界矩阵
	if( NULL != entity->parent ) matrix3D_append_self(entity->world, entity->parent->world);

	matrix3D_copy( entity->worldInvert, entity->world );
	//世界逆矩阵
	matrix3D_invert( entity->worldInvert );
	//从世界矩阵获得世界位置
	matrix3D_getPosition( entity->worldPosition, entity->world );
}

INLINE void mesh_transformAABB( Entity * entity )
{
	AABB * aabb, * worldAabb;

	aabb = entity->mesh->aabb;
	worldAabb = entity->mesh->worldAABB;

	//用实体的世界矩阵变换它的AABB到世界坐标系
	matrix3D_transformVector( worldAabb->min, entity->world, aabb->min );
	matrix3D_transformVector( worldAabb->max, entity->world, aabb->max );

	//根据变换后的AABB重新构造新的AABB（这也就是实体经过旋转、位移、缩放后的新AABB）
	aabb_createFromSelf( & worldAabb );
}

/**
属性设置
**/
INLINE void entity_setX(Entity * entity, float value)
{
	entity->position->x = value;
}

INLINE float entity_getX(Entity * entity)
{
	return entity->position->x;
}

INLINE void entity_setY(Entity * entity, float value)
{
	entity->position->y = value;
}

INLINE float entity_getY(Entity * entity)
{
	return entity->position->y;
}

INLINE void entity_setZ(Entity * entity, float value)
{
	entity->position->z = value;
}

INLINE float entity_getZ(Entity * entity)
{
	return entity->position->z;
}

INLINE void entity_setScaleX(Entity * entity, float value)
{
	entity->scale->x = value;
}

INLINE float entity_getScaleX(Entity * entity)
{
	return entity->scale->x;
}

INLINE void entity_setScaleY(Entity * entity, float value)
{
	entity->scale->y = value;
}

INLINE float entity_getScaleY(Entity * entity)
{
	return entity->scale->y;
}

INLINE void entity_setScaleZ(Entity * entity, float value)
{
	entity->scale->z = value;
}

INLINE float entity_getScaleZ(Entity * entity)
{
	return entity->scale->z;
}

INLINE void entity_setRotationX(Entity * entity, float value)
{
	entity->direction->x = value;
}

INLINE float entity_getRotationX(Entity * entity)
{
	return entity->direction->x;
}

INLINE void entity_setRotationY(Entity * entity, float value)
{
	entity->direction->y = value;
}

INLINE float entity_getRotationY(Entity * entity)
{
	return entity->direction->y;
}

INLINE void entity_setRotationZ(Entity * entity, float value)
{
	entity->direction->z = value;
}

INLINE float entity_getRotationZ(Entity * entity)
{
	return entity->direction->z;
}

INLINE void entity_setPosition(Entity * entity, Vector3D * v)
{
	entity->position->x = v->x;
	entity->position->y = v->y;
	entity->position->z = v->z;
}

INLINE Vector3D * entity_getPosition(Vector3D * output, Entity * entity)
{
	vector3D_copy(output, entity->position);

	return output;
}

INLINE void entity_setScale(Entity * entity, Vector3D * v)
{
	entity->scale->x = v->x;
	entity->scale->y = v->y;
	entity->scale->z = v->z;
}

INLINE Vector3D * entity_getScale(Vector3D * output, Entity * entity)
{
	vector3D_copy(output, entity->scale);

	return output;
}

INLINE void entity_setRotation(Entity * entity, Vector3D * v)
{
	entity->direction->x = v->x;
	entity->direction->y = v->y;
	entity->direction->z = v->z;
}

INLINE Vector3D * entity_getRotation(Vector3D * output, Entity * entity)
{
	vector3D_copy(output, entity->direction);

	return output;
}

#endif
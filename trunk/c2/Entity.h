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

typedef struct Entity
{
	//RW
	int visible, transformDirty, offScreen;
	//N
	struct Entity * parent;
	//RW
	Vector3D * position, * direction, * scale, * worldPosition;
	//R
	Matrix3D * transform, * world, * worldInvert;
	//R
	Mesh * mesh;

	Material * material;

	Texture * texture;

}Entity;

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

	entity->parent			= NULL;
	entity->material		= NULL;
	entity->texture			= NULL;

	entity->visible = entity->transformDirty = entity->offScreen = TRUE;

	return entity;
}

void entity_setMesh( Entity * entity, Mesh * m )
{
	VertexNode * vNode;
	ContectedFaces * cf;
	Vector3D * nv;

	vNode = m->vertices->nodes;

	while ( NULL != vNode )
	{
		nv = newVector3D( 0.0f, 0.0f, 0.0f, 1.0f );

		cf = vNode->vertex->contectedFaces;

		while ( NULL != cf )
		{

			vector3D_add( nv, nv, cf->face->normal );

			cf = cf->next;
		}

		vector3D_normalize( nv );

		vNode->vertex->normal = nv;

		vNode = vNode->next;
	}

	entity->mesh = m;
}

void entity_setMaterial( Entity * entity, Material * m )
{
	struct VertexNode * vNode;

	vNode = entity->mesh->vertices->nodes;

	while ( NULL != vNode )
	{
		vNode->vertex->color = newColor( 0.0f, 0.0f, 0.0f, 0.0f );

		color_copy( vNode->vertex->color, m->ambient);

		vNode = vNode->next;
	}

	entity->material = m;
}

static Matrix3D quaMtr;
static Quaternion qua;

void entity_updateTransform(Entity * entity)
{
	//单位化
	matrix3D_identity( entity->transform );
	//缩放
	matrix3D_appendScale( entity->transform, entity->scale->x, entity->scale->y, entity->scale->z );
	//旋转
	matrix3D_append( entity->transform, quaternoin_toMatrix( &quaMtr, quaternoin_setFromEuler( &qua, DEG2RAD( - entity->direction->y ), DEG2RAD( entity->direction->x ), DEG2RAD( entity->direction->z ) ) ) );
	//位移
	matrix3D_appendTranslation( entity->transform, entity->position->x, - entity->position->y, entity->position->z );

	matrix3D_copy( entity->world, entity->transform );

	matrix3D_copy( entity->worldInvert, entity->world );
	//世界逆矩阵
	matrix3D_fastInvert( entity->worldInvert );

	matrix3D_getPosition( entity->worldPosition, entity->world );
}

/**
属性设置
**/
void entity_setX(Entity * entity, float value)
{
	entity->position->x = value;
}

float entity_getX(Entity * entity)
{
	return entity->position->x;
}

void entity_setY(Entity * entity, float value)
{
	entity->position->y = value;
}

float entity_getY(Entity * entity)
{
	return entity->position->y;
}

void entity_setZ(Entity * entity, float value)
{
	entity->position->z = value;
}

float entity_getZ(Entity * entity)
{
	return entity->position->z;
}

void entity_setScaleX(Entity * entity, float value)
{
	entity->scale->x = value;
}

float entity_getScaleX(Entity * entity)
{
	return entity->scale->x;
}

void entity_setScaleY(Entity * entity, float value)
{
	entity->scale->y = value;
}

float entity_getScaleY(Entity * entity)
{
	return entity->scale->y;
}

void entity_setScaleZ(Entity * entity, float value)
{
	entity->scale->z = value;
}

float entity_getScaleZ(Entity * entity)
{
	return entity->scale->z;
}

void entity_setRotationX(Entity * entity, float value)
{
	entity->direction->x = value;
}

float entity_getRotationX(Entity * entity)
{
	return entity->direction->x;
}

void entity_setRotationY(Entity * entity, float value)
{
	entity->direction->y = value;
}

float entity_getRotationY(Entity * entity)
{
	return entity->direction->y;
}

void entity_setRotationZ(Entity * entity, float value)
{
	entity->direction->z = value;
}

float entity_getRotationZ(Entity * entity)
{
	return entity->direction->z;
}

void entity_setPosition(Entity * entity, Vector3D * v)
{
	entity->position->x = v->x;
	entity->position->y = v->y;
	entity->position->z = v->z;
}

Vector3D * entity_getPosition(Vector3D * output, Entity * entity)
{
	vector3D_copy(output, entity->position);

	return output;
}

void entity_setScale(Entity * entity, Vector3D * v)
{
	entity->scale->x = v->x;
	entity->scale->y = v->y;
	entity->scale->z = v->z;
}

Vector3D * entity_getScale(Vector3D * output, Entity * entity)
{
	vector3D_copy(output, entity->scale);

	return output;
}

void entity_setRotation(Entity * entity, Vector3D * v)
{
	entity->direction->x = v->x;
	entity->direction->y = v->y;
	entity->direction->z = v->z;
}

Vector3D * entity_getRotation(Vector3D * output, Entity * entity)
{
	vector3D_copy(output, entity->direction);

	return output;
}

#endif
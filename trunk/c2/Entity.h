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
	VertexNode * vNode;
	ContectedFaces * cf;
	Vector3D * nv, posN;
	float d;

	vNode = m->vertices->nodes;

	while ( NULL != vNode && vNode->vertex->nContectedFaces != 0 )
	{
		nv = newVector3D( 0.0f, 0.0f, 0.0f, 1.0f );

		cf = vNode->vertex->contectedFaces;

		while ( NULL != cf )
		{
			vector3D_add( nv, nv, cf->face->normal );

			cf = cf->next;
		}

		vector3D_normalize( nv );

		vector3D_copy( vNode->vertex->normal, nv );

		vector3D_copy( & posN, vNode->vertex->position );

		d = 1.0f / vNode->vertex->nContectedFaces;

		vector3D_scaleBy( & posN, d );

		vector3D_normalize( & posN );

		vector3D_add( nv, nv, & posN );

		vector3D_normalize( nv );

		vNode->vertex->normal2 = nv;

		vNode = vNode->next;
	}

	entity->mesh = m;
}

void entity_setMaterial( Entity * entity, Material * m )
{
	/*struct VertexNode * vNode;

	vNode = entity->mesh->vertices->nodes;

	while ( NULL != vNode )
	{
		vNode->vertex->color = newColor( 0.0f, 0.0f, 0.0f, 0.0f );

		color_copy( vNode->vertex->color, m->ambient );

		vNode = vNode->next;
	}*/

	entity->material = m;
}

void entity_setTexture( Entity * entity, Texture * t )
{
	entity->texture = t;
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
	matrix3D_append_self( entity->transform, quaternoin_toMatrix( &quaMtr, quaternoin_setFromEuler( &qua, DEG2RAD( entity->direction->y ), DEG2RAD( entity->direction->x ), DEG2RAD( entity->direction->z ) ) ) );
	//位移
	matrix3D_appendTranslation( entity->transform, entity->position->x, entity->position->y, entity->position->z );

	matrix3D_copy( entity->world, entity->transform );

	matrix3D_copy( entity->worldInvert, entity->world );
	//世界逆矩阵
	matrix3D_invert( entity->worldInvert );
	//从世界矩阵获得世界位置
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
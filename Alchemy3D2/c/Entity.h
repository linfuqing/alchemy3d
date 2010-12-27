#ifndef __ENTITY_H
#define __ENTITY_H

#include <malloc.h>

#include "Vector3D.h"
#include "Matrix3D.h"
#include "Quaternion.h"
#include "Mesh.h"
#include "Math3D.h"
#include "AABB.h"
#include "Physics.h"
//#include "Animation.h"

struct Animation;
struct SkeletalChannel;
struct MorphChannel;

typedef struct Entity
{
	char * name;

	//是否可见，是否进行变换，子结点数
	int visible, transformDirty, nChildren, isBone, matrixDirty, terrainTrace;

	//BOOLEAN hit;

	//子结点
	struct SceneNode * children;

	//父结点
	struct Entity * parent;

	//场景
	struct Scene * scene;

	//局部坐标，朝向，缩放因子，世界坐标，相机坐标，裁剪空间坐标，视点局部坐标
	Vector3D * position, * direction, * scale, * w_pos, * s_pos, * CVVPosition, * viewerToLocal;

	//局部矩阵，世界矩阵，世界逆矩阵，相机矩阵，投影矩阵
	Matrix4x4 * transform, * world, * worldInvert, * view, * projection;

	AABB * aabb;
	AABB * worldAABB;

	//网格
	Mesh * mesh;

	Physics * physics;
	//动画
	struct Animation * animation;
	struct Animation * parentAnimation;
	struct SkeletalChannel * skeletalChannel;
	struct MorphChannel* morphChannel;
}Entity;

typedef struct SceneNode
{
	Entity * entity;

	int ID;

	struct SceneNode * next;

}SceneNode;

Entity * newEntity()
{
	Entity * entity;

	if( ( entity = ( Entity * )malloc( sizeof( Entity ) ) ) == NULL ) exit( TRUE );

	entity->position		= newVector3D(0.0f, 0.0f, 0.0f, 1.0f);
	entity->direction		= newVector3D(0.0f, 0.0f, 0.0f, 1.0f);
	entity->scale			= newVector3D(1.0f, 1.0f, 1.0f, 1.0f);
	entity->w_pos			= newVector3D(0.0f, 0.0f, 0.0f, 1.0f);
	entity->s_pos			= newVector3D(0.0f, 0.0f, 0.0f, 1.0f);
	entity->CVVPosition		= newVector3D(0.0f, 0.0f, 0.0f, 1.0f);
	entity->viewerToLocal	= newVector3D(0.0f, 0.0f, 0.0f, 1.0f);

	entity->transform		= newMatrix4x4(NULL);
	entity->world			= newMatrix4x4(NULL);
	entity->worldInvert		= newMatrix4x4(NULL);
	entity->view			= newMatrix4x4(NULL);
	entity->projection		= newMatrix4x4(NULL);

	entity->aabb            = newAABB();
	entity->worldAABB       = newAABB();

	entity->name			   = NULL;
	entity->children		   = NULL;
	entity->parent			   = NULL;
	entity->mesh		   	   = NULL;
	entity->scene			   = NULL;
	entity->physics            = NULL;
	entity->animation          = NULL;
	entity->parentAnimation    = NULL;
	entity->skeletalChannel    = NULL;
	entity->morphChannel       = NULL;

	//entity->hit             = FALSE;

	entity->visible = TRUE;

	entity->transformDirty = TRUE;

	entity->matrixDirty = TRUE;

	entity->isBone = FALSE;

	entity->terrainTrace = FALSE;

	entity->nChildren = 0;

	return entity;
}

/*
void entity_dirty(Entity* entity)
{
	if(!entity)
		return;

	if(ENTITY_TRANSFORM_DIRTY_INVERT == entity->transformDirty)
		matrix4x4_decompose(entity->transform, entity->position, entity->scale, entity->direction, NULL);

	entity->transformDirty = ENTITY_TRANSFORM_DIRTY_NORMAL;
}*/

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

/*void entity_removeChild( Entity * entity, Entity * child )
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
}*/

SceneNode* entity_getTreeTail(SceneNode* root)
{
	//if(root && root->next && root->next->entity->parent == root->entity)
	//	return entity_getTreeTail(root->next);

	SceneNode* node = root;

	if(!root)
		return NULL;

	while(node->next && node->next->entity->parent == root->entity)
		node = entity_getTreeTail(node->next);

	return node;
}

int entity_removeChild(SceneNode* previous, SceneNode* child)
{
	SceneNode* node = previous, *tail;

	if(!child)
		return FALSE;

	tail = entity_getTreeTail(child);

	if(previous != child)
	{
		while(node && node->next != child)
			node = node->next;

		if(node)
			node->next = tail->next;
	}

	tail->next = NULL;

	if(child->entity->parent)
		child->entity->parent->nChildren --;

	child->entity->parent = NULL;

	node = child;

	while(node)
	{
		node->entity->scene = NULL;

		node = node->next;
	}

	return TRUE;
}

void entity_dispose( Entity * entity )
{
	vector3D_dispose( entity->position );
	vector3D_dispose( entity->w_pos );
	vector3D_dispose( entity->s_pos );
	vector3D_dispose( entity->CVVPosition );
	vector3D_dispose( entity->viewerToLocal );
	vector3D_dispose( entity->direction );
	vector3D_dispose( entity->scale );
	matrix4x4_dispose( entity->transform );
	matrix4x4_dispose( entity->world );
	matrix4x4_dispose( entity->worldInvert );
	matrix4x4_dispose( entity->view );
	matrix4x4_dispose( entity->projection );

	memset( entity, 0, sizeof(Entity));
}

void entity_destroy(Entity **entity)
{
	vector3D_destroy(&(*entity)->position     );
	vector3D_destroy(&(*entity)->w_pos        );
	vector3D_destroy(&(*entity)->s_pos        );
	vector3D_destroy(&(*entity)->CVVPosition  );
	vector3D_destroy(&(*entity)->viewerToLocal);
	vector3D_destroy(&(*entity)->direction    );
	vector3D_destroy(&(*entity)->scale        );

	matrix4x4_destroy(&(*entity)->transform   );
	matrix4x4_destroy(&(*entity)->world       );
	matrix4x4_destroy(&(*entity)->worldInvert );
	matrix4x4_destroy(&(*entity)->view        );
	matrix4x4_destroy(&(*entity)->projection  );

	aabb_destroy(&(*entity)->aabb             );
	aabb_destroy(&(*entity)->worldAABB        );

	free(*entity);

	*entity = NULL;
}

void sceneNode_destroy(SceneNode** sceneNode)
{
	entity_destroy(&(*sceneNode)->entity);

	free(*sceneNode);

	*sceneNode = NULL;
}

INLINE void entity_setMesh( Entity * entity, Mesh * m )
{
	if ( ! m || ! m->vertices || ! m->faces || m->nFaces == 0 || m->nVertices == 0 ) return;

	entity->mesh = m;
}

/*INLINE void entity_updateAfterRender( Entity * entity )
{
	if ( entity->mesh )
	{
		entity->transformDirty = FALSE;
		entity->mesh->v_dirty = FALSE;
		entity->mesh->texTransformDirty = FALSE;
		entity->mesh->nRenderList = entity->mesh->nCullList = entity->mesh->nClippList = 0;
	}
}*/

INLINE void entity_updateTransform( Entity * entity )
{
	Quaternion qua;
	Matrix4x4 quaMtr;
	Entity* parent;

	if(entity->matrixDirty)
	{
		//单位化
		matrix4x4_identity( entity->transform );
		//缩放
		matrix4x4_appendScale( entity->transform, entity->scale->x, entity->scale->y, entity->scale->z );
		//旋转
		matrix4x4_append_self( entity->transform, quaternoin_toMatrix( &quaMtr, quaternoin_setFromEuler( &qua, DEG2RAD( entity->direction->y ), DEG2RAD( entity->direction->x ), DEG2RAD( entity->direction->z ) ) ) );
		//位移
		matrix4x4_appendTranslation( entity->transform, entity->position->x, entity->position->y, entity->position->z );
	}

	matrix4x4_copy( entity->world, entity->transform );

	//如果存在父结点，则连接父结点世界矩阵
	if(NULL != entity->parent)
	{
		if(entity->isBone)
		{
			if(entity->parent->isBone)
				matrix4x4_append_self(entity->world, entity->parent->world);
		}
		else
		{
			matrix4x4_append_self(entity->world, entity->parent->world);

			if(entity->parent->isBone)
			{
				parent = entity->parent;

				while(parent && parent->isBone)
					parent = parent->parent;

				if(parent)
					matrix4x4_append_self(entity->world, parent->world);
			}
		}

	}
	
	//if( NULL != entity->parent && (!entity->isBone || entity->parent->isBone) ) matrix4x4_append_self(entity->world, entity->parent->world);

	matrix4x4_copy( entity->worldInvert, entity->world );
	//世界逆矩阵
	matrix4x4_fastInvert( entity->worldInvert );
	//从世界矩阵获得世界位置
	matrix4x4_getPosition( entity->w_pos, entity->world );

	entity->transformDirty = TRUE;
}

INLINE void entity_decompose( Entity * entity )
{
	matrix4x4_decompose(entity->transform, entity->position, entity->scale, entity->direction, NULL);

	entity->matrixDirty = TRUE;
}

/**
属性设置
**/
INLINE void entity_setX(Entity * entity, float value)
{
	entity->position->x = value;

	entity->transformDirty = TRUE;
}

INLINE float entity_getX(Entity * entity)
{
	return entity->position->x;
}

INLINE void entity_setY(Entity * entity, float value)
{
	entity->position->y = value;

	entity->transformDirty = TRUE;
}

INLINE float entity_getY(Entity * entity)
{
	return entity->position->y;
}

INLINE void entity_setZ(Entity * entity, float value)
{
	entity->position->z = value;

	entity->transformDirty = TRUE;
}

INLINE float entity_getZ(Entity * entity)
{
	return entity->position->z;
}

INLINE void entity_setScaleX(Entity * entity, float value)
{
	entity->scale->x = value;

	entity->transformDirty = TRUE;
}

INLINE float entity_getScaleX(Entity * entity)
{
	return entity->scale->x;
}

INLINE void entity_setScaleY(Entity * entity, float value)
{
	entity->scale->y = value;

	entity->transformDirty = TRUE;
}

INLINE float entity_getScaleY(Entity * entity)
{
	return entity->scale->y;
}

INLINE void entity_setScaleZ(Entity * entity, float value)
{
	entity->scale->z = value;

	entity->transformDirty = TRUE;
}

INLINE float entity_getScaleZ(Entity * entity)
{
	return entity->scale->z;
}

INLINE void entity_setRotationX(Entity * entity, float value)
{
	entity->direction->x = value;

	entity->transformDirty = TRUE;
}

INLINE float entity_getRotationX(Entity * entity)
{
	return entity->direction->x;
}

INLINE void entity_setRotationY(Entity * entity, float value)
{
	entity->direction->y = value;

	entity->transformDirty = TRUE;
}

INLINE float entity_getRotationY(Entity * entity)
{
	return entity->direction->y;
}

INLINE void entity_setRotationZ(Entity * entity, float value)
{
	entity->direction->z = value;

	entity->transformDirty = TRUE;
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

	entity->transformDirty = TRUE;
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

	entity->transformDirty = TRUE;
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

	entity->transformDirty = TRUE;
}

INLINE Vector3D * entity_getRotation(Vector3D * output, Entity * entity)
{
	vector3D_copy(output, entity->direction);

	return output;
}

#endif
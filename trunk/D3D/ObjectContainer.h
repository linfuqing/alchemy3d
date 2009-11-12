#pragma once
#include "object.h"

class ObjectContainer :
	public Object
{
protected:

	typedef struct OBJECTS
	{
		Object* object;

		struct OBJECTS* next;

		OBJECTS(Object* pObject): object(pObject), next(NULL)
		{
		}

	}OBJECTS, * LPOBJECTS;

	LPOBJECTS m_pChildren;

public:
	ObjectContainer(void);
	~ObjectContainer(void);

	
	void addChild(Object* pChild);
	bool removeChild(Object* pChild);

	Object* getChildByIndex(const UINT nIndex);

	virtual void render(Viewport* pViewport);

	UINT numChildren()const;
};
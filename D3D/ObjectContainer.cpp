#include "StdAfx.h"
#include "ObjectContainer.h"

ObjectContainer::ObjectContainer(void) : m_pChildren(NULL)
{
}

ObjectContainer::~ObjectContainer(void)
{
	LPOBJECTS pObject = m_pChildren, pPrevious;

	while(pObject)
	{
		pPrevious = pObject;
		pObject   = pObject->next;
		delete pPrevious;
	}
}

Object* ObjectContainer::getChildByIndex(const UINT nIndex)
{
	LPOBJECTS pObject = m_pChildren;
	UINT i;

	for(i = nIndex; i > 0 && pObject; i--)
	{
		pObject = pObject->next;
	}

	return i ? NULL : pObject->object;
}

UINT ObjectContainer::numChildren()const
{
	LPOBJECTS pObject = m_pChildren;
	UINT      nNumber = 0;

	while(pObject)
	{
		nNumber ++;

		pObject = pObject -> next;
	}

	return nNumber;
}


void ObjectContainer::addChild(Object* pChild)
{
	LPOBJECTS pObject = new OBJECTS(pChild);

	pChild->m_pWorldMatrix = &m_Matrix;
	pObject->next          = m_pChildren;
	m_pChildren            = pObject;
}

bool ObjectContainer::removeChild(Object* pChild)
{
	if(!m_pChildren)
		return false;

	LPOBJECTS pObject = m_pChildren;

	bool bSucceed     = false;

	if(m_pChildren->object == pChild)
	{
		m_pChildren = m_pChildren->next;

		bSucceed    = true;
		
		delete pObject;
	}

	while(pObject->next)
	{
		if(pObject->next)
		{
			LPOBJECTS pNext = pObject->next;
	
			pObject->next   = pObject->next->next;

			bSucceed        = true;
			
			delete pNext;
		}

		pObject = pObject->next;
	}

	return bSucceed;
}

void ObjectContainer::render(Viewport* pViewport)
{
	LPOBJECTS pObject = m_pChildren;

	while(pObject)
	{
		pObject->object->render(pViewport);

		pObject = pObject -> next;
	}
}
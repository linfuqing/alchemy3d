#pragma once
#include "object.h"
#include "RenderManager.h"

class RenderObject :
	public Object
{
private:

	RenderManager m_RenderManager;

public:
	RenderObject(void);
	~RenderObject(void);

	inline RenderManager& getRenderManager();

	virtual void render(Viewport* pViewport);
};

inline RenderManager& RenderObject::getRenderManager()
{
	return m_RenderManager;
}

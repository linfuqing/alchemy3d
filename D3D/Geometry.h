#pragma once
#include "ObjectContainer.h"
#include "RenderObject.h"
#include "RenderManager.h"

class Geometry :
	public ObjectContainer
{
private:
	LPOBJECTS m_pSubsets;

	//Texture* m_pgTexture[MAX_TEXTURE];

	RenderManager m_RenderManager;

	//void __renderParent(Viewport* pViewport);

public:
	Geometry(LPMESH mesh);
	~Geometry(void);

	LPMESH mesh;

	/*bool lightEnable;

	RENDERSTATE renderState;

	MATERIAL material;*/

	virtual void render(Viewport* pViewport);
	RenderObject* getSubsetRenderObject(DWORD nAttribId);

	inline RenderManager& getRenderManager();
};

inline RenderManager& Geometry::getRenderManager()
{
	return m_RenderManager;
}
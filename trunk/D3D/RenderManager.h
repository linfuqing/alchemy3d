#pragma once

#include "TextureManager.h"

class Geometry;
class RenderObject;

class RenderManager
{
	friend class Geometry;
	friend class RenderObject;
private:
	TextureManager m_gTexture[MAX_TEXTURE];

	void __beginRender(Viewport* pViewport);
	void __endRender(Viewport* pViewport);
public:
	RenderManager(void);
	~RenderManager(void);

	bool lightEnable;

	bool specularEnable;

	RENDERSTATE renderState;

	MATERIAL material;

	inline TextureManager& getTextureManagerByStage(DWORD nStage);
};

inline TextureManager& RenderManager::getTextureManagerByStage(DWORD nStage)
{
	return nStage >= MAX_TEXTURE ? m_gTexture[MAX_TEXTURE - 1] : m_gTexture[nStage]; 
}

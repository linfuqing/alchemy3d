#pragma once

#include"ObjectContainer.h"

class Scene : public ObjectContainer
{
private:

	LPLIGHT m_gLights[MAX_LIGHTS];

	DWORD m_nLights;

public:
	Scene(void);
	~Scene(void);

	bool addLight(   LPLIGHT pLight);
	bool removeLight(LPLIGHT pLight);

	bool lighting;

	virtual void render(Viewport* pViewport);

	inline DWORD numLights()const;
};

inline DWORD Scene::numLights() const
{
	return m_nLights;
}
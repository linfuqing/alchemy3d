#include "StdAfx.h"
#include "Scene.h"

Scene::Scene(void):m_nLights(0), lighting(true)
{
	for(int i = 0; i < MAX_LIGHTS; i ++)
		m_gLights[i] = NULL;
}

Scene::~Scene(void)
{
}

bool Scene::addLight(LPLIGHT pLight)
{
	if(m_nLights == MAX_LIGHTS)
		return false;

	m_gLights[m_nLights] = pLight;

	m_nLights ++;

	return true;
}

bool Scene::removeLight(LPLIGHT pLight)
{
	bool bSucceed = false;

	for(DWORD i = 0; i < m_nLights; i++)
	{
		if(m_gLights[i] == pLight)
		{
			bSucceed = true;

			if(i == MAX_LIGHTS)
				m_gLights[MAX_LIGHTS - 1] = NULL;
			else
				for(DWORD j = i; j < m_nLights - 1; j++)
					m_gLights[j] = m_gLights[j + 1];
		}
	}

	return bSucceed;
}

void Scene::render(Viewport* pViewport)
{
	getTransform();

	pViewport->device->SetRenderState(D3DRS_LIGHTING, lighting);

	for(DWORD i = 0; i < m_nLights; i ++)
		pViewport->device->SetLight(i, m_gLights[i]);

	ObjectContainer::render(pViewport);
}
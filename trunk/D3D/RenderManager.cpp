#include "StdAfx.h"
#include "RenderManager.h"
#include "Scene.h"

RenderManager::RenderManager(void) :
lightEnable(false), 
specularEnable(false),
renderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME)
{
	this->material.Ambient  = RED;
	this->material.Diffuse  = RED;
	this->material.Specular = RED;
	this->material.Emissive = BLACK;
	this->material.Power    = 2.0f;
}

RenderManager::~RenderManager(void)
{
}

void RenderManager::__beginRender(Viewport* pViewport)
{
	DWORD i = 0;

	pViewport->device->SetMaterial(&material);

	for(; i < pViewport->scene->numLights(); i++)
		pViewport->device->LightEnable(i, lightEnable);

	for(; i < MAX_LIGHTS; i++)
		pViewport->device->LightEnable(i, false);

	pViewport->device->SetRenderState(D3DRS_SPECULARENABLE, specularEnable);

	for(i = 0; i < MAX_TEXTURE; i++)
		m_gTexture[i].mapOn(pViewport->device, i);

	pViewport->device->SetRenderState(renderState.state, renderState.value);
}

void RenderManager::__endRender(Viewport* pViewport)
{
	pViewport->device->SetRenderState(renderState.state, 0x7fffffff);
}
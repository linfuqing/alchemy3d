#include "StdAfx.h"
#include "RenderObject.h"
#include "Scene.h"

RenderObject::RenderObject(void)
{
}

RenderObject::~RenderObject(void)
{
}

void RenderObject::render(Viewport* pViewport)
{
	pViewport->device->SetTransform( D3DTS_WORLD, &getTransform() );

	m_RenderManager.__beginRender(pViewport);
}
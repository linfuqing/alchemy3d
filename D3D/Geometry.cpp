#include "StdAfx.h"
#include "Geometry.h"
#include "Scene.h"

Geometry::Geometry(LPMESH mesh) : 
//lightEnable(false), 
//renderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME),
mesh(mesh),
m_pSubsets(NULL)
{
	/*this->material.Ambient  = RED;
	this->material.Diffuse  = RED;
	this->material.Specular = RED;
	this->material.Emissive = RED;
	this->material.Power    = 8.0f;

	for(DWORD i = 0; i < MAX_TEXTURE; i++)
		m_pgTexture[i] = NULL;*/
}

Geometry::~Geometry(void)
{
}


/*void Geometry::__renderParent(Viewport* viewport)
{
	viewport->device->SetTransform(D3DTS_WORLD, &m_Matrix);
	viewport->device->SetMaterial(&material);

	for(DWORD i = 0; i < viewport->scene->numLights(); i ++)
	{
		viewport->device->LightEnable(i, lightEnable);
	}

	viewport->device->SetRenderState(renderState.state, renderState.value);
}*/

void Geometry::render(Viewport* pViewport)
{
	getTransform();

	if(mesh)
	{
		/*Direct3DDevice* device;

		mesh->GetDevice(&device);

		if(device != viewport->device)
		{
			mesh->CloneMeshFVF(mesh->GetOptions(), mesh->GetFVF(), device, &mesh);
		}*/

		pViewport->device->SetTransform(D3DTS_WORLD, &m_Matrix);
		m_RenderManager.__beginRender(pViewport);
		//__renderParent(pViewport);

		RenderObject*  renderObject = NULL;
		DWORD nSubsets;
		LPOBJECTS pObject = m_pSubsets;

		mesh->GetAttributeTable( NULL, &nSubsets );

		for(DWORD i = 0; i <= nSubsets; i ++)
		{
			if(pObject)
			{
				if(pObject->object)
				{
					m_RenderManager.__endRender(pViewport);
					pObject->object->render(pViewport);
					renderObject = dynamic_cast<RenderObject*>(pObject->object);
				}

				pObject = pObject->next;
			}

			mesh->DrawSubset(i);

			if(renderObject)
			{
				renderObject->getRenderManager().__endRender(pViewport);

				pViewport->device->SetTransform(D3DTS_WORLD, &m_Matrix);

				m_RenderManager.__beginRender(pViewport);
				//__renderParent(pViewport);

				renderObject = NULL;
			}
		}

		m_RenderManager.__endRender(pViewport);
	}

	ObjectContainer::render(pViewport);
}

RenderObject* Geometry::getSubsetRenderObject(DWORD nAttribId)
{
	if(!mesh)
		return NULL;

	DWORD nSubsets = 0;
	mesh->GetAttributeTable(NULL, &nSubsets);

	if(nSubsets < nAttribId)
		return NULL;

	LPOBJECTS pObject = m_pSubsets;

	if(!pObject)
	{
		m_pSubsets = new OBJECTS(NULL);
	}

	for(DWORD i = 0; i < nAttribId; i ++)
	{
		pObject->next = pObject->next ? pObject->next : new OBJECTS(NULL);
		pObject       = pObject->next;
	}

	pObject->object = new RenderObject;

	pObject->object->m_pWorldMatrix = &m_Matrix;

	return dynamic_cast<RenderObject*>(pObject->object);
}

#include "StdAfx.h"
#include "MeshManager.h"

MeshManager::MeshManager(DWORD nNumVertices, DWORD nNumTriangles) :
m_pVertexManager(NULL), m_pVertices(NULL), m_pTriangles(NULL)
{
	reset(nNumVertices, nNumTriangles);
}

MeshManager::~MeshManager(void)
{
	delete [] m_pVertices;
	delete [] m_pTriangles;

	delete [] m_pVertexManager;
}


void MeshManager::reset(DWORD nNumVertices, DWORD nNumTriangles)
{
	if(m_pVertices)
		delete [] m_pVertices;
	if(m_pTriangles)
		delete [] m_pTriangles;

	if(m_pVertexManager)
		delete [] m_pVertexManager;

	this->m_nNumVertices  = nNumVertices;
	this->m_nNumTriangles = nNumTriangles;

	m_pVertices           = new VERTEX[nNumVertices];
	m_pTriangles          = new TRIANGLE[nNumTriangles];

	m_pVertexManager      = new VERTEXMANAGER[nNumVertices];

	for(DWORD i = 0; i < nNumVertices; i++)
		m_pVertexManager[i].vertex = m_pVertices + i;
}

LPVERTEX MeshManager::getVertices()
{
	return m_pVertices;
}

LPTRIANGLE MeshManager::getTriangles()
{
	return m_pTriangles;
}

DWORD MeshManager::__getTextureCount(DWORD nFVF)
{
	DWORD nTextureCountShift, i;

	for(i = MAX_TEXTURE; i > 0; i--)
	{
		nTextureCountShift = i << D3DFVF_TEXCOUNT_SHIFT;
		if( (nTextureCountShift & nFVF) == nTextureCountShift )
			break;
	}

	return i;
}

LPMESH MeshManager::createMesh(DWORD nFVF, DWORD nOptions, LPDEVICE pDevice)
{
	DWORD i, j;
	//Build vertices:
	for(i = 0; i < m_nNumTriangles; i++)
		for(j = 0; j < TRIANGLE_NODE_NUMBERS; j++)
			m_pVertexManager[m_pTriangles[i].index[j]].addTriangle(m_pTriangles + i, j);


	//Check FVF to add vertices:
	DWORD nTexcount = __getTextureCount(nFVF), nNumVertices = 0;

	bool bDiffuse   = (D3DFVF_DIFFUSE  & nFVF) == D3DFVF_DIFFUSE,
		 bPsize     = (D3DFVF_PSIZE    & nFVF) == D3DFVF_PSIZE,
		 bSpecular  = (D3DFVF_SPECULAR & nFVF) == D3DFVF_SPECULAR, 

		 bSameUV, bSameColor;

	LPTRIANGLEMANAGER pTraingleManager, pSame, pPrevious;

	if(bSpecular || nTexcount)
	{
		for(i = 0; i < m_nNumVertices; i++)
		{
			pTraingleManager = m_pVertexManager[i].triangleManager;

			while(pTraingleManager)
			{
				pSame = pTraingleManager->next;
				while(pSame)
				{
					bSameUV = bSameColor = true;

					for(j = 0; j < nTexcount; j++)
					{
						if(pSame->triangle->uv[pSame->index][j] != pTraingleManager->triangle->uv[pTraingleManager->index][j])
						{
							bSameUV = false;
							break;
						}
					}

					if(!bSameUV)
					{
						pSame = pSame->next;
						continue;
					}

					if(bSpecular && pSame->triangle->color[pSame->index] != pTraingleManager->triangle->color[pTraingleManager->index])
						bSameColor = false;

					if(bSameUV && bSameColor)
					{
						if(pSame->next)
							pSame->next->previous = pSame->previous;

						pSame->previous->next = pSame->next;
						pPrevious             = pSame;
						pSame                 = pSame->previous;
						pPrevious->next       = NULL;
						pPrevious->previous   = NULL;

						delete pPrevious;
					}

					pSame = pSame->next;
				}

				nNumVertices++;

				pTraingleManager = pTraingleManager->next;
			}
		}
	}
	else
		nNumVertices = m_nNumVertices;

	//Create mesh:
	LPMESH pMesh = NULL;

	D3DXCreateMeshFVF(nNumVertices, m_nNumVertices, nOptions, nFVF, pDevice, &pMesh);

	//Fill mesh:

	return pMesh;
}
#include "StdAfx.h"
#include "TextureManager.h"

TextureManager::TextureManager(LPTEXTURE pTexture) : texture(pTexture), m_pSamplerStateList(NULL)
{
}

TextureManager::~TextureManager()
{
	LPSAMPLERSTATELIST pSamplerStateList = m_pSamplerStateList, pPrevious;

	while(pSamplerStateList)
	{
		pPrevious         = pSamplerStateList;
		pSamplerStateList = pSamplerStateList->next;

		delete pPrevious;
	}
}

void TextureManager::addSamlerState(   const SAMPLERSTATE& SamplerState)
{
	LPSAMPLERSTATELIST pSamplerStateList = m_pSamplerStateList;

	if(!pSamplerStateList)
	{
		m_pSamplerStateList = new SAMPLERSTATELIST(SamplerState);
		return;
	}

	while(pSamplerStateList->next)
		pSamplerStateList = pSamplerStateList->next;

	pSamplerStateList->next = new SAMPLERSTATELIST(SamplerState);
}

bool TextureManager::removeSamlerState(const SAMPLERSTATE& SamplerState)
{
	if(!m_pSamplerStateList)
		return false;

	
	bool bSucceed  = false;

	if(m_pSamplerStateList->samplerState == SamplerState)
	{
		delete m_pSamplerStateList;

		m_pSamplerStateList = NULL;
		bSucceed            = true;
	}

	LPSAMPLERSTATELIST pSamplerStateList = m_pSamplerStateList;

	while(pSamplerStateList->next)
	{
		if(pSamplerStateList->next->samplerState == SamplerState)
		{
			LPSAMPLERSTATELIST pNext = pSamplerStateList->next;

			pSamplerStateList->next  = pSamplerStateList->next->next;

			bSucceed                 = true;

			delete pNext;
		}
	}

	return bSucceed;
}

void TextureManager::mapOn(LPDEVICE pDevice, DWORD nStage)
{
	LPSAMPLERSTATELIST pSamplerStateList = m_pSamplerStateList;

	pDevice->SetTexture(nStage, texture);

	while(pSamplerStateList)
	{
		pDevice->SetSamplerState(nStage, pSamplerStateList->samplerState.state, pSamplerStateList->samplerState.value);

		pSamplerStateList = pSamplerStateList->next;
	}
}
#pragma once

#include "Viewport.h"

class TextureManager
{
private:

	typedef struct SAMPLERSTATELIST
	{
		SAMPLERSTATE      samplerState;
		SAMPLERSTATELIST* next;

		SAMPLERSTATELIST(const SAMPLERSTATE& samplerState) : samplerState(samplerState.state, samplerState.value), next(NULL)
		{
		}
	}SAMPLERSTATELIST, *LPSAMPLERSTATELIST;

	LPSAMPLERSTATELIST m_pSamplerStateList;
public:

	TextureManager(LPTEXTURE pTexture = NULL);
	~TextureManager();

	TEXTURE* texture;

	void addSamlerState(   const SAMPLERSTATE& SamplerState);
	bool removeSamlerState(const SAMPLERSTATE& SamplerState);

	void mapOn(LPDEVICE pDevice, DWORD nStage);
};

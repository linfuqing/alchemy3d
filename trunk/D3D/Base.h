#pragma once

//Base:
#define OFF (int)(-1)

//Max:
#define MAX_LIGHTS  (DWORD)8
#define MAX_TEXTURE (DWORD)8
#define MAX_BLEND   (DWORD)3

//Default:
typedef IDirect3DDevice9       DEVICE,       * LPDEVICE;
typedef IDirect3DVertexBuffer9 VERTEXBUFFER, * LPVERTEXBUFFER;
typedef IDirect3DIndexBuffer9  INDEXBUFFER,  * LPINDEXBUFFER;
typedef D3DXVECTOR3            VECTOR3D,     * LPVECTOR3D;
typedef D3DXMATRIX             MATRIX3D,     * LPMATRIX3D;
typedef D3DMATERIAL9           MATERIAL,     * LPMATERIAL;
typedef D3DLIGHT9              LIGHT,        * LPLIGHT;
typedef IDirect3DTexture9      TEXTURE,      * LPTEXTURE;
typedef D3DXCOLOR              COLOR,        * LPCOLOR;
typedef D3DXVECTOR2            VECTOR2D,     * LPVECTOR2D;
typedef ID3DXMesh              MESH,         * LPMESH;
typedef D3DXVECTOR4            VECTOR4D,     * LPVECTOR4D;
typedef D3DVIEWPORT9           VIEWPORT,     * LPVIEWPORT;
typedef D3DXPLANE              PLANE,        * LPPLANE;

//State:
typedef struct RENDERSTATE
{
	D3DRENDERSTATETYPE state;
	DWORD              value;

	RENDERSTATE(const D3DRENDERSTATETYPE State, const DWORD nValue) : state(State), value(nValue){}
}RENDERSTATE, * LPRENDERSTATE;

typedef struct SAMPLERSTATE
{
	D3DSAMPLERSTATETYPE state;
	DWORD               value;

	SAMPLERSTATE(const D3DSAMPLERSTATETYPE State, const DWORD nValue) : state(State), value(nValue){}

	friend const bool operator == (const SAMPLERSTATE& s1,const SAMPLERSTATE& s2)
	{
		return (s1.state == s2.state)&&(s1.value == s2.value);
	}
}SAMPLERSTATE, * LPSAMPLERSTATE;

//Color:
const COLOR WHITE(   D3DCOLOR_XRGB(255, 255, 255) );
const COLOR BLACK(   D3DCOLOR_XRGB(0  , 0  , 0  ) );
const COLOR RED(     D3DCOLOR_XRGB(255, 0  , 0  ) );
const COLOR GREEN(   D3DCOLOR_XRGB(0  , 255, 0  ) );
const COLOR BLUE(    D3DCOLOR_XRGB(0  , 0  , 255) );
const COLOR YELLOW(  D3DCOLOR_XRGB(255, 255, 0  ) );
const COLOR CYAN(    D3DCOLOR_XRGB(0  , 255, 255) );
const COLOR MAGENTA( D3DCOLOR_XRGB(255, 0  , 255) );

//Axis:
const VECTOR3D X_AXIS(1.0f, 0.0f, 0.0f);
const VECTOR3D Y_AXIS(0.0f, 1.0f, 0.0f);
const VECTOR3D Z_AXIS(0.0f, 0.0f, 1.0f);
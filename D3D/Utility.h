#pragma once

namespace d3d
{
	bool InitD3D
		(
		HWND hwnd,
		int width, int height,
		bool windowed,
		D3DDEVTYPE deviceType,
		IDirect3DDevice9** device);

	WPARAM EnterMsgLoop( bool (*ptr_display)(float timeDelta));

};

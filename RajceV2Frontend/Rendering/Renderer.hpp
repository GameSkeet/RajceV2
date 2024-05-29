#pragma once

namespace RajceV2 {
	namespace Rendering {
		extern HWND window;
		extern IDXGISwapChain* d3d_swap;
		extern ID3D11Device* d3d_device;
		extern ID3D11DeviceContext* d3d_context;
		extern ID3D11RenderTargetView* d3d_rendertarget;

		bool InitRenderer();
		bool HookD3D();

		void UnhookD3D();
		void DestroyRenderer();
	}
}
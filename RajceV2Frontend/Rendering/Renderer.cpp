// The colors are fucked/the rendering always has some alpha added

#include "../Source.hpp"
// Pointless to include in Source.hpp as it will be used only here
#include "../Depends/imgui/imgui_impl_dx11.h"
#include "../Depends/imgui/imgui_impl_win32.h"

#pragma region Types & Definitions

typedef LRESULT(__stdcall* fWndProc)(HWND, UINT, WPARAM, LPARAM);
typedef HRESULT(__stdcall* fPresent)(IDXGISwapChain*, UINT, UINT);
typedef HRESULT(__stdcall* fResizeBuffers)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);

#define PRESENT_INDEX 8
#define RESIZEBUFFERS_INDEX 13

#define RendererVar(name, defVal) RajceV2::Rendering::##name = defVal
#define RendererFunc(name, args) RajceV2::Rendering::##name##args

#pragma endregion
#pragma region Variables

static fWndProc oWndProc = nullptr;
static fPresent oPresent = nullptr;
static fResizeBuffers oResizeBuffers = nullptr;

static bool m_bPresentInit = false;

HWND RendererVar(window, 0);
IDXGISwapChain* RendererVar(d3d_swap, nullptr);
ID3D11Device* RendererVar(d3d_device, nullptr);
ID3D11DeviceContext* RendererVar(d3d_context, nullptr);
ID3D11RenderTargetView* RendererVar(d3d_rendertarget, nullptr);

#pragma endregion
#pragma region Functions

__forceinline void InitImGui() {
	using namespace RajceV2::Rendering;
	using namespace imelems;

	ImGuiIO& io = ImGui::GetIO();
	// IO here
	{
		io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	}

	ImGuiStyle& style = ImGui::GetStyle();
	// Styles here
	{
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.WindowPadding = ImVec2(0, 0);
		style.ItemSpacing = ImVec2(0, 0);

		style.WindowBorderSize = 0.f;
		style.Alpha = 1.f;

		style.WindowRounding = Window::Rounding;

		style.Colors[ImGuiCol_Border] = Window::BorderColor;
		style.Colors[ImGuiCol_WindowBg] = Window::WindowBg;
		style.Colors[ImGuiCol_TitleBg] = Window::TitleInactive;
		style.Colors[ImGuiCol_TitleBgActive] = Window::TitleActive;
	}

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(d3d_device, d3d_context);
}

#pragma endregion
#pragma region Hooks

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall hkWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
	// Get the menu keybind
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wp, lp))
		return TRUE;

	return CallWindowProc(oWndProc, hwnd, msg, wp, lp);
}

HRESULT __stdcall hkPresent(IDXGISwapChain* swap, UINT sync, UINT flags) {
	using namespace RajceV2::Rendering;

	if (!m_bPresentInit) {
		if (SUCCEEDED(swap->GetDevice(IID_PPV_ARGS(&d3d_device)))) {
			d3d_device->GetImmediateContext(&d3d_context);

			DXGI_SWAP_CHAIN_DESC sd;
			swap->GetDesc(&sd);
			window = sd.OutputWindow;

			ID3D11Texture2D* buff;
			swap->GetBuffer(0, IID_PPV_ARGS(&buff));
			d3d_device->CreateRenderTargetView(buff, 0, &d3d_rendertarget);
			buff->Release();

			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)hkWndProc);

			RajceV2::Menu::Init();
			InitImGui();

			d3d_swap = swap;
			m_bPresentInit = true;
		}
		else
			return oPresent(swap, sync, flags);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Call ESP and stuff before the menu
	// Cause then it wont be infront of the menu visualy
	RajceV2::Menu::Render();
	RajceV2::KeybindList::Render();

	ImGui::Render();

	d3d_context->OMSetRenderTargets(1, &d3d_rendertarget, 0);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return oPresent(swap, sync, flags);
}

HRESULT __stdcall hkResizeBuffers(IDXGISwapChain* swap, UINT buffCnt, UINT w, UINT h, DXGI_FORMAT format, UINT flags) {
	using namespace RajceV2::Rendering;

	if (d3d_rendertarget) {
		d3d_context->OMSetRenderTargets(0, 0, 0);
		d3d_rendertarget->Release();
	}
	
	HRESULT res = oResizeBuffers(swap, buffCnt, w, h, format, flags);

	ID3D11Texture2D* buff;
	swap->GetBuffer(0, IID_PPV_ARGS(&buff));
	d3d_device->CreateRenderTargetView(buff, 0, &d3d_rendertarget);
	buff->Release();

	D3D11_VIEWPORT vp;
	vp.Width = w;
	vp.Height = h;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	d3d_context->RSSetViewports(1, &vp);

	return res;
}

#pragma endregion


bool RendererFunc(InitRenderer, ()) {
	kiero::Status::Enum status = kiero::init(kiero::RenderType::D3D11);
	if (status == kiero::Status::Success || status == kiero::Status::AlreadyInitializedError) {
		ImGui::CreateContext();
		return true;
	}

	return false;
}
bool RendererFunc(HookD3D, ()) {
	kiero::Status::Enum pstat = kiero::bind(PRESENT_INDEX, (void**)&oPresent, hkPresent);
	if (pstat != kiero::Status::Success)
		return false;
	
	kiero::Status::Enum rstat = kiero::bind(RESIZEBUFFERS_INDEX, (void**)&oResizeBuffers, hkResizeBuffers);
	if (rstat != kiero::Status::Success)
		return false;

	return true;
}

void RendererFunc(UnhookD3D, ()) {
	// Unhook the d3d functions
	kiero::unbind(PRESENT_INDEX);
	kiero::unbind(RESIZEBUFFERS_INDEX);

	RajceV2::Menu::Destroy();

	// Release the render target we created
	if (d3d_rendertarget) {
		d3d_context->OMSetRenderTargets(0, 0, 0);
		d3d_rendertarget->Release();
	}

	m_bPresentInit = false;

	// Set WNDPROC to the original
	SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);
	oWndProc = nullptr;

	// Destroy/Shutdown imgui
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	// Reset variables to default
	window = 0;
	d3d_swap = nullptr;
	d3d_device = nullptr;
	d3d_context = nullptr;
	d3d_rendertarget = nullptr;
}
void RendererFunc(DestroyRenderer, ()) {
	ImGui::DestroyContext();
	kiero::shutdown();
}
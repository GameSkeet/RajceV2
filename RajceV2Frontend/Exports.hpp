#pragma once

// Each w/char* should be allocated with GlobalAlloc
extern "C" {
	__declspec(dllexport) InitCode Init();
	__declspec(dllexport) void Destroy();

	// Keybind management
	__declspec(dllexport) void RegisterKeybind(const wchar_t* name, Keybind* key);
	__declspec(dllexport) void UnregisterKeybind(uint32_t id);
	__declspec(dllexport) void UnregisterKeybindByName(const wchar_t* name);

	// Menu
	__declspec(dllexport) void SetMenuIcon(ID3D11Resource* icon);
	__declspec(dllexport) void SetMenuKeybind(Keybind* keybind);

	// UI builder
	__declspec(dllexport) uint32_t BeginTab(const wchar_t* name);
	__declspec(dllexport) void EndTab();
	__declspec(dllexport) void RemoveTab(uint32_t tab_id);

	// Keybind list
	__declspec(dllexport) bool* GetKeybindListVisiblePtr();

	// Misc stuff
	__declspec(dllexport) void LoadFont(uint32_t type, byte* buff, int len);
	// The resulting resource should be released by the user
	__declspec(dllexport) ID3D11ShaderResourceView* LoadUnityTexture(ID3D11Resource* tex);
}
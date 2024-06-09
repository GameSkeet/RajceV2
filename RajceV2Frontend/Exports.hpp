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
	__declspec(dllexport) void SetMenuKeybindConverter(void* func); // The function should return a wchar_t* and its first param should be an int

	// UI builder
	__declspec(dllexport) uint32_t BeginTab(const wchar_t* name);
	__declspec(dllexport) void EndTab();
	__declspec(dllexport) void RemoveTab(uint32_t tab_id);

	__declspec(dllexport) void BeginSection(const wchar_t* name);
	__declspec(dllexport) void EndSection();

	__declspec(dllexport) void BeginContentBox(const wchar_t* name);
	__declspec(dllexport) void EndContentBox();

	__declspec(dllexport) void* AddElement(uint32_t type, const wchar_t* text, void* target, void* callback);
	__declspec(dllexport) void SetElementMinMax(void* elem, float min, float max);
	__declspec(dllexport) void SetComboValues(void* elem, const wchar_t** options, int num_options, bool multi);

	// Keybind list
	__declspec(dllexport) bool* GetKeybindListVisiblePtr();

	// Misc stuff
	__declspec(dllexport) void LoadFont(uint32_t type, byte* buff, int len);
	// The resulting resource should be released by the user
	__declspec(dllexport) void LoadUnityTexture(ID3D11Resource* tex, ID3D11ShaderResourceView** res);
}
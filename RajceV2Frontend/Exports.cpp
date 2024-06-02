#include "Source.hpp"

using namespace RajceV2;

InitCode Init() {
	if (!Rendering::InitRenderer())
		return InitCode_InitFailed;

	if (!Rendering::HookD3D())
		return InitCode_HookFailed;

	return InitCode_OK;
}
void Destroy() {
	// This should destroy/free/clear data that was created/allocated durring init and runtime
	// Free keybind names with GlobalFree

	Rendering::UnhookD3D();
	Rendering::DestroyRenderer();
#ifdef PRIVATE_CODE_INCLUDED
	PCode::Destroy(); // This just frees stuff created in the private code section
#endif

	UIBuilder::Destroy();
	KeybindManager::DestroyKeybinds();
}

// Keybind management
void RegisterKeybind(const wchar_t* name, Keybind* key) {
	key->ID = KeybindManager::RegisterKeybind(name, key);
}
void UnregisterKeybind(uint32_t id) {
	KeybindManager::UnregisterKeybind(id);
}
void UnregisterKeybindByName(const wchar_t* name) {
	KeybindManager::UnregisterKeybind(name);
}

// Menu
void SetMenuIcon(ID3D11Resource* icon) {
	Menu::SetIcon(icon);
}
void SetMenuKeybind(Keybind* keybind) {
	Menu::SetKeybind(keybind);
}

// UI builder
uint32_t BeginTab(const wchar_t* name) {
	return UIBuilder::BeginTab(name);
}
void EndTab() {
	UIBuilder::EndTab();
}
void RemoveTab(uint32_t id) {
	UIBuilder::RemoveTab(id);
}

void BeginSection(const wchar_t* name) {
	UIBuilder::BeginSection(name);
}
void EndSection() {
	UIBuilder::EndSection();
}

void BeginContentBox(const wchar_t* name) {
	UIBuilder::BeginContentBox(name);
}
void EndContentBox() {
	UIBuilder::EndContentBox();
}

void* AddElement(uint32_t type, const wchar_t* text, void* target, void* callback) {
	return (void*)UIBuilder::AddElement((UIBuilder::ElementType)type, text, target, callback);
}
void SetElementMinMax(void* elem, float min, float max) {
	UIBuilder::SetElementMinMax((UIBuilder::ElementEntry*)elem, min, max);
}

// Keybind list
bool* GetKeybindListVisiblePtr() {
	return &KeybindList::IsVisible;
}

// Misc
void LoadFont(uint32_t type, byte* bytes, int len) {
	ImGui::LoadFont((UIFonts)type, bytes, len);
}
void LoadUnityTexture(ID3D11Resource* tex, ID3D11ShaderResourceView** res) {
	// People that want textures/images inside the menu that come from unity
	// will need to add their own implementation.
	// This is a protection against pasters, cause it will work on every single unity version.
	// also this isn't hard to implement if you know D11 basics
#ifdef PRIVATE_CODE_INCLUDED
	PCode::LoadUnityTexture(tex, res);
#else
	if (res)
		*res = nullptr;
#endif
}
#include "../Source.hpp"

#pragma region Types & Denitions

#define mapType std::unordered_map<uint32_t, void*>
#define UIBFunc(name, args) RajceV2::UIBuilder::##name##args

#pragma endregion
#pragma region Variables

static uint32_t m_iTabCntr = 1;
static uint32_t m_iCurrTab = 0; // 0 means there is no tab selected
static mapType tabs;

#pragma endregion


uint32_t UIBFunc(BeginTab, (const wchar_t* name)) {
	return m_iTabCntr++;
}
void UIBFunc(EndTab, ()) {
	m_iCurrTab = 0;
}
void UIBFunc(RemoveTab, (uint32_t id)) {
	if (id == 0)
		return;

	if (!tabs.contains(id))
		return;

	void* tab = tabs[id];
	// Do some clean up
}

float UIBFunc(GetWidestTab, ()) {
	return 0.f;
}
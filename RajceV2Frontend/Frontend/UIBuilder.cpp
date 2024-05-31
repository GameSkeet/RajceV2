#include "../Source.hpp"

using namespace RajceV2;

#pragma region Types & Denitions

#define mapType std::unordered_map<uint32_t, UIBuilder::TabEntry*>
#define UIBFunc(name, args) RajceV2::UIBuilder::##name##args

#pragma endregion
#pragma region Variables

static uint32_t m_iTabCntr = 1;
static uint32_t m_iCurrTab = 0; // 0 means there is no tab selected

static float m_fWidestTab = 0.f;

static mapType tabs;

#pragma endregion
#pragma region Functions

static ImVec2 CalcTextSize(const char* text) {
	const char* text_end = ImGui::FindRenderedTextEnd(text);

	ImFont* font = ImGui::GetIO().Fonts->Fonts[UIFonts_TabsText];
	return font->CalcTextSizeA(font->FontSize, FLT_MAX, -1, text, text_end, 0);
}
static void DestroySection(UIBuilder::SectionEntry* sec) {
	if (sec->icon) 
		sec->icon->Release(); 

	free(sec->name); 

	delete sec;
}
static void DestroyTab(UIBuilder::TabEntry* tab) {
	if (tab->icon) 
		tab->icon->Release();

	free(tab->name);
	for (UIBuilder::SectionEntry* sec : tab->sections)
		DestroySection(sec);

	delete tab;
}

#pragma endregion


uint32_t UIBFunc(BeginTab, (const wchar_t* name, ID3D11Resource* icon)) {
	uint32_t tabId = m_iTabCntr++;
	TabEntry* tab = new TabEntry();
	ImGuiIO& io = ImGui::GetIO();

	ImGui::ConvertStrToUTF8(&tab->name, name);
	LoadUnityTexture(icon, &tab->icon);
	tab->nameSize = CalcTextSize(tab->name);
	tab->id = tabId;

	float width = tab->nameSize.x;
	if (tab->icon) {
		width += UIFonts_TabsText_Size; // Add the icon size
		width += imelems::Tabs::PaddingTextLeft; // Add space between text and icon
	}

	if (m_fWidestTab < width)
		m_fWidestTab = width;

	tabs[tabId] = tab;
	return tabId;
}
void UIBFunc(EndTab, ()) {
	m_iCurrTab = 0;
}
void UIBFunc(RemoveTab, (uint32_t id)) {
	if (id == 0)
		return;

	if (!tabs.contains(id))
		return;

	DestroyTab(tabs[id]);
	tabs.erase(id);
}

float UIBFunc(GetWidestTab, ()) {
	return m_fWidestTab;
}
UIBuilder::TabEntry** UIBFunc(GetTabs, (size_t* cnt)) {
	TabEntry** entries = new TabEntry*[*cnt = tabs.size()];

	size_t i = 0;
	for (mapType::iterator it = tabs.begin(); it != tabs.end(); it++)
		entries[i++] = it->second;

	return entries;
}
void UIBFunc(Destroy, ()) {
	for (mapType::iterator it = tabs.begin(); it != tabs.end(); it++) {
		TabEntry* tab = it->second;
		if (!tab)
			continue;

		DestroyTab(tab);
	}
	tabs.clear();

	m_iCurrTab = 0;
	m_fWidestTab = 0.f;
}
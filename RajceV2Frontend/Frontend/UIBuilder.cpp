#include "../Source.hpp"

using namespace RajceV2;

#pragma region Types & Denitions

#define mapType std::unordered_map<uint32_t, UIBuilder::TabEntry*>
#define UIBFunc(name, args) RajceV2::UIBuilder::##name##args

#pragma endregion
#pragma region Variables

static uint32_t m_iTabCntr = 1;
static UIBuilder::SectionEntry* m_kCurrSection = nullptr;

static uint32_t m_iCurrTab = 0; // 0 means there is no tab selected

static float m_fWidestTab = 0.f;

static mapType tabs;

#pragma endregion
#pragma region Functions

static ImVec2 CalcTextSize(const char* text, UIFonts fontt) {
	const char* text_end = ImGui::FindRenderedTextEnd(text);

	ImFont* font = ImGui::GetIO().Fonts->Fonts[fontt];
	return font->CalcTextSizeA(font->FontSize, FLT_MAX, -1, text, text_end, 0);
}
static void DestroySection(UIBuilder::SectionEntry* sec) {
	free(sec->name); 

	delete sec;
}
static void DestroyTab(UIBuilder::TabEntry* tab) {
	free(tab->name);
	for (UIBuilder::SectionEntry* sec : tab->sections)
		DestroySection(sec);

	delete tab;
}

#pragma endregion


uint32_t UIBFunc(BeginTab, (const wchar_t* name)) {
	uint32_t tabId = m_iTabCntr++;
	TabEntry* tab = new TabEntry();
	ImGuiIO& io = ImGui::GetIO();

	ImGui::ConvertStrToUTF8(&tab->name, name);
	GlobalFree((void*)name);

	tab->nameSize = CalcTextSize(tab->name, UIFonts_SectionText);
	tab->id = tabId;

	float width = tab->nameSize.x;
	if (m_fWidestTab < width)
		m_fWidestTab = width;

	tabs[tabId] = tab;
	m_iCurrTab = tabId;
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

uint32_t UIBFunc(BeginSection, (const wchar_t* name)) {
	if (m_iCurrTab == 0)
		return 0;
	if (!tabs.contains(m_iCurrTab)) // This should not really be possible if someone isn't a complete dumbass
		return 0;

	TabEntry* tab = tabs[m_iCurrTab];
	SectionEntry* sec = new SectionEntry();
	m_kCurrSection = sec;

	uint32_t secId = sec->id = tab->sections.size();
	tab->sections.push_back(sec);
	ImGuiIO& io = ImGui::GetIO();

	ImGui::ConvertStrToUTF8(&sec->name, name);
	GlobalFree((void*)name);

	sec->nameSize = CalcTextSize(sec->name, UIFonts_SectionText);

	return secId;
}
void UIBFunc(EndSection, ()) {
	m_kCurrSection = nullptr;
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
#include "../Source.hpp"

using namespace RajceV2::KeybindManager;

#pragma region Types & Definitions

#define PADDING_X 5
#define PADDING_Y 5
#define PADDING_BETWEEN_NAME_TYPE 10

#define CheckEntry(entry) IsEntryValid(entry, false, false)
#define KListVar(name, defVal) RajceV2::KeybindList::##name = defVal
#define KListFunc(name, args) RajceV2::KeybindList::##name##args

#pragma endregion
#pragma region Variables

bool KListVar(IsVisible, true);

static const DWORD windowFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings;

#pragma endregion
#pragma region Functions

bool IsEntryValid(KeybindEntry* entry, bool showAlways = false, bool showInactive = false) {
	if (!entry)
		return false;

	Keybind* key = entry->keybind;
	if (!key->ShowInList)
		return false;

	if (key->Type == KeybindType_Always)
		return showAlways;
	if (!key->State)
		return showInactive;

	return true;
}

#pragma endregion


void KListFunc(Render, ()) {
	if (!IsVisible)
		return;

	DWORD flags = windowFlags;
	bool isMenuClosed = false;
	if (!RajceV2::Menu::GetState()) {
		flags |= ImGuiWindowFlags_NoMove;
		isMenuClosed = true;
	}

	size_t kcnt = 0;
	KeybindEntry** keybinds = GetKeybinds(&kcnt);
	ImVec2 wndSize = ImVec2(0, PADDING_Y);

	bool isSomeKeybindShown = false;
	for (size_t i = 0; i < kcnt; i++) {
		KeybindEntry* entry = keybinds[i];
		if (!CheckEntry(entry))
			continue;

		isSomeKeybindShown = true;

		ImVec2 tsize = ImGui::CalcTextSize(entry->name);
		ImVec2 ssize;
		switch (entry->keybind->Type)
		{
			case KeybindType_Hold:
				ssize = ImGui::CalcTextSize("[holding]");
				break;
			case KeybindType_Toggle:
				ssize = ImGui::CalcTextSize("[toggled]");
				break;
			case KeybindType_Always:
				ssize = ImGui::CalcTextSize("[always]");
				break;
		}

		float twidth = tsize.x + PADDING_BETWEEN_NAME_TYPE + ssize.x;
		if (wndSize.x < twidth)
			wndSize.x = twidth;

		wndSize.y += tsize.y + PADDING_Y;
	}

	// We dont need to render when no keybinds are active and the user can't move the Window
	if (!isSomeKeybindShown && !RajceV2::Menu::GetState()) {
		free(keybinds);
		return;
	}

	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	float old_alpha = style.Alpha;
	float titleBarHeight = ImGui::GetFontSize() + style.FramePadding.y * 2;

	if (isMenuClosed)
		style.Alpha = 0.75f;
	
	ImGui::PushStyleColor(ImGuiCol_WindowBg, imelems::Tabs::TabActiveColor.Value);
	ImGui::SetNextWindowSize(wndSize + ImVec2(PADDING_X * 2, 0) + ImVec2(style.WindowBorderSize * 2, style.WindowBorderSize + titleBarHeight));
	ImGui::Begin("Keybinds", nullptr, flags);
	{
		ImGui::Dummy(ImVec2(0, PADDING_Y));
		ImGui::Dummy(ImVec2(PADDING_X, 0));
		ImGui::SameLine();
		ImGui::BeginGroup();
		{
			for (size_t i = 0; i < kcnt; i++) {
				KeybindEntry* entry = keybinds[i];
				if (!CheckEntry(entry))
					continue;

				ImGui::Text(entry->name);
				ImGui::SameLine();
				ImGui::Dummy(ImVec2(PADDING_BETWEEN_NAME_TYPE, 0));
				ImGui::SameLine();

				if (entry->keybind->State)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
				else ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));

				switch (entry->keybind->Type)
				{
					case KeybindType_Hold:
						ImGui::Text("[holding]");
						break;
					case KeybindType_Toggle:
						ImGui::Text("[toggled]");
						break;
					case KeybindType_Always:
						ImGui::Text("[always]");
						break;
				}

				ImGui::PopStyleColor();
				ImGui::Dummy(ImVec2(0, PADDING_Y));
			}
		}
		ImGui::EndGroup();
	}
	ImGui::PopStyleColor();
	ImGui::End();

	style.Alpha = old_alpha;

	// OBV we dont want a mem leak so we need to delete the keybinds arrays
	free(keybinds);
}
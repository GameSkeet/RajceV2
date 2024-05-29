#include "../Source.hpp"

using RajceV2::KeybindManager::KeybindEntry;

#pragma region Types & Definitions

#define mapType std::unordered_map<const wchar_t*, KeybindEntry*>
#define MngrFunc(name, args) RajceV2::KeybindManager::##name##args

#pragma endregion
#pragma region Variables

static uint32_t m_iIDCounter = 1;
static mapType keybinds;

#pragma endregion


uint32_t MngrFunc(RegisterKeybind, (const wchar_t* name, Keybind* key)) {
	if (keybinds.contains(name)) {
		MessageBoxW(0, L"Keybind with this name already exists!", name, MB_ICONERROR);
		GlobalFree((void*)name);

		return 0;
	}

	KeybindEntry* entry = new KeybindEntry();
	entry->keybind = key;
	ImGui::ConvertStrToUTF8(&entry->name, name);
	keybinds[name] = entry;

	GlobalFree((void*)name);

	return m_iIDCounter++;
}

void MngrFunc(UnregisterKeybind, (uint32_t id)) {
	for (mapType::iterator it = keybinds.begin(); it != keybinds.end(); it++) {
		KeybindEntry* entry = it->second;
		if (!entry) {
			keybinds.erase(it->first);
			continue;
		}

		if (entry->keybind->ID != id)
			continue;

		keybinds.erase(it->first); // remove the keybind from the map

		entry->keybind->ID = 0;
		free(entry->name);
		delete entry;

		break;
	}
}
void MngrFunc(UnregisterKeybind, (const wchar_t* name)) {
	if (!keybinds.contains(name)) {
		GlobalFree((void*)name);
		return;
	}

	KeybindEntry* entry = keybinds[name];
	entry->keybind->ID = 0;
	free(entry->name);
	delete entry;

	keybinds.erase(name);
	GlobalFree((void*)name);
}

Keybind* MngrFunc(GetKeybind, (const wchar_t* name)) {
	if (!keybinds.contains(name))
		return nullptr;

	return keybinds[name]->keybind;
}
KeybindEntry** MngrFunc(GetKeybinds, (size_t* cnt)) {
	KeybindEntry** entries = (KeybindEntry**)calloc(*cnt = keybinds.size(), sizeof(KeybindEntry*));
	
	size_t i = 0;
	for (mapType::iterator it = keybinds.begin(); it != keybinds.end(); it++) 
		entries[i++] = it->second;

	return entries;
}

void MngrFunc(DestroyKeybinds, ()) {
	for (mapType::iterator it = keybinds.begin(); it != keybinds.end(); it++) {
		KeybindEntry* entry = it->second;
		if (!entry) 
			continue;

		entry->keybind->ID = 0;
		free(entry->name);
		delete entry;
	}

	keybinds.clear();
}
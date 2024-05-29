#pragma once

namespace RajceV2 {
	namespace KeybindManager {
		struct KeybindEntry {
		public:
			Keybind* keybind;
			char* name; // UTF-8 string
		};

		uint32_t RegisterKeybind(const wchar_t* name, Keybind* key);

		void UnregisterKeybind(uint32_t id);
		void UnregisterKeybind(const wchar_t* name);

		// For internal use
		Keybind* GetKeybind(const wchar_t* name);
		KeybindEntry** GetKeybinds(size_t* cnt);

		void DestroyKeybinds();
	}
}
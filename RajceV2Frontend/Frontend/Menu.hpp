#pragma once

namespace RajceV2 {
	namespace Menu {
		void Init();

		bool GetState();
		void Render();

		void Destroy();

		void SetIcon(ID3D11Resource* icon);
		void SetKeybind(Keybind* keybind);
		void SetKeybindConverter(void* func);
	}
}
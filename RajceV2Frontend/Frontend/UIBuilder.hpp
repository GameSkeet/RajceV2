#pragma once

namespace RajceV2 {
	namespace UIBuilder {
		uint32_t BeginTab(const wchar_t* name);
		void EndTab();
		void RemoveTab(uint32_t id);

		// Internal functions
		float GetWidestTab();
	}
}
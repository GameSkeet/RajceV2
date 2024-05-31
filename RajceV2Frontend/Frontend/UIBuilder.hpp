#pragma once

namespace RajceV2 {
	namespace UIBuilder {
		struct SectionEntry {
		public:
			char* name;
			ID3D11ShaderResourceView* icon;
			
			uint32_t id;
			ImVec2 nameSize;
		};
		struct TabEntry {
		public:
			char* name;
			ID3D11ShaderResourceView* icon;

			uint32_t id;
			ImVec2 nameSize;

			uint32_t currSection;
			std::vector<SectionEntry*> sections;
		};

		uint32_t BeginTab(const wchar_t* name, ID3D11Resource* icon);
		void EndTab();
		void RemoveTab(uint32_t id);

		uint32_t BeginSection(const wchar_t* name, ID3D11Resource* icon);
		void EndSection();
		void RemoveSection(uint32_t id);

		// Internal functions
		float GetWidestTab();
		TabEntry** GetTabs(size_t* cnt);
		void Destroy();
	}
}
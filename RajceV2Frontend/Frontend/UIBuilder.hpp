#pragma once

namespace RajceV2 {
	namespace UIBuilder {
		struct SectionEntry {
		public:
			char* name;
			
			uint32_t id;
			ImVec2 nameSize;
		};
		struct TabEntry {
		public:
			char* name;

			uint32_t id;
			ImVec2 nameSize;

			uint32_t currSection;
			std::vector<SectionEntry*> sections;
		};

		uint32_t BeginTab(const wchar_t* name);
		void EndTab();
		void RemoveTab(uint32_t id);

		uint32_t BeginSection(const wchar_t* name);
		void EndSection();

		// Internal functions
		float GetWidestTab();
		TabEntry** GetTabs(size_t* cnt);
		void Destroy();
	}
}
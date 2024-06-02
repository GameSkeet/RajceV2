#pragma once

#define MAX_BOXES_PER_SECTION 4

namespace RajceV2 {
	namespace UIBuilder {
		enum ElementType : uint32_t {
			ElementType_None,
			ElementType_Button,
			ElementType_Checkbox,
			ElementType_SliderInt,
			ElementType_SliderFloat,
			ElementType_Colorpicker,
			ElementType_Combo
		};

		struct ElementEntry {
			char* text = nullptr;
			ElementType type = ElementType_None;

			void* target;
			void* callback;
			float min, max;
		};
		struct ContentBoxEntry {
		public:
			char* name = nullptr;
			ImVec2 nameSize = ImVec2(0, 0);

			std::vector<ElementEntry*> elements;
		};
		struct SectionEntry {
		public:
			char* name = nullptr;
			
			uint32_t id = 0;
			ImVec2 nameSize = ImVec2(0, 0);

			std::vector<ContentBoxEntry*> boxes;
		};
		struct TabEntry {
		public:
			char* name = nullptr;

			uint32_t id = 0;
			ImVec2 nameSize = ImVec2(0, 0);

			uint32_t currSection = 0;
			std::vector<SectionEntry*> sections;
		};

		uint32_t BeginTab(const wchar_t* name);
		void EndTab();
		void RemoveTab(uint32_t id);

		uint32_t BeginSection(const wchar_t* name);
		void EndSection();

		void BeginContentBox(const wchar_t* name);
		void EndContentBox();

		ElementEntry* AddElement(ElementType type, const wchar_t* text, void* target, void* callback);
		void SetElementMinMax(ElementEntry* elem, float min, float max);

		// Internal functions
		float GetWidestTab();
		TabEntry** GetTabs(size_t* cnt);
		void Destroy();
	}
}
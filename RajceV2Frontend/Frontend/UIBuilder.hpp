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
			ElementType_Combo,
			ElementType_Keybind
		};

		struct ElementEntry {
			char* text = nullptr;
			ElementType type = ElementType_None;

			void* target;
			void* callback;

			// Slider specific
			float min, max;

			// Combo specific
			const char** options;
			int num_options;
			bool is_multi;

			// States
			bool is_open;
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
		void SetComboValues(ElementEntry* elem, const wchar_t** options, int num_options, bool multi);

		// Internal functions
		float GetWidestTab();
		TabEntry** GetTabs(size_t* cnt);
		void Destroy();
	}
}

/*
ImGuiID oid = cwnd->GetID(i);

					const ImRect obb(oPos, oPos + oSize);
					ItemSize(oSize);
					if (!ItemAdd(obb, oid, nullptr, ImGuiItemFlags_AllowOverlap)) {
						oPos += ImVec2(0, oSize.y);
						continue;
					}

					bool ohovered, oheld;
					bool oclicked = ButtonBehavior(obb, oid, &ohovered, &oheld, ImGuiButtonFlags_AllowOverlap);
					if (oclicked)
						if (!is_multi)
							*selected = (*selected == i ? -1 : i);
						else selected[i] = (selected[i] != -1 ? -1 : i);

					bool isSelected; // This is here it will be needed in the future, it will get optimized anyway
					if (!is_multi)
						isSelected = (*selected != -1 ? *selected == i : false);
					else isSelected = selected[i] != -1;

					if (isSelected)
						PushStyleColor(ImGuiCol_Text, Elements::SecondaryColor.Value);
					RenderTextClipped(
						obb.Min - ImVec2(-Elements::PaddingSides, 2),
						obb.Max - ImVec2(-Elements::PaddingSides, 2),
						options[i],
						nullptr,
						nullptr,
						ImVec2(0, 0.5f)
					);
					if (isSelected)
						PopStyleColor();

					if (oheld || oclicked)
						cdraw->AddRectFilled(
							obb.Min,
							obb.Max,
							Elements::HeldColor,
							Elements::Rounding
						);
					else if (ohovered)
						cdraw->AddRectFilled(
							obb.Min,
							obb.Max,
							Elements::HoveredColor,
							Elements::Rounding
						);

					oPos += ImVec2(0, oSize.y);*/
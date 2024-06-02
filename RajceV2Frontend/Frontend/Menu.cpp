// I recommend not changing any Menu value cause its been designed to be on those values
// You can still change it tho, but its on you to fix it

#include "../Source.hpp"

#pragma region Types & Definitions

typedef void(__cdecl* fBtnCallback)();
typedef void(__cdecl* fCheckboxCallback)(bool val);
typedef void(__cdecl* fSliderCallback)(float val);

#define MenuSize 840, 525
#define MenuFunc(name, args) RajceV2::Menu::##name##args

#pragma endregion
#pragma region Variables

static bool m_bInvokedInit = false;
static ID3D11Resource* iconResource = nullptr;
static ID3D11ShaderResourceView* menuIcon = nullptr;
static Keybind* menuKey = nullptr;

static RajceV2::UIBuilder::TabEntry* m_ActiveTab = nullptr;

static const DWORD windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground;

#pragma endregion
#pragma region Custom imgui elements

namespace ImGui {
	bool CButton(const char* text) {
		using namespace imelems;

		ImGuiWindow* wnd = GetCurrentWindow();
		if (wnd->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = wnd->GetID(text);
		const ImVec2 label_size = CalcTextSize(text, 0, true);
		ImDrawList* draw = wnd->DrawList;

		ImVec2 pos = wnd->DC.CursorPos;
		ImVec2 cSize = GetContentRegionMax();
		ImVec2 size = ImVec2(cSize.x - Elements::PaddingSides * 2, Elements::MainElementHeight * 2);

		const ImRect bb(pos, pos + size);
		ItemSize(size);
		if (!ItemAdd(bb, id, nullptr, ImGuiItemFlags_AllowOverlap))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(bb, id, &hovered, &held, ImGuiButtonFlags_AllowOverlap);

		draw->AddRectFilled(
			bb.Min,
			bb.Max,
			Elements::PrimaryColor,
			Elements::Rounding
		);
		RenderTextClipped(
			bb.Min,
			bb.Max,
			text,
			nullptr,
			&label_size,
			ImVec2(0.5, 0.5)
		);

		if (held || pressed) {
			draw->AddRectFilled(
				bb.Min,
				bb.Max,
				Elements::HeldColor,
				Elements::Rounding
			);
		}
		else if (hovered) {
			draw->AddRectFilled(
				bb.Min,
				bb.Max,
				Elements::HoveredColor,
				Elements::Rounding
			);
		}

		return pressed;
	}
	bool CCheckbox(const char* text, bool* target) {
		using namespace imelems;

		ImGuiWindow* wnd = GetCurrentWindow();
		if (wnd->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = wnd->GetID(text);
		const ImVec2 label_size = CalcTextSize(text, 0, true);
		ImDrawList* draw = wnd->DrawList;

		ImVec2 pos = wnd->DC.CursorPos;
		ImVec2 cSize = GetContentRegionMax();
		ImVec2 size = ImVec2(label_size.x + Elements::InnerPadding + Elements::MainElementHeight, Elements::MainElementHeight);

		const ImRect bb(pos, pos + size);
		ItemSize(bb);
		if (!ItemAdd(bb, id, nullptr, ImGuiItemFlags_AllowOverlap))
			return false;

		bool hovered, held;
		bool pressed = ButtonBehavior(bb, id, &hovered, &held, ImGuiButtonFlags_AllowOverlap);
		if (pressed) {
			*target = !(*target);
			MarkItemEdited(id);
		}

		draw->AddRectFilled(
			bb.Min,
			bb.Min + ImVec2(size.y, size.y),
			(*target ? Elements::SecondaryColor : Elements::PrimaryColor),
			Elements::Rounding
		);
		if (held || pressed) {
			draw->AddRectFilled(
				bb.Min,
				bb.Min + ImVec2(size.y, size.y),
				Elements::HeldColor,
				Elements::Rounding
			);
		}
		else if (hovered) {
			draw->AddRectFilled(
				bb.Min,
				bb.Min + ImVec2(size.y, size.y),
				Elements::HoveredColor,
				Elements::Rounding
			);
		}

		RenderTextClipped(
			bb.Min + ImVec2(size.y + Elements::InnerPadding, -2),
			bb.Max,
			text,
			nullptr,
			&label_size,
			ImVec2(0, 0.5)
		);

		return pressed;
	}
	bool CSliderInt(const char* text, int* val, float min, float max) {
		return false;
	}
	bool CSliderFloat(const char* text, float* val, float min, float max) {
		return false;
	}
	bool CColorPicker(const char* text, float* rgba) {
		return false;
	}
}

#pragma endregion


void MenuFunc(Init, ()) {
	LoadUnityTexture(iconResource, &menuIcon);

	m_bInvokedInit = true;
	iconResource = nullptr;
}

bool MenuFunc(GetState, ()) {
	if (!menuKey)
		return false;

	return menuKey->State;
}
void MenuFunc(Render, ()) {
	using namespace imelems;

	if (!GetState())
		return;

	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	ImVec2 displaySize = io.DisplaySize;
	ImVec2 wndSize = ImVec2(MenuSize);

	ImGui::SetNextWindowSize(wndSize);
	ImGui::SetNextWindowPos(ImVec2(displaySize.x / 2 - wndSize.x / 2, displaySize.y / 2 - wndSize.y / 2), ImGuiCond_Once);
	ImGui::Begin("RajceV2", nullptr, windowFlags);
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		ImDrawList* draw = window->DrawList;
		ImDrawList* fdraw = ImGui::GetForegroundDrawList();

		ImVec2 pos = window->Pos;
		ImVec2 wndSize = window->SizeFull;
		ImVec2 posOffset = ImVec2(0, 0);

		// Header
		{
			ImVec2 headerSize = ImVec2(wndSize.x, Window::HeaderHeight);

			draw->AddRectFilled(
				pos,
				pos + headerSize,
				(ImGui::IsWindowFocused() ? Window::TitleActive : Window::TitleInactive),
				Window::Rounding,
				ImDrawFlags_RoundCornersTop
			);

			ImGui::PushFont(io.Fonts->Fonts[UIFonts_Header]);

			float imgSize = (menuIcon == nullptr ? 0 : (UIFonts_Header_Size + Window::PaddingBetweenImageText));
			ImVec2 txtSize = ImGui::CalcTextSize("RajceV2");
			ImVec2 hPos = pos + (headerSize / 2 - (txtSize + ImVec2(imgSize, 0)) / 2);

			if (menuIcon && menuIcon != TEXTURE_QUEUED) {
				hPos.x -= 2;
				draw->AddImage(
					menuIcon,
					hPos,
					hPos + ImVec2(UIFonts_Header_Size, UIFonts_Header_Size),
					ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)
				);
				hPos.x += imgSize;
			}

			int vert_start = draw->VtxBuffer.Size;
			draw->AddText(
				hPos,
				Window::TextColor,
				"RajceV2"
			);
			int vert_end = draw->VtxBuffer.Size;
			ImGui::ShadeVertsLinearColorGradientKeepAlpha(
				draw,
				vert_start,
				vert_end,
				hPos,
				hPos + ImVec2(txtSize.x - ImGui::CalcTextSize("V2").x, 0),
				IM_COL32(0, 220, 0, 255),
				IM_COL32(220, 0, 0, 255)
			);

			ImGui::PopFont();
		}

		// Background
		draw->AddRectFilled(
			pos + ImVec2(0, Window::HeaderHeight),
			pos + wndSize,
			Window::WindowBg,
			Window::Rounding,
			ImDrawFlags_RoundCornersBottom
		);

		posOffset.y += Window::HeaderHeight;

		// Tabs
		{
			size_t tcnt = 0;
			UIBuilder::TabEntry** tabs = UIBuilder::GetTabs(&tcnt);
			float widest = max(UIBuilder::GetWidestTab() + Tabs::PaddingSides * 2, wndSize.x / tcnt);
			ImVec2 tPos = pos + posOffset;
			ImVec2 tSize = ImVec2(widest, Tabs::TabHeight);

			ImGui::PushFont(io.Fonts->Fonts[UIFonts_SectionText]);
			for (size_t i = 0; i < tcnt; i++) {
				UIBuilder::TabEntry* tab = tabs[i];
				ImGuiID tabId = window->GetID(tab->name);

				const ImRect bb(tPos, tPos + tSize);
				ImGui::ItemSize(bb);
				if (!ImGui::ItemAdd(bb, tabId, nullptr, ImGuiItemFlags_AllowOverlap)) {
					tPos.x += widest;
					continue;
				}

				bool held, hovered;
				bool pressed = ImGui::ButtonBehavior(bb, tabId, &hovered, &held, ImGuiButtonFlags_AllowOverlap);
				if (pressed || m_ActiveTab == nullptr)
					m_ActiveTab = tab;

				draw->AddRectFilled(
					bb.Min,
					bb.Max,
					(m_ActiveTab == tab ? Window::TextColor : Tabs::TabActiveColor)
				);
				draw->AddText(
					bb.Min + (tSize / 2 - tab->nameSize / 2) - ImVec2(0, 2),
					(m_ActiveTab == tab ? Tabs::TabActiveColor : Window::TextColor),
					tab->name
				);
				if (hovered) {
					draw->AddRectFilled(
						bb.Min,
						bb.Max,
						Tabs::TabHoveredColor
					);
				}

				tPos.x += widest;
			}
			ImGui::PopFont();

			// Shadow
			{
				ImColor shadow_top = IM_COL32(0, 0, 0, 180);
				ImColor shadow_bottom = IM_COL32(0, 0, 0, 0);

				tPos.x = pos.x;
				fdraw->AddRectFilledMultiColor(
					tPos,
					tPos + ImVec2(wndSize.x, tSize.y / 3),
					shadow_top, shadow_top, shadow_bottom, shadow_bottom
				);
			}

			posOffset.y += Tabs::TabHeight;
		}

		// Sections
		{
			ImVec2 sPos = pos + posOffset;
			float widest = wndSize.x / m_ActiveTab->sections.size();
			ImVec2 sSize = ImVec2(widest, Tabs::TabHeight);

			ImGui::PushFont(io.Fonts->Fonts[UIFonts_SectionText]);
			for (UIBuilder::SectionEntry* sec : m_ActiveTab->sections) {
				ImGuiID secId = window->GetID(sec->name);

				const ImRect bb(sPos, sPos + sSize);
				ImGui::ItemSize(bb);
				if (!ImGui::ItemAdd(bb, secId, nullptr, ImGuiItemFlags_AllowOverlap)) {
					sPos.x += widest;
					continue;
				}

				bool held, hovered;
				bool pressed = ImGui::ButtonBehavior(bb, secId, &hovered, &held, ImGuiButtonFlags_AllowOverlap);
				if (pressed)
					m_ActiveTab->currSection = sec->id;

				draw->AddRectFilled(
					bb.Min,
					bb.Max,
					(m_ActiveTab->currSection == sec->id ? Window::TextColor : Tabs::TabActiveColor)
				);
				draw->AddText(
					bb.Min + (sSize / 2 - sec->nameSize / 2) - ImVec2(0, 2),
					(m_ActiveTab->currSection == sec->id ? Tabs::TabActiveColor : Window::TextColor),
					sec->name
				);
				if (hovered) {
					draw->AddRectFilled(
						bb.Min,
						bb.Max,
						Tabs::TabHoveredColor
					);
				}

				sPos.x += widest;
			}
			ImGui::PopFont();

			// Shadow
			{
				ImColor shadow_top = IM_COL32(0, 0, 0, 180);
				ImColor shadow_bottom = IM_COL32(0, 0, 0, 0);

				sPos.x = pos.x;
				fdraw->AddRectFilledMultiColor(
					sPos,
					sPos + ImVec2(wndSize.x, sSize.y / 3),
					shadow_top, shadow_top, shadow_bottom, shadow_bottom
				);
			}

			posOffset.y += Tabs::TabHeight;
		}

		// Shadow
		{
			ImColor shadow_top = IM_COL32(0, 0, 0, 180);
			ImColor shadow_bottom = IM_COL32(0, 0, 0, 0);

			fdraw->AddRectFilledMultiColor(
				pos + posOffset,
				pos + posOffset + ImVec2(wndSize.x, Tabs::TabHeight),
				shadow_top, shadow_top, shadow_bottom, shadow_bottom
			);
		}

		window->DC.CursorPos = pos + posOffset;

		ImVec2 contentSize = wndSize - ImVec2(0, posOffset.y);
		ImGui::BeginChild("##Content", contentSize, false, ImGuiWindowFlags_NoScrollbar);
		{
			ImGuiWindow* cwindow = ImGui::GetCurrentWindow();
			ImDrawList* cdraw = cwindow->DrawList;
			ImVec2 cPos = cwindow->DC.CursorPos;

			UIBuilder::SectionEntry* sec = m_ActiveTab->sections.at(m_ActiveTab->currSection);
			float paddingWidth = ContentBox::PaddingSides * 2 + (ContentBox::SpaceBetween * (sec->boxes.size() - 1));
			float widthForBoxes = contentSize.x - paddingWidth;
			ImVec2 bSize = ImVec2(widthForBoxes / sec->boxes.size(), contentSize.y - (ContentBox::PaddingSides + ContentBox::PaddingTop));

			cPos += ImVec2(ContentBox::PaddingSides, ContentBox::PaddingTop);

			ImGui::PushFont(io.Fonts->Fonts[UIFonts_Text]);
			for (UIBuilder::ContentBoxEntry* box : sec->boxes) {
				ImVec2 txtOffset = ImVec2(ContentBox::TextPaddingFromLeft, 0);
				ImVec2 borderPosOffset = ImVec2(0, UIFonts_Text_Size - ContentBox::BorderThickness);
				ImVec2 innerSize = bSize - ImVec2(ContentBox::BorderThickness * 2, UIFonts_Text_Size + ContentBox::BorderThickness);

				// Visualize the box
				{
					cdraw->AddRectFilled(
						cPos + borderPosOffset,
						cPos + bSize,
						ContentBox::BoxBg
					);
					cdraw->AddRect(
						cPos + borderPosOffset,
						cPos + bSize,
						ContentBox::BoxBorderColor,
						0,
						0,
						ContentBox::BorderThickness
					);
					cdraw->AddRect(
						cPos + txtOffset - ImVec2(ContentBox::BorderThickness + ContentBox::TextPaddingSides, 1),
						cPos + txtOffset + box->nameSize + ImVec2(ContentBox::BorderThickness + ContentBox::TextPaddingSides, -1),
						ContentBox::BoxBorderColor,
						0,
						0,
						ContentBox::BorderThickness
					);

					cdraw->AddRectFilled(
						cPos + txtOffset - ImVec2(ContentBox::TextPaddingSides, 0),
						cPos + txtOffset + box->nameSize + ImVec2(ContentBox::TextPaddingSides, 0),
						ContentBox::BoxBg
					);
					cdraw->AddText(
						cPos + txtOffset - ImVec2(0, 2),
						Window::TextColor,
						box->name
					);
				}

				cwindow->DC.CursorPos = cPos + borderPosOffset + ImVec2(ContentBox::BorderThickness + 1, ContentBox::BorderThickness);
				ImGui::BeginChild(box->name, innerSize);
				{
					ImGuiWindow* bwindow = ImGui::GetCurrentWindow();
					ImVec2* bPos = &bwindow->DC.CursorPos;

					*bPos += ImVec2(0, Elements::PaddingSides);
					for (UIBuilder::ElementEntry* elem : box->elements) {
						*bPos += ImVec2(Elements::PaddingSides, 0);

						switch (elem->type)
						{
							case UIBuilder::ElementType_Button:
								if (ImGui::CButton(elem->text) && elem->callback)
									((fBtnCallback)elem->callback)();
								break;
							case UIBuilder::ElementType_Checkbox:
								if (ImGui::CCheckbox(elem->text, (bool*)elem->target) && elem->callback)
									((fCheckboxCallback)elem->callback)(*((bool*)elem->target));
								break;
							case UIBuilder::ElementType_SliderInt:
								if (ImGui::CSliderInt(elem->text, (int*)elem->target, elem->min, elem->max) && elem->callback)
									((fSliderCallback)elem->callback)(*((int*)elem->target));
								break;
							case UIBuilder::ElementType_SliderFloat:
								if (ImGui::CSliderFloat(elem->text, (float*)elem->target, elem->min, elem->max) && elem->callback)
									((fSliderCallback)elem->callback)(*((float*)elem->target));
								break;
							case UIBuilder::ElementType_Colorpicker:
								ImGui::CColorPicker(elem->text, (float*)elem->target);
								break;
						}

						*bPos += ImVec2(0, Elements::PaddingBetweenElems);
					}
				}
				ImGui::EndChild();

				cPos.x += bSize.x + ContentBox::SpaceBetween;
			}
			ImGui::PopFont();
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void MenuFunc(Destroy, ()) {
	if (menuIcon) {
		menuIcon->Release();
		menuIcon = nullptr;
	}
}

void MenuFunc(SetIcon, (ID3D11Resource* icon)) {
	if (!m_bInvokedInit) {
		iconResource = icon;
		return;
	}

	LoadUnityTexture(icon, &menuIcon);
}
void MenuFunc(SetKeybind, (Keybind* key)) {
	menuKey = key;
}
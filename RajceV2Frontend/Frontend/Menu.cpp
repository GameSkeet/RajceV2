// I recommend not changing any Menu value cause its been designed to be on those values
// You can still change it tho, but its on you to fix it

#include "../Source.hpp"

#pragma region Types & Definitions

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
		ImGui::BeginChild("##Content", wndSize - ImVec2(0, posOffset.y));
		{

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
// I recommend not changing any Menu value cause its been designed to be on those values
// You can still change it tho, but its on you to fix it

#include "../Source.hpp"

using namespace imelems;

#pragma region Types & Definitions

#define MenuSize 840, 525
#define MenuHeaderHeight 64
#define MenuLogoSizeX 200

#define MenuFunc(name, args) RajceV2::Menu::##name##args

#pragma endregion
#pragma region Variables

static bool m_bInvokedInit = false;
static ID3D11Resource* iconResource = nullptr;
static ID3D11ShaderResourceView* menuIcon = nullptr;
static Keybind* menuKey = nullptr;
static const DWORD windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar;

#pragma endregion

void MenuFunc(Init, ()) {
	menuIcon = LoadUnityTexture(iconResource);

	m_bInvokedInit = true;
	iconResource = nullptr;
}

bool MenuFunc(GetState, ()) {
	if (!menuKey)
		return false;

	return menuKey->State;
}
void MenuFunc(Render, ()) {
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
		ImVec2 logoSize = ImVec2(MenuLogoSizeX, MenuHeaderHeight);
		ImVec2 tabsSize = ImVec2(wndSize.x - (logoSize.x + Window::DividerSize), MenuHeaderHeight);
		ImVec2 sectionsSize = ImVec2(logoSize.x, wndSize.y - (logoSize.y + Window::DividerSize));
		ImVec2 contentSize = ImVec2(wndSize.x - (sectionsSize.x + Window::DividerSize), wndSize.y - (tabsSize.y + Window::DividerSize));

		// doing 0,0 vec just incase somebody wants to move it for some fucking reason
		// These calculations can also be simplified to just use logoSize
		ImVec2 logoPos = ImVec2(0, 0);
		ImVec2 tabsPos = ImVec2(logoPos.x + logoSize.x + Window::DividerSize, logoPos.y);
		ImVec2 sectionsPos = ImVec2(logoPos.x, logoPos.y + logoSize.y + Window::DividerSize);
		ImVec2 contentPos = logoPos + ImVec2(sectionsSize.x + Window::DividerSize, tabsSize.y + Window::DividerSize);

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		ImDrawList* draw = window->DrawList;
		ImDrawList* fdraw = ImGui::GetForegroundDrawList();
		ImVec2 pos = window->Pos;
		
		// Debug boxes
		{
			ImColor dbgCol = IM_COL32(0, 0, 255, 255);

			// Logo box
			{
				/*fdraw->AddRect(
					pos + logoPos,
					pos + logoPos + logoSize,
					dbgCol
				);

				ImVec2 txtSize = ImGui::CalcTextSize("Logo");
				fdraw->AddText(
					pos + logoPos + (logoSize / 2 - txtSize / 2),
					dbgCol,
					"Logo"
				);*/
			}

			// Tabs box
			{
				/*fdraw->AddRect(
					pos + tabsPos,
					pos + tabsPos + tabsSize,
					dbgCol
				);

				ImVec2 txtSize = ImGui::CalcTextSize("Tabs");
				fdraw->AddText(
					pos + tabsPos + (tabsSize / 2 - txtSize / 2),
					dbgCol,
					"Tabs"
				);*/
			}

			// Sections box
			{
				fdraw->AddRect(
					pos + sectionsPos,
					pos + sectionsPos + sectionsSize,
					dbgCol
				);

				ImVec2 txtSize = ImGui::CalcTextSize("Sections");
				fdraw->AddText(
					pos + sectionsPos + (sectionsSize / 2 - txtSize / 2),
					dbgCol,
					"Sections"
				);
			}

			// Content box
			{
				fdraw->AddRect(
					pos + contentPos,
					pos + contentPos + contentSize,
					dbgCol
				);

				ImVec2 txtSize = ImGui::CalcTextSize("Content");
				fdraw->AddText(
					pos + contentPos + (contentSize / 2 - txtSize / 2),
					dbgCol,
					"Content"
				);
			}
		}

		// Separators
		{
			// Vertical line
			draw->AddRectFilled(
				pos + ImVec2(logoSize.x, 0),
				pos + ImVec2(logoSize.x + Window::DividerSize, wndSize.y),
				Window::DividerColor
			);

			// Horizontal line
			draw->AddRectFilled(
				pos + ImVec2(0, logoSize.y),
				pos + ImVec2(wndSize.x, logoSize.y + Window::DividerSize),
				Window::DividerColor
			);
		}

		// Logo
		{
			ImGui::PushFont(io.Fonts->Fonts[UIFonts_Header]);

			ImColor sCol = IM_COL32(0, 255, 0, 255); 
			ImColor eCol = IM_COL32(255, 0, 0, 255); 
			
			float imageSize = min(logoSize.x, logoSize.y);
			ImVec2 txtSize = ImGui::CalcTextSize("RajceV2");
			ImVec2 curPos = logoPos + ImVec2(logoSize.x / 2 - (txtSize.x + imageSize) / 2, 0);
			curPos.x -= 2;

			// Need a better way to flip images than this
			draw->AddImage(menuIcon, pos + curPos, pos + curPos + ImVec2(imageSize, imageSize), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
			curPos.x += imageSize;
			curPos.y = logoPos.y + (logoSize.y / 2 - txtSize.y / 2);

			int vert_start = draw->VtxBuffer.Size;
			draw->AddText(
				pos + curPos,
				sCol,
				"RajceV2"
			);
			int vert_end = draw->VtxBuffer.Size;

			ImVec2 offset = ImVec2(txtSize.x / 3, 0);
			ImGui::ShadeVertsLinearColorGradientKeepAlpha(
				draw,
				vert_start,
				vert_end,
				pos + curPos + offset,
				pos + curPos + (txtSize - offset),
				sCol,
				eCol
			);

			ImGui::PopFont();
		}

		// Tabs
		{
			window->DC.CursorPos = pos + tabsPos;

			ImGui::BeginChild("##Tabs", tabsSize, true);
			{
				ImGuiWindow* cwindow = ImGui::GetCurrentWindow();
				ImDrawList* cdraw = cwindow->DrawList;
				ImVec2* cpos = &cwindow->DC.CursorPos;


			}
			ImGui::EndChild();
		}
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

	menuIcon = LoadUnityTexture(icon);
}
void MenuFunc(SetKeybind, (Keybind* key)) {
	menuKey = key;
}
// I recommend not changing any Menu value cause its been designed to be on those values
// You can still change it tho, but its on you to fix it

#include "../Source.hpp"

#pragma region Types & Definitions

typedef void(__cdecl* fBtnCallback)();
typedef void(__cdecl* fCheckboxCallback)(bool val);
typedef void(__cdecl* fSliderCallback)(float val);
typedef void(__cdecl* fComboBoxCallback)();
typedef wchar_t*(__cdecl* fConvertKeyCode)(int keycode);

#define MenuSize 840, 525
#define MenuFunc(name, args) RajceV2::Menu::##name##args
#define keyMap std::unordered_map<int, char*>

#pragma endregion
#pragma region Variables

static bool m_bInvokedInit = false;
static ID3D11Resource* iconResource = nullptr;
static ID3D11ShaderResourceView* menuIcon = nullptr;
static Keybind* menuKey = nullptr;
static fConvertKeyCode convertKeycode = nullptr;

static RajceV2::UIBuilder::TabEntry* m_ActiveTab = nullptr;
static keyMap key2name;

static const DWORD windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground;

#pragma endregion
#pragma region Custom imgui elements

// Any offsets to text on y axis by 2px is just correction for the used font
// Should prob put that as a macro
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

		draw->AddRect(
			bb.Min,
			bb.Max,
			Elements::BorderColor,
			Elements::Rounding,
			0,
			Elements::BorderThickness
		);

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

		draw->AddRect(
			bb.Min,
			bb.Min + ImVec2(size.y, size.y),
			Elements::BorderColor,
			Elements::Rounding,
			0,
			Elements::BorderThickness
		);

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
	bool CSliderInt(const char* text, int* val, float v_min, float v_max) {
		using namespace imelems;
		const ImGuiDataType_ DATA_TYPE = ImGuiDataType_S32;

		ImGuiWindow* wnd = GetCurrentWindow();
		if (wnd->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = wnd->GetID(text);
		const char* format = DataTypeGetInfo(DATA_TYPE)->PrintFmt;

		ImDrawList* draw = wnd->DrawList;
		ImVec2 pos = wnd->DC.CursorPos;
		ImVec2 cSize = GetContentRegionMax();
		ImVec2 size = ImVec2(cSize.x - Elements::PaddingSides * 2, Elements::MainElementHeight + UIFonts_Text_Size * 2 + Elements::InnerPadding * 2);

		const ImRect bb(pos, pos + size);
		ImVec2 sPos = ImVec2(bb.Min.x, bb.Min.y + UIFonts_Text_Size + Elements::InnerPadding);
		const ImRect slider_bb(sPos, ImVec2(bb.Max.x, sPos.y + Elements::MainElementHeight));
		ImVec2 vPos = ImVec2(bb.Min.x, slider_bb.Max.y + Elements::InnerPadding);
		const ImRect val_bb(vPos, bb.Max);

		ItemSize(bb);
		if (!ItemAdd(bb, id, &bb, ImGuiItemFlags_Inputable))
			return false;

		const bool hovered = ItemHoverable(slider_bb, id, g.LastItemData.InFlags);
		const bool clicked = hovered && IsMouseClicked(0, id);
		const bool make_active = (clicked || g.NavActivateId == id);
		if (make_active && clicked)
			SetKeyOwner(ImGuiKey_MouseLeft, id);
		if (make_active) {
			SetActiveID(id, wnd);
			SetFocusID(id, wnd);
			FocusWindow(wnd);
			g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
		}

		// Slider text
		RenderTextClipped(
			bb.Min,
			ImVec2(bb.Max.x, bb.Min.y + UIFonts_Text_Size + Elements::InnerPadding),
			text,
			nullptr,
			nullptr
		);

		// Slider thing
		bool value_changed = false;
		{
			float height = Elements::MainElementHeight / 4.f; // The height of the middle bar
			float gwidth = style.GrabMinSize;
			float space = (Elements::MainElementHeight - height * 2) / 2.f;

			int iv_min = (int)v_min;
			int iv_max = (int)v_max;

			ImRect grab_bb;
			value_changed = SliderBehavior(slider_bb, id, DATA_TYPE, val, &iv_min, &iv_max, format, 0, &grab_bb);
			if (value_changed)
				MarkItemEdited(id);

			draw->AddRectFilled(
				slider_bb.Min + ImVec2(3, space),
				slider_bb.Max - ImVec2(3, space),
				Elements::SecondaryColor,
				Elements::Rounding
			);

			// Grab
			draw->AddRectFilled(
				grab_bb.Min,
				ImVec2(grab_bb.Min.x + gwidth, grab_bb.Max.y),
				Elements::PrimaryColor,
				Elements::Rounding
			);
			if (g.ActiveId == id)
				draw->AddRectFilled(
					grab_bb.Min,
					ImVec2(grab_bb.Min.x + gwidth, grab_bb.Max.y),
					Elements::HeldColor,
					Elements::Rounding
				);
			else if (hovered)
				draw->AddRectFilled(
					grab_bb.Min,
					ImVec2(grab_bb.Min.x + gwidth, grab_bb.Max.y),
					Elements::HoveredColor,
					Elements::Rounding
				);
		}

		char val_buf[64];
		const char* val_buf_end = val_buf + DataTypeFormatString(val_buf, IM_ARRAYSIZE(val_buf), DATA_TYPE, val, format);
		RenderTextClipped(
			val_bb.Min - ImVec2(0, 2),
			val_bb.Max - ImVec2(0, 2),
			val_buf,
			val_buf_end,
			nullptr,
			ImVec2(0.5f, 0.5f)
		);

		return value_changed;
	}
	bool CSliderFloat(const char* text, float* val, float v_min, float v_max) {
		using namespace imelems;
		const ImGuiDataType_ DATA_TYPE = ImGuiDataType_Float;

		ImGuiWindow* wnd = GetCurrentWindow();
		if (wnd->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = wnd->GetID(text);
		const char* format = DataTypeGetInfo(DATA_TYPE)->PrintFmt;

		ImDrawList* draw = wnd->DrawList;
		ImVec2 pos = wnd->DC.CursorPos;
		ImVec2 cSize = GetContentRegionMax();
		ImVec2 size = ImVec2(cSize.x - Elements::PaddingSides * 2, Elements::MainElementHeight + UIFonts_Text_Size * 2 + Elements::InnerPadding * 2);

		const ImRect bb(pos, pos + size);
		ImVec2 sPos = ImVec2(bb.Min.x, bb.Min.y + UIFonts_Text_Size + Elements::InnerPadding);
		const ImRect slider_bb(sPos, ImVec2(bb.Max.x, sPos.y + Elements::MainElementHeight));
		ImVec2 vPos = ImVec2(bb.Min.x, slider_bb.Max.y + Elements::InnerPadding);
		const ImRect val_bb(vPos, bb.Max);

		ItemSize(bb);
		if (!ItemAdd(bb, id, &bb, ImGuiItemFlags_Inputable))
			return false;

		const bool hovered = ItemHoverable(slider_bb, id, g.LastItemData.InFlags);
		const bool clicked = hovered && IsMouseClicked(0, id);
		const bool make_active = (clicked || g.NavActivateId == id);
		if (make_active && clicked)
			SetKeyOwner(ImGuiKey_MouseLeft, id);
		if (make_active) {
			SetActiveID(id, wnd);
			SetFocusID(id, wnd);
			FocusWindow(wnd);
			g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
		}

		// Slider text
		RenderTextClipped(
			bb.Min,
			ImVec2(bb.Max.x, bb.Min.y + UIFonts_Text_Size + Elements::InnerPadding),
			text,
			nullptr,
			nullptr
		);

		// Slider thing
		bool value_changed = false;
		{
			float height = Elements::MainElementHeight / 4.f; // The height of the middle bar
			float gwidth = style.GrabMinSize;
			float space = (Elements::MainElementHeight - height * 2) / 2.f;

			ImRect grab_bb;
			value_changed = SliderBehavior(slider_bb, id, DATA_TYPE, val, &v_min, &v_max, format, 0, &grab_bb);
			if (value_changed)
				MarkItemEdited(id);

			draw->AddRectFilled(
				slider_bb.Min + ImVec2(3, space),
				slider_bb.Max - ImVec2(3, space),
				Elements::SecondaryColor,
				Elements::Rounding
			);

			// Grab
			draw->AddRectFilled(
				grab_bb.Min,
				ImVec2(grab_bb.Min.x + gwidth, grab_bb.Max.y),
				Elements::PrimaryColor,
				Elements::Rounding
			);
			if (g.ActiveId == id)
				draw->AddRectFilled(
					grab_bb.Min,
					ImVec2(grab_bb.Min.x + gwidth, grab_bb.Max.y),
					Elements::HeldColor,
					Elements::Rounding
				);
			else if (hovered)
				draw->AddRectFilled(
					grab_bb.Min,
					ImVec2(grab_bb.Min.x + gwidth, grab_bb.Max.y),
					Elements::HoveredColor,
					Elements::Rounding
				);
		}

		char val_buf[64];
		const char* val_buf_end = val_buf + DataTypeFormatString(val_buf, IM_ARRAYSIZE(val_buf), DATA_TYPE, val, format);
		RenderTextClipped(
			val_bb.Min - ImVec2(0, 2),
			val_bb.Max - ImVec2(0, 2),
			val_buf,
			val_buf_end,
			nullptr,
			ImVec2(0.5f, 0.5f)
		);

		return value_changed;
	}
	void CColorPicker(const char* text, float* rgba) {
		//TODO
	}
	bool CComboBox(const char* text, int* selected, const char** options, int num_options, bool is_multi, bool* is_open) {
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
		ImVec2 size = ImVec2(cSize.x - Elements::PaddingSides * 2, Elements::MainElementHeight + Elements::InnerPadding + UIFonts_Text_Size);

		const ImRect bb(pos, pos + size);
		const ImRect strip_bb(ImVec2(bb.Min.x, bb.Min.y + UIFonts_Text_Size + Elements::InnerPadding), bb.Max);

		ItemSize(size);
		if (!ItemAdd(bb, id, nullptr, ImGuiItemFlags_AllowOverlap))
			return false;

		bool hovered, held;
		bool clicked = ButtonBehavior(strip_bb, id, &hovered, &held, ImGuiButtonFlags_AllowOverlap);

		// Combo header text
		RenderTextClipped(
			bb.Min - ImVec2(0, 2),
			ImVec2(bb.Max.x, bb.Min.y + UIFonts_Text_Size + Elements::InnerPadding - 2),
			text,
			nullptr,
			nullptr
		);

		if (clicked) 
			*is_open = !*is_open;

		// Combo strip
		{
			draw->AddRectFilled(
				strip_bb.Min,
				strip_bb.Max,
				Elements::PrimaryColor,
				Elements::Rounding
			);
			draw->AddRect(
				strip_bb.Min,
				strip_bb.Max,
				Elements::BorderColor,
				Elements::Rounding,
				0,
				Elements::BorderThickness
			);

			float openerWidth = Elements::InnerPadding * 2 + UIFonts_Text_Size;
			const ImRect opener_bb(ImVec2(strip_bb.Max.x - openerWidth, strip_bb.Min.y), strip_bb.Max);
			draw->AddLine(
				opener_bb.Min,
				opener_bb.Max - ImVec2(openerWidth, 0),
				Elements::BorderColor,
				Elements::BorderThickness
			);

			// Draw the arrow
			{
				float radius = UIFonts_Text_Size / 2.f;
				float offset = (opener_bb.GetHeight() - radius) / 2.f;
				ImVec2 center = opener_bb.Min + (opener_bb.GetSize() - ImVec2(openerWidth, opener_bb.GetHeight()) / 2.f) + ImVec2(0, offset);

				if (*is_open) {
					draw->AddLine(
						ImVec2(center.x - radius, center.y),
						ImVec2(center.x, center.y - radius),
						Window::TextColor,
						2
					);
					draw->AddLine(
						ImVec2(center.x + radius, center.y),
						ImVec2(center.x, center.y - radius),
						Window::TextColor,
						2
					);
				}
				else {
					draw->AddLine(
						ImVec2(center.x - radius, center.y - radius),
						ImVec2(center.x, center.y),
						Window::TextColor,
						2
					);
					draw->AddLine(
						ImVec2(center.x + radius, center.y - radius),
						ImVec2(center.x, center.y),
						Window::TextColor,
						2
					);
				}
			}
			// Draw the selected values (this shit could have been much better, but currently idk how to make it better)
			{
				const ImRect strip2_bb(strip_bb.Min + ImVec2(Elements::PaddingSides, 0), strip_bb.Max - ImVec2(openerWidth, 0));

				bool nothingSelected = true;
				if (!is_multi) {
					int s_val = *selected;
					if (s_val != -1) {
						nothingSelected = false;

						RenderTextClipped(
							strip2_bb.Min - ImVec2(0, 2),
							strip2_bb.Max - ImVec2(0, 2),
							options[s_val],
							nullptr,
							nullptr,
							ImVec2(0, 0.5f)
						);
					}
				}
				else {
					std::vector<char> strip_text;

					for (int i = 0; i < num_options; i++)
						if (selected[i] != -1) {
							for (size_t len = 0; len < strlen(options[i]); len++)
								strip_text.push_back(options[i][len]);
							strip_text.push_back(',');
							strip_text.push_back(' ');
						}

					if (strip_text.size() != 0) {
						nothingSelected = false;

						char* text = strip_text.data();
						RenderTextClipped(
							strip2_bb.Min - ImVec2(0, 2),
							strip2_bb.Max - ImVec2(0, 2),
							text,
							text + (strip_text.size() - 2),
							nullptr,
							ImVec2(0, 0.5f)
						);
					}
				}

				if (nothingSelected)
					RenderTextClipped(
						strip2_bb.Min - ImVec2(0, 2),
						strip2_bb.Max - ImVec2(0, 2),
						"Nothing",
						nullptr,
						nullptr,
						ImVec2(0, 0.5f)
					);
			}

			if (held || clicked)
				draw->AddRectFilled(
					strip_bb.Min,
					strip_bb.Max,
					Elements::HeldColor,
					Elements::Rounding
				);
			else if (hovered)
				draw->AddRectFilled(
					strip_bb.Min,
					strip_bb.Max,
					Elements::HoveredColor,
					Elements::Rounding
				);
		}

		if (!*is_open)
			return false;

		SetNextWindowPos(ImVec2(bb.Min.x, bb.Max.y + 1));
		SetNextWindowSize(ImVec2(bb.GetWidth(), min(num_options, Elements::ComboBoxHeight) * UIFonts_Text_Size));
		Begin("##ComboPopup", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
		{
			ImGuiWindow* cwnd = GetCurrentWindow();
			ImDrawList* cdraw = cwnd->DrawList;
			ImVec2 oPos = cwnd->DC.CursorPos;

			if (!held && !clicked)
				*is_open = IsWindowFocused();

			if (*is_open) {
				ImVec2 maxCont = GetContentRegionMax();
				ImVec2 oSize = ImVec2(maxCont.x, UIFonts_Text_Size);

				for (int i = 0; i < num_options; i++) {
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

					oPos += ImVec2(0, oSize.y);
				}
			}
		}
		End();
	}
	void CKeybind(const char* text, Keybind* keybind, bool* is_open) {
		using namespace imelems;

		ImGuiWindow* wnd = GetCurrentWindow();
		if (wnd->SkipItems)
			return;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		const ImGuiID id = wnd->GetID(text);
		const ImVec2 label_size = CalcTextSize(text, 0, true);
		ImDrawList* draw = wnd->DrawList;

		ImVec2 pos = wnd->DC.CursorPos;
		ImVec2 cSize = GetContentRegionMax();
		ImVec2 size = ImVec2(cSize.x - Elements::PaddingSides * 2, Elements::MainElementHeight);

		const ImRect bb(pos, pos + size);
		ItemSize(size);
		if (!ItemAdd(bb, id, nullptr, ImGuiItemFlags_AllowOverlap))
			return;

		RenderTextClipped(
			bb.Min - ImVec2(0, 2),
			bb.Max - ImVec2(0, 2),
			text,
			nullptr,
			&label_size,
			ImVec2(0, 0.5f)
		);

		char* key_name;
		if (!key2name.contains(keybind->Keycode)) {
			if (!convertKeycode)
				key_name = (char*)"Unknown";
			else {
				wchar_t* result = convertKeycode(keybind->Keycode);
				ConvertStrToUTF8(&key_name, result);
				GlobalFree((void*)result);

				key2name[keybind->Keycode] = key_name;
			}
		}
		else key_name = key2name[keybind->Keycode];

		ImVec2 keySize = CalcTextSize(key_name);
		const ImRect keybox_bb(ImVec2(bb.Max.x - (keySize.x + Elements::InnerPadding * 2), bb.Min.y), bb.Max);
		
		bool hovered, held;
		bool clicked = ButtonBehavior(keybox_bb, id, &hovered, &held, ImGuiButtonFlags_AllowOverlap | ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		if (clicked && IsMouseReleased(ImGuiMouseButton_Left))
			keybind->Rebinding = true;
		if (clicked && IsMouseReleased(ImGuiMouseButton_Right))
			*is_open = true;

		draw->AddRectFilled(
			keybox_bb.Min,
			keybox_bb.Max,
			Elements::PrimaryColor,
			Elements::Rounding
		);
		draw->AddRect(
			keybox_bb.Min,
			keybox_bb.Max,
			Elements::BorderColor,
			Elements::Rounding,
			0,
			Elements::BorderThickness
		);
		RenderTextClipped(
			keybox_bb.Min - ImVec2(0, 2),
			keybox_bb.Max - ImVec2(0, 2),
			keybind->Rebinding ? "..." : key_name,
			nullptr,
			nullptr, // Could use the keySize if the rebind wasn't used
			ImVec2(0.5f, 0.5f)
		);

		if (held || clicked)
			draw->AddRectFilled(
				keybox_bb.Min,
				keybox_bb.Max,
				Elements::HeldColor,
				Elements::Rounding
			);
		else if (hovered)
			draw->AddRectFilled(
				keybox_bb.Min,
				keybox_bb.Max,
				Elements::HoveredColor,
				Elements::Rounding
			);

		if (!*is_open)
			return;

		ImVec2 popupSize = ImVec2(CalcTextSize("Reset key").x + Elements::PaddingSides * 2, 4 * UIFonts_Text_Size); // Hard-coded 4 cause there will always be 4 elements
		if (keybox_bb.GetWidth() > popupSize.x)
			SetNextWindowPos(ImVec2(keybox_bb.Min.x, keybox_bb.Max.y + 1));
		else SetNextWindowPos(ImVec2(keybox_bb.Max.x - popupSize.x, keybox_bb.Max.y + 1));

		SetNextWindowSize(popupSize); 
		Begin("##KeybindPopup", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
		{
			ImGuiWindow* cwnd = GetCurrentWindow();
			ImDrawList* cdraw = cwnd->DrawList;
			ImVec2 iPos = cwnd->DC.CursorPos;

			if (!held && !clicked)
				*is_open = IsWindowFocused();

			// Idk why i named some txx and some ixx, am i just retarded?
			if (*is_open) {
				ImVec2 maxCont = GetContentRegionMax();
				ImVec2 iSize = ImVec2(maxCont.x, UIFonts_Text_Size);
				int currType = (int)keybind->Type; // Adding this just so no visual stuff happens

				static const char* Types[4] = { "Hold", "Toggle", "Always", "Reset key" };
				for (int i = 0; i < 4; i++) {
					const char* type = Types[i];
					ImGuiID tid = cwnd->GetID(type);

					const ImRect tbb(iPos, iPos + iSize);
					ItemSize(iSize);
					if (!ItemAdd(tbb, tid, nullptr, ImGuiItemFlags_AllowOverlap)) {
						iPos += ImVec2(0, iSize.y);
						continue;
					}

					bool ihovered, iheld;
					bool iclicked = ButtonBehavior(tbb, tid, &ihovered, &iheld, ImGuiButtonFlags_AllowOverlap);
					if (iclicked) {
						if (i == 3)
							keybind->Keycode = 0;
						else keybind->Type = (KeybindType)i;

						*is_open = false;
					}

					if (currType == i)
						PushStyleColor(ImGuiCol_Text, Elements::SecondaryColor.Value);
					RenderTextClipped(
						tbb.Min - ImVec2(-Elements::PaddingSides, 2),
						tbb.Max - ImVec2(-Elements::PaddingSides, 2),
						type,
						nullptr,
						nullptr,
						ImVec2(0, 0.5f)
					);
					if (currType == i)
						PopStyleColor();

					if (iheld || iclicked)
						cdraw->AddRectFilled(
							tbb.Min,
							tbb.Max,
							Elements::HeldColor,
							Elements::Rounding
						);
					else if (ihovered)
						cdraw->AddRectFilled(
							tbb.Min,
							tbb.Max,
							Elements::HoveredColor,
							Elements::Rounding
						);

					iPos += ImVec2(0, iSize.y);
				}
			}
		}
		End();
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
							case UIBuilder::ElementType_Combo:
								if (ImGui::CComboBox(elem->text, (int*)elem->target, elem->options, elem->num_options, elem->is_multi, &elem->is_open) && elem->callback)
									((fComboBoxCallback)elem->callback)();
								break;
							case UIBuilder::ElementType_Keybind:
								ImGui::CKeybind(elem->text, (Keybind*)elem->target, &elem->is_open);
								break;
						}

						*bPos += ImVec2(0, Elements::PaddingBetweenElems);
					}

					ImGui::Dummy(ImVec2(0, 0)); // Just so imgui adds the bottom padding
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

	for (keyMap::iterator it = key2name.begin(); it != key2name.end(); it++) {
		free(it->second);
	}
	key2name.clear();
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
void MenuFunc(SetKeybindConverter, (void* func)) {
	convertKeycode = (fConvertKeyCode)func;
}
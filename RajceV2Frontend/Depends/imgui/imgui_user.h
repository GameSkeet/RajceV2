#pragma once

enum UIFonts {
	UIFonts_Text,
	UIFonts_BiggerText,
	UIFonts_Header,
	UIFonts_Icons
};

namespace ImGui {
	void ConvertStrToUTF8(char** out_buf, const wchar_t* str);

	void LoadFont(UIFonts font, byte* font_data, int data_len);
}
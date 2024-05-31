#pragma once

#define UIFonts_Text_Size 18
#define UIFonts_SectionText_Size 20
#define UIFonts_TabsText_Size 24
#define UIFonts_Header_Size 36

enum UIFonts {
	UIFonts_Text,
	UIFonts_SectionText,
	UIFonts_TabsText,
	UIFonts_Header,
};

namespace ImGui {
	void ConvertStrToUTF8(char** out_buf, const wchar_t* str);

	void LoadFont(UIFonts font, byte* font_data, int data_len);
}
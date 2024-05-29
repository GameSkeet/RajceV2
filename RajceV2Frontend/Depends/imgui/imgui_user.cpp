#include "../../Source.hpp"

void ImGui::ConvertStrToUTF8(char** out_buf, const wchar_t* str) {
	ImWchar* _str = (ImWchar*)str;

	int str_len = ImStrlenW(_str);
	int len = ImTextCountUtf8BytesFromStr(_str, _str + str_len) + 1;
	*out_buf = (char*)malloc(len * sizeof(char));

	ImTextStrToUtf8(*out_buf, len, _str, _str + str_len);
}

void ImGui::LoadFont(UIFonts type, byte* buff, int len) {
	ImFontConfig cfg;
	cfg.FontDataOwnedByAtlas = false;
	
	ImGuiIO& io = ImGui::GetIO();
	float size = 18;
	switch (type)
	{
		case UIFonts_BiggerText:
			size = 28;
			break;
		case UIFonts_Header:
			size = 36;
			break;
		case UIFonts_Icons:
			break;
	}

	io.Fonts->Fonts[type] = io.Fonts->AddFontFromMemoryTTF(buff, len, size, &cfg, io.Fonts->GetGlyphRangesCyrillic());
}
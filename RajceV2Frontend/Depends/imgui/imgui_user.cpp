#include "../../Source.hpp"
#include "imgui_impl_dx11.h"

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
	float size = UIFonts_Text_Size;
	switch (type)
	{
		case UIFonts_SectionText:
			size = UIFonts_SectionText_Size;
			break;
		case UIFonts_TabsText:
			size = UIFonts_TabsText_Size;
			break;
		case UIFonts_Header:
			size = UIFonts_Header_Size;
			break;
	}

	io.Fonts->Fonts[type] = io.Fonts->AddFontFromMemoryTTF(buff, len, size, &cfg, io.Fonts->GetGlyphRangesCyrillic());
	io.Fonts->Build();
	ImGui_ImplDX11_InvalidateDeviceObjects();
}
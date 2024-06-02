#pragma once

namespace imelems {
	namespace Window {
		static float Rounding = 5.f;
		
		static float PaddingBetweenImageText = 5.f;
		static float HeaderHeight = 48.f;

		static ImColor BorderColor = IM_COL32(255, 0, 0, 255);

		static ImColor WindowBg = IM_COL32(220, 30, 30, 255);

		static ImColor TitleInactive = IM_COL32(10, 120, 20, 150);
		static ImColor TitleActive = IM_COL32(20, 170, 20, 180);

		static ImColor TextColor = IM_COL32(255, 255, 255, 255);
	}

	// This is also used for sections
	namespace Tabs {
		static float PaddingSides = 5.f;
		static float TabHeight = UIFonts_SectionText_Size + 6;

		static ImColor TabHoveredColor = IM_COL32(255, 180, 180, 30);
		static ImColor TabActiveColor = IM_COL32(60, 36, 36, 255);
	}

	namespace ContentBox {
		static float PaddingSides = 5.f;
		static float PaddingTop = 15.f;
		static float SpaceBetween = 5.f;
		static float TextPaddingFromLeft = 10.f;
		static float BorderThickness = 2.f;
		static float TextPaddingSides = 2.f;

		static ImColor BoxBg = IM_COL32(180, 30, 30, 255);
		static ImColor BoxBorderColor = IM_COL32(220, 60, 60, 255);
	}

	namespace Elements {
		static float Rounding = 5.f;
		static float PaddingSides = 5.f;
		static float InnerPadding = 5.f;
		static float PaddingBetweenElems = 10.f;
		static float MainElementHeight = UIFonts_Text_Size; // This just sets the minimal "main" element height (by main i mean the thing you can interact with)

		static ImColor PrimaryColor = IM_COL32(120, 30, 30, 255);
		static ImColor HeldColor = IM_COL32(255, 180, 180, 80);
		static ImColor HoveredColor = IM_COL32(255, 180, 180, 30);

		static ImColor SecondaryColor = IM_COL32(20, 170, 20, 255);
	}
}
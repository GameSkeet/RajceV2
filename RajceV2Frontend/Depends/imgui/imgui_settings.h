#pragma once

#define RGBA_TO_IMVEC(r, g, b, a) ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f)

namespace imelems {
	namespace Window {
		static float Rounding = 5.f;
		static int DividerSize = 3;

		static ImVec4 BorderColor = RGBA_TO_IMVEC(255, 0, 0, 255);

		static ImVec4 WindowBg = RGBA_TO_IMVEC(80, 30, 30, 255);

		static ImVec4 TitleInactive = RGBA_TO_IMVEC(10, 120, 20, 220);
		static ImVec4 TitleActive = RGBA_TO_IMVEC(20, 170, 20, 255);

		static ImColor DividerColor = IM_COL32(15, 15, 15, 255);
	}

	namespace Tabs {
		static float Rounding = 5.f;
		static float PaddingSides = 5.f;
		static float PaddingTextLeft = 5.f;

		// Inactive color is WindowBg
		static ImColor TabHoveredColor = IM_COL32(100, 30, 30, 255);
		static ImColor TabActiveColor = IM_COL32(160, 40, 40, 255);

		static ImColor TabTextColor = IM_COL32(255, 255, 255, 255);
	}
}
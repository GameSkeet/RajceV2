#pragma once

struct Keybind {
public:
	int32_t Keycode;
	KeybindType Type;
	bool ShowInList;
	bool State;

	uint32_t ID;
};
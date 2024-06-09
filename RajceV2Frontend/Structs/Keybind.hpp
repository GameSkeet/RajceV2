#pragma once

struct Keybind {
public:
	int32_t Keycode;
	KeybindType Type;
	bool ShowInList;
	bool State;
	bool Rebinding;

	uint32_t ID;
};
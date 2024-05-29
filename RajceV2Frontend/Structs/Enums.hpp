#pragma once

enum InitCode : int32_t {
	InitCode_OK = 0,

	InitCode_InitFailed = 1,
	InitCode_HookFailed = 2
};

enum KeybindType : uint8_t {
	KeybindType_Hold,
	KeybindType_Toggle,
	KeybindType_Always
};
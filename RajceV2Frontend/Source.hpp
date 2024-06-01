#pragma once
#include <Windows.h>
#include <iostream>
#include <assert.h>
#include <Psapi.h>
#include <cstdint>

#include <vector>
#include <unordered_map>
#include <map>

#include <d3d11.h>
#include <dxgi.h>

#include "Depends/imgui/imconfig.h"
#include "Depends/imgui/imgui.h"
#include "Depends/imgui/imgui_internal.h"
#include "Depends/imgui/imgui_user.h" // Has custom imgui functions
#include "Depends/imgui/imgui_settings.h" // This contains colors and stuff

#include "Depends/minhook/minhook.h"
#include "Rendering/Kiero.hpp"
#include "Rendering/Renderer.hpp"

#include "Structs/Enums.hpp"
#include "Structs/Keybind.hpp"

#include "PrivateCode.hpp" // You can remove this include

#include "Frontend/KeybindManager.hpp"
#include "Frontend/Menu.hpp"
#include "Frontend/KeybindList.hpp"
#include "Frontend/UIBuilder.hpp"

#include "Exports.hpp"

#define TEXTURE_QUEUED (ID3D11ShaderResourceView*)0x5555 // This is used inside the private code
#define WAIT_FOR_DEBUGGER while (!IsDebuggerPresent()) Sleep(10)
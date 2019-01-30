#pragma once

#include "imgui.h"
#include "imgui_sw.hpp"

IMGUI_IMPL_API bool ImGui_ImplZDLib_Init(int width, int height);
IMGUI_IMPL_API void ImGui_ImplZDLib_Shutdown();
IMGUI_IMPL_API void ImGui_ImplZDLib_NewFrame();
IMGUI_IMPL_API void ImGui_ImplZDLib_Render();

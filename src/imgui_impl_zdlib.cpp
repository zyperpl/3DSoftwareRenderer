#include "imgui_impl_zdlib.h"
#include "imgui.h"
#include "imgui_sw.hpp"

#include "zdlib.h"

#include <vector>

std::vector<uint32_t> g_pixelBuffer;

int g_width, g_height;
imgui_sw::SwOptions g_sw_options;

bool ImGui_ImplZDLib_Init(int width, int height)
{
  g_pixelBuffer.resize(width*height);
  g_width = width;
  g_height = height;

  auto *ctx = ImGui::CreateContext();

  imgui_sw::bind_imgui_painting();
  
  ImGuiIO& io = ImGui::GetIO();
  io.IniFilename = NULL; // disable ini file
  io.DisplaySize = { static_cast<float>(width), static_cast<float>(height) };
 
  ImGuiStyle * style = &ImGui::GetStyle();
  style->Colors[ImGuiCol_MenuBarBg] = ImVec4(.0f, .0f, .0f, .0f); // set menubar color
    
  io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
  io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
  io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
  io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
  io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
  io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
  io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
  io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
  io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
  io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
  io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
  io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
  io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
  io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
  io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
  io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
  io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
  io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
  io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
  io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
  io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

  return ctx != nullptr;
}

void ImGui_ImplZDLib_Shutdown()
{
  g_pixelBuffer.clear();
}

void ImGui_ImplZDLib_NewFrame()
{
  std::fill_n(g_pixelBuffer.data(), g_pixelBuffer.size(), 0x00000000u);
  
  static ImGuiIO& io = ImGui::GetIO();
    
  auto mouse = zGetMousePosition();
  io.MousePos = ImVec2((float)mouse.x, (float)mouse.y);
  io.MouseDown[0] = zMouseButton(ZMOUSE_BUTTON_LEFT);
  io.MouseDown[1] = zMouseButton(ZMOUSE_BUTTON_MIDDLE);
  io.MouseDown[2] = zMouseButton(ZMOUSE_BUTTON_RIGHT);
  auto lastCharacter = zLastCharacter();
  if (lastCharacter > 0) io.AddInputCharacter(lastCharacter);
  
  #pragma omp parallel for
  for (int i = 0; i < 512; i++)
  {
    io.KeysDown[i] = zKey(i);
  }
}

void ImGui_ImplZDLib_Render()
{
  paint_imgui(g_pixelBuffer.data(), g_width, g_height, g_sw_options);

  auto *d = g_pixelBuffer.data();
  auto *wnd = zGetWindow()->buffer;
  
  #pragma omp parallel for
  for (int i = 0; i < g_width*g_height; i++)
  {
    wnd[i*Z_COMPONENTS+0] = static_cast<uint8_t>(d[i] >> (8*0));
    wnd[i*Z_COMPONENTS+1] = static_cast<uint8_t>(d[i] >> (8*1));
    wnd[i*Z_COMPONENTS+2] = static_cast<uint8_t>(d[i] >> (8*2));
  }
}

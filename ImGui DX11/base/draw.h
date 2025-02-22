#include <Windows.h>
#include <sstream>
#include "../imgui/imgui.h"

extern void add_text(ImVec2 pos, char* text, ImColor color);
extern float DrawOutlinedText(ImFont* pFont, const ImVec2& pos, float size, ImU32 color, bool center, const char* text, ...);
extern float DrawOutlinedTextForeground(ImFont* pFont, const ImVec2& pos, float size, ImU32 color, bool center, const char* text, ...);
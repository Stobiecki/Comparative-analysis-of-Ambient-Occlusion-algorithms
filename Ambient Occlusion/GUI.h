#pragma once

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"

#include "RenderData.h"

using std::cout;
using std::endl;
using std::string;

class GUI
{
public:
	static void DockspaceInit();
	static void Draw();
	static void HelpMarker(const char* desc);
};
/**
 * @author Andrej Bartulin
 * PROJECT: Termi-Windows version with OpenGL and ImGUI rendering system
 * LICENSE: BSD-3-Clause-License
 * DESCRIPTION: Header file for ImGUI code
 * INFORAMTION: Compile this Visual Studio solution
*/

#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Settings.hpp"

#include <iostream>
#include <fstream>

struct Canvas
{
    int width;
    int height;
};

struct Status
{
	bool alReadyPrinted;
};

class ContextMenu
{
    public:
        std::string log_1;

        void DrawContextMenu();

    private:
        void DrawNewTab();
};

void main_code();

extern ContextMenu* contextmenu;
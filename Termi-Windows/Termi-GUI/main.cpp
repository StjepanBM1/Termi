/**
 * @author Andrej Bartulin
 * PROJECT: Termi-Windows version with OpenGL and Dear ImGui rendering system
 * LICENSE: ringwormGO General License 1.0 | (RGL) 2022
 * DESCRIPTION: Main file, entry point for dll
*/

#include "pch.h" // use stdafx.h in Visual Studio 2017 and earlier
#include <utility>
#include <limits.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "imgui_code.hpp"

#include <iostream>
#include <string>
#include <fstream>
#include <signal.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Settings.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

int width;
int height;

static BOOL WINAPI end(DWORD dwCtrlType)
{
	int key;

	switch (dwCtrlType)
	{
	case CTRL_C_EVENT: // Ctrl+C
		std::cout << "\nPress any key to continue...\n";
		key = std::cin.get();
		if (key != 10)
		{
			/* we need to do something here; input is broken */
			exit(0);
		}
		else
		{
			exit(0);
		}
		break;
	case CTRL_BREAK_EVENT: // Ctrl+Break
		break;
	case CTRL_CLOSE_EVENT: // Closing the console window
		break;
	case CTRL_LOGOFF_EVENT: // User logs off. Passed only to services!
		break;
	case CTRL_SHUTDOWN_EVENT: // System is shutting down. Passed only to services!
		break;
	}

	// Return TRUE if handled this message, further handler functions won't be called.
	// Return FALSE to pass this message to further handlers until default handler calls ExitProcess().
	return FALSE;
}

int fn()
{
	std::cout << "fn\n";
	return 0;
}

int tmain()
{
	std::cout << "\n\n";

	/*
	 * Creating variables which points to struct(s) or class(es)
	 * vars gets value once,
	 * while render is always getting value
	*/

	Vars* vars = new Vars();
	vars->language = "english";

	Renderer* render = new Renderer();

	bool arg = false;
	bool alreadyarg = false;
	bool iconReady = false;

	/* Catch CTRL-C */
	SetConsoleCtrlHandler(end, TRUE);

	std::cout << "ooooooooooo                              " << std::endl;
	std::cout << "    888      ooooooooooo                          o88   " << std::endl;
	std::cout << "    888      888    88  oo oooooo  oo ooo oooo   oooo  " << std::endl;
	std::cout << "    888      888ooo8     888    888 888 888 888   888 " << std::endl;
	std::cout << "    888      888    oo   888        888 888 888   888  " << std::endl;
	std::cout << "   o888o    o888ooo8888 o888o      o888o888o888o o888o " << std::endl;
	std::cout << "------------------------------------------------------- " << std::endl;

	if (render->CheckFile("termi.png") == false)
	{
		std::cout << "Icon wasn't found!\n";
		std::cout << "Do you want download it [y (requires curl) /n]? : " << std::endl;

		std::string yn;
		std::cin >> yn;

		if (yn == "y")
		{
			system("wget https://raw.githubusercontent.com/ringwormGO-organization/Termi/main/Termi-Linux-OpenGL/termi.png");

			if (render->CheckFile("termi.png"))
			{
				iconReady = true;
			}
		}
	}

	else
	{
		iconReady = true;
	}

	glfwInit();
	GLFWwindow* window = glfwCreateWindow(render->Settings(1, 0), render->Settings(2, 0), "Termi (OpenGL)", NULL, NULL);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	if (window == NULL)
	{
		std::cout << "Unable to create OpenGL window!\nExiting...\n";
		return 1;
	}

	else
	{
#ifdef PRINT_WHEN_WINDOW_IS_CREATED
		std::cout << "OpenGL window is created.\n";
#endif
	}

	if (iconReady)
	{
		GLFWimage images[1];
		images[0].pixels = stbi_load("termi.png", &images[0].width, &images[0].height, 0, 4); /* rgba channels */
		glfwSetWindowIcon(window, 1, images);
		stbi_image_free(images[0].pixels);
	}

	else
	{
		std::cout << "Continuing without an icon!\n";
	}

	/* Initialize Dear ImGui */
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	render->Settings(3, 0);

	if (render->font_name != "default")
	{
		if (render->CheckFile(render->font_name.c_str()) == 1)
		{
			io.Fonts->AddFontFromFileTTF(render->font_name.c_str(), render->Settings(4, 0));
		}
	}

	render->Settings(0, 0);

	if (render->startup_command != "none")
	{
		arg = true;
	}

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		/* Tell OpenGL a new frame is about to begin */
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glfwGetWindowSize(window, &width, &height);
		window_width = static_cast<float>(width);
		window_height = static_cast<float>(height);

		ImGui::SetNextWindowPos(ImVec2(pos_x, pos_y));
		ImGui::SetNextWindowSize(ImVec2(window_width, window_height));

		/* main Dear ImGui code */
		main_code(vars, render);

#ifdef PRINT_FPS_CONSOLE
		printf("Application average %.3f ms/frame (%.1f FPS)\r", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
#endif

		/* Renders the Dear ImGui elements */
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	delete render;
	delete vars;

	return 0;
}
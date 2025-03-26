#include "Application.h"
#include "../UI/IMGUI_UI.h"
#include "../Algorithm/Math.h"
#include "../IO/FileOperations.h"
#include "../Algorithm/String.h"
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>


Application::Application() {
	// Window initialization
	m_window.setWindowSize(windowWidth, windowHeight);
	m_window.init();
	m_window.setBlend(true);

	// Logger initialization
	ent::util::Logger* logger = ent::util::Logger::getInstance();

	// Imgui initialization
	ent::ui::imgui::init((GLFWwindow*)m_window.getHandle());
	ent::ui::imgui::SetupImGuiStyle();

	// Font
	std::string fontPath = "res/fonts/Carlito-Bold.ttf";
	std::ifstream fontFile(fontPath);

	if (fontFile.good()) {
		ImGuiIO& io = ImGui::GetIO();
		static const ImWchar fullRange[] = { 0x0020, 0xFFFF, 0 }; // Unicode range from space to the end of BMP

		io.Fonts->AddFontFromFileTTF(fontPath.c_str(), fontSize, 0, fullRange);
		io.Fonts->Build();
	} else {
		logger->addLog("Failed to load font" + fontPath, ent::util::level::F);
	}

	m_needToUpdate = false;
}

void Application::runLoop() {
	while (!glfwWindowShouldClose((GLFWwindow*)m_window.getHandle())) {
		handleUI();
		handleProcessing();
	}
}

void Application::handleUI() {
	//////// Clear & handle events ////////////////////////////////////
	glfwPollEvents();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ent::ui::imgui::newFrame();
	///////////////////////////////////////////////////////////////////

	///////// UI //////////////////////////////////////////////////////
	ImGui::PushFont(0);


	ImGui::PopFont();
	///////////////////////////////////////////////////////////////////
	
	//////// Render ///////////////////////////////////////////////////
	ent::ui::imgui::renderFrame((GLFWwindow*)m_window.getHandle());
	glfwSwapBuffers((GLFWwindow*)m_window.getHandle());
	///////////////////////////////////////////////////////////////////
}

void Application::handleProcessing() {
	if (!m_needToUpdate) 
		return;

	m_needToUpdate = false;
}

void Application::imguiPadding(i32 x, i32 y) {
	if (x) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x);
	if (y) ImGui::SetCursorPosY(ImGui::GetCursorPosY() + y);
}

void Application::drawCircle(i32v2& center, f32 radius, f32v4 color, bool alwaysOnTop, bool filled, float thickness, int segments) {
	ImDrawList* draw_list = alwaysOnTop ? ImGui::GetForegroundDrawList() : ImGui::GetWindowDrawList();

	if (filled) {
		draw_list->AddCircleFilled(ImVec2(center.x, center.y), radius, ImColor(color.r, color.g, color.b, color.a), segments);
	} else {
		draw_list->AddCircle(ImVec2(center.x, center.y), radius, ImColor(color.r, color.g, color.b, color.a), segments, thickness);
	}
}

ui32 Application::createTexture(ui8* data, i32 width, i32 height, i32 nChannels) {
	if (!data) {
		return 0;
	}

	ui32 textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	ui32 mode;
	if (nChannels == 1) {
		mode = GL_RED;
	} else if (nChannels == 3) {
		mode = GL_RGB;
	} else if (nChannels == 4) {
		mode = GL_RGBA;
	} else {
		glDeleteTextures(1, &textureID);
		return 0;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, mode, width, height, 0, mode, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return textureID;
}

void Application::deleteTexture(ui32 textureID) {
	glDeleteTextures(1, &textureID);
}

void Application::drawRectangle(i32v2 position, i32v2 size, f32v4 color, bool filled , float outlineThickness, bool alwaysOnTop) {
	// Get the current window's draw list
	ImDrawList* draw_list = alwaysOnTop ? ImGui::GetForegroundDrawList() : ImGui::GetWindowDrawList();

	ImVec2 rect_min = ImVec2(position.x, position.y);					  // Top-left corner
	ImVec2 rect_max = ImVec2(position.x + size.x, position.y + size.y);	  // Bottom-right corner

	ImColor col = ImColor((i32)(color.r * 255.0), (i32)(color.g * 255.0), (i32)(color.b * 255.0), (i32)(color.a * 255.0));

	// Draw the filled rectangle
	if (filled) draw_list->AddRectFilled(rect_min, rect_max, col);

	// Draw the outline
	draw_list->AddRect(rect_min, rect_max, col, 0.0f, 0, outlineThickness);
}

void Application::drawPattern(std::string pattern, f32v4 color) {
	float availableWidth = ImGui::GetContentRegionAvail().x;
	ImGuiStyle style = ImGui::GetStyle();

	std::string line;
	float currentLineWidth = 0.0f;
	for (size_t i = 0; currentLineWidth < availableWidth; i++) {
		char nextChar = pattern[i % pattern.size()];  // Get the next character from the pattern
		std::string nextCharStr(1, nextChar);  // Convert char to string

		// Measure the width of the next character
		ImVec2 charSize = ImGui::CalcTextSize(nextCharStr.c_str());
		if (currentLineWidth + charSize.x > availableWidth + style.WindowPadding.x) {
			break;  // Stop if the next character exceeds available width
		}

		line += nextChar;
		currentLineWidth += charSize.x;
	}

	setTextColor(color);
	ImGui::Text(line.c_str());
	endTextColor();
}

void Application::setTextColor(f32v4 color) {
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(color.r, color.g, color.b, color.a));
}

void Application::endTextColor() {
	ImGui::PopStyleColor();
}
#pragma once
#include "../UI/GameWindow.h"
#include "../Utility/Timer.h"
#include <vector>


class Application {
public:
	Application();

	void runLoop();
private:
	i32 windowWidth = 900;
	i32 windowHeight = 900;
	i32 imagePathBufferSize = 4096;
	i32 fontSize = 18;
	i32 paddingSmall = 4;
	i32 paddingMedium = 12;
	i32 paddingBig = 24;
	i32 buttonHeightSmall = 30;
	i32 buttonHeightMedium = 40;
	f32v4 defaultTextColor = { 1, 1, 1, 1 };

	ent::ui::GameWindow m_window;
	ent::util::Logger* m_logger;

	bool m_needToUpdate;

	void handleUI();
	void handleProcessing();


	///////// UI helping elements //////////////////////////////////////////

	/*
	*  Moves current IMGUI cursor by a given pixels amount
	*  If applyToWindow is set to true applies to the next Window
	*/
	void imguiPadding(i32 x = 0, i32 y = 0);

	// Creates OpenGL texture ID 
	ui32 createTexture(ui8* data, i32 width, i32 height, i32 nChannels);

	// Deletes OpenGL texture
	void deleteTexture(ui32 textureID);


	// Adds circle drawCall using IMGUI
	void drawCircle(i32v2& center, f32 radius, f32v4 color, bool filled = false, bool alwaysOnTop = false, float thickness = 1.0f, int segments = 64);   

	// Adds rectangle drawCall using IMGUI
	void drawRectangle(i32v2 position, i32v2 size, f32v4 color, bool filled = false, float outlineThickness = 0.0f, bool alwaysOnTop = false);

	// Adds text pattern drawCall using IMGUI
	void drawPattern(std::string pattern, f32v4 color = { 1,1,1,1 });

	// Adds image drawCall using IMGUI
	void drawImage(i32v2 windowPosition, i32v2 windowSize, i32 imageTextureId, i32v2 imageSize);

	void fitImageIntoRectangle(i32v2 imageSize, i32v2 rectSize, i32v2* outPosition, i32v2* outSize);

	void setTextColor(f32v4 color = { 1, 1, 1, 1 });
	void endTextColor();

};
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
#include <tinyxml2.h>
#include <unordered_map>

// Todo reduce magic nubmers, rewrite code, continue with UI implementation & Minti algorithm

Node* getNodeById(i32 id, std::vector<Node>& nodes) {
	for (auto& node : nodes) {
		if (node.id == id)
			return &node;
	}
	return nullptr;
}

static void findShortestPathMinti(i32 startId, i32 endId, std::vector<Node>& nodes) {
	std::vector<Node*> availableNodes;
	std::vector<Node*> unAvailableNodes;

	for (auto& node : nodes) {
		node.totalWeight = -1;
		for (auto& weight : node.weights) {
			std::get<2>(weight) = false;
			std::get<3>(weight) = false;
		}

		if (node.id != startId) {
			unAvailableNodes.push_back(&node);
		}
		else {
			availableNodes.push_back(&node);
			node.totalWeight = 0;
		}
	}

	while (unAvailableNodes.size()) {
		auto initialNodes = availableNodes;
		// Todo zero checks and etc
		i32 minWeightId = 0;
		i32 minWeight = INT_MAX;
		Node* minNodeDest = nullptr;
		Node* minNodeSrc = nullptr;

		for (auto& node : initialNodes) {
			for (int i = 0; i < node->weights.size(); i++) {
				bool& processed = std::get<3>(node->weights[i]);
				bool& optimal = std::get<2>(node->weights[i]);
				i32 destId = std::get<0>(node->weights[i]);
				i32 weight = std::get<1>(node->weights[i]) + (node->isTotalWeightSet() ? node->totalWeight : 0);

				if (processed)
					continue;

				optimal = false;
				Node* destNode = getNodeById(destId, nodes);
				if (!destNode)
					continue;

				if (weight < minWeight) {
					if (destNode->isTotalWeightSet() && weight > destNode->totalWeight)
 						continue;

					minWeight = weight;
					minWeightId = i;
					minNodeDest = destNode;
					minNodeSrc = node;
				}
			}
		}

		bool& processed = std::get<3>(minNodeSrc->weights[minWeightId]);
		i32 destId = std::get<0>(minNodeSrc->weights[minWeightId]);
 		std::get<2>(minNodeSrc->weights[minWeightId]) = true;
		i32& totalWeigth = minNodeDest->totalWeight;
		totalWeigth = minWeight;
		processed = true;

		availableNodes.push_back(minNodeDest);

		unAvailableNodes.erase(
			std::remove_if(unAvailableNodes.begin(), unAvailableNodes.end(),
						   [&](const Node* node) { return node->id == minNodeDest->id; }),
			unAvailableNodes.end());
	}
}

Application::Application() {

	// Logger initialization
	m_logger = ent::util::Logger::getInstance();
	m_logger->setFilePath(logsFilePath);

	if (ent::io::fileExists(settingsFilePath)) {
		if (!loadSettings(settingsFilePath)) {
			setDefaultSettings();
		}
	} else {
		if (saveSettings(settingsFilePath)) {
			setDefaultSettings();
		}
	}

	// Window initialization
	m_window.setWindowSize(windowWidth, windowHeight);
	m_window.init();
	m_window.setBlend(true);

	ent::io::createDirectory(resFilepath);
	ent::io::createDirectory(fontsFilePath);
	ent::io::createDirectory(logsFilePath);

	
	// Imgui initialization
	ent::ui::imgui::init((GLFWwindow*)m_window.getHandle());
	ent::ui::imgui::SetupImGuiStyle();

	// Font
	std::ifstream fontFile(fontFilePath);
	if (fontFile.good()) {
		ImGuiIO& io = ImGui::GetIO();
		static const ImWchar fullRange[] = { 0x0020, 0xFFFF, 0 };

		io.Fonts->AddFontFromFileTTF(fontFilePath.c_str(), fontSize, 0, fullRange);
		io.Fonts->Build();
	} else {
		m_logger->addLog("Failed to load font " + fontFilePath, ent::util::level::F);
	}

	m_needToUpdate = false;

	addNode(200, 200);
	addNode(50, 50);
	addNode(222, 50);
	addNode(300, 50);
	addNode(300, 100);
	addNode(300, 150);
	addNode(300, 200);

	setNodeWeight(1, 2, 9);
	setNodeWeight(1, 3, 7);
	setNodeWeight(1, 4, 6);
	setNodeWeight(2, 3, 2);
	setNodeWeight(2, 4, 8);
	setNodeWeight(2, 5, 4);
	setNodeWeight(2, 6, 6);
	setNodeWeight(3, 5, 1);
	setNodeWeight(2, 7, 7);
	setNodeWeight(4, 6, 2);
	setNodeWeight(5, 7, 8);
	setNodeWeight(6, 7, 5);
	setNodeWeight(7, 3, 4);
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
	ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX));
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x * 0.7f, ImGui::GetIO().DisplaySize.y));
	ImGui::SetNextWindowPos(ImVec2(0, 0));

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove;

	ImGui::Begin("##HiddenTitle", nullptr, flags);
	handleWeightModify();
	moveNodes();
	drawNodes();

	ImGui::End();
	ImGui::PopFont();
	///////////////////////////////////////////////////////////////////
	
	//////// Render ///////////////////////////////////////////////////
	ent::ui::imgui::renderFrame((GLFWwindow*)m_window.getHandle());
	glfwSwapBuffers((GLFWwindow*)m_window.getHandle());
	///////////////////////////////////////////////////////////////////
}

void Application::handleProcessing() {
	modifyWeights();

	findShortestPathMinti(1, nodes.size(), nodes);
	if (!m_needToUpdate) 
		return;

	m_needToUpdate = false;
}

bool Application::loadSettings(const std::string& filepath) {
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError eResult = doc.LoadFile(filepath.c_str());
	if (eResult != tinyxml2::XML_SUCCESS) {
		std::string message = "Error loading XML file: " + std::to_string(static_cast<int>(eResult));
		m_logger->addLog(message, ent::util::level::E);
		return false;
	}

	tinyxml2::XMLElement* root = doc.FirstChildElement("Settings");
	if (!root) {
		m_logger->addLog("Missing <Settings> element in XML file", ent::util::level::E);
		return false;
	}

	const char* requiredAttributes[] = {
		"windowWidth", "windowHeight", "imagePathBufferSize", "fontSize",
		"paddingSmall", "paddingMedium", "paddingBig",
		"buttonHeightSmall", "buttonHeightMedium", "font"
	};

	for (const char* attr : requiredAttributes) {
		if (!root->Attribute(attr)) {
			m_logger->addLog(std::string("Missing required attribute: ") + std::string(attr), ent::util::level::E);
			return false;
		}
	}

	tinyxml2::XMLElement* textColorElem = root->FirstChildElement("DefaultTextColor");
	glm::vec4 defaultTextColor = {1.0f, 1.0f, 1.0f, 1.0f};
	if (textColorElem) {
		textColorElem->QueryFloatAttribute("r", &defaultTextColor.r);
		textColorElem->QueryFloatAttribute("g", &defaultTextColor.g);
		textColorElem->QueryFloatAttribute("b", &defaultTextColor.b);
		textColorElem->QueryFloatAttribute("a", &defaultTextColor.a);
	} else {
		m_logger->addLog("Missing <DefaultTextColor> element, using default color", ent::util::level::W);
	}

	return true;
}

bool Application::saveSettings(const std::string& filepath) {
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* root = doc.NewElement("Settings");
	doc.InsertFirstChild(root);

	root->SetAttribute("windowWidth", 900);
	root->SetAttribute("windowHeight", 900);
	root->SetAttribute("imagePathBufferSize", 4096);
	root->SetAttribute("fontSize", 18);
	root->SetAttribute("paddingSmall", 4);
	root->SetAttribute("paddingMedium", 12);
	root->SetAttribute("paddingBig", 24);
	root->SetAttribute("buttonHeightSmall", 30);
	root->SetAttribute("buttonHeightMedium", 40);
	root->SetAttribute("font", fontFileName.c_str());

	glm::vec4 defaultTextColor = {1.0f, 1.0f, 1.0f, 1.0f};
	tinyxml2::XMLElement* textColor = doc.NewElement("DefaultTextColor");
	textColor->SetAttribute("r", defaultTextColor.r);
	textColor->SetAttribute("g", defaultTextColor.g);
	textColor->SetAttribute("b", defaultTextColor.b);
	textColor->SetAttribute("a", defaultTextColor.a);
	root->InsertEndChild(textColor);

	tinyxml2::XMLError eResult = doc.SaveFile(filepath.c_str());
	if (eResult != tinyxml2::XML_SUCCESS) {
		std::string message = "Error saving XML file: " + std::to_string(static_cast<int>(eResult));
		m_logger->addLog(message, ent::util::level::E);
		return false;
	}
	return true;
}

void Application::setDefaultSettings() {
	windowWidth = 900;
	windowHeight = 900;
	imagePathBufferSize = 4096;
	fontSize = 18;
	paddingSmall = 4;
	paddingMedium = 12;
	paddingBig = 24;
	buttonHeightSmall = 30;
	buttonHeightMedium = 40;
	defaultTextColor = {1, 1, 1, 1};
	fontFileName = "Carlito - Bold.ttf";
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

void Application::drawText(const i32v2& center, const std::string& text, const f32v4& color, bool alwaysOnTop) {
	ImDrawList* drawList = alwaysOnTop? ImGui::GetForegroundDrawList() : ImGui::GetWindowDrawList();

	ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
	ImVec2 centeredPos(center.x - textSize.x * 0.5f, center.y - textSize.y * 0.5f);

	drawList->AddText(centeredPos, ImColor(color.r, color.g, color.b, color.a), text.c_str());
}

void Application::drawLineWithText(const f32v2& start, const f32v2& end, f32 offset, const std::string& text, const f32v4& lineColor, const f32v4& textColor) {
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	drawList->AddLine(ImVec2(start.x, start.y), ImVec2(end.x, end.y), ImColor(lineColor.r, lineColor.g, lineColor.b, lineColor.a), 2.0f);

	auto drawArrow = [&](const i32v2& from, const i32v2& to) {
		const float arrowSize = 20.0f;
		float angle = atan2f((float)(to.y - from.y), (float)(to.x - from.x));

		ImVec2 p1(to.x, to.y);
		ImVec2 p2(to.x - arrowSize * cosf(angle - 0.5f), to.y - arrowSize * sinf(angle - 0.5f));
		ImVec2 p3(to.x - arrowSize * cosf(angle + 0.5f), to.y - arrowSize * sinf(angle + 0.5f));

		drawList->AddTriangleFilled(p1, p2, p3, ImColor(lineColor.r, lineColor.g, lineColor.b, lineColor.a));
		};

	drawArrow(start, end);

	f32v2 dir = glm::normalize(end - start);
	f32 lengt = glm::length(end - start);
	f32v2 midpoint = start + dir * lengt * offset;
	ImVec2 textSize = ImGui::CalcTextSize(text.c_str());

	drawList->AddText(ImVec2(midpoint.x - textSize.x / 2, midpoint.y - textSize.y / 2),
					  ImColor(textColor.r, textColor.g, textColor.b, textColor.a),
					  text.c_str());
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

static bool isNodeTwoSided(const Node& node, const Node& otherNode, const std::vector<Node>& nodes) {
	bool directDir = false;
	bool reverseDir = false;

	for (const auto& weight : node.weights) {
		if (std::get<0>(weight) == otherNode.id) {
			directDir = true;
			break;
		}
	}

	for (const auto& weight : otherNode.weights) {
		if (std::get<0>(weight) == node.id) {
			reverseDir = true;
			break;
		}
	}

	return directDir && reverseDir;
}

void Application::drawNodes() {
	for (auto& node : nodes) {
		// Render lines
		for (auto& weight : node.weights) {
			auto it = std::find_if(nodes.begin(), nodes.end(), [weight](const Node& otherNode) {
				return otherNode.id == std::get<0>(weight); });

			if (it == nodes.end())
				continue;

			f32v2 start = node.center;
			f32v2 end = it->center;
			f32v2 dir = glm::normalize(glm::vec3(end.x - start.x, end.y - start.y, 0));
			f32v2 right = {dir.y, -dir.x};

			if (isNodeTwoSided(node, *it, nodes)) {
				start += right * (float)node.size * 0.5f;
				end += right * (float)node.size * 0.5f;
				start += dir * (float)node.size * cos(0.5f);
				end -= dir * (float)node.size * cos(0.5f);
			} else {
				start += dir * (float)node.size;
				end -= dir * (float)node.size;
			}
		
			std::string weidghtText = std::to_string(std::get<1>(weight));
			Node* destNode = getNodeById(std::get<0>(weight), nodes);
			std::string totalWeightText = std::to_string(destNode->totalWeight);

			drawText(destNode->center - i32v2(0, destNode->size * 1.5), totalWeightText, {1,1,1,1}, true);

			bool optimal = std::get<2>(weight);
			if (optimal) {
				drawLineWithText(start, end, 0.5, weidghtText, {0.569,0.196,0.1,1}, {1,1,1,1});
			} else {
				drawLineWithText(start, end, 0.5, weidghtText, {0.269,0.496,0.5,1}, {1,1,1,1});
			}
		}

		// Render nodes
		drawCircle(node.center, node.size, node.color, false, true);
		drawText(node.center, std::to_string(node.id));
	}
}

void Application::handleWeightModify() {
	if (!inputActive || !inputModifyValue)
		return;

	static bool initial = true;
	static i32 oldValue;

	ImGuiIO& io = ImGui::GetIO();
	ImDrawList* drawList = ImGui::GetForegroundDrawList();

	static char inputBuf[64];
	if (initial) {
		snprintf(inputBuf, sizeof(inputBuf), "%d", *inputModifyValue);
		initial = false;
		oldValue = *inputModifyValue;
		*inputModifyValue = -1;
	}

	std::string inputId = "##inputField";
	const ImVec2 padding = ImGui::GetStyle().FramePadding;
	const ImVec2 itemSpacing = ImGui::GetStyle().ItemSpacing;
	ImVec2 availableSpace = ImGui::GetContentRegionAvail();
	ImVec2 textSize = ImGui::CalcTextSize(inputBuf);
	ImVec2 oneLetterSize = ImGui::CalcTextSize("0");
	ImVec2 fieldSize = ImVec2(oneLetterSize.x + textSize.x + padding.x * 2.0f, textSize.y + padding.y * 2.0f);
	ImVec2 fieldPos = ImVec2(inputPos.x - fieldSize.x * 0.5f, inputPos.y - fieldSize.y * 0.5f);

	ImGui::SetNextWindowPos(fieldPos);
	ImGui::SetNextWindowBgAlpha(1.0f);
	ImGui::SetNextWindowSize(fieldSize);

	ImGui::Begin(inputId.c_str(), nullptr,
				 ImGuiWindowFlags_NoTitleBar |
				 ImGuiWindowFlags_NoScrollbar |
				 ImGuiWindowFlags_NoCollapse |
				 ImGuiWindowFlags_NoMove |
				 ImGuiWindowFlags_NoResize |
				 ImGuiWindowFlags_NoSavedSettings);

	ImGui::SetKeyboardFocusHere();

	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x); 
	if (ImGui::InputText("##intInput", inputBuf, sizeof(inputBuf), ImGuiInputTextFlags_EnterReturnsTrue)
		|| ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		*inputModifyValue = std::atoi(inputBuf);
		inputActive = false;
		initial = true;
	}

	ImGui::End();
}


void Application::addNode(i32 x, i32 y) {
	i32 id = nodes.size() + 1;
	Node node({x, y}, nodeSize, {0.972,0.624,0.357,1}, id, -1);
	nodes.push_back(node);
}


static bool isValidDistance(const Node& node, const std::vector<Node>& nodes) {
	for (const auto& otherNode : nodes) {
		if (otherNode.id == node.id)
			continue;

		float dx = node.center.x - otherNode.center.x;
		float dy = node.center.y - otherNode.center.y;
		float distance = std::sqrt(dx * dx + dy * dy);

		if (distance < (node.size + otherNode.size)) {
			return false; 
		}
	}
	return true; 
}

void Application::moveNodes() {
	ImVec2 mousePos = ImGui::GetIO().MousePos;
	static bool moving = false;

	for (auto& node : nodes) {
		bool isMouseOver = (mousePos.x >= node.center.x - node.size && mousePos.x <= node.center.x + node.size &&
							mousePos.y >= node.center.y - node.size && mousePos.y <= node.center.y + node.size);

		if (!isMouseOver && !moving)
			continue;

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			moving = true;
		}

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
			moving = false;
		}

		if (moving) {
			Node newNode = node;

			newNode.center.x = mousePos.x;
			newNode.center.y = mousePos.y;

			if (isValidDistance(newNode, nodes)) {
				node = newNode;
			}
		}
	}
}

void Application::modifyWeights() {
	ImVec2 mousePos = ImGui::GetIO().MousePos;

	for (auto& node : nodes) {

		for (auto& weight : node.weights) {
			auto it = std::find_if(nodes.begin(), nodes.end(), [weight](const Node& otherNode) {
				return otherNode.id == std::get<0>(weight); });

			if (it == nodes.end())
				continue;

			f32v2 start = it->center;
			f32v2 end = node.center;
			f32v2 dir = glm::normalize(glm::vec3(end.x - start.x, end.y - start.y, 0));
			f32v2 right = {dir.y, -dir.x};

			if (isNodeTwoSided(node, *it, nodes)) {
				start += right * (float)node.size * 0.5f;
				end += right * (float)node.size * 0.5f;
				start += dir * (float)node.size * cos(0.5f);
				end -= dir * (float)it->size * cos(0.5f);
			} else {
				start += dir * (float)node.size;
				end -= dir * (float)it->size;
			}

			f32v2 mid = (end + start) * 0.5f;
			bool isMouseOver = (mousePos.x >= mid.x - node.size && mousePos.x <= mid.x + node.size &&
								mousePos.y >= mid.y - node.size && mousePos.y <= mid.y + node.size);

			if (!isMouseOver || !ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				continue;

			if (!weightTimer.active()) {
				weightTimer.setTimer(doubleClickThresh);
				return;
			}

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
				inputActive = true;
				inputPos = mid;
				inputModifyValue = &std::get<1>(weight);
			}
		}
	
	}
}

void Application::setNodeWeight(i32 nodeId, i32 destId, i32 cost) {
	for (auto& node : nodes) {
		if (node.id != nodeId)
			continue;

		bool exists = false;
		bool optimal = false;
		for (auto& weight : node.weights) {
			if (std::get<1>(weight) > cost) {
				std::get<2>(weight) = false;
				optimal = true;
			}
	
			if (std::get<0>(weight) != destId)
				continue;

			std::get<1>(weight) = cost;
			exists = true;
		}

		if (!node.weights.size())
			optimal = true;

		if (!exists)
			node.weights.emplace_back(destId, cost, optimal, false);
	}
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
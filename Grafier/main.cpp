#include "App/Application.h"

int main() {
	std::string logsFilePath = "res/logs/";
	ent::util::Logger* logger = ent::util::Logger::getInstance();
	logger->setFilePath(logsFilePath);
	logger->setLogToFile(true);
	Application app;

	app.runLoop();

	return 0;
}
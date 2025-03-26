#pragma once
#include <string>
#include <iostream>
#include <fstream>

namespace ent {
	namespace util {
		enum class level {
			INFO,
			WARN,
			ERROR,
			FATAL,
			DEBUG,

			I = INFO,
			W = WARN,
			E = ERROR,
			F = FATAL,
			D = DEBUG

		};

		// Singleton instance logger. Logs to cmd or/and file
		class Logger {
		public:
			static Logger* getInstance();
			void setLogToConsole(bool state);
			void setLogToFile(bool state);
			void setFilePath(std::string& path);

			// Sets log level up to desired
			// @param level: INFO, WARN, ERROR, FATAL, DEBUG
			void setLogLevel(level logLevel);
			void addLog(std::string msg, level logLevel);


			~Logger();
		private:
			bool logToConsole;
			bool logToFile;

			level logLevel;

			std::string filePath;
			std::ofstream file;

			std::string getTime();
			std::string getDate();

			Logger();
			static Logger* _instance;
		};

	}
}
#pragma once
#include <glad/glad.h>
#include "../Utility/DataTypes.h"
#include "../Logger/Logger.h"
#include <string>

namespace ent {
	namespace render {
		class Shader {
		public:
			// Basic constructor. Does not load or modify any shaders
			Shader();
			// Loads & compile shaders. Use .use() function to bind shader to current render
			Shader(std::string& vertexShPath, std::string& fragmentShPath);
			Shader(const char* vertexShPath, const char* fragmentShPath);
			~Shader() = default;

			// Loads & compile shaders. Use .use() function to bind shader to current render
			bool load(std::string& vertexShPath, std::string& fragmentShPath);
			bool load(const char* vertexShPath, const char* fragmentShPath);
			void use();

			void setBool(const char* name, bool value);
			void setInt(const char* name, int value);
			void setFloat(const char* name, float value);
			void setVec3(const char* name, float value1, float value2, float value3);
			void setVec3(const char* name, glm::vec3 value);
			void setMat4(const char* name, glm::mat4 matrix);

			glm::mat4 getMat4(const char* name);
		private:
			ui32 id;

			std::string readShaderFromFile(std::string shaderPath);

			util::Logger* logger;
		};
	}
}